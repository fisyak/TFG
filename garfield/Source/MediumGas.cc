#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <array>
#include <ctime>

#include "MediumGas.hh"
#include "OpticalData.hh"
#include "FundamentalConstants.hh"

namespace {

void PrintExtrapolation(const std::pair<unsigned int, unsigned int>& extr) {

  std::cout << "        Low field extrapolation: ";
  if (extr.first == 0)
    std::cout << " constant\n";
  else if (extr.first == 1)
    std::cout << " linear\n";
  else if (extr.first == 2)
    std::cout << " exponential\n";
  else
    std::cout << " unknown\n";
  std::cout << "        High field extrapolation: ";
  if (extr.second == 0)
    std::cout << " constant\n";
  else if (extr.second == 1)
    std::cout << " linear\n";
  else if (extr.second == 2)
    std::cout << " exponential\n";
  else
    std::cout << " unknown\n";
}

}

namespace Garfield {

MediumGas::MediumGas() : Medium(),
      m_pressureTable(m_pressure),
      m_temperatureTable(m_temperature) {

  m_className = "MediumGas";

  // Default gas mixture: pure argon
  for (unsigned int i = 0; i < m_nMaxGases; ++i) {
    m_fraction[i] = 0.;
    m_gas[i] = "";
    m_atWeight[i] = 0.;
    m_atNum[i] = 0.;
  }
  m_gas[0] = "Ar";
  m_fraction[0] = 1.;
  m_name = m_gas[0];
  GetGasInfo(m_gas[0], m_atWeight[0], m_atNum[0]);

  m_isChanged = true;

  EnableDrift();
  EnablePrimaryIonisation();

  // Initialise Penning parameters
  for (unsigned int i = 0; i < m_nMaxGases; ++i) {
    m_rPenningGas[i] = 0.;
    m_lambdaPenningGas[i] = 0.;
  }

}

bool MediumGas::SetComposition(const std::string& gas1, const double f1,
                               const std::string& gas2, const double f2,
                               const std::string& gas3, const double f3,
                               const std::string& gas4, const double f4,
                               const std::string& gas5, const double f5,
                               const std::string& gas6, const double f6) {

  // Make a backup copy of the gas composition.
  std::string gasOld[m_nMaxGases];
  for (unsigned int i = 0; i < m_nMaxGases; ++i) {
    gasOld[i] = m_gas[i];
  }
  const unsigned int nComponentsOld = m_nComponents;
  m_nComponents = 0;

  std::string gases[6] = {gas1, gas2, gas3, gas4, gas5, gas6};
  double fractions[6] = {f1, f2, f3, f4, f5, f6};
  for (unsigned int i = 0; i < 6; ++i) {
    // Find the gas name corresponding to the input string.
    std::string gasname = "";
    if (fractions[i] > 0. && GetGasName(gases[i], gasname)) {
      m_gas[m_nComponents] = gasname;
      m_fraction[m_nComponents] = fractions[i];
      ++m_nComponents;
    }
  }

  // Check if at least one valid ingredient was specified.
  if (m_nComponents == 0) {
    std::cerr << m_className << "::SetComposition:\n"
              << "    Error setting the composition.\n"
              << "    No valid ingredients were specified.\n";
    return false;
  }

  // Establish the name.
  m_name = "";
  double sum = 0.;
  for (unsigned int i = 0; i < m_nComponents; ++i) {
    if (i > 0) m_name += "/";
    m_name += m_gas[i];
    sum += m_fraction[i];
  }
  // Normalise the fractions to one.
  for (unsigned int i = 0; i < m_nMaxGases; ++i) {
    if (i < m_nComponents) {
      m_fraction[i] /= sum;
    } else {
      m_fraction[i] = 0.;
    }
  }

  // Set the atomic weight and number
  for (unsigned int i = 0; i < m_nComponents; ++i) {
    m_atWeight[i] = 0.;
    m_atNum[i] = 0.;
    GetGasInfo(m_gas[i], m_atWeight[i], m_atNum[i]);
  }

  // Print the composition.
  std::cout << m_className << "::SetComposition:\n    " << m_name;
  if (m_nComponents > 1) {
    std::cout << " (" << m_fraction[0] * 100;
    for (unsigned int i = 1; i < m_nComponents; ++i) {
      std::cout << "/" << m_fraction[i] * 100;
    }
    std::cout << ")";
  }
  std::cout << "\n";

  // Force a recalculation of the collision rates.
  m_isChanged = true;

  // Copy the previous Penning transfer parameters.
  double rPenningGasOld[m_nMaxGases];
  double lambdaPenningGasOld[m_nMaxGases];
  for (unsigned int i = 0; i < m_nMaxGases; ++i) {
    rPenningGasOld[i] = m_rPenningGas[i];
    lambdaPenningGasOld[i] = m_lambdaPenningGas[i];
    m_rPenningGas[i] = 0.;
    m_lambdaPenningGas[i] = 0.;
  }
  for (unsigned int i = 0; i < m_nComponents; ++i) {
    for (unsigned int j = 0; j < nComponentsOld; ++j) {
      if (m_gas[i] != gasOld[j]) continue;
      if (rPenningGasOld[j] > 0.) {
        m_rPenningGas[i] = rPenningGasOld[j];
        m_lambdaPenningGas[i] = lambdaPenningGasOld[i];
        std::cout << m_className << "::SetComposition:\n"
                  << "    Using Penning transfer parameters for " 
                  << m_gas[i] << " from previous mixture.\n"
                  << "      r      = " << m_rPenningGas[i] << "\n"
                  << "      lambda = " << m_lambdaPenningGas[i] << " cm\n";
      }
    }
  }
  return true;
}

void MediumGas::GetComposition(std::string& gas1, double& f1, std::string& gas2,
                               double& f2, std::string& gas3, double& f3,
                               std::string& gas4, double& f4, std::string& gas5,
                               double& f5, std::string& gas6, double& f6) {

  gas1 = m_gas[0];
  gas2 = m_gas[1];
  gas3 = m_gas[2];
  gas4 = m_gas[3];
  gas5 = m_gas[4];
  gas6 = m_gas[5];
  f1 = m_fraction[0];
  f2 = m_fraction[1];
  f3 = m_fraction[2];
  f4 = m_fraction[3];
  f5 = m_fraction[4];
  f6 = m_fraction[5];
}

void MediumGas::GetComponent(const unsigned int i, 
                             std::string& label, double& f) {

  if (i >= m_nComponents) {
    std::cerr << m_className << "::GetComponent: Index out of range.\n";
    label = "";
    f = 0.;
    return;
  }

  label = m_gas[i];
  f = m_fraction[i];
}

void MediumGas::SetAtomicNumber(const double z) {

  std::cerr << m_className << "::SetAtomicNumber:\n"
            << "    Effective Z cannot be changed directly to " << z << ".\n"
            << "    Use SetComposition to define the gas mixture.\n";
}

void MediumGas::SetAtomicWeight(const double a) {

  std::cerr << m_className << "::SetAtomicWeight:\n"
            << "    Effective A cannot be changed directly to " << a << ".\n"
            << "    Use SetComposition to define the gas mixture.\n";
}

void MediumGas::SetNumberDensity(const double n) {

  std::cerr << m_className << "::SetNumberDensity:\n"
            << "    Density cannot directly be changed to " << n << ".\n"
            << "    Use SetTemperature and SetPressure.\n";
}

void MediumGas::SetMassDensity(const double rho) {

  std::cerr << m_className << "::SetMassDensity:\n"
            << "    Density cannot directly be changed to " << rho << ".\n"
            << "    Use SetTemperature, SetPressure and SetComposition.\n";
}

double MediumGas::GetAtomicWeight() const {

  // Effective A, weighted by the fractions of the components.
  double a = 0.;
  for (unsigned int i = 0; i < m_nComponents; ++i) {
    a += m_atWeight[i] * m_fraction[i];
  }
  return a;
}

double MediumGas::GetNumberDensity() const {

  // Ideal gas law.
  return LoschmidtNumber * (m_pressure / AtmosphericPressure) *
         (ZeroCelsius / m_temperature);
}

double MediumGas::GetMassDensity() const {

  return GetNumberDensity() * GetAtomicWeight() * AtomicMassUnit;
}

double MediumGas::GetAtomicNumber() const {

  // Effective Z, weighted by the fractions of the components.
  double z = 0.;
  for (unsigned int i = 0; i < m_nComponents; ++i) {
    z += m_atNum[i] * m_fraction[i];
  }
  return z;
}

bool MediumGas::LoadGasFile(const std::string& filename) {

  std::ifstream gasfile;
  // Open the file.
  gasfile.open(filename.c_str());
  // Make sure the file could be opened.
  if (!gasfile.is_open()) {
    std::cerr << m_className << "::LoadGasFile:\n"
              << "    Gas file could not be opened.\n";
    return false;
  }

  char line[256];
  char* token;

  // GASOK bits
  std::string gasBits = "";

  // Gas composition
  const int nMagboltzGases = 60;
  std::vector<double> mixture(nMagboltzGases, 0.);

  int excCount = 0;
  int ionCount = 0;

  int nE = 1;
  int nB = 1;
  int nA = 1;

  int version = 12;

  // Start reading the data.
  bool atTables = false;
  while (!atTables) {
    gasfile.getline(line, 256);
    if (strncmp(line, " The gas tables follow:", 8) == 0 ||
        strncmp(line, "The gas tables follow:", 7) == 0) {
      atTables = true;
      if (m_debug) {
        std::cout << "    Entering tables.\n";
        getchar();
      }
    }
    if (m_debug) {
      std::cout << "    Line: " << line << "\n";
      getchar();
    }
    if (!atTables) {
      token = strtok(line, " :,%");
      while (token) {
        if (m_debug) std::cout << "    Token: " << token << "\n";
        if (strcmp(token, "Version") == 0) {
          token = strtok(NULL, " :,%");
          version = atoi(token);
          // Check the version number.
          if (version != 10 && version != 11 && version != 12) {
            std::cerr << m_className << "::LoadGasFile:\n"
                      << "    The file has version number " << version << ".\n"
                      << "    Files written in this format cannot be read.\n";
            gasfile.close();
            return false;
          } else {
            std::cout << m_className << "::LoadGasFile:\n"
                      << "    Version: " << version << "\n";
          }
        } else if (strcmp(token, "GASOK") == 0) {
          // Get the GASOK bits indicating if a parameter
          // is present in the table (T) or not (F).
          token = strtok(NULL, " :,%\t");
          token = strtok(NULL, " :,%\t");
          gasBits += token;
        } else if (strcmp(token, "Identifier") == 0) {
          // Get the identification string.
          std::string identifier = "";
          token = strtok(NULL, "\n");
          if (token != NULL) identifier += token;
          if (m_debug) {
            std::cout << m_className << "::LoadGasFile:\n"
                      << "    Identifier: " << token << "\n";
          }
        } else if (strcmp(token, "Dimension") == 0) {
          token = strtok(NULL, " :,%\t");
          if (strcmp(token, "F") == 0) {
            m_map2d = false;
          } else {
            m_map2d = true;
          }
          token = strtok(NULL, " :,%\t");
          nE = atoi(token);
          // Check the number of E points.
          if (nE <= 0) {
            std::cerr << m_className << "::LoadGasFile:\n"
                      << "    Number of E fields out of range.\n";
            gasfile.close();
            return false;
          }
          token = strtok(NULL, " :,%\t");
          nA = atoi(token);
          // Check the number of angles.
          if (m_map2d && nA <= 0) {
            std::cerr << m_className << "::LoadGasFile:\n"
                      << "    Number of E-B angles out of range.\n";
            gasfile.close();
            return false;
          }

          token = strtok(NULL, " :,%\t");
          nB = atoi(token);
          // Check the number of B points.
          if (m_map2d && nB <= 0) {
            std::cerr << m_className << "::LoadGasFile:\n"
                      << "    Number of B fields out of range.\n";
            gasfile.close();
            return false;
          }

          m_eFields.resize(nE);
          m_bFields.resize(nB);
          m_bAngles.resize(nA);

          // Fill in the excitation/ionisation structs
          // Excitation
          token = strtok(NULL, " :,%\t");
          if (m_debug) std::cout << "    " << token << "\n";
          m_excitationList.clear();
          const int nexc = atoi(token);
          if (nexc > 0) m_excitationList.resize(nexc);
          // Ionization
          token = strtok(NULL, " :,%\t");
          const int nion = atoi(token);
          m_ionisationList.clear();
          if (nion > 0) m_ionisationList.resize(nion);
          if (m_debug) {
            std::cout << "    " << nexc << " excitations, " 
                     << nion << " ionisations.\n";
          }
        } else if (strcmp(token, "E") == 0) {
          token = strtok(NULL, " :,%");
          if (strcmp(token, "fields") == 0) {
            for (int i = 0; i < nE; ++i) gasfile >> m_eFields[i];
          }
        } else if (strcmp(token, "E-B") == 0) {
          token = strtok(NULL, " :,%");
          if (strcmp(token, "angles") == 0) {
            for (int i = 0; i < nA; ++i) gasfile >> m_bAngles[i];
          }
        } else if (strcmp(token, "B") == 0) {
          token = strtok(NULL, " :,%");
          if (strcmp(token, "fields") == 0) {
            double bstore = 0.;
            for (int i = 0; i < nB; i++) {
              // B fields are stored in hGauss (to be checked!).
              gasfile >> bstore;
              m_bFields[i] = bstore / 100.;
            }
          }
        } else if (strcmp(token, "Mixture") == 0) {
          for (int i = 0; i < nMagboltzGases; ++i) {
            gasfile >> mixture[i];
          }
        } else if (strcmp(token, "Excitation") == 0) {
          // Skip number.
          token = strtok(NULL, " :,%");
          // Get label.
          token = strtok(NULL, " :,%");
          m_excitationList[excCount].label = token;
          // Get energy.
          token = strtok(NULL, " :,%");
          m_excitationList[excCount].energy = atof(token);
          // Get Penning probability.
          token = strtok(NULL, " :,%");
          m_excitationList[excCount].prob = atof(token);
          m_excitationList[excCount].rms = 0.;
          m_excitationList[excCount].dt = 0.;
          if (version >= 11) {
            // Get Penning rms distance.
            token = strtok(NULL, " :,%");
            if (token) {
              m_excitationList[excCount].rms = atof(token);
              // Get decay time.
              token = strtok(NULL, " :,%");
              if (token) m_excitationList[excCount].dt = atof(token);
            }
          }
          // Increase counter.
          ++excCount;
        } else if (strcmp(token, "Ionisation") == 0) {
          // Skip number.
          token = strtok(NULL, " :,%");
          // Get label.
          token = strtok(NULL, " :,%");
          m_ionisationList[ionCount].label += token;
          // Get energy.
          token = strtok(NULL, " :,%");
          m_ionisationList[ionCount].energy = atof(token);
          // Increase counter.
          ++ionCount;
        }
        token = strtok(NULL, " :,%");
      }
    }
  }

  // Decode the GASOK bits.
  // GASOK(I)   : .TRUE. if present
  // (1)  electron drift velocity || E
  // (2)  ion mobility,
  // (3)  longitudinal diffusion || E
  // (4)  Townsend coefficient,
  // (5)  cluster size distribution.
  // (6)  attachment coefficient,
  // (7)  Lorentz angle,
  // (8)  transverse diffusion || ExB and Bt
  // (9)  electron drift velocity || Bt
  // (10) electron drift velocity || ExB
  // (11) diffusion tensor
  // (12) ion dissociation
  // (13) allocated for SRIM data (not used)
  // (14) allocated for HEED data (not used)
  // (15) excitation rates
  // (16) ionisation rates

  if (m_debug) {
    std::cout << m_className << "::LoadGasFile:\n";
    std::cout << "    GASOK bits: " << gasBits << "\n";
  }

  if (gasBits[0] == 'T') {
    InitTable(nE, nB, nA, m_eVelocityE, 0.);
  } else {
    m_eVelocityE.clear();
  }
  if (gasBits[1] == 'T') {
    InitTable(nE, nB, nA, m_ionMobility, 0.);
  } else {
    m_ionMobility.clear();
  }
  if (gasBits[2] == 'T') {
    InitTable(nE, nB, nA, m_eDiffLong, 0.);
  } else {
    m_eDiffLong.clear();
  }
  if (gasBits[3] == 'T') {
    InitTable(nE, nB, nA, m_eTownsend, -30.);
    InitTable(nE, nB, nA, m_eTownsendNoPenning, -30.);
  } else {
    m_eTownsend.clear();
    m_eTownsendNoPenning.clear();
  }
  // gasBits[4]: cluster size distribution; skipped
  if (gasBits[5] == 'T') {
    InitTable(nE, nB, nA, m_eAttachment, -30.);
  } else {
    m_eAttachment.clear();
  }
  if (gasBits[6] == 'T') {
    InitTable(nE, nB, nA, m_eLorentzAngle, -30.);
  } else {
    m_eLorentzAngle.clear();
  }
  if (gasBits[7] == 'T') {
    InitTable(nE, nB, nA, m_eDiffTrans, 0.);
  } else {
    m_eDiffTrans.clear();
  }
  if (gasBits[8] == 'T') {
    InitTable(nE, nB, nA, m_eVelocityB, 0.);
  } else {
    m_eVelocityB.clear();
  }
  if (gasBits[9] == 'T') {
    InitTable(nE, nB, nA, m_eVelocityExB, 0.);
  } else {
    m_eVelocityExB.clear();
  }
  if (gasBits[10] == 'T') {
    InitTensor(nE, nB, nA, 6, m_eDiffTens, 0.);
  } else {
    m_eDiffTens.clear();
  }
  if (gasBits[11] == 'T') {
    InitTable(nE, nB, nA, m_ionDissociation, -30.);
  } else {
    m_ionDissociation.clear();
  }
  // gasBits[12]: SRIM; skipped
  // gasBits[13]: HEED; skipped
  if (gasBits[14] == 'T') {
    InitTensor(nE, nB, nA, m_excitationList.size(), m_excRates, 0.);
  } else {
    m_excRates.clear();
  }
  if (gasBits[15] == 'T') {
    InitTensor(nE, nB, nA, m_ionisationList.size(), m_ionRates, 0.);
  } else {
    m_ionRates.clear();
  }

  // Check the gas mixture.
  std::vector<std::string> gasnames;
  std::vector<double> percentages;
  bool gasMixOk = true;
  unsigned int gasCount = 0;
  for (int i = 0; i < nMagboltzGases; ++i) {
    if (mixture[i] > 0.) {
      std::string gasname = "";
      if (!GetGasName(i + 1, version, gasname)) {
        std::cerr << m_className << "::LoadGasFile:\n";
        std::cerr << "    Unknown gas (gas number ";
        std::cerr << i + 1 << ")\n";
        gasMixOk = false;
        break;
      }
      gasnames.push_back(gasname);
      percentages.push_back(mixture[i]);
      ++gasCount;
    }
  }
  if (gasCount > m_nMaxGases) {
    std::cerr << m_className << "::LoadGasFile:\n";
    std::cerr << "    Gas mixture has " << gasCount << " components.\n";
    std::cerr << "    Number of gases is limited to " << m_nMaxGases << ".\n";
    gasMixOk = false;
  } else if (gasCount == 0) {
    std::cerr << m_className << "::LoadGasFile:\n";
    std::cerr << "    Gas mixture is not defined (zero components).\n";
    gasMixOk = false;
  }
  double sum = 0.;
  for (unsigned int i = 0; i < gasCount; ++i) sum += percentages[i];
  if (gasMixOk && sum != 100.) {
    std::cerr << m_className << "::LoadGasFile:\n";
    std::cerr << "    Percentages are not normalized to 100.\n";
    for (unsigned int i = 0; i < gasCount; ++i) percentages[i] *= 100. / sum;
  }

  // Force re-initialisation of collision rates etc.
  m_isChanged = true;

  if (gasMixOk) {
    m_name = "";
    m_nComponents = gasCount;
    for (unsigned int i = 0; i < m_nComponents; ++i) {
      if (i > 0) m_name += "/";
      m_name += gasnames[i];
      m_gas[i] = gasnames[i];
      m_fraction[i] = percentages[i] / 100.;
      GetGasInfo(m_gas[i], m_atWeight[i], m_atNum[i]);
    }
    std::cout << m_className << "::LoadGasFile:\n";
    std::cout << "    Gas composition set to " << m_name;
    if (m_nComponents > 1) {
      std::cout << " (" << m_fraction[0] * 100;
      for (unsigned int i = 1; i < m_nComponents; ++i) {
        std::cout << "/" << m_fraction[i] * 100;
      }
      std::cout << ")";
    }
    std::cout << "\n";
  } else {
    std::cerr << m_className << "::LoadGasFile:\n";
    std::cerr << "    Gas composition could not be established.\n";
  }

  if (m_debug) {
    std::cout << m_className << "::LoadGasFile:\n";
    std::cout << "    Reading gas tables.\n";
  }

  // Temporary variables
  // Velocities
  double ve = 0., vb = 0., vexb = 0.;
  // Lorentz angle
  double lor = 0.;
  // Diffusion coefficients
  double dl = 0., dt = 0.;
  // Towsend and attachment coefficients
  double alpha = 0., alpha0 = 0., eta = 0.;
  // Ion mobility and dissociation coefficient
  double mu = 0., diss = 0.;
  double ionDiffLong = 0., ionDiffTrans = 0.;
  double diff = 0.;
  double rate = 0.;
  double waste = 0.;

  if (m_map2d) {
    if (m_debug) {
      std::cout << m_className << "::LoadGasFile:\n";
      std::cout << "    Gas table is 3D.\n";
    }
    for (int i = 0; i < nE; i++) {
      for (int j = 0; j < nA; j++) {
        for (int k = 0; k < nB; k++) {
          // Drift velocity along E, Bt and ExB
          gasfile >> ve >> vb >> vexb;
          // Convert from cm / us to cm / ns
          ve *= 1.e-3;
          vb *= 1.e-3;
          vexb *= 1.e-3;
          if (!m_eVelocityE.empty()) m_eVelocityE[j][k][i] = ve;
          if (!m_eVelocityB.empty()) m_eVelocityB[j][k][i] = vb;
          if (!m_eVelocityExB.empty()) m_eVelocityExB[j][k][i] = vexb;
          // Longitudinal and transverse diffusion coefficient
          gasfile >> dl >> dt;
          if (!m_eDiffLong.empty()) m_eDiffLong[j][k][i] = dl;
          if (!m_eDiffTrans.empty()) m_eDiffTrans[j][k][i] = dt;
          // Townsend and attachment coefficient
          gasfile >> alpha >> alpha0 >> eta;
          if (!m_eTownsend.empty()) {
            m_eTownsend[j][k][i] = alpha;
            m_eTownsendNoPenning[j][k][i] = alpha0;
          }
          if (!m_eAttachment.empty()) {
            m_eAttachment[j][k][i] = eta;
          }
          // Ion mobility
          gasfile >> mu;
          // Convert from cm2 / (V us) to cm2 / (V ns)
          mu *= 1.e-3;
          if (!m_ionMobility.empty()) m_ionMobility[j][k][i] = mu;
          // Lorentz angle
          gasfile >> lor;
          if (!m_eLorentzAngle.empty()) m_eLorentzAngle[j][k][i] = lor;
          // Ion dissociation
          gasfile >> diss;
          if (!m_ionDissociation.empty()) m_ionDissociation[j][k][i] = diss;
          // Diffusion tensor
          for (int l = 0; l < 6; l++) {
            gasfile >> diff;
            if (!m_eDiffTens.empty()) m_eDiffTens[l][j][k][i] = diff;
          }
          // Excitation rates
          const unsigned int nexc = m_excitationList.size();
          for (unsigned int l = 0; l < nexc; ++l) {
            gasfile >> rate;
            if (!m_excRates.empty()) m_excRates[l][j][k][i] = rate;
          }
          // Ionization rates
          const unsigned int nion = m_ionisationList.size();
          for (unsigned int l = 0; l < nion; ++l) {
            gasfile >> rate;
            if (!m_ionRates.empty()) m_ionRates[l][j][k][i] = rate;
          }
        }
      }
    }
  } else {
    if (m_debug) {
      std::cout << m_className << "::LoadGasFile:\n";
      std::cout << "    Gas table is 1D.\n";
    }
    for (int i = 0; i < nE; i++) {
      if (m_debug) std::cout << "    Done table: " << i << "\n";
      // Drift velocity along E, Bt, ExB
      gasfile >> ve >> waste >> vb >> waste >> vexb >> waste;
      ve *= 1.e-3;
      vb *= 1.e-3;
      vexb *= 1.e-3;
      if (!m_eVelocityE.empty()) m_eVelocityE[0][0][i] = ve;
      if (!m_eVelocityB.empty()) m_eVelocityB[0][0][i] = vb;
      if (!m_eVelocityExB.empty()) m_eVelocityExB[0][0][i] = vexb;
      // Longitudinal and transferse diffusion coefficients
      gasfile >> dl >> waste >> dt >> waste;
      if (!m_eDiffLong.empty()) m_eDiffLong[0][0][i] = dl;
      if (!m_eDiffTrans.empty()) m_eDiffTrans[0][0][i] = dt;
      // Townsend and attachment coefficients
      gasfile >> alpha >> waste >> alpha0 >> eta >> waste;
      if (!m_eTownsend.empty()) {
        m_eTownsend[0][0][i] = alpha;
        m_eTownsendNoPenning[0][0][i] = alpha0;
      }
      if (!m_eAttachment.empty()) {
        m_eAttachment[0][0][i] = eta;
      }
      // Ion mobility
      gasfile >> mu >> waste;
      mu *= 1.e-3;
      if (!m_ionMobility.empty()) m_ionMobility[0][0][i] = mu;
      // Lorentz angle
      gasfile >> lor >> waste;
      if (!m_eLorentzAngle.empty()) m_eLorentzAngle[0][0][i] = lor;
      // Ion dissociation
      gasfile >> diss >> waste;
      if (!m_ionDissociation.empty()) m_ionDissociation[0][0][i] = diss;
      // Diffusion tensor
      for (int j = 0; j < 6; j++) {
        gasfile >> diff >> waste;
        if (!m_eDiffTens.empty()) m_eDiffTens[j][0][0][i] = diff;
      }
      // Excitation rates
      const unsigned int nexc = m_excitationList.size();
      for (unsigned int j = 0; j < nexc; ++j) {
        gasfile >> rate >> waste;
        if (!m_excRates.empty()) m_excRates[j][0][0][i] = rate;
      }
      // Ionization rates
      const unsigned int nion = m_ionisationList.size();
      for (unsigned int j = 0; j < nion; ++j) {
        gasfile >> rate >> waste;
        if (!m_ionRates.empty()) m_ionRates[j][0][0][i] = rate;
      }
    }
  }
  if (m_debug) std::cout << "    Done with gas tables.\n";

  // Extrapolation methods
  std::array<unsigned int, 13> hExtrap = {{0}};
  std::array<unsigned int, 13> lExtrap = {{1}};
  // Interpolation methods
  std::array<unsigned int, 13> interpMeth = {{2}};

  // Moving on to the file footer
  bool done = false;
  while (!done) {
    gasfile.getline(line, 256);
    token = strtok(line, " :,%=\t");
    while (token != NULL) {
      if (strcmp(token, "H") == 0) {
        token = strtok(NULL, " :,%=\t");
        for (int i = 0; i < 13; i++) {
          token = strtok(NULL, " :,%=\t");
          if (token != NULL) hExtrap[i] = atoi(token);
        }
      } else if (strcmp(token, "L") == 0) {
        token = strtok(NULL, " :,%=\t");
        for (int i = 0; i < 13; i++) {
          token = strtok(NULL, " :,%=\t");
          if (token != NULL) lExtrap[i] = atoi(token);
        }
      } else if (strcmp(token, "Thresholds") == 0) {
        token = strtok(NULL, " :,%=\t");
        if (token != NULL) thrElectronTownsend = atoi(token);
        token = strtok(NULL, " :,%=\t");
        if (token != NULL) thrElectronAttachment = atoi(token);
        token = strtok(NULL, " :,%=\t");
        if (token != NULL) thrIonDissociation = atoi(token);
      } else if (strcmp(token, "Interp") == 0) {
        for (int i = 0; i < 13; i++) {
          token = strtok(NULL, " :,%=\t");
          if (token != NULL) interpMeth[i] = atoi(token);
        }
      } else if (strcmp(token, "A") == 0) {
        token = strtok(NULL, " :,%=\t");
        // Parameter for energy loss distribution, currently not used
        // double a;
        // if (token != NULL) a = atof(token);
      } else if (strcmp(token, "Z") == 0) {
        // Parameter for energy loss distribution, currently not used
        token = strtok(NULL, " :,%=\t");
        // double z;
        // if (token != NULL) z = atof(token);
      } else if (strcmp(token, "EMPROB") == 0) {
        // Parameter for energy loss distribution, currently not used
        token = strtok(NULL, " :,%=\t");
        // double emprob;
        // if (token != NULL) emprob = atof(token);
      } else if (strcmp(token, "EPAIR") == 0) {
        // Parameter for energy loss distribution, currently not used
        token = strtok(NULL, " :,%=\t");
        // double epair;
        // if (token != NULL) epair = atof(token);
      } else if (strcmp(token, "Ion") == 0) {
        // Ion diffusion coefficients
        token = strtok(NULL, " :,%=\t");
        token = strtok(NULL, " :,%=\t");
        if (token != NULL) ionDiffLong = atof(token);
        token = strtok(NULL, " :,%=\t");
        if (token != NULL) ionDiffTrans = atof(token);
      } else if (strcmp(token, "CMEAN") == 0) {
        // Cluster parameter, currently not used
        token = strtok(NULL, " :,%=\t");
        // double clsPerCm;
        // if (token != NULL) clsPerCm = atof(token);
      } else if (strcmp(token, "RHO") == 0) {
        // Parameter for energy loss distribution, currently not used
        token = strtok(NULL, " :,%=\t");
        // double rho;
        // if (token != NULL) rho = atof(token);
      } else if (strcmp(token, "PGAS") == 0) {
        token = strtok(NULL, " :,%=\t");
        double pTorr = 760.;
        if (token != NULL) pTorr = atof(token);
        if (pTorr > 0.) m_pressure = pTorr;
      } else if (strcmp(token, "TGAS") == 0) {
        token = strtok(NULL, " :,%=\t");
        double tKelvin = 293.15;
        if (token != NULL) tKelvin = atof(token);
        if (tKelvin > 0.) m_temperature = tKelvin;
        done = true;
        break;
      } else {
        done = true;
        break;
      }
      token = strtok(NULL, " :,%=\t");
    }
  }

  gasfile.close();

  // Set the reference pressure and temperature.
  m_pressureTable = m_pressure;
  m_temperatureTable = m_temperature;

  // Multiply the E/p values by the pressure.
  for (int i = nE; i--;) {
    m_eFields[i] *= m_pressureTable;
  }
  // Scale the parameters.
  const double sqrtPressure = sqrt(m_pressureTable);
  const double logPressure = log(m_pressureTable);
  for (int i = nE; i--;) {
    for (int j = nA; j--;) {
      for (int k = nB; k--;) {
        if (!m_eDiffLong.empty()) {
          m_eDiffLong[j][k][i] /= sqrtPressure;
        }
        if (!m_eDiffTrans.empty()) {
          m_eDiffTrans[j][k][i] /= sqrtPressure;
        }
        if (!m_eDiffTens.empty()) {
          for (int l = 6; l--;) {
            m_eDiffTens[l][j][k][i] /= m_pressureTable;
          }
        }
        if (!m_eTownsend.empty()) {
          m_eTownsend[j][k][i] += logPressure;
        }
        if (!m_eAttachment.empty()) {
          m_eAttachment[j][k][i] += logPressure;
        }
        if (!m_ionDissociation.empty()) {
          m_ionDissociation[j][k][i] += logPressure;
        }
      }
    }
  }

  // Decode the extrapolation and interpolation tables.
  m_extrVel = {lExtrap[0], hExtrap[0]};
  m_intpVel = interpMeth[0];
  // Indices 1 and 2 correspond to velocities along Bt and ExB.
  m_extrDiff = {lExtrap[3], hExtrap[3]};
  m_intpDiff = interpMeth[3];
  m_extrTownsend = {lExtrap[4], hExtrap[4]};
  m_intpTownsend = interpMeth[4];
  m_extrAttachment = {lExtrap[5], hExtrap[5]};
  m_intpAttachment = interpMeth[5];
  m_extrMobility = {lExtrap[6], hExtrap[6]};
  m_intpMobility = interpMeth[6];
  m_extrLorentzAngle = {lExtrap[7], hExtrap[7]};
  m_intpLorentzAngle = interpMeth[7];
  // Index 8: transv. diff.
  m_extrDissociation = {lExtrap[9], hExtrap[9]};
  m_intpDissociation = interpMeth[9];
  // Index 10: diff. tensor
  m_extrExcRates = {lExtrap[11], hExtrap[11]};
  m_intpExcRates = interpMeth[11];
  m_extrIonRates = {lExtrap[12], hExtrap[12]};
  m_intpIonRates = interpMeth[12];

  // Ion diffusion
  if (ionDiffLong > 0.) {
    InitTable(nE, nB, nA, m_ionDiffLong, ionDiffLong);
  } else {
    m_ionDiffLong.clear();
  }
  if (ionDiffTrans > 0.) {
    InitTable(nE, nB, nA, m_ionDiffTrans, ionDiffTrans);
  } else {
    m_ionDiffTrans.clear();
  }

  if (m_debug) {
    std::cout << m_className << "::LoadGasFile:\n";
    std::cout << "    Gas file sucessfully read.\n";
  }

  return true;
}

bool MediumGas::WriteGasFile(const std::string& filename) {

  const int nMagboltzGases = 60;
  std::vector<double> mixture(nMagboltzGases);
  for (int i = nMagboltzGases; i--;) mixture[i] = 0.;
  // Set the gas mixture.
  for (unsigned int i = 0; i < m_nComponents; ++i) {
    int ng = 0;
    if (!GetGasNumberGasFile(m_gas[i], ng)) {
      std::cerr << m_className << "::WriteGasFile:\n";
      std::cerr << "    Error retrieving gas number for gas " << m_gas[i]
                << ".\n";
    } else {
      mixture[ng - 1] = m_fraction[i] * 100.;
    }
  }

  const unsigned int nE = m_eFields.size();
  const unsigned int nB = m_bFields.size();
  const unsigned int nA = m_bAngles.size();

  if (m_debug) {
    std::cout << m_className << "::WriteGasFile:\n";
    std::cout << "    Writing gas tables to file: " << filename << "\n";
  }

  std::ofstream outFile;
  outFile.open(filename.c_str(), std::ios::out);
  if (!outFile.is_open()) {
    std::cerr << m_className << "::WriteGasFile:\n";
    std::cerr << "    File could not be opened.\n";
    outFile.close();
    return false;
  }

  // Assemble the GASOK bits.
  std::string gasBits = "FFFFFFFFFFFFFFFFFFFF";
  if (!m_eVelocityE.empty()) gasBits[0] = 'T';
  if (!m_ionMobility.empty()) gasBits[1] = 'T';
  if (!m_eDiffLong.empty()) gasBits[2] = 'T';
  if (!m_eTownsend.empty()) gasBits[3] = 'T';
  // Cluster size distribution; skipped
  if (!m_eAttachment.empty()) gasBits[5] = 'T';
  if (!m_eLorentzAngle.empty()) gasBits[6] = 'T';
  if (!m_eDiffTrans.empty()) gasBits[7] = 'T';
  if (!m_eVelocityB.empty()) gasBits[8] = 'T';
  if (!m_eVelocityExB.empty()) gasBits[9] = 'T';
  if (!m_eDiffTens.empty()) gasBits[10] = 'T';
  if (!m_ionDissociation.empty()) gasBits[11] = 'T';
  // SRIM, HEED; skipped
  if (!m_excRates.empty()) gasBits[14] = 'T';
  if (!m_ionRates.empty()) gasBits[15] = 'T';

  // Get the current time.
  time_t rawtime = time(0);
  tm timeinfo = *localtime(&rawtime);
  char datebuf[80] = {0};
  char timebuf[80] = {0};
  // Format date and time.
  strftime(datebuf, sizeof(datebuf) - 1, "%d/%m/%y", &timeinfo);
  strftime(timebuf, sizeof(timebuf) - 1, "%H.%M.%S", &timeinfo);
  // Set the member name.
  std::string member = "< none >";
  // Write the header.
  outFile << "*----.----1----.----2----.----3----.----4----.----"
          << "5----.----6----.----7----.----8----.----9----.---"
          << "10----.---11----.---12----.---13--\n";
  outFile << "% Created " << datebuf << " at " << timebuf << " ";
  outFile << member << " GAS      ";
  // Add remark.
  std::string buffer;
  buffer = std::string(25, ' ');
  outFile << "\"none" << buffer << "\"\n";
  const int version = 12;
  outFile << " Version   : " << version << "\n";
  outFile << " GASOK bits: " << gasBits << "\n";
  std::stringstream idStream;
  idStream.str("");
  idStream << m_name << ", p = " << m_pressureTable / AtmosphericPressure
           << " atm, T = " << m_temperatureTable << " K";
  std::string idString = idStream.str();
  outFile << " Identifier: " << std::setw(80) << std::left << idString << "\n";
  outFile << std::right;
  buffer = std::string(80, ' ');
  outFile << " Clusters  : " << buffer << "\n";
  outFile << " Dimension : ";
  if (m_map2d) {
    outFile << "T ";
  } else {
    outFile << "F ";
  }
  outFile << std::setw(9) << nE << " " << std::setw(9) << nA << " "
          << std::setw(9) << nB << " " 
          << std::setw(9) << m_excitationList.size() << " " 
          << std::setw(9) << m_ionisationList.size() << "\n";
  outFile << " E fields   \n";
  outFile << std::scientific << std::setw(15) << std::setprecision(8);
  for (unsigned int i = 0; i < nE; ++i) {
    // List 5 values, then new line.
    outFile << std::setw(15) << m_eFields[i] / m_pressure;
    if ((i + 1) % 5 == 0) outFile << "\n";
  }
  if (nE % 5 != 0) outFile << "\n";
  outFile << " E-B angles \n";
  for (unsigned int i = 0; i < nA; ++i) {
    // List 5 values, then new line.
    outFile << std::setw(15) << m_bAngles[i];
    if ((i + 1) % 5 == 0) outFile << "\n";
  }
  if (nA % 5 != 0) outFile << "\n";
  outFile << " B fields   \n";
  for (unsigned int i = 0; i < nB; ++i) {
    // List 5 values, then new line.
    // B fields are stored in hGauss (to be checked!).
    outFile << std::setw(15) << m_bFields[i] * 100.;
    if ((i + 1) % 5 == 0) outFile << "\n";
  }
  if (nB % 5 != 0) outFile << "\n";
  outFile << " Mixture:   \n";
  for (int i = 0; i < nMagboltzGases; i++) {
    // List 5 values, then new line.
    outFile << std::setw(15) << mixture[i];
    if ((i + 1) % 5 == 0) outFile << "\n";
  }
  if (nMagboltzGases % 5 != 0) outFile << "\n";
  const int nexc = m_excitationList.size();
  for (int i = 0; i < nexc; ++i) {
    outFile << " Excitation " << std::setw(5) << i + 1 << ": " << std::setw(45)
            << std::left << m_excitationList[i].label << "  " << std::setw(15)
            << std::right << m_excitationList[i].energy << std::setw(15)
            << m_excitationList[i].prob << std::setw(15) << m_excitationList[i].rms
            << std::setw(15) << m_excitationList[i].dt << "\n";
  }
  const int nion = m_ionisationList.size();
  for (int i = 0; i < nion; ++i) {
    outFile << " Ionisation " << std::setw(5) << i + 1 << ": " << std::setw(45)
            << std::left << m_ionisationList[i].label << "  " << std::setw(15)
            << std::right << m_ionisationList[i].energy << "\n";
  }

  const double sqrtPressure = sqrt(m_pressureTable);
  const double logPressure = log(m_pressureTable);

  outFile << " The gas tables follow:\n";
  int cnt = 0;
  for (unsigned int i = 0; i < nE; ++i) {
    for (unsigned int j = 0; j < nA; ++j) {
      for (unsigned int k = 0; k < nB; ++k) {
        double ve = 0., vb = 0., vexb = 0.;
        if (!m_eVelocityE.empty()) ve = m_eVelocityE[j][k][i];
        if (!m_eVelocityB.empty()) vb = m_eVelocityB[j][k][i];
        if (!m_eVelocityExB.empty()) vexb = m_eVelocityExB[j][k][i];
        // Convert from cm / ns to cm / us.
        ve *= 1.e3;
        vb *= 1.e3;
        vexb *= 1.e3;
        double dl = 0., dt = 0.;
        if (!m_eDiffLong.empty()) dl = m_eDiffLong[j][k][i];
        if (!m_eDiffTrans.empty()) dt = m_eDiffTrans[j][k][i];
        dl *= sqrtPressure;
        dt *= sqrtPressure;
        double alpha = -30., alpha0 = -30., eta = -30.;
        if (!m_eTownsend.empty()) {
          alpha = m_eTownsend[j][k][i];
          alpha0 = m_eTownsendNoPenning[j][k][i];
          alpha -= logPressure;
          alpha0 -= logPressure;
        }
        if (!m_eAttachment.empty()) {
          eta = m_eAttachment[j][k][i];
          eta -= logPressure;
        }
        // Ion mobility
        double mu = 0.;
        if (!m_ionMobility.empty()) mu = m_ionMobility[j][k][i];
        // Convert from cm2 / (V ns) to cm2 / (V us).
        mu *= 1.e3;
        // Lorentz angle
        double lor = 0.;
        if (!m_eLorentzAngle.empty()) lor = m_eLorentzAngle[j][k][i];
        // Dissociation coefficient
        double diss = -30.;
        if (!m_ionDissociation.empty()) {
          diss = m_ionDissociation[j][k][i];
          diss -= logPressure;
        }
        // Set spline coefficient to dummy value.
        const double spl = 0.;
        // Write the values to file.
        outFile << std::setw(15);
        outFile << ve;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << vb;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << vexb;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << dl;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << dt;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << alpha;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << alpha0;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        outFile << std::setw(15);
        outFile << eta;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        outFile << std::setw(15);
        if (!m_map2d) {
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
          outFile << std::setw(15);
        }
        outFile << mu;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << lor;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        outFile << diss;
        ++cnt;
        if (cnt % 8 == 0) outFile << "\n";
        if (!m_map2d) {
          outFile << std::setw(15);
          outFile << spl;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
        }
        outFile << std::setw(15);
        for (int l = 0; l < 6; ++l) {
          double diff = 0.;
          if (!m_eDiffTens.empty()) {
            diff = m_eDiffTens[l][j][k][i];
            diff *= m_pressureTable;
          }
          outFile << std::setw(15);
          outFile << diff;
          ++cnt;
          if (cnt % 8 == 0) outFile << "\n";
          if (!m_map2d) {
            outFile << std::setw(15) << spl;
            ++cnt;
            if (cnt % 8 == 0) outFile << "\n";
          }
        }
        if (!m_excRates.empty() && !m_excitationList.empty()) {
          for (int l = 0; l < nexc; ++l) {
            outFile << std::setw(15);
            outFile << m_excRates[l][j][k][i];
            ++cnt;
            if (cnt % 8 == 0) outFile << "\n";
            if (!m_map2d) {
              outFile << std::setw(15) << spl;
              ++cnt;
              if (cnt % 8 == 0) outFile << "\n";
            }
          }
        }
        if (!m_ionRates.empty() && !m_ionisationList.empty()) {
          for (int l = 0; l < nion; ++l) {
            outFile << std::setw(15);
            outFile << m_ionRates[l][j][k][i];
            ++cnt;
            if (cnt % 8 == 0) outFile << "\n";
            if (!m_map2d) {
              outFile << std::setw(15) << spl;
              ++cnt;
              if (cnt % 8 == 0) outFile << "\n";
            }
          }
        }
      }
    }
    if (cnt % 8 != 0) outFile << "\n";
    cnt = 0;
  }

  // Extrapolation methods
  int hExtrap[13], lExtrap[13];
  int interpMeth[13];

  lExtrap[0] = lExtrap[1] = lExtrap[2] = m_extrVel.first;
  hExtrap[0] = hExtrap[1] = hExtrap[2] = m_extrVel.second;
  interpMeth[0] = interpMeth[1] = interpMeth[2] = m_intpVel;
  lExtrap[3] = lExtrap[8] = lExtrap[10] = m_extrDiff.first;
  hExtrap[3] = hExtrap[8] = hExtrap[10] = m_extrDiff.second;
  interpMeth[3] = interpMeth[8] = interpMeth[10] = m_intpDiff;
  lExtrap[4] = m_extrTownsend.first;
  hExtrap[4] = m_extrTownsend.second;
  interpMeth[4] = m_intpTownsend;
  lExtrap[5] = m_extrAttachment.first;
  hExtrap[5] = m_extrAttachment.second;
  interpMeth[5] = m_intpAttachment;
  lExtrap[6] = m_extrMobility.first;
  hExtrap[6] = m_extrMobility.second;
  interpMeth[6] = m_intpMobility;
  // Lorentz angle
  lExtrap[7] = m_extrLorentzAngle.first;
  hExtrap[7] = m_extrLorentzAngle.second;
  interpMeth[7] = m_intpLorentzAngle;
  lExtrap[9] = m_extrDissociation.first;
  hExtrap[9] = m_extrDissociation.second;
  interpMeth[9] = m_intpDissociation;
  lExtrap[11] = m_extrExcRates.first;
  hExtrap[11] = m_extrExcRates.second;
  interpMeth[11] = m_intpExcRates;
  lExtrap[12] = m_extrIonRates.first;
  hExtrap[12] = m_extrIonRates.second;
  interpMeth[12] = m_intpIonRates;

  outFile << " H Extr: ";
  for (int i = 0; i < 13; i++) {
    outFile << std::setw(5) << hExtrap[i];
  }
  outFile << "\n";
  outFile << " L Extr: ";
  for (int i = 0; i < 13; i++) {
    outFile << std::setw(5) << lExtrap[i];
  }
  outFile << "\n";
  outFile << " Thresholds: " << std::setw(10) << thrElectronTownsend
          << std::setw(10) << thrElectronAttachment << std::setw(10)
          << thrIonDissociation << "\n";
  outFile << " Interp: ";
  for (int i = 0; i < 13; i++) {
    outFile << std::setw(5) << interpMeth[i];
  }
  outFile << "\n";
  outFile << " A     =" << std::setw(15) << 0. << ","
          << " Z     =" << std::setw(15) << 0. << ","
          << " EMPROB=" << std::setw(15) << 0. << ","
          << " EPAIR =" << std::setw(15) << 0. << "\n";
  double ionDiffLong = 0., ionDiffTrans = 0.;
  if (!m_ionDiffLong.empty()) ionDiffLong = m_ionDiffLong[0][0][0];
  if (!m_ionDiffTrans.empty()) ionDiffTrans = m_ionDiffTrans[0][0][0];
  outFile << " Ion diffusion: " << std::setw(15) << ionDiffLong << std::setw(15)
          << ionDiffTrans << "\n";
  outFile << " CMEAN =" << std::setw(15) << 0. << ","
          << " RHO   =" << std::setw(15) << 0. << ","
          << " PGAS  =" << std::setw(15) << m_pressureTable << ","
          << " TGAS  =" << std::setw(15) << m_temperatureTable << "\n";
  outFile << " CLSTYP    : NOT SET   \n";
  buffer = std::string(80, ' ');
  outFile << " FCNCLS    : " << buffer << "\n";
  outFile << " NCLS      : " << std::setw(10) << 0 << "\n";
  outFile << " Average   : " << std::setw(25) << std::setprecision(18) << 0.
          << "\n";
  outFile << "  Heed initialisation done: F\n";
  outFile << "  SRIM initialisation done: F\n";
  outFile.close();

  return true;
}

void MediumGas::PrintGas() {

  // Print a summary.
  std::cout << m_className << "::PrintGas:\n";
  std::cout << "    Gas composition: " << m_name;
  if (m_nComponents > 1) {
    std::cout << " (" << m_fraction[0] * 100;
    for (unsigned int i = 1; i < m_nComponents; ++i) {
      std::cout << "/" << m_fraction[i] * 100;
    }
    std::cout << ")";
  }
  std::cout << "\n";
  std::cout << "    Pressure:    " << m_pressure << " Torr\n";
  std::cout << "    Temperature: " << m_temperature << " K\n";
  std::cout << "    Gas file:\n";
  std::cout << "      Pressure:    " << m_pressureTable << " Torr\n";
  std::cout << "      Temperature: " << m_temperatureTable << " K\n";
  if (m_eFields.size() > 1) {
    std::cout << "    Electric field range:  " << m_eFields[0] << " - "
              << m_eFields.back() << " V/cm in " << m_eFields.size() - 1
              << " steps.\n";
  } else if (m_eFields.size() == 1) {
    std::cout << "    Electric field:        " << m_eFields[0] << " V/cm\n";
  } else {
    std::cout << "    Electric field range: not set\n";
  }
  if (m_bFields.size() > 1) {
    std::cout << "    Magnetic field range:  " << m_bFields[0] << " - "
              << m_bFields.back() << " T in " << m_bFields.size() - 1
              << " steps.\n";
  } else if (m_bFields.size() == 1) {
    std::cout << "    Magnetic field:        " << m_bFields[0] << "\n";
  } else {
    std::cout << "    Magnetic field range: not set\n";
  }
  if (m_bAngles.size() > 1) {
    std::cout << "    Angular range:         " << m_bAngles[0] << " - "
              << m_bAngles.back() << " in " << m_bAngles.size() - 1 
              << " steps.\n";
  } else if (m_bAngles.size() == 1) {
    std::cout << "    Angle between E and B: " << m_bAngles[0] << "\n";
  } else {
    std::cout << "    Angular range: not set\n";
  }

  std::cout << "    Available electron transport data:\n";
  if (!m_eVelocityE.empty()) {
    std::cout << "      Velocity along E\n";
  }
  if (!m_eVelocityB.empty()) {
    std::cout << "      Velocity along Bt\n";
  }
  if (!m_eVelocityExB.empty()) {
    std::cout << "      Velocity along ExB\n";
  }
  if (!m_eVelocityE.empty() || !m_eVelocityB.empty() || !m_eVelocityExB.empty()) {
    PrintExtrapolation(m_extrVel);
    std::cout << "        Interpolation order: " << m_intpVel << "\n";
  }
  if (!m_eDiffLong.empty()) {
    std::cout << "      Longitudinal diffusion coefficient\n";
  }
  if (!m_eDiffTrans.empty()) {
    std::cout << "      Transverse diffusion coefficient\n";
  }
  if (!m_eDiffTens.empty()) {
    std::cout << "      Diffusion tensor\n";
  }
  if (!m_eDiffLong.empty() || !m_eDiffTrans.empty() || !m_eDiffTens.empty()) {
    PrintExtrapolation(m_extrDiff);
    std::cout << "        Interpolation order: " << m_intpDiff << "\n";
  }
  if (!m_eTownsend.empty()) {
    std::cout << "      Townsend coefficient\n";
    PrintExtrapolation(m_extrTownsend);
    std::cout << "        Interpolation order: " << m_intpTownsend << "\n";
  }
  if (!m_eAttachment.empty()) {
    std::cout << "      Attachment coefficient\n";
    PrintExtrapolation(m_extrAttachment);
    std::cout << "        Interpolation order: " << m_intpAttachment << "\n";
  }
  if (!m_eLorentzAngle.empty()) {
    std::cout << "      Lorentz Angle\n";
    PrintExtrapolation(m_extrLorentzAngle);
    std::cout << "        Interpolation order: " << m_intpLorentzAngle << "\n";
  }
  if (!m_excRates.empty()) {
    std::cout << "      Excitation rates\n";
    PrintExtrapolation(m_extrExcRates);
    std::cout << "        Interpolation order: " << m_intpExcRates << "\n";
  }
  if (!m_ionRates.empty()) {
    std::cout << "      Ionisation rates\n";
    PrintExtrapolation(m_extrIonRates);
    std::cout << "        Interpolation order: " << m_intpIonRates << "\n";
  }
  if (m_eVelocityE.empty() && m_eVelocityB.empty() && m_eVelocityExB.empty() &&
      m_eDiffLong.empty() && m_eDiffTrans.empty() && m_eDiffTens.empty() && 
      m_eTownsend.empty() && m_eAttachment.empty() && 
      m_excRates.empty() && m_ionRates.empty() && m_eLorentzAngle.empty()) {
    std::cout << "      none\n";
  }

  std::cout << "    Available ion transport data:\n";
  if (!m_ionMobility.empty()) {
    std::cout << "      Mobility\n";
    PrintExtrapolation(m_extrMobility);
    std::cout << "        Interpolation order: " << m_intpMobility << "\n";
  }
  if (!m_ionDiffLong.empty()) {
    std::cout << "      Longitudinal diffusion coefficient\n";
  }
  if (!m_ionDiffTrans.empty()) {
    std::cout << "      Transverse diffusion coefficient\n";
  }
  if (!m_ionDiffLong.empty() || !m_ionDiffTrans.empty()) {
    PrintExtrapolation(m_extrDiff);
    std::cout << "        Interpolation order: " << m_intpDiff << "\n";
  }
  if (!m_ionDissociation.empty()) {
    std::cout << "      Dissociation coefficient\n";
    PrintExtrapolation(m_extrDissociation);
    std::cout << "        Interpolation order: " << m_intpDissociation << "\n";
  }
  if (m_ionMobility.empty() && m_ionDiffLong.empty() && m_ionDiffTrans.empty() &&
      m_ionDissociation.empty()) {
    std::cout << "      none\n";
  }
}

bool MediumGas::LoadIonMobility(const std::string& filename) {

  // Open the file.
  std::ifstream infile;
  infile.open(filename.c_str(), std::ios::in);
  // Make sure the file could actually be opened.
  if (!infile) {
    std::cerr << m_className << "::LoadIonMobility:\n"
              << "    Error opening file " << filename << ".\n";;
    return false;
  }

  double field = -1., mu = -1.;
  double lastField = field;
  std::vector<double> efields;
  std::vector<double> mobilities;

  // Read the file line by line.
  char line[100];

  int i = 0;
  while (!infile.eof()) {
    ++i;
    // Read the next line.
    infile.getline(line, 100);

    char* token = NULL;
    token = strtok(line, " ,\t");
    if (!token) {
      break;
    } else if (strcmp(token, "#") == 0 || strcmp(token, "*") == 0 ||
               strcmp(token, "//") == 0) {
      continue;
    } else {
      field = atof(token);
      token = strtok(NULL, " ,\t");
      if (!token) {
        std::cerr << m_className << "::LoadIonMobility:\n"
                  << "    Found E/N but no mobility before the end-of-line.\n"
                  << "    Skipping line " << i << ".\n";
        continue;
      }
      mu = atof(token);
    }
    token = strtok(NULL, " ,\t");
    if (token && strcmp(token, "//") != 0) {
      std::cerr << m_className << "::LoadIonMobility:\n"
                << "    Unexpected non-comment characters after the mobility.\n"
                << "    Skipping line " << i << ".\n";
      continue;
    }
    if (m_debug) {
      std::cout << "    E/N = " << field << " Td: mu = " << mu << " cm2/(Vs)\n";
    }
    // Check if the data has been read correctly.
    if (infile.fail() && !infile.eof()) {
      std::cerr << m_className << "::LoadIonMobility:\n";
      std::cerr << "    Error reading file\n";
      std::cerr << "    " << filename << " (line " << i << ").\n";
      return false;
    }
    // Make sure the values make sense.
    // Negative field values are not allowed.
    if (field < 0.) {
      std::cerr << m_className << "::LoadIonMobility:\n";
      std::cerr << "    Negative electric field (line " << i << ").\n";
      return false;
    }
    // The table has to be in ascending order.
    if (field <= lastField) {
      std::cerr << m_className << "::LoadIonMobility:\n";
      std::cerr << "    Table is not in ascending order (line " << i << ").\n";
      return false;
    }
    // Add the values to the list.
    efields.push_back(field);
    mobilities.push_back(mu);
    lastField = field;
  }

  const int ne = efields.size();
  if (ne <= 0) {
    std::cerr << m_className << "::LoadIonMobilities:\n";
    std::cerr << "    No valid data found.\n";
    return false;
  }

  // The E/N values in the file are supposed to be in Td (10^-17 V cm2).
  const double scaleField = 1.e-17 * GetNumberDensity();
  // The reduced mobilities in the file are supposed to be in cm2/(V s).
  const double scaleMobility =
      1.e-9 * (AtmosphericPressure / m_pressure) * (m_temperature / ZeroCelsius);
  for (int j = ne; j--;) {
    // Scale the fields and mobilities.
    efields[j] *= scaleField;
    mobilities[j] *= scaleMobility;
  }

  std::cout << m_className << "::LoadIonMobility:\n";
  std::cout << "    Read " << ne << " values from file " << filename << "\n";

  return SetIonMobility(efields, mobilities);
}

void MediumGas::SetExtrapolationMethodExcitationRates(
    const std::string& low, const std::string& high) {

  SetExtrapolationMethod(low, high, m_extrExcRates, "ExcitationRates");
}

void MediumGas::SetExtrapolationMethodIonisationRates(
    const std::string& low, const std::string& high) {

  SetExtrapolationMethod(low, high, m_extrIonRates, "IonisationRates");
}

void MediumGas::SetInterpolationMethodExcitationRates(const int intrp) {

  if (intrp > 0) m_intpExcRates = intrp;
}

void MediumGas::SetInterpolationMethodIonisationRates(const int intrp) {

  if (intrp > 0) m_intpIonRates = intrp;
}

bool MediumGas::GetGasInfo(const std::string& gasname, double& a,
                           double& z) const {

  if (gasname == "CF4") {
    a = 12.0107 + 4 * 18.9984032;
    z = 6 + 4 * 9;
    return true;
  } else if (gasname == "Ar") {
    a = 39.948;
    z = 18;
  } else if (gasname == "He") {
    a = 4.002602;
    z = 2;
  } else if (gasname == "He-3") {
    a = 3.01602931914;
    z = 2;
  } else if (gasname == "Ne") {
    a = 20.1797;
    z = 10;
  } else if (gasname == "Kr") {
    a = 37.798;
    z = 36;
  } else if (gasname == "Xe") {
    a = 131.293;
    z = 54;
  } else if (gasname == "CH4") {
    a = 12.0107 + 4 * 1.00794;
    z = 6 + 4;
  } else if (gasname == "C2H6") {
    a = 2 * 12.0107 + 6 * 1.00794;
    z = 2 * 6 + 6;
  } else if (gasname == "C3H8") {
    a = 3 * 12.0107 + 8 * 1.00794;
    z = 3 * 6 + 8;
  } else if (gasname == "iC4H10") {
    a = 4 * 12.0107 + 10 * 1.00794;
    z = 4 * 6 + 10;
  } else if (gasname == "CO2") {
    a = 12.0107 + 2 * 15.9994;
    z = 6 + 2 * 8;
  } else if (gasname == "neoC5H12") {
    a = 5 * 12.0107 + 12 * 1.00794;
    z = 5 * 6 + 12;
  } else if (gasname == "H2O") {
    a = 2 * 1.00794 + 15.9994;
    z = 2 + 8;
  } else if (gasname == "O2") {
    a = 2 * 15.9994;
    z = 2 * 8;
  } else if (gasname == "N2") {
    a = 2 * 14.0067;
    z = 2 * 7;
  } else if (gasname == "NO") {
    a = 14.0067 + 15.9994;
    z = 7 + 8;
  } else if (gasname == "N2O") {
    a = 2 * 14.0067 + 15.9994;
    z = 2 * 7 + 8;
  } else if (gasname == "C2H4") {
    a = 2 * 12.0107 + 4 * 1.00794;
    z = 2 * 6 + 4;
  } else if (gasname == "C2H2") {
    a = 2 * 12.0107 + 2 * 1.00794;
    z = 2 * 6 + 2;
  } else if (gasname == "H2") {
    a = 2 * 1.00794;
    z = 2;
  } else if (gasname == "D2") {
    a = 2 * 2.01410177785;
    z = 2;
  } else if (gasname == "CO") {
    a = 12.0107 + 15.9994;
    z = 6 + 8;
  } else if (gasname == "Methylal") {
    a = 3 * 12.0107 + 8 * 1.00794 + 2 * 15.9994;
    z = 3 * 6 + 8 + 2 * 8;
  } else if (gasname == "DME") {
    a = 4 * 12.0107 + 10 * 1.00794 + 2 * 15.9994;
    z = 4 * 6 + 10 + 2 * 8;
  } else if (gasname == "Reid-Step" || gasname == "Mawell-Model" ||
             gasname == "Reid-Ramp") {
    a = 1.;
    z = 1.;
  } else if (gasname == "C2F6") {
    a = 2 * 12.0107 + 6 * 18.9984032;
    z = 2 * 6 + 6 * 9;
  } else if (gasname == "SF6") {
    a = 32.065 + 6 * 18.9984032;
    z = 16 + 6 * 9;
  } else if (gasname == "NH3") {
    a = 14.0067 + 3 * 1.00794;
    z = 7 + 3;
  } else if (gasname == "C3H6") {
    a = 3 * 12.0107 + 6 * 1.00794;
    z = 3 * 6 + 6;
  } else if (gasname == "cC3H6") {
    a = 3 * 12.0107 + 6 * 1.00794;
    z = 3 * 6 + 6;
  } else if (gasname == "CH3OH") {
    a = 12.0107 + 4 * 1.00794 + 15.9994;
    z = 6 + 4 + 8;
  } else if (gasname == "C2H5OH") {
    a = 2 * 12.0107 + 6 * 1.00794 + 15.9994;
    z = 2 * 6 + 6 + 8;
  } else if (gasname == "C3H7OH") {
    a = 3 * 12.0107 + 8 * 1.00794 + 15.9994;
    z = 3 * 6 + 8 * 8;
  } else if (gasname == "Cs") {
    a = 132.9054519;
    z = 55;
  } else if (gasname == "F2") {
    a = 2 * 18.9984032;
    z = 2 * 9;
  } else if (gasname == "CS2") {
    a = 12.0107 + 2 * 32.065;
    z = 6 + 2 * 16;
  } else if (gasname == "COS") {
    a = 12.0107 + 15.9994 + 32.065;
    z = 6 + 8 + 16;
  } else if (gasname == "CD4") {
    a = 12.0107 + 4 * 2.01410177785;
    z = 6 + 4;
  } else if (gasname == "BF3") {
    a = 10.811 + 3 * 18.9984032;
    z = 5 + 3 * 9;
  } else if (gasname == "C2H2F4") {
    a = 2 * 12.0107 + 2 * 1.00794 + 4 * 18.9984032;
    z = 2 * 6 + 2 + 4 * 9;
  } else if (gasname == "CHF3") {
    a = 12.0107 + 1.00794 + 3 * 18.9984032;
    z = 6 + 1 + 3 * 9;
  } else if (gasname == "CF3Br") {
    a = 12.0107 + 3 * 18.9984032 + 79.904;
    z = 6 + 3 * 9 + 35;
  } else if (gasname == "C3F8") {
    a = 3 * 12.0107 + 8 * 18.9984032;
    z = 3 * 6 + 8 * 9;
  } else if (gasname == "O3") {
    a = 3 * 15.9994;
    z = 3 * 8;
  } else if (gasname == "Hg") {
    a = 2 * 200.59;
    z = 80;
  } else if (gasname == "H2S") {
    a = 2 * 1.00794 + 32.065;
    z = 2 + 16;
  } else if (gasname == "nC4H10") {
    a = 4 * 12.0107 + 10 * 1.00794;
    z = 4 * 6 + 10;
  } else if (gasname == "nC5H12") {
    a = 5 * 12.0107 + 12 * 1.00794;
    z = 5 * 6 + 12;
  } else if (gasname == "N2") {
    a = 2 * 14.0067;
    z = 2 * 7;
  } else if (gasname == "GeH4") {
    a = 72.64 + 4 * 1.00794;
    z = 32 + 4;
  } else if (gasname == "SiH4") {
    a = 28.0855 + 4 * 1.00794;
    z = 14 + 4;
  } else {
    a = 0.;
    z = 0.;
    return false;
  }

  return true;
}

bool MediumGas::GetGasName(const int gasnumber, const int version,
                           std::string& gasname) {

  switch (gasnumber) {
    case 1:
      gasname = "CF4";
      break;
    case 2:
      gasname = "Ar";
      break;
    case 3:
      gasname = "He";
      break;
    case 4:
      gasname = "He-3";
      break;
    case 5:
      gasname = "Ne";
      break;
    case 6:
      gasname = "Kr";
      break;
    case 7:
      gasname = "Xe";
      break;
    case 8:
      gasname = "CH4";
      break;
    case 9:
      gasname = "C2H6";
      break;
    case 10:
      gasname = "C3H8";
      break;
    case 11:
      gasname = "iC4H10";
      break;
    case 12:
      gasname = "CO2";
      break;
    case 13:
      gasname = "neoC5H12";
      break;
    case 14:
      gasname = "H2O";
      break;
    case 15:
      gasname = "O2";
      break;
    case 16:
      gasname = "N2";
      break;
    case 17:
      gasname = "NO";
      break;
    case 18:
      gasname = "N2O";
      break;
    case 19:
      gasname = "C2H4";
      break;
    case 20:
      gasname = "C2H2";
      break;
    case 21:
      gasname = "H2";
      break;
    case 22:
      gasname = "D2";
      break;
    case 23:
      gasname = "CO";
      break;
    case 24:
      gasname = "Methylal";
      break;
    case 25:
      gasname = "DME";
      break;
    case 26:
      gasname = "Reid-Step";
      break;
    case 27:
      gasname = "Maxwell-Model";
      break;
    case 28:
      gasname = "Reid-Ramp";
      break;
    case 29:
      gasname = "C2F6";
      break;
    case 30:
      gasname = "SF6";
      break;
    case 31:
      gasname = "NH3";
      break;
    case 32:
      gasname = "C3H6";
      break;
    case 33:
      gasname = "cC3H6";
      break;
    case 34:
      gasname = "CH3OH";
      break;
    case 35:
      gasname = "C2H5OH";
      break;
    case 36:
      gasname = "C3H7OH";
      break;
    case 37:
      gasname = "Cs";
      break;
    case 38:
      gasname = "F2";
      break;
    case 39:
      gasname = "CS2";
      break;
    case 40:
      gasname = "COS";
      break;
    case 41:
      gasname = "CD4";
      break;
    case 42:
      gasname = "BF3";
      break;
    case 43:
      gasname = "C2H2F4";
      break;
    case 44:
      if (version <= 11) {
        gasname = "He-3";
      } else {
        gasname = "TMA";
      }
      break;
    case 45:
      gasname = "He";
      break;
    case 46:
      gasname = "Ne";
      break;
    case 47:
      gasname = "Ar";
      break;
    case 48:
      gasname = "Kr";
      break;
    case 49:
      gasname = "Xe";
      break;
    case 50:
      gasname = "CHF3";
      break;
    case 51:
      gasname = "CF3Br";
      break;
    case 52:
      gasname = "C3F8";
      break;
    case 53:
      gasname = "O3";
      break;
    case 54:
      gasname = "Hg";
      break;
    case 55:
      gasname = "H2S";
      break;
    case 56:
      gasname = "nC4H10";
      break;
    case 57:
      gasname = "nC5H12";
      break;
    case 58:
      gasname = "N2";
      break;
    case 59:
      gasname = "GeH4";
      break;
    case 60:
      gasname = "SiH4";
      break;
    default:
      gasname = "";
      return false;
      break;
  }
  return true;
}

bool MediumGas::GetGasName(std::string input, std::string& gasname) const {

  // Convert to upper-case
  for (unsigned int i = 0; i < input.length(); ++i) {
    input[i] = toupper(input[i]);
  }

  gasname = "";

  if (input == "") return false;

  // CF4
  if (input == "CF4" || input == "FREON" || input == "FREON-14" ||
      input == "TETRAFLUOROMETHANE") {
    gasname = "CF4";
    return true;
  }
  // Argon
  if (input == "AR" || input == "ARGON") {
    gasname = "Ar";
    return true;
  }
  // Helium 4
  if (input == "HE" || input == "HELIUM" || input == "HE-4" ||
      input == "HE 4" || input == "HE4" || input == "4-HE" || input == "4 HE" ||
      input == "4HE" || input == "HELIUM-4" || input == "HELIUM 4" ||
      input == "HELIUM4") {
    gasname = "He";
    return true;
  }
  // Helium 3
  if (input == "HE-3" || input == "HE3" || input == "HELIUM-3" ||
      input == "HELIUM 3" || input == "HELIUM3") {
    gasname = "He-3";
    return true;
  }
  // Neon
  if (input == "NE" || input == "NEON") {
    gasname = "Ne";
    return true;
  }
  // Krypton
  if (input == "KR" || input == "KRYPTON") {
    gasname = "Kr";
    return true;
  }
  // Xenon
  if (input == "XE" || input == "XENON") {
    gasname = "Xe";
    return true;
  }
  // Methane
  if (input == "CH4" || input == "METHANE") {
    gasname = "CH4";
    return true;
  }
  // Ethane
  if (input == "C2H6" || input == "ETHANE") {
    gasname = "C2H6";
    return true;
  }
  // Propane
  if (input == "C3H8" || input == "PROPANE") {
    gasname = "C3H8";
    return true;
  }
  // Isobutane
  if (input == "C4H10" || input == "ISOBUTANE" || input == "ISO" ||
      input == "IC4H10" || input == "ISO-C4H10" || input == "ISOC4H10") {
    gasname = "iC4H10";
    return true;
  }
  // Carbon dioxide (CO2)
  if (input == "CO2" || input == "CARBON-DIOXIDE" ||
      input == "CARBON DIOXIDE" || input == "CARBONDIOXIDE") {
    gasname = "CO2";
    return true;
  }
  // Neopentane
  if (input == "NEOPENTANE" || input == "NEO-PENTANE" || input == "NEO-C5H12" ||
      input == "NEOC5H12" || input == "DIMETHYLPROPANE" || input == "C5H12") {
    gasname = "neoC5H12";
    return true;
  }
  // Water
  if (input == "H2O" || input == "WATER" || input == "WATER-VAPOUR" ||
      input == "WATER VAPOUR") {
    gasname = "H2O";
    return true;
  }
  // Oxygen
  if (input == "O2" || input == "OXYGEN") {
    gasname = "O2";
    return true;
  }
  // Nitrogen
  if (input == "NI" || input == "NITRO" || input == "N2" ||
      input == "NITROGEN") {
    gasname = "N2";
    return true;
  }
  // Nitric oxide (NO)
  if (input == "NO" || input == "NITRIC-OXIDE" || input == "NITRIC OXIDE" ||
      input == "NITROGEN-MONOXIDE" || input == "NITROGEN MONOXIDE") {
    gasname = "NO";
    return true;
  }
  // Nitrous oxide (N2O)
  if (input == "N2O" || input == "NITROUS-OXIDE" || input == "NITROUS OXIDE" ||
      input == "DINITROGEN-MONOXIDE" || input == "LAUGHING-GAS") {
    gasname = "N2O";
    return true;
  }
  // Ethene (C2H4)
  if (input == "C2H4" || input == "ETHENE" || input == "ETHYLENE") {
    gasname = "C2H4";
    return true;
  }
  // Acetylene (C2H2)
  if (input == "C2H2" || input == "ACETYL" || input == "ACETYLENE" ||
      input == "ETHYNE") {
    gasname = "C2H2";
    return true;
  }
  // Hydrogen
  if (input == "H2" || input == "HYDROGEN") {
    gasname = "H2";
    return true;
  }
  // Deuterium
  if (input == "D2" || input == "DEUTERIUM") {
    gasname = "D2";
    return true;
  }
  // Carbon monoxide (CO)
  if (input == "CO" || input == "CARBON-MONOXIDE" ||
      input == "CARBON MONOXIDE") {
    gasname = "CO";
    return true;
  }
  // Methylal (dimethoxymethane, CH3-O-CH2-O-CH3, "hot" version)
  if (input == "METHYLAL" || input == "METHYLAL-HOT" || input == "DMM" ||
      input == "DIMETHOXYMETHANE" || input == "FORMAL" || input == "C3H8O2") {
    gasname = "Methylal";
    return true;
  }
  // DME
  if (input == "DME" || input == "DIMETHYL-ETHER" || input == "DIMETHYLETHER" ||
      input == "DIMETHYL ETHER" || input == "METHYL ETHER" ||
      input == "METHYL-ETHER" || input == "METHYLETHER" ||
      input == "WOOD-ETHER" || input == "WOODETHER" || input == "WOOD ETHER" ||
      input == "DIMETHYL OXIDE" || input == "DIMETHYL-OXIDE" ||
      input == "DEMEON" || input == "METHOXYMETHANE" || input == "C4H10O2") {
    gasname = "DME";
    return true;
  }
  // Reid step
  if (input == "REID-STEP") {
    gasname = "Reid-Step";
    return true;
  }
  // Maxwell model
  if (input == "MAXWELL-MODEL") {
    gasname = "Maxwell-Model";
    return true;
  }
  // Reid ramp
  if (input == "REID-RAMP") {
    gasname = "Reid-Ramp";
    return true;
  }
  // C2F6
  if (input == "C2F6" || input == "FREON-116" || input == "ZYRON-116" ||
      input == "ZYRON-116-N5" || input == "HEXAFLUOROETHANE") {
    gasname = "C2F6";
    return true;
  }
  // SF6
  if (input == "SF6" || input == "SULPHUR-HEXAFLUORIDE" ||
      input == "SULFUR-HEXAFLUORIDE" || input == "SULPHUR HEXAFLUORIDE" ||
      input == "SULFUR HEXAFLUORIDE") {
    gasname = "SF6";
    return true;
  }
  // NH3
  if (input == "NH3" || input == "AMMONIA") {
    gasname = "NH3";
    return true;
  }
  // Propene
  if (input == "C3H6" || input == "PROPENE" || input == "PROPYLENE") {
    gasname = "C3H6";
    return true;
  }
  // Cyclopropane
  if (input == "C-PROPANE" || input == "CYCLO-PROPANE" ||
      input == "CYCLO PROPANE" || input == "CYCLOPROPANE" ||
      input == "C-C3H6" || input == "CC3H6" || input == "CYCLO-C3H6") {
    gasname = "cC3H6";
    return true;
  }
  // Methanol
  if (input == "METHANOL" || input == "METHYL-ALCOHOL" ||
      input == "METHYL ALCOHOL" || input == "WOOD ALCOHOL" ||
      input == "WOOD-ALCOHOL" || input == "CH3OH") {
    gasname = "CH3OH";
    return true;
  }
  // Ethanol
  if (input == "ETHANOL" || input == "ETHYL-ALCOHOL" ||
      input == "ETHYL ALCOHOL" || input == "GRAIN ALCOHOL" ||
      input == "GRAIN-ALCOHOL" || input == "C2H5OH") {
    gasname = "C2H5OH";
    return true;
  }
  // Propanol
  if (input == "PROPANOL" || input == "2-PROPANOL" || input == "ISOPROPYL" ||
      input == "ISO-PROPANOL" || input == "ISOPROPANOL" ||
      input == "ISOPROPYL ALCOHOL" || input == "ISOPROPYL-ALCOHOL" ||
      input == "C3H7OH") {
    gasname = "C3H7OH";
    return true;
  }
  // Cesium / Caesium.
  if (input == "CS" || input == "CESIUM" || input == "CAESIUM") {
    gasname = "Cs";
    return true;
  }
  // Fluorine
  if (input == "F2" || input == "FLUOR" || input == "FLUORINE") {
    gasname = "F2";
    return true;
  }
  // CS2
  if (input == "CS2" || input == "CARBON-DISULPHIDE" ||
      input == "CARBON-DISULFIDE" || input == "CARBON DISULPHIDE" ||
      input == "CARBON DISULFIDE") {
    gasname = "CS2";
    return true;
  }
  // COS
  if (input == "COS" || input == "CARBONYL-SULPHIDE" ||
      input == "CARBONYL-SULFIDE" || input == "CARBONYL SULFIDE") {
    gasname = "COS";
    return true;
  }
  // Deuterated methane
  if (input == "DEUT-METHANE" || input == "DEUTERIUM-METHANE" ||
      input == "DEUTERATED-METHANE" || input == "DEUTERATED METHANE" ||
      input == "DEUTERIUM METHANE" || input == "CD4") {
    gasname = "CD4";
    return true;
  }
  // BF3
  if (input == "BF3" || input == "BORON-TRIFLUORIDE" ||
      input == "BORON TRIFLUORIDE") {
    gasname = "BF3";
    return true;
  }
  // C2H2F4 (and C2HF5).
  if (input == "C2HF5" || input == "C2H2F4" || input == "C2F5H" ||
      input == "C2F4H2" || input == "FREON 134" || input == "FREON 134A" ||
      input == "FREON-134" || input == "FREON-134-A" || input == "FREON 125" ||
      input == "ZYRON 125" || input == "FREON-125" || input == "ZYRON-125" ||
      input == "TETRAFLUOROETHANE" || input == "PENTAFLUOROETHANE") {
    gasname = "C2H2F4";
    return true;
  }
  // TMA
  if (input == "TMA" || input == "TRIMETHYLAMINE" || input == "N(CH3)3" ||
      input == "N-(CH3)3") {
    gasname = "TMA";
    return true;
  }
  // CHF3
  if (input == "CHF3" || input == "FREON-23" || input == "TRIFLUOROMETHANE" ||
      input == "FLUOROFORM") {
    gasname = "CHF3";
    return true;
  }
  // CF3Br
  if (input == "CF3BR" || input == "TRIFLUOROBROMOMETHANE" ||
      input == "BROMOTRIFLUOROMETHANE" || input == "HALON-1301" ||
      input == "HALON 1301" || input == "FREON-13B1" || input == "FREON 13BI") {
    gasname = "CF3Br";
    return true;
  }
  // C3F8
  if (input == "C3F8" || input == "OCTAFLUOROPROPANE" || input == "R218" ||
      input == "R-218" || input == "FREON 218" || input == "FREON-218" ||
      input == "PERFLUOROPROPANE" || input == "RC 218" || input == "PFC 218" ||
      input == "RC-218" || input == "PFC-218" || input == "FLUTEC PP30" ||
      input == "GENETRON 218") {
    gasname = "C3F8";
    return true;
  }
  // Ozone
  if (input == "OZONE" || input == "O3") {
    gasname = "O3";
    return true;
  }
  // Mercury
  if (input == "MERCURY" || input == "HG" || input == "HG2") {
    gasname = "Hg";
    return true;
  }
  // H2S
  if (input == "H2S" || input == "HYDROGEN SULPHIDE" || input == "SEWER GAS" ||
      input == "HYDROGEN-SULPHIDE" || input == "SEWER-GAS" ||
      input == "HYDROGEN SULFIDE" || input == "HEPATIC ACID" ||
      input == "HYDROGEN-SULFIDE" || input == "HEPATIC-ACID" ||
      input == "SULFUR HYDRIDE" || input == "DIHYDROGEN MONOSULFIDE" ||
      input == "SULFUR-HYDRIDE" || input == "DIHYDROGEN-MONOSULFIDE" ||
      input == "DIHYDROGEN MONOSULPHIDE" || input == "SULPHUR HYDRIDE" ||
      input == "DIHYDROGEN-MONOSULPHIDE" || input == "SULPHUR-HYDRIDE" ||
      input == "STINK DAMP" || input == "SULFURATED HYDROGEN" ||
      input == "STINK-DAMP" || input == "SULFURATED-HYDROGEN") {
    gasname = "H2S";
    return true;
  }
  // n-Butane
  if (input == "N-BUTANE" || input == "N-C4H10" || input == "NBUTANE" ||
      input == "NC4H10") {
    gasname = "nC4H10";
    return true;
  }
  // n-Pentane
  if (input == "N-PENTANE" || input == "N-C5H12" || input == "NPENTANE" ||
      input == "NC5H12") {
    gasname = "nC5H12";
    return true;
  }
  // Nitrogen
  if (input == "NI-PHELPS" || input == "NI PHELPS" ||
      input == "NITROGEN-PHELPS" || input == "NITROGEN PHELPHS" ||
      input == "N2-PHELPS" || input == "N2 PHELPS" || input == "N2 (PHELPS)") {
    gasname = "N2 (Phelps)";
    return true;
  }
  // Germane, GeH4
  if (input == "GERMANE" || input == "GERM" || input == "GERMANIUM-HYDRIDE" ||
      input == "GERMANIUM HYDRIDE" || input == "GERMANIUM TETRAHYDRIDE" ||
      input == "GERMANIUM-TETRAHYDRIDE" || input == "GERMANOMETHANE" ||
      input == "MONOGERMANE" || input == "GEH4") {
    gasname = "GeH4";
    return true;
  }
  // Silane, SiH4
  if (input == "SILANE" || input == "SIL" || input == "SILICON-HYDRIDE" ||
      input == "SILICON HYDRIDE" || input == "SILICON-TETRAHYDRIDE" ||
      input == "SILICANE" || input == "MONOSILANE" || input == "SIH4") {
    gasname = "SiH4";
    return true;
  }

  std::cerr << m_className << "::GetGasName:\n";
  std::cerr << "    Gas " << input << " is not defined.\n";
  return false;
}

bool MediumGas::GetGasNumberGasFile(const std::string& input,
                                    int& number) const {

  if (input == "") {
    number = 0;
    return false;
  }

  // CF4
  if (input == "CF4") {
    number = 1;
    return true;
  }
  // Argon
  if (input == "Ar") {
    number = 2;
    return true;
  }
  // Helium 4
  if (input == "He" || input == "He-4") {
    number = 3;
    return true;
  }
  // Helium 3
  if (input == "He-3") {
    number = 4;
    return true;
  }
  // Neon
  if (input == "Ne") {
    number = 5;
    return true;
  }
  // Krypton
  if (input == "Kr") {
    number = 6;
    return true;
  }
  // Xenon
  if (input == "Xe") {
    number = 7;
    return true;
  }
  // Methane
  if (input == "CH4") {
    number = 8;
    return true;
  }
  // Ethane
  if (input == "C2H6") {
    number = 9;
    return true;
  }
  // Propane
  if (input == "C3H8") {
    number = 10;
    return true;
  }
  // Isobutane
  if (input == "iC4H10") {
    number = 11;
    return true;
  }
  // Carbon dioxide (CO2)
  if (input == "CO2") {
    number = 12;
    return true;
  }
  // Neopentane
  if (input == "neoC5H12") {
    number = 13;
    return true;
  }
  // Water
  if (input == "H2O") {
    number = 14;
    return true;
  }
  // Oxygen
  if (input == "O2") {
    number = 15;
    return true;
  }
  // Nitrogen
  if (input == "N2") {
    number = 16;
    return true;
  }
  // Nitric oxide (NO)
  if (input == "NO") {
    number = 17;
    return true;
  }
  // Nitrous oxide (N2O)
  if (input == "N2O") {
    number = 18;
    return true;
  }
  // Ethene (C2H4)
  if (input == "C2H4") {
    number = 19;
    return true;
  }
  // Acetylene (C2H2)
  if (input == "C2H2") {
    number = 20;
    return true;
  }
  // Hydrogen
  if (input == "H2") {
    number = 21;
    return true;
  }
  // Deuterium
  if (input == "D2") {
    number = 22;
    return true;
  }
  // Carbon monoxide (CO)
  if (input == "CO") {
    number = 23;
    return true;
  }
  // Methylal (dimethoxymethane, CH3-O-CH2-O-CH3, "hot" version)
  if (input == "Methylal") {
    number = 24;
    return true;
  }
  // DME
  if (input == "DME") {
    number = 25;
    return true;
  }
  // Reid step
  if (input == "Reid-Step") {
    number = 26;
    return true;
  }
  // Maxwell model
  if (input == "Maxwell-Model") {
    number = 27;
    return true;
  }
  // Reid ramp
  if (input == "Reid-Ramp") {
    number = 28;
    return true;
  }
  // C2F6
  if (input == "C2F6") {
    number = 29;
    return true;
  }
  // SF6
  if (input == "SF6") {
    number = 30;
    return true;
  }
  // NH3
  if (input == "NH3") {
    number = 31;
    return true;
  }
  // Propene
  if (input == "C3H6") {
    number = 32;
    return true;
  }
  // Cyclopropane
  if (input == "cC3H6") {
    number = 33;
    return true;
  }
  // Methanol
  if (input == "CH3OH") {
    number = 34;
    return true;
  }
  // Ethanol
  if (input == "C2H5OH") {
    number = 35;
    return true;
  }
  // Propanol
  if (input == "C3H7OH") {
    number = 36;
    return true;
  }
  // Cesium / Caesium.
  if (input == "Cs") {
    number = 37;
    return true;
  }
  // Fluorine
  if (input == "F2") {
    number = 38;
    return true;
  }
  // CS2
  if (input == "CS2") {
    number = 39;
    return true;
  }
  // COS
  if (input == "COS") {
    number = 40;
    return true;
  }
  // Deuterated methane
  if (input == "CD4") {
    number = 41;
    return true;
  }
  // BF3
  if (input == "BF3") {
    number = 42;
    return true;
  }
  // C2HF5 and C2H2F4.
  if (input == "C2HF5" || input == "C2H2F4") {
    number = 43;
    return true;
  }
  // TMA
  if (input == "TMA") {
    number = 44;
    return true;
  }
  // CHF3
  if (input == "CHF3") {
    number = 50;
    return true;
  }
  // CF3Br
  if (input == "CF3Br") {
    number = 51;
    return true;
  }
  // C3F8
  if (input == "C3F8") {
    number = 52;
    return true;
  }
  // Ozone
  if (input == "O3") {
    number = 53;
    return true;
  }
  // Mercury
  if (input == "Hg") {
    number = 54;
    return true;
  }
  // H2S
  if (input == "H2S") {
    number = 55;
    return true;
  }
  // n-butane
  if (input == "nC4H10") {
    number = 56;
    return true;
  }
  // n-pentane
  if (input == "nC5H12") {
    number = 57;
    return true;
  }
  // Nitrogen
  if (input == "N2 (Phelps)") {
    number = 58;
    return true;
  }
  // Germane, GeH4
  if (input == "GeH4") {
    number = 59;
    return true;
  }
  // Silane, SiH4
  if (input == "SiH4") {
    number = 60;
    return true;
  }

  std::cerr << m_className << "::GetGasNumberGasFile:\n";
  std::cerr << "    Gas " << input << " is not defined.\n";
  return false;
}

bool MediumGas::GetPhotoabsorptionCrossSection(const double& e, double& sigma,
                                               const unsigned int& i) {

  if (i >= m_nMaxGases) {
    std::cerr << m_className << "::GetPhotoabsorptionCrossSection:\n";
    std::cerr << "    Index (" << i << ") out of range.\n";
    return false;
  }

  OpticalData optData;
  if (!optData.IsAvailable(m_gas[i])) return false;
  double eta = 0.;
  return optData.GetPhotoabsorptionCrossSection(m_gas[i], e, sigma, eta);
}
}

#ifndef ENTRANFCS_H
#define ENTRANFCS_H

#include "heed++/code/HeedMatterDef.h"

namespace Heed {

//#define DEBUG_EnTransfCS  // allows to print additional information
// and keeps it in class, which makes it bigger

#define EXCLUDE_A_VALUES   // exclude absorption values
#define EXCLUDE_VAL_FADDA  // exclude values not necessary for MC

/// The PAI cross section of energy transfers from charged particle to media.
/// The particle has fixed parameters (energy, speed, etc.), which
/// are not affected by energy transfers, since they are considered
/// too small compared with the particle energy.
///
/// 2003, I. Smirnov

class EnTransfCS : public RegPassivePtr {
 public:
  /// Default constructor
  EnTransfCS() {}
  /// Constructor
  EnTransfCS(double fparticle_mass, double fgamma_1, int fs_primary_electron,
             HeedMatterDef* fhmd, long fparticle_charge = 1);
  virtual void print(std::ostream& file, int l) const;
  virtual EnTransfCS* copy() const { return new EnTransfCS(*this); }

  /// Particle mass [MeV]
  double particle_mass;
  /// Total energy [MeV]
  double particle_ener;
  /// Charge in units of electron charge (used square, sign does not matter).
  long particle_charge;

  /// Lorentz factor - 1 (the best dimensionless measurement of speed).
  double gamma_1;

  /// Max. energy transfer [MeV]
  double max_etransf;
  /// Flag controlling the form of Rutherford scattering.
  /// For our purposes it is good to have simple form,
  /// so this variable is initialized to 1.
  /// Simple form means that there are two terms.
  /// The third term is assumed zero.
  bool s_simple_form;
  /// Flag that the primary particle is the electron
  int s_primary_electron;

  PassivePtr<HeedMatterDef> hmd;

  /// In the following arrays there is the only index: the energy.
  /// The meaning: the average value on the energy interval.
  std::vector<double> log1C;        ///< common first log without cs
  std::vector<double> log2C;        ///< common second log without cs
  std::vector<double> chereC;       ///< Cherenkov's radiation
  std::vector<double> chereCangle;  ///< angle of Cherenkov's radiation
  std::vector<double> Rruth;        ///< term called R in my paper
#ifdef DEBUG_EnTransfCS
  // Total Rutherford, sum of fruth by atoms and shells, per one electron
  // (in the paper it is per atom).
  std::vector<double> truth;
#endif

  /// Sum of (ionization) differential cross-section terms
  std::vector<double> addaC;
  /// Integrated (ionization) cross-section
  double quanC;

#ifndef EXCLUDE_A_VALUES
  /// Sum of (absorption) differential cross-section terms
  std::vector<double> addaC_a;
  /// Integrated (absorption) cross-section
  double quanC_a;
#endif

  // First moment (mean restricted energy loss) [MeV]
  double meanC;
  // First moment with additional tail to max. kinematically allowed transfer,
  // calculated only for heavy particles (integral for electrons non-trivial).
  double meanC1;
#ifndef EXCLUDE_A_VALUES
  double meanC1_a;
  double meanC_a;
#endif
  // Secondary ionization
  double meaneleC;
  double meaneleC1;

  /// In the following arrays there are three indices:
  /// atom number in the matter, shell number in atom, energy
  /// Fraction of Cherenkov term.
  std::vector<std::vector<std::vector<double> > > cher;
  /// Rutherford term
  std::vector<std::vector<std::vector<double> > > fruth;
  /// Sum
  std::vector<std::vector<std::vector<double> > > adda;
  /// Integral, normalised to unity
  std::vector<std::vector<std::vector<double> > > fadda;
#ifndef EXCLUDE_A_VALUES
  std::vector<std::vector<std::vector<double> > > cher_a;
  std::vector<std::vector<std::vector<double> > > adda_a;
  std::vector<std::vector<std::vector<double> > > fadda_a;
#endif

#ifndef EXCLUDE_VAL_FADDA
  /// The true values of the integral (should be equal to quan)
  std::vector<std::vector<double> > val_fadda;  // integral * hmd->xeldens;
#ifndef EXCLUDE_A_VALUES
  std::vector<std::vector<double> > val_fadda_a;  // integral * hmd->xeldens;
#endif
#endif

  /// In the following arrays there are two indices:
  /// atom number in the matter, shell number in atom.
  std::vector<std::vector<double> > quan;  // per 1 cm, used for path length
  std::vector<std::vector<double> > mean;
#ifndef EXCLUDE_A_VALUES
  std::vector<std::vector<double> > quan_a;
  std::vector<std::vector<double> > mean_a;
#endif

  std::vector<double> length_y0;
};
}

#endif

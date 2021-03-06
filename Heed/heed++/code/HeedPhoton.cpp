#include <algorithm>
#include "wcpplib/clhep_units/WPhysicalConstants.h"
#include "wcpplib/random/ranluxint.h"
#include "wcpplib/random/chisran.h"
#include "wcpplib/random/pois.h"
#include "heed++/code/HeedDeltaElectron.h"
#include "heed++/code/HeedDeltaElectronCS.h"
#include "heed++/code/EnTransfCS.h"
#include "heed++/code/HeedPhoton.h"

// 2003, I. Smirnov

namespace Heed {

using CLHEP::cm;
using CLHEP::cm3;
using CLHEP::gram;
using CLHEP::mole;
using CLHEP::c_light;
using CLHEP::electron_mass_c2;
using CLHEP::Avogadro;

HeedPhoton::HeedPhoton(manip_absvol* primvol, const point& pt, const vec& vel,
                       vfloat time, long fparent_particle_number,
                       double fenergy, HeedFieldMap* fieldmap,
                       const bool fs_print_listing)
    : gparticle(primvol, pt, vel, time),
      particle_number(last_particle_number++),
      parent_particle_number(fparent_particle_number),
      energy(fenergy),
      s_photon_absorbed(false),
#ifdef SFER_PHOTOEL
      s_sfer_photoel(0),
#endif
      s_delta_generated(false),
      s_print_listing(fs_print_listing),
      m_fieldMap(fieldmap) {
  mfunname("HeedPhoton::HeedPhoton(...)");
  double length_vel = vel.length();
  check_econd11(fabs(length_vel - c_light) / (length_vel + c_light), > 1.0e-10,
                mcerr);
}

void HeedPhoton::physics(std::vector<gparticle*>& /*secondaries*/) {
  mfunname("void HeedPhoton::physics()");
  if (s_print_listing) mcout << "HeedPhoton::physics() starts\n";
  // Stop here if the photon has already been absorbed.
  if (s_photon_absorbed) return;
  if (nextpos.prange <= 0.0) return;
  // Get least address of volume
  const absvol* av = currpos.tid.G_lavol();
  HeedMatterDef* hmd = nullptr;
  auto etcs = dynamic_cast<const EnTransfCS*>(av);
  if (etcs) {
    hmd = etcs->hmd;
  } else {
    auto hdecs = dynamic_cast<const HeedDeltaElectronCS*>(av);
    if (hdecs) hmd = hdecs->hmd;
  }
  // Stop here if we couldn't retrieve the material definition.
  if (!hmd) return;
  // Sum up the cross-sections.
  std::vector<double> cs;
  std::vector<long> nat;
  std::vector<long> nsh;
  double s = 0.0;
  const long qa = hmd->matter->qatom();
  for (long na = 0; na < qa; na++) {
    const long qs = hmd->apacs[na]->get_qshell();
    const double awq = hmd->matter->weight_quan(na);
    for (long ns = 0; ns < qs; ns++) {
      cs.push_back(hmd->apacs[na]->get_ICS(ns, energy) * awq);
      // threshold is taken into account in apacs[na]->get_ACS(ns,..)
      nat.push_back(na);
      nsh.push_back(ns);
      s += cs.back();
    }
  }
  if (s_print_listing) Iprintn(mcout, s);
  // Multiply with the density and calculate the path length.
  // s = s * hmd->eldens / hmd->matter->Z_mean() * C1_MEV_CM;
  s = s * 1.0e-18 * Avogadro / (hmd->matter->A_mean() / (gram / mole)) *
      hmd->matter->density() / (gram / cm3);
  if (s_print_listing) Iprintn(mcout, s);
  const double path_length = 1.0 / s;  // cm
  if (s_print_listing) Iprint2n(mcout, energy, path_length);
  // Draw a random step length.
  const double xleng = -path_length * log(1.0 - SRANLUX());
  if (s_print_listing) Iprint2n(mcout, xleng, nextpos.prange / cm);
  if (xleng * cm < nextpos.prange) {
    s_photon_absorbed = true;
#ifdef SFER_PHOTOEL
    // Assume that virtual photons are already
    // absorbed and s_sfer_photoel is 0 for them
    s_sfer_photoel = 1;  
#endif
    // Sample the shell.
    chispre(cs);
    const double r = chisran(SRANLUX(), cs);
    const long n = std::min(std::max(long(r), 0L), long(cs.size() - 1));
    if (s_print_listing) Iprintn(mcout, n);
    na_absorbing = nat[n];
    ns_absorbing = nsh[n];
    nextpos.prange = xleng * cm;
    nextpos.pt = currpos.pt + nextpos.prange * currpos.dir;
    nextpos.ptloc = nextpos.pt;
    nextpos.tid.up_absref(&nextpos.ptloc);
  }
}

void HeedPhoton::physics_after_new_speed(std::vector<gparticle*>& secondaries) {
  mfunname("void HeedPhoton::physics_after_new_speed()");
  if (s_print_listing) mcout << "HeedPhoton::physics_after_new_speed starts\n";
  // Stop if the photon has not been absorbed.
  if (!s_photon_absorbed) return;
  // Stop if the delta electrons have already been generated.
  if (s_delta_generated) return;
  // Get least address of volume
  const absvol* av = currpos.tid.G_lavol();
  HeedMatterDef* hmd = nullptr;
  auto etcs = dynamic_cast<const EnTransfCS*>(av);
  if (etcs) {
    hmd = etcs->hmd;
  } else {
    auto hdecs = dynamic_cast<const HeedDeltaElectronCS*>(av);
    if (hdecs) hmd = hdecs->hmd;
  }
  // Stop here if we couldn't retrieve the material definition.
  if (!hmd) return;
  // Generate delta-electrons.
  std::vector<double> el_energy;
  std::vector<double> ph_energy;
  hmd->apacs[na_absorbing]
      ->get_escape_particles(ns_absorbing, energy, el_energy, ph_energy);
  if (s_print_listing) {
    mcout << "The condition:\n";
    Iprint2n(mcout, na_absorbing, ns_absorbing);
    mcout << "The decay products:\n";
    for (unsigned int k = 0; k < el_energy.size(); ++k)
      mcout << el_energy[k] << "\n";
    for (unsigned int k = 0; k < ph_energy.size(); ++k)
      mcout << ph_energy[k] << "\n";
  }
  const long qel = el_energy.size();
  for (long nel = 0; nel < qel; nel++) {
    vec vel = currpos.dir;
    if (nel == 0) {  
      // The first in the list should be the photoelectron.
#ifdef SFER_PHOTOEL
      if (s_sfer_photoel == 1) {
        vel.random_sfer_vec();
      } else {
        vel = currpos.dir;
      }
#else
      vel = currpos.dir;  // direction is OK
#endif
    } else {
      vel.random_sfer_vec();
    }
    const double gam_1 = el_energy[nel] / electron_mass_c2;
    const double inv = 1.0 / (gam_1 + 1.0);
    const double beta = sqrt(1.0 - inv * inv);
    const double mod_v = beta * c_light;
    vel = vel * mod_v;
    if (s_print_listing) {
      mcout << "Initializing delta electron\n";
      Iprint4n(mcout, el_energy[nel], gam_1, beta, mod_v);
    }
    HeedDeltaElectron* hd =
        new HeedDeltaElectron(currpos.tid.eid[0], currpos.pt, vel,
                              currpos.time, particle_number, m_fieldMap);
    secondaries.push_back(hd);
  }
  const long qph = ph_energy.size();
  for (long nph = 0; nph < qph; nph++) {
    vec vel;
    vel.random_sfer_vec();
    vel *= c_light;
    if (s_print_listing) {
      mcout << "Initializing photon\n";
      Iprint2n(mcout, el_energy[nph], vel);
    }
    HeedPhoton* hp = new HeedPhoton(currpos.tid.eid[0], currpos.pt,
                                    vel, currpos.time, particle_number,
                                    ph_energy[nph], m_fieldMap);
    secondaries.push_back(hp);
  }
  s_delta_generated = true;
  s_life = false;
  if (s_print_listing) mcout << "HeedPhoton::physics_after_new_speed exited\n";
}

void HeedPhoton::print(std::ostream& file, int l) const {
  if (l < 0) return;
  Ifile << "HeedPhoton (l=" << l << "): particle_number=" << particle_number
        << " energy=" << energy << "MeV\n";
  if (l == 1) return;
  indn.n += 2;
  Ifile << "s_photon_absorbed=" << s_photon_absorbed
        << " na_absorbing=" << na_absorbing << " ns_absorbing=" << ns_absorbing
        << " s_delta_generated=" << s_delta_generated
#ifdef SFER_PHOTOEL
        << " s_sfer_photoel=" << s_sfer_photoel
#endif
        << " parent_particle_number=" << parent_particle_number
        << " s_print_listing=" << s_print_listing << '\n';
  gparticle::print(file, l - 1);
  indn.n -= 2;
}
}

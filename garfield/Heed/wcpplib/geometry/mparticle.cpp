#include "wcpplib/geometry/mparticle.h"
#include "wcpplib/clhep_units/WPhysicalConstants.h"
/*
Copyright (c) 2000 Igor B. Smirnov

The file can be used, copied, modified, and distributed
according to the terms of GNU Lesser General Public License version 2.1
as published by the Free Software Foundation,
and provided that the above copyright notice, this permission notice,
and notices about any modifications of the original text
appear in all copies and in supporting documentation.
The file is provided "as is" without express or implied warranty.
*/

namespace Heed {

using CLHEP::c_light;
using CLHEP::c_squared;

mparticle::mparticle(manip_absvol* primvol, const point& pt, const vec& vel,
                     vfloat time, double fmass) 
    : gparticle(primvol, pt, vel, time),
      mass(fmass) {

  mfunname("mparticle::mparticle(...)");

  orig_gamma_1 = lorgamma_1(origin.speed / c_light);
  orig_kin_energy = orig_gamma_1 * mass * c_squared;
  prev_gamma_1 = lorgamma_1(prevpos.speed / c_light);
  prev_kin_energy = prev_gamma_1 * mass * c_squared;
  curr_gamma_1 = lorgamma_1(currpos.speed / c_light);
  curr_kin_energy = curr_gamma_1 * mass * c_squared;
  check_consistency();
}

void mparticle::check_consistency() const {
  mfunname("void mparticle::check_consistency() const");
  check_econd11(vecerror, != 0, mcerr);
  double speed = c_light * lorbeta(orig_gamma_1);
  check_econd11a(fabs(speed - origin.speed) / (speed + origin.speed), > 1.0e-10,
                 (*this), mcerr);
  speed = c_light * lorbeta(prev_gamma_1);
  check_econd11a(fabs(speed - prevpos.speed) / (speed + prevpos.speed),
                 > 1.0e-10, (*this), mcerr);
  speed = c_light * lorbeta(curr_gamma_1);
  check_econd11a(fabs(speed - currpos.speed) / (speed + currpos.speed),
                 > 1.0e-10, (*this), mcerr);
  double kin_ener = orig_gamma_1 * mass * c_squared;
  if (kin_ener > 1000.0 * DBL_MIN) {
    check_econd11a(
        fabs(orig_kin_energy - kin_ener) / (orig_kin_energy + kin_ener),
        > 1.0e-9, "kin_ener=" << kin_ener << '\n' << (*this), mcerr);
  }
  kin_ener = prev_gamma_1 * mass * c_squared;
  if (kin_ener > 1000.0 * DBL_MIN) {
    check_econd11a(
        fabs(prev_kin_energy - kin_ener) / (prev_kin_energy + kin_ener),
        > 1.0e-9, "kin_ener=" << kin_ener << '\n' << (*this), mcerr);
  }
  kin_ener = curr_gamma_1 * mass * c_squared;
  if (kin_ener > 1000.0 * DBL_MIN) {
    check_econd11a(
        fabs(curr_kin_energy - kin_ener) / (curr_kin_energy + kin_ener),
        > 1.0e-9, "kin_ener=" << kin_ener << '\n' << (*this), mcerr);
  }
}

void mparticle::step(std::vector<gparticle*>& secondaries) {
  // Make step to nextpos and calculate new step to border
  mfunname("void mparticle::step(...)");
  prevpos = currpos;
  prev_kin_energy = curr_kin_energy;
  prev_gamma_1 = curr_gamma_1;
  currpos = nextpos;
  curr_relcen = dv0;
  total_range_from_origin += currpos.prange;
  nstep++;
  if (currpos.prange == 0) {
    n_zero_step++;
    check_econd12a(n_zero_step, >, max_q_zero_step,
                   "too many zero steps, possible infinite loop\n";
                   print(mcout, 10);, mcerr);
  } else {
    n_zero_step = 0;
  }
  // Calculate new current speed, direction and time.
  new_speed();
  physics_after_new_speed(secondaries);

  if (s_life) {
    if (prevpos.tid != currpos.tid) change_vol();
    nextpos = calc_step_to_bord();
  }
}

void mparticle::curvature(int& fs_cf, vec& frelcen, vfloat& fmrange,
                          vfloat prec) {

  pvecerror(
      "void mparticle::curvature(int& fs_cf, vec& frelcen, vfloat& fmrange)");
  vec f;
  vec f_perp_fl;
  int i = force(currpos.pt, f, f_perp_fl, fmrange);
  vec f_perp = currpos.speed * (currpos.dir || f_perp_fl);
  f += f_perp;
  if (i == 0 || f == dv0) {
    fs_cf = 0;
    frelcen = dv0;
    if (currpos.dir == dv0) fmrange = 0;  // to stay in the place
    return;
  }
  if (currpos.dir == dv0) {
    // starting to move in the direction of force
    currpos.dir = unit_vec(f);
  }
  const int j = check_par(currpos.dir, f, prec);
  if (j != 0) {
    fs_cf = 0;
    frelcen = dv0;
    if (j == -1) {
      // decelerate, search for stop point
      const double ran = curr_kin_energy / f.length();
      if (fmrange > ran) fmrange = ran;
    }
  } else {
    fs_cf = 1;
    vec fn = project_to_plane(f, currpos.dir);  // normal component
    frelcen = unit_vec(fn);
    double len = fn.length();
    vfloat rad =
        (currpos.speed * currpos.speed * (curr_gamma_1 + 1) * mass) / len;
    frelcen *= rad;
  }
  currpos.dirloc = currpos.dir;
  currpos.tid.up_absref(&currpos.dirloc);
}

int mparticle::force(const point& /*pt*/, vec& f, vec& f_perp, vfloat& mrange) {
  f = vec(0, 0, 0);
  f_perp = vec(0, 0, 0);
  mrange = max_vfloat;
  return 0;
}

void mparticle::new_speed() {
  pvecerror("void mparticle::new_speed(void)");
  if (currpos.prange == 0.0) {
    check_consistency();
    return;
  }
  vec f1, f2, f_perp1, f_perp2, f_perp_fl1, f_perp_fl2;
  vec f_mean;
  vfloat r1, r2;  // ranges, do not need here
  int i = force(prevpos.pt, f1, f_perp_fl1, r1);
  int j = force(currpos.pt, f2, f_perp_fl2, r2);
  check_econd11a(vecerror, != 0, "position 1, after computing force\n", mcerr);
  f_perp1 = prevpos.speed * (prevpos.dir || f_perp_fl1);
  f_perp2 = currpos.speed * (currpos.dir || f_perp_fl2);
  // Later f_perp are ignored since they can not do the work;
  f_mean = 0.5 * (f1 + f2);
  check_econd11a(vecerror, != 0, "position 2, after computing f_perp\n", mcerr);

  if ((i == 0 && j == 0) || f_mean == dv0) {
    curr_kin_energy = prev_kin_energy;
    curr_gamma_1 = prev_gamma_1;
    // speed is preserved by gparticle
    currpos.speed = prevpos.speed;  
  } else {
    vec r = currpos.pt - prevpos.pt;
    double W = 0;  // force * range * cos() = work * cos() (may be negative)
    if (r != dv0) W = f_mean * r;
    // This is work which should lead to increase or decrease the speed
    if (W == 0) {
      curr_kin_energy = prev_kin_energy;
      curr_gamma_1 = prev_gamma_1;
      currpos.speed = prevpos.speed;
    } else {
      curr_kin_energy = prev_kin_energy + W;
      if (curr_kin_energy <= 0) {
        curr_kin_energy = 0;
        currpos.speed = 0;
        curr_gamma_1 = 0;
        currpos.dir = dv0;
      } else {
        double resten = mass * c_squared;
        curr_gamma_1 = curr_kin_energy / resten;
        currpos.speed = c_light * lorbeta(curr_gamma_1);
      }
    }
  }
  if (!(i == 0 && j == 0)) {
    vec fn1 = project_to_plane(f1, prevpos.dir);  // normal component
    vec fn2 = project_to_plane(f2, currpos.dir);  // normal component
    check_econd11a(vecerror, != 0, "position 3, after computing fn2\n", mcerr);
    // mean ortogonal component of working force
    vec mean_fn = 0.5 * (fn1 + fn2);  
    double mean_fn_len = mean_fn.length();
    vec fdir = prevpos.dir;
    if (mean_fn_len > 0.0) {
      vec relcen = unit_vec(mean_fn);
      double mean_speed = (prevpos.speed + currpos.speed) * 0.5;
      vfloat new_rad = (mean_speed * mean_speed *
                        ((prev_gamma_1 + curr_gamma_1) * 0.5 + 1) * mass) /
                       mean_fn_len;
      if (new_rad > 0.0) {
        vfloat ang = currpos.prange / new_rad;  // angle to turn
        fdir.turn(prevpos.dir || relcen, ang);  // direction at the end
      }
    }
    check_econd11a(vecerror, != 0, "position 4\n", mcerr);
    vec mean_f_perp_fl = 0.5 * (f_perp_fl1 + f_perp_fl2);
    double len_mean_f_perp_fl = mean_f_perp_fl.length();
    f_perp2 = currpos.speed * (currpos.dir || f_perp_fl2);
    double mean_f_perp = 0.5 * (f_perp1.length() + f_perp2.length());
    check_econd11a(vecerror, != 0, "position 5\n", mcerr);
    if (len_mean_f_perp_fl > 0.0) {
      vec fdir_proj = project_to_plane(prevpos.dir, mean_f_perp_fl);
      if (!apeq(fdir_proj.length(), 0.0)) {
        check_econd11a(vecerror, != 0, "position 6\n", mcerr);
        double length_proj = currpos.prange * cos2vec(prevpos.dir, fdir_proj);
        check_econd11a(vecerror, != 0, "position 7\n", mcerr);
        double acc =
            mean_f_perp / (((prev_gamma_1 + curr_gamma_1) * 0.5 + 1) * mass);
        double mean_speed = (prevpos.speed + currpos.speed) * 0.5;
        double new_rad = pow(mean_speed * fdir_proj.length(), 2) / acc;
        double ang = length_proj / new_rad;
        if (new_rad > 0 && ang > 0) {
          fdir.turn(mean_f_perp_fl, -ang);  // direction at the end
          check_econd11a(vecerror, != 0, "position 8\n", mcerr);
        }
      }
    }
    currpos.dir = fdir;
    check_econd11a(vecerror, != 0, "position 9, after turn\n", mcerr);
  }
  currpos.dirloc = currpos.dir;
  currpos.tid.up_absref(&currpos.dirloc);
  currpos.time =
      prevpos.time + currpos.prange / (0.5 * (prevpos.speed + currpos.speed));
  check_consistency();
}
void mparticle::print(std::ostream& file, int l) const {
  if (l < 0)
    Ifile << "mparticle: mass=" << mass << " (" << mass / CLHEP::kg << " kg, "
          << mass* c_squared / CLHEP::GeV << " GeV)\n";
  Ifile << "orig_kin_energy=" << orig_kin_energy << " ("
        << orig_kin_energy / CLHEP::GeV << " GeV)"
        << " orig_gamma_1=" << orig_gamma_1 << '\n';
  Ifile << "prev_kin_energy=" << prev_kin_energy << " ("
        << prev_kin_energy / CLHEP::GeV << " GeV)"
        << " prev_gamma_1=" << prev_gamma_1 << '\n';
  Ifile << "curr_kin_energy=" << curr_kin_energy << " ("
        << curr_kin_energy / CLHEP::GeV << " GeV)"
        << " curr_gamma_1=" << curr_gamma_1 << '\n';
  gparticle::print(file, l);
}

std::ostream& operator<<(std::ostream& file, const mparticle& f) {
  (&f)->print(file, 10);
  return file;
}
}

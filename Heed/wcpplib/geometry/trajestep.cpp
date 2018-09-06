#include "wcpplib/geometry/trajestep.h"
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

void trajestep_limit::range(const int fs_cf0, const vfloat rad, int& fs_cf1,
                            vfloat& mrange) const {
  if (mrange < 0 || mrange > max_range) mrange = max_range;
  fs_cf1 = fs_cf0;
  if (fs_cf1 != 1) return;
  if (rad >= rad_for_straight) {
    fs_cf1 = 0;
    mrange = std::min(mrange, rad * max_straight_arange);
  } else {
    mrange = std::min(mrange, rad * max_circumf_arange);
  }
}

absref absref::*(trajestep::aref[4]) = {(absref absref::*)&trajestep::currpos,
                                        (absref absref::*)&trajestep::dir,
                                        (absref absref::*)&trajestep::relcen,
                                        (absref absref::*)&trajestep::mpoint};

absref_transmit trajestep::get_components() {
  return absref_transmit(4, aref);
}

trajestep::trajestep(trajestep_limit* ftl, const point& fcurrpos,
                     const vec& fdir, int fs_cf, const vec& frelcen,
                     vfloat fmrange, vfloat prec)
    : m_tl(ftl),
      currpos(fcurrpos),
      dir(),
      s_cf(fs_cf),
      relcen(frelcen),
      s_prec(1),
      mrange(fmrange) {
  pvecerror("trajestep::trajestep(...)");
  if (fdir == dv0) {
    dir = dv0;
    mrange = 0;
  } else {
    dir = unit_vec(fdir);
    if (s_cf == 1) {
      check_econd11a(check_perp(dir, relcen, prec), != 1,
                     "dir=" << dir << "relcen=" << relcen
                            << "fcurrpos=" << fcurrpos << "fdir=" << fdir,
                     mcerr);
    }
    m_tl->range(s_cf, relcen.length(), s_range_cf, mrange);
  }
}

trajestep::trajestep(const trajestep& fts, vfloat fmrange) {
  mfunname("trajestep::trajestep(const trajestep& fts, vfloat fmrange)");
  // Continue propagation from the end point of the old step.
  point fpos;
  vec fdir;
  vec frelcen;
  fts.Gnextpoint1(fts.mrange, fpos, fdir, frelcen);
  vfloat prec = 0.1;  // not important here
  *this =
      trajestep(fts.m_tl, fpos, fdir, fts.s_cf, frelcen, fmrange, prec);
}

void trajestep::Gnextpoint(vfloat frange, point& fpos, vec& fdir) const {
  pvecerror("int trajestep::Gnextpoint(vfloat frange, point& fpos, vec& fdir)");
  check_econd12(frange, >, mrange, mcerr);
  if (s_range_cf == 0) {
    // interpolation by straight line
    fpos = currpos + frange * dir;
    if (s_cf == 0) {
      // no curvature
      fdir = dir;
      return;
    } else {
      vfloat ang = frange / relcen.length();
      fdir = dir;
      fdir.turn(dir || relcen, ang);
      return;
    }
  } else {
    vfloat ang = frange / relcen.length();  // angle to turn
    fdir = dir;
    fdir.turn(dir || relcen, ang);  // direction at the end
    vec frelcen = relcen;
    frelcen.turn(dir || relcen, ang);
    fpos = currpos + relcen - frelcen;
    return;
  }
}

void trajestep::Gnextpoint1(vfloat frange, point& fpos, vec& fdir,
                            vec& frelcen) const {
  pvecerror(
      "int trajestep::Gnextpoint(vfloat frange, point& fpos, vec& fdir, "
      "vec& frelcen)");
  check_econd12(frange, >, mrange, mcerr);
  if (s_range_cf == 0) {
    // interpolation by straight line
    fpos = currpos + frange * dir;
    if (s_cf == 0) {
      // no curvature
      fdir = dir;
      frelcen = relcen;  // whatever it is
      return;
    } else {
      vfloat ang = frange / relcen.length();
      fdir = dir;
      fdir.turn(dir || relcen, ang);
      frelcen = relcen;
      frelcen.turn(dir || relcen, ang);
      return;
    }
  } else {
    vfloat ang = frange / relcen.length();  // angle to turn
    fdir = dir;
    fdir.turn(dir || relcen, ang);  // direction at the end
    frelcen = relcen;
    frelcen.turn(dir || relcen, ang);
    fpos = currpos + relcen - frelcen;
    return;
  }
}

std::ostream& operator<<(std::ostream& file, const trajestep& f) {
  Ifile << "trajestep: s_cf=" << f.s_cf << "\n";
  indn.n += 2;
  Ifile << "currpos:" << f.currpos << indn << "dir=" << f.dir << indn
        << "relcen=" << f.relcen << indn << "s_range_cf=" << f.s_range_cf
        << " s_prec=" << f.s_prec << " mrange=" << f.mrange << '\n' << indn
        << "mpoint=" << f.mpoint;
  indn.n -= 2;
  return file;
}
}

#ifndef G_TRACK_BICHSEL_H
#define G_TRACK_BICHSEL_H

#include "Track.hh"

namespace Garfield {

/// Generate tracks using differential cross-sections 
/// for silicon computed by Hans Bichsel.
/// References:
///   - H. Bichsel, Rev. Mod. Phys. 60 (1988), 663-699
///   - https://faculty.washington.edu/hbichsel/

class TrackBichsel : public Track {

 public:
  /// Constructor
  TrackBichsel();
  /// Destructor
  virtual ~TrackBichsel() {}

  virtual bool NewTrack(const double x0, const double y0, const double z0,
                        const double t0, const double dx0, const double dy0,
                        const double dz0);
  virtual bool GetCluster(double& xcls, double& ycls, double& zcls, double& tcls,
                          int& n, double& e, double& extra);

  virtual double GetClusterDensity();
  virtual double GetStoppingPower();

  void SetDataFile(const std::string& filename) { m_datafile = filename; }

 private:
  /// Particle rel. momentum.
  double m_bg;
  /// Particle speed
  double m_speed;

  // Particle position and direction
  double m_x, m_y, m_z, m_t;
  double m_dx, m_dy, m_dz;

  /// Inverse mean free path
  double m_imfp;

  std::string m_datafile;
  /// Table of cumulative distribution functions
  std::vector<std::vector<double> > m_cdf;
  int m_iCdf;
  int m_nCdfEntries;

  bool m_isInitialised;
  bool m_isInMedium;

  double GetInverseMeanFreePath(const double bg);
  bool LoadCrossSectionTable(const std::string& filename);
  void SelectCrossSectionTable();
};
}

#endif

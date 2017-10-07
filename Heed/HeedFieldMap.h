
#ifndef G_HEED_FIELDMAP_H
#define G_HEED_FIELDMAP_H

#include "wcpplib/geometry/vec.h"

namespace Garfield {
class Sensor;
}

namespace Heed {

/// Retrieve electric and magnetic field from Sensor.

class HeedFieldMap {
 public:
  HeedFieldMap() : m_sensor(NULL), m_useEfield(false), m_useBfield(false) {}

  void SetSensor(Garfield::Sensor* sensor) { m_sensor = sensor; }
  void UseEfield(const bool flag) { m_useEfield = flag; }
  void UseBfield(const bool flag) { m_useBfield = flag; }

  void field_map(const point& pt, vec& efield, vec& bfield,
                 vfloat& mrange) const;
  bool inside(const point& pt);

 private:
  /// Conversion factor from mm to cm.
  static const double conv;

  Garfield::Sensor* m_sensor;
  bool m_useEfield;
  bool m_useBfield;
};
}

#endif

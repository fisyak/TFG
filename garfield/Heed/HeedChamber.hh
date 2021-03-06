
#ifndef G_HEED_CHAMBER_H
#define G_HEED_CHAMBER_H

#include "wcpplib/clhep_units/WSystemOfUnits.h"
#include "wcpplib/geometry/box.h"
#include "heed++/code/EnTransfCS.h"
#include "heed++/code/HeedCondElectron.h"
#include "heed++/code/HeedDeltaElectronCS.h"

namespace Garfield {

class HeedChamber : public Heed::sh_manip_absvol,
                    public Heed::box,
                    public Heed::EnTransfCS,
                    public Heed::HeedDeltaElectronCS {

 public:
  HeedChamber(const Heed::abssyscoor& fcsys, const double dx, const double dy,
              const double dz, const Heed::EnTransfCS& etcs,
              const Heed::HeedDeltaElectronCS& hdecs)
      : Heed::sh_manip_absvol(fcsys),
        Heed::box(dx * Heed::CLHEP::cm, 
                  dy * Heed::CLHEP::cm, 
                  dz * Heed::CLHEP::cm, "chamber"),
        Heed::EnTransfCS(etcs),
        Heed::HeedDeltaElectronCS(hdecs) {

    s_sensitive = true;
  }

  virtual Garfield::HeedChamber* copy() const {
    return new Garfield::HeedChamber(*this);
  }
  virtual absvol* Gavol() const { return (Heed::box*)this; }

 protected:
  virtual void get_components(Heed::ActivePtr<Heed::absref_transmit>& aref_tran) {
    sh_manip_absvol::get_components(aref_tran);
  }
};
}

#endif

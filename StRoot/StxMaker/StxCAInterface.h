#ifndef __StxCAInterface_h__
#define __StxCAInterface_h__
#include "StxSeedFinder.h"
#include "TPCCATracker/AliHLTTPCCAGBTracker.h"
#include "TPCCATracker/StTPCCAInterface.h"

class StxCAInterface : public StTPCCAInterface {
 public:
  StxCAInterface();
  virtual ~StxCAInterface() {fgStxCAInterface = 0;}
  static StxCAInterface &Instance();
  virtual void SetNewEvent() {fSeedFinder = 0; fSeeds.clear(); fSeedHits.clear(); StTPCCAInterface::SetNewEvent();}
  virtual vector<Seedx_t> &GetSeeds(){ return fSeeds; };                   // get seeds. Should be called after Run(...).
  virtual vector<SeedHit_t>        &GetSeedHits()    { return fSeedHits;}
  static  void ConvertPars(const AliHLTTPCCATrackParam& caPar, double _alpha, StxNodePars& nodePars, StxNodeErrs& nodeErrs); // convert caPars into NodePars

 protected:
#if 0
  virtual void MakeSettings(); // fill fCaParam
#endif
  virtual void MakeHits();     // fill fCaHits & fSeedHits
  virtual void MakeSeeds();    // fill fSeeds & fTrackParameters

  vector<Seedx_t>         fSeeds;
  StxSeedFinder          *fSeedFinder;
  vector<SeedHit_t>       fSeedHits;          // hits to make seeds
  static StxCAInterface  *fgStxCAInterface;
};
#endif //  __StxCAInterface_h__

#include <limits>

#include "TMath.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StMuDSTMaker/COMMON/StMuBTofPidTraits.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "St_base/StMessMgr.h"

#include "StPicoEvent/StPicoBTofPidTraits.h"


//----------------------------------------------------------------------------------
StPicoBTofPidTraits::StPicoBTofPidTraits() :
  mTrackIndex(-1),
  mBTofCellId(-1), mBTofMatchFlag(-1),
  mBTof(0), mBTofBeta(0),
  mBTofYLocal(-999), mBTofZLocal(-999),
  mBTofHitPosX(-999), mBTofHitPosY(-999), mBTofHitPosZ(-999)
{
  // constructor
}

//----------------------------------------------------------------------------------
StPicoBTofPidTraits::StPicoBTofPidTraits(const StMuTrack* gTrack,
    const StMuTrack* pTrack,
    const Int_t idx2PicoTrack): StPicoBTofPidTraits()
{
  mTrackIndex = (idx2PicoTrack > std::numeric_limits<short>::max()) ? -1 : (Short_t)idx2PicoTrack;

  StMuBTofHit* btofHit = (StMuBTofHit*)gTrack->tofHit();
  Int_t tray         = btofHit->tray();
  Int_t module       = btofHit->module();
  Int_t cell         = btofHit->cell();
  Float_t tof        = gTrack->btofPidTraits().timeOfFlight();
  const StMuBTofPidTraits &btofPid = gTrack->btofPidTraits();
  Float_t beta       = (pTrack) && ! TMath::IsNaN(btofPid.beta()) ? btofPid.beta() : -999.;
  StThreeVectorF tofPoint = btofPid.position();
  // Maksym correction ================================================================================
  double timeTof = btofPid.timeOfFlight();
  double lengthTof = btofPid.pathLength();
  if(lengthTof < 0.)     {
    double dlDCA = 0;
    if (StMuDst::instance()->currentVertexIndex() >= 0) {
      const StThreeVectorF & dcaPoint  = gTrack->dca(StMuDst::instance()->currentVertexIndex()); 
      StPhysicalHelixD innerHelix = gTrack->helix();
      dlDCA = fabs( innerHelix.pathLength( StThreeVector<double>(dcaPoint.x(), dcaPoint.y(), dcaPoint.z()) ) );
    }
    StPhysicalHelixD outerHelix = gTrack->outerHelix();
    double dlTOF = fabs( outerHelix.pathLength( StThreeVector<double>(tofPoint.x(), tofPoint.y(), tofPoint.z()) ) );
    
    double l = gTrack->length();
    lengthTof = l + dlDCA + dlTOF;
  }
  if(timeTof > 0. && lengthTof > 0.)     {
    beta = lengthTof/timeTof/(1e-9*TMath::Ccgs());
  }
  // end of Maksym correction ================================================================================

  mBTofCellId  = (Short_t)((tray - 1) * 192 + (module - 1) * 6 + (cell - 1));
  mBTofMatchFlag = (UChar_t)(btofPid.matchFlag());
  if (tof < 0)
  {
    mBTof = 0;
  }
  else
  {
    mBTof = (tof * 1000. > std::numeric_limits<unsigned short>::max()) ? std::numeric_limits<unsigned short>::max() : (UShort_t)(TMath::Nint(tof * 1000.));
  }
  if (beta < 0)
  {
    mBTofBeta = 0;
  }
  else
  {
    mBTofBeta = (beta * 20000. > std::numeric_limits<unsigned short>::max()) ? std::numeric_limits<unsigned short>::max() : (UShort_t)(TMath::Nint(beta * 20000.));
  }
  mBTofHitPosX = (fabs(tofPoint.x() * 100.) > std::numeric_limits<short>::max()) ? std::numeric_limits<short>::max() : (Short_t)(TMath::Nint(tofPoint.x() * 100.));
  mBTofHitPosY = (fabs(tofPoint.y() * 100.) > std::numeric_limits<short>::max()) ? std::numeric_limits<short>::max() : (Short_t)(TMath::Nint(tofPoint.y() * 100.));
  mBTofHitPosZ = (fabs(tofPoint.z() * 100.) > std::numeric_limits<short>::max()) ? std::numeric_limits<short>::max() : (Short_t)(TMath::Nint(tofPoint.z() * 100.));
  mBTofYLocal  = (fabs(btofPid.yLocal()) * 1000. > std::numeric_limits<short>::max()) ? std::numeric_limits<short>::max() : (Short_t)(TMath::Nint(btofPid.yLocal() * 1000.));
  mBTofZLocal  = (fabs(btofPid.zLocal()) * 1000. > std::numeric_limits<short>::max()) ? std::numeric_limits<short>::max() : (Short_t)(TMath::Nint(btofPid.zLocal() * 1000.));
}

//----------------------------------------------------------------------------------
StPicoBTofPidTraits::~StPicoBTofPidTraits()
{
  // destructor
}

//----------------------------------------------------------------------------------
void StPicoBTofPidTraits::Print(const Char_t* option) const
{
  LOG_INFO << " Matched track index = " << mTrackIndex << endm;
  LOG_INFO << " BTOF cellId = " << btofCellId() << " tof = " << btof() << " beta = " << btofBeta() << endm;
  LOG_INFO << " BTOF match = " << btofMatchFlag() << " yLocal/zLocal " << btofYLocal() << " " << btofZLocal() << endm;
}

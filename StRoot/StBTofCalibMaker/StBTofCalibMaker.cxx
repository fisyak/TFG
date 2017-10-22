/*******************************************************************
 *
 * $Id: StBTofCalibMaker.cxx,v 1.17 2017/10/20 17:50:32 smirnovd Exp $
 *
 * Author: Xin Dong
 *****************************************************************
 *
 * Description: - Tof Calibration Maker to do the calibration for pVPD 
 *              (start timing) , TOF
 *              - store into StBTofPidTraits
 *
 *****************************************************************
 *
 * $Log: StBTofCalibMaker.cxx,v $
 * Revision 1.17  2017/10/20 17:50:32  smirnovd
 * Squashed commit of the following:
 *
 *     StBTof: Remove outdated ClassImp macro
 *
 *     Prefer explicit namespace for std:: names in header files
 *
 *     Removed unnecessary specification of default std::allocator
 *
 * Frank signed-off
 *
 * Revision 1.16  2017/03/02 18:30:44  jeromel
 * Changes by jdb, nl - inData.open() of files on live disk TBF later
 *
 * Revision 1.10 2016/11/14 11:32:15  nluttrel
 * Simulated hits no longer undergo electronics corrections
 * If StVpdSimMaker used in chain, defaults to use Vpd start time
 *
 * Revision 1.15  2016/06/30 17:09:59  jdb
 * Fixed Several errors identified by Coverity
 *
 * Revision 1.14  2011/07/27 15:44:32  geurts
 * minor bug update: mProjVtxZ does not get initialized when mUseEventVertex is false, but is printed regardless.
 *
 * Revision 1.13  2011/05/09 14:32:10  geurts
 * use appropriate log level for debug messages
 *
 * Revision 1.12  2010/10/31 05:52:11  geurts
 * fixed module index range for read in loop for BOARD (TDIG) based calibration
 *
 * Revision 1.11  2010/10/30 05:20:50  geurts
 * Calibration Maker reads (file/dbase) in and applies cell-based, module-based, or board-based (TDIG) calibration parameters
 *
 * Revision 1.10  2010/05/27 21:41:14  geurts
 * Pick the default primary vertex (for mUseEventVertex). Additional cuts in selecting the vertex for tstart() have been removed.
 *
 * Revision 1.9  2010/05/25 22:09:18  geurts
 * improved database handling and reduced log output
 *
 * Revision 1.8  2010/05/12 22:46:21  geurts
 * Startless BTOF self-calibration method (Xin)
 *
 * Revision 1.7  2010/04/29 03:42:37  dongx
 * Remove ranking>0 cut in event vertex selection for start time calculation
 *
 * Revision 1.6  2010/04/09 21:26:51  geurts
 * Introduced "UseProjectedVertex" maker attribute to allow selection of the
 * standard event vertex or one determined by track extrapolation
 * (typically used in pp collisions).
 *
 * Revision 1.5  2010/04/03 15:43:58  dongx
 * Change the default to use event vertex for start position for Run10 AuAu
 *
 * Revision 1.4  2010/03/04 23:10:20  dongx
 * Added cleanup for PID variables in MuBTofPidTraits when processMuDst()
 *
 * Revision 1.3  2009/12/04 22:26:34  geurts
 * Split original CalibMaker into dedicated StVpdCalibMaker and BTOF-specific StBTofCalibMaker (Xin):
 * - function added to directly access the MuDst
 * - clean up those VPD members and functions as they are moved to the StVpdCalibMaker
 * - add VPD related functions to load/write the calibration VPD information in the BTofHeader
 * - few small algorithm updates to be consistent with what is used in calibration procedures
 * - several minor code cleanups
 *
 * Revision 1.2  2009/11/21 00:29:52  geurts
 * Dtabase readout made more robust, static const moved to cxx.
 *
 * Revision 1.1  2009/09/23 02:28:41  geurts
 * first version: Combined BTOF & VPD CalibMaker
 *
 *
 *******************************************************************/
#include <iostream>
#include "StEvent.h"
#include "StBTofCollection.h"
#include "StBTofHit.h"
#include "StBTofHeader.h"
#include "StBTofPidTraits.h"
#include "StEventTypes.h"
#include "Stypes.h"
#include "StThreeVectorD.hh"
#include "StHelix.hh"
#include "StTrackGeometry.h"
#include "StTrackPidTraits.h"
#include "StEventUtilities/StuRefMult.hh"
#include "PhysicalConstants.h"
#include "StPhysicalHelixD.hh"
#include "StDetectorDbMaker/St_tofTOffsetC.h"
#include "StDetectorDbMaker/St_tofTotbCorrC.h"
#include "StDetectorDbMaker/St_tofZbCorrC.h"

#include "StDetectorDbMaker/St_vertexSeedC.h"

#include "StBTofUtil/tofPathLength.hh"
#include "StBTofUtil/StBTofHitCollection.h"
#include "StBTofUtil/StBTofGeometry.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuBTofHit.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuBTofPidTraits.h"

#include "StBTofCalibMaker.h"
#include "StVpdCalibMaker/StVpdCalibMaker.h"


 /// Very High resolution mode, pico-second per bin
const Double_t StBTofCalibMaker::VHRBIN2PS =  24.4140625; // 1000*25/1024 (ps/chn)
/// High resolution mode, pico-second per bin                                                       
const Double_t StBTofCalibMaker::HRBIN2PS = 97.65625; // 97.65625= 1000*100/1024  (ps/chn)  
/// tdc limit                                          
const Double_t StBTofCalibMaker::TMAX = 51200.;           
///   VzVpd - VzProj cut
const Double_t StBTofCalibMaker::VZDIFFCUT=6.;          
///   DCAR cut
const Double_t StBTofCalibMaker::DCARCUT=1.;
const Double_t StBTofCalibMaker::mC_Light = C_C_LIGHT/1.e9;


//_____________________________________________________________________________
StBTofCalibMaker::StBTofCalibMaker(const char *name) : StMaker(name)
{
  /// default constructor
  /// set the default parameters for TDC, ADC cut etc.
  /// Reset the calibration parameters
  setVPDHitsCut(1,1);
  setOuterGeometry(true);

  mEvent = 0;
  mBTofHeader = 0;
  mMuDst = 0;
  mZCalibType = NOTSET;
  mTotCalibType = NOTSET;

  mSlewingCorr = kTRUE;
  mMuDstIn = kFALSE;
  mUseVpdStart = kTRUE;
#if 0
  setCreateHistoFlag(kFALSE);
  //  setHistoFileName("btofcalib.root");
#endif


  StThreeVectorD MomFstPt(0.,0.,9999.);
  StThreeVectorD origin(0.,0.,0.);
  mBeamHelix = new StPhysicalHelixD(MomFstPt,origin,0.5*tesla,1.);
}

//_____________________________________________________________________________
StBTofCalibMaker::~StBTofCalibMaker()
{  /* noop */ }

//_____________________________________________________________________________
void StBTofCalibMaker::resetPars()
{
}

//_____________________________________________________________________________
void StBTofCalibMaker::resetVpd()
{
  memset(mVPDLeTime, 0, sizeof(mVPDLeTime));
  mTSumWest = 0;
  mTSumEast = 0;
  mTStart = -9999.;
  mTDiff  = -9999.;
  mProjVtxZ = -9999.;
  mVPDVtxZ = -9999.;
  mVPDHitPatternEast = 0;
  mVPDHitPatternWest = 0;
  mNEast = 0;
  mNWest = 0;
  mValidStartTime = kFALSE;
  mNTzero = 0;
}

//____________________________________________________________________________
Int_t StBTofCalibMaker::Init()
{
  resetPars();
  resetVpd();

  mUseEventVertex = ! IAttr("UseProjectedVertex");
  if (mUseEventVertex) {
    LOG_INFO << "Use event vertex position." << endm;
  } else {
    LOG_INFO << "Use projected vertex position." << endm;
  }
#if 0
  // m_Mode can be set by SetMode() method
  if(m_Mode) {
//    setHistoFileName("btofcalib.root");
  } else {
    setHistoFileName("");
  }

  if (mHisto){
    bookHistograms();
    LOG_INFO << "Histograms are booked" << endm;
    if (mHistoFileName!="") {
      LOG_INFO << "Histograms will be stored in " << mHistoFileName.c_str() << endm;
    }
  }
#endif
  return kStOK;
}

//____________________________________________________________________________
Int_t StBTofCalibMaker::InitRun(int runnumber)
{
  // tof run configurations

  /// retrieve the BTOF calibration parameters (from database or file)
  Int_t val = initParameters(runnumber);
  if(val==kStOK) {
    mValidCalibPar = kTRUE;
    LOG_DEBUG << "Initialized valid calibration parameters." << endm;
  } else {
    mValidCalibPar = kFALSE;
    LOG_WARN << "No valid calibration parameters! " << endm;
  }

  /// Look for StVpCalibMaker and decide on its setting (based on its dbase entry) to use VPD for TOF start-timing
  StVpdCalibMaker *vpdCalib = (StVpdCalibMaker *)GetMaker("vpdCalib");
  if(vpdCalib) {
    mUseVpdStart = vpdCalib->useVpdStart();
    if (mUseVpdStart) {LOG_INFO << "Found VPD Calibration Maker: use vpd for start timing" << endm;}
    else         {LOG_INFO << "Found VPD Calibration Maker: vpd **NOT** used for start timing" << endm;}
  } else {
    mUseVpdStart = kFALSE;  // no vpdCalibMaker, do tray self calibration
    LOG_INFO << "NO VPD Calibration Maker found:  vpd **NOT** used for start timing" << endm;
  }

  /// If no VPD is used then one should have selected to use the EventVertex from the TPC, and warn if not.
  if(!mUseVpdStart && !mUseEventVertex) {
    LOG_WARN << " Try to run calibration without VPD as the start time and DON'T use the event vertex! Wrong command! Exit!" << endm;
    return kStOK;
  }


  return kStOK;

}

//_____________________________________________________________________________
Int_t StBTofCalibMaker::initParameters(int runnumber)
{
  
  // ========== Set Beam Line =====================
  St_vertexSeedC* vSeed = St_vertexSeedC::instance();
  Float_t x0   = vSeed->x0()  ; 
  Float_t y0   = vSeed->y0()  ; 
  Float_t dxdz = vSeed->dxdz(); 
  Float_t dydz = vSeed->dydz(); 
  LOG_INFO << "BeamLine Constraint: " << endm;
  LOG_INFO << "x(z) = " << x0 << " + " << dxdz << " * z" << endm;
  LOG_INFO << "y(z) = " << y0 << " + " << dydz << " * z" << endm;
  StThreeVectorD origin(x0,y0,0.0);
  double pt = 88889999;
  double nxy=::sqrt(dxdz*dxdz +  dydz*dydz);
  if(nxy<1.e-5){ // beam line _MUST_ be tilted
    LOG_WARN << "Beam line must be tilted!" << endm;
    nxy=dxdz=1.e-5;
  }
  double p0=pt/nxy;
  double px   = p0*dxdz;
  double py   = p0*dydz;
  double pz   = p0; // approximation: nx,ny<<0
  StThreeVectorD MomFstPt(px*GeV, py*GeV, pz*GeV);
  if(mBeamHelix) delete mBeamHelix;
  mBeamHelix = new StPhysicalHelixD(MomFstPt,origin,0.5*tesla,1.);
  
  
  return kStOK;
}

//____________________________________________________________________________
Int_t StBTofCalibMaker::FinishRun(int runnumber)
{
  if(mBeamHelix) delete mBeamHelix;  
  mBeamHelix = 0;
  
  return kStOK;
}

//_____________________________________________________________________________
Int_t StBTofCalibMaker::Finish()
{
#if 0
  if (mHistoFileName!="") writeHistograms();
#endif
  return kStOK;
}

//_____________________________________________________________________________
Int_t StBTofCalibMaker::Make()
{
  LOG_DEBUG << "StBTofCalibMaker::Maker: starting ..." << endm;
  if(!mValidCalibPar) {
    LOG_WARN << "No valid calibration parameters. Skip ... " << endm;
    return kStOK;
  }
  
  initEvent();
  resetVpd();
  if(mUseVpdStart) {
    loadVpdData();
  }
  
  if(!mMuDstIn) processStEvent();
  else          processMuDst();
  
  writeStartTime();
  
  return kStOK;
}

//_____________________________________________________________________________
void StBTofCalibMaker::processStEvent()
{
  // event selection  // no primary vertex required
  if( !mEvent ) {LOG_WARN << "No StEvent" << endm; return;}
  if (!mEvent->btofCollection()) {LOG_WARN << "No BTOFCollection" << endm; return;}
  if (!mEvent->btofCollection()->hitsPresent()) {LOG_WARN << "No hits present" << endm; return;}
  
  StBTofCollection *theTof = mEvent->btofCollection();
  StSPtrVecBTofHit &tofHits = theTof->tofHits();
  Int_t nhits = tofHits.size();
  LOG_INFO << " Fired TOF cells + upVPD tubes : " << nhits << endm;
  
  if(mUseVpdStart) {
    
    mEvtVtxZ = -9999.;
    mProjVtxZ = -9999.;
    float dcaRmin = 9999.;   
    //    float rankHmax = -1.e9;
    
    if(mUseEventVertex) {
      //      ///
      //      /// select the vertex with highest positive rank within the VPDVtxZ cut range
      //      ///
      //       int nVtx = mEvent->numberOfPrimaryVertices();
      //       for(int i=0;i<nVtx;i++) {
      //         StPrimaryVertex *pVtx = mEvent->primaryVertex(i);
      //         if(!pVtx) continue;
      // //        if(pVtx->ranking()<0) continue;               //! select positive ranking vertex
      //         if(fabs(pVtx->position().z())>200.) continue;   //! within 200 cm
      //         if(fabs(pVtx->position().z()-mVPDVtxZ)>VZDIFFCUT) continue;  //! VPDVtxZ cut
      //         if(pVtx->ranking()<rankHmax) continue;
      //         mEvtVtxZ = pVtx->position().z();
      //         rankHmax = pVtx->ranking();
      //       }
      /// Select default primary vertex.
      //  (Future version should allow for non-default vertex selections)
      StPrimaryVertex *pVtx = mEvent->primaryVertex();
      if (pVtx){
        mEvtVtxZ = pVtx->position().z();
      }
      else {
        LOG_WARN << "No (default) primary vertex information for this (st-) event"  << endm;
      };
      
      tstart(mEvtVtxZ, &mTStart, &mTDiff);
      
    } else {
      ///
      /// select the projection position with smallest dcaR within the VPDVtxZ cut range
      ///
      for(int i=0;i<nhits;i++) {
        StBTofHit *aHit = dynamic_cast<StBTofHit*>(tofHits[i]);
        if(!aHit) continue;
        int trayId = aHit->tray();
        if(trayId>0&&trayId<=St_tofCorrC::mNTray) {
          StGlobalTrack *gTrack = dynamic_cast<StGlobalTrack*>(aHit->associatedTrack());
          if(!gTrack) continue;
          StTrackGeometry *theTrackGeometry = gTrack->geometry();
	  
          StThreeVectorD tofPos =  theTrackGeometry->helix().at(theTrackGeometry->helix().pathLengths(*mBeamHelix).first);
          StThreeVectorD beamPos = mBeamHelix->at(theTrackGeometry->helix().pathLengths(*mBeamHelix).second);
          StThreeVectorD dcatof = tofPos - beamPos;
	  
	  LOG_DEBUG<<" tofPos(x,y,z) = "<<tofPos.x()<<","<<tofPos.y()<<","<<tofPos.z()<<endm;
	  LOG_DEBUG<<"beamPos(x,y,z) = "<<beamPos.x()<<","<<beamPos.y()<<","<<beamPos.z()<<endm;
	  LOG_DEBUG<<"  dca  (x,y,z) = "<<dcatof.x()<<","<<dcatof.y()<<","<<dcatof.z()<<endm;
	  LOG_DEBUG<<" 2D dca        = "<<sqrt(pow(dcatof.x(),2)+pow(dcatof.y(),2))<<endm;
	  LOG_DEBUG<<" 2D signed dca = "<<theTrackGeometry->helix().geometricSignedDistance(beamPos.x(),beamPos.y())<<endm;
	  
          /// track projection z should be close to vzvpd
	    if(fabs(tofPos.z()-mVPDVtxZ)>VZDIFFCUT) continue;
	    
	    if(dcaRmin>dcatof.perp()) {
	      mProjVtxZ = tofPos.z();
	      dcaRmin = dcatof.perp();
	    }
        } // end if
      } // end loop tofhits
      
      if(dcaRmin>DCARCUT)  mProjVtxZ = -9999.;  // beam line contrain
      tstart(mProjVtxZ, &mTStart, &mTDiff);
      
    } // end if (mUseEventVertex)
    
  } else {   // Don't use VPD as the start time
    
    StPrimaryVertex *pVtx = mEvent->primaryVertex();
    if(!pVtx) {
      LOG_WARN << " No primary vertex ... bye-bye" << endm;
      return;
    }
    mEvtVtxZ = pVtx->position().z();
    
    tstart_NoVpd(theTof, pVtx, &mTStart);
    
  }  // end if(mUseVpdStart)
  
  LOG_INFO << "primVz = " << mEvtVtxZ << " projVz = " << mProjVtxZ << "  vpdVz = " << mVPDVtxZ << endm;
  LOG_INFO << "Tstart = " << mTStart << " Tdiff = " << mTDiff  << "  NTzero = " << mNTzero << endm;
  LOG_INFO << "NWest = " << mNWest << " NEast = " << mNEast << " TdcSum West = " << mTSumWest << " East = " << mTSumEast << endm;
  
  
  if(mTStart<-1000.) {
    if (IAttr("UseMCTstart")) {
      mTStart = 0;
      LOG_INFO << "use MC start time = " << mTStart << " for this event." << endm;
      mValidStartTime = kTRUE;
    } else {
      LOG_INFO << "No valid start time for this event. Skip ..." << endm;
      mValidStartTime = kFALSE;
      return;
    }
  } else {
    mValidStartTime = kTRUE;
  }
  
  //---------------------------------------
  // BTof calibration
  //---------------------------------------
  
  for(int i=0;i<nhits;i++) {
    StBTofHit *aHit = dynamic_cast<StBTofHit*>(tofHits[i]);
    if(!aHit) continue;
    int trayId = aHit->tray();
    if(trayId<=0 || trayId>St_tofCorrC::mNTray) continue;
    
    StGlobalTrack *gTrack = dynamic_cast<StGlobalTrack*>(aHit->associatedTrack());
    if(!gTrack) {
      LOG_DEBUG << " No associated Track with this hit." << endm;
      continue;
    }
    
    const StPtrVecTrackPidTraits& theTofPidTraits = gTrack->pidTraits(kTofId);
    if(!theTofPidTraits.size()) continue;
    
    StTrackPidTraits *theSelectedTrait = theTofPidTraits[theTofPidTraits.size()-1];
    if(!theSelectedTrait) continue;
    
    StBTofPidTraits *pidTof = dynamic_cast<StBTofPidTraits *>(theSelectedTrait);
    if(!pidTof) continue;
    
    double tot = aHit->tot(); // ns
    double tdc = aHit->leadingEdgeTime();
    double tof = tdc - mTStart; 
    Double_t zhit = pidTof->zLocal();
    
    int moduleChan = (aHit->module()-1)*6 + (aHit->cell()-1);
    Double_t tofcorr = tofAllCorr(tof, tot, zhit, trayId, moduleChan);
    if(tofcorr<0.) {
      LOG_DEBUG << " Calibration failed! ... " << endm;
      continue;
    }
    
    pidTof->setTimeOfFlight((Float_t)tofcorr);
    
    /// find the primary track if any
      StPrimaryTrack *pTrack = dynamic_cast<StPrimaryTrack *>(gTrack->node()->track(primary));
      StBTofPidTraits *ppidTof = 0;
      if(pTrack) {
	const StPtrVecTrackPidTraits& pTofPidTraits = pTrack->pidTraits(kTofId);
	if(!pTofPidTraits.size()) continue;
	
	StTrackPidTraits *pSelectedTrait = pTofPidTraits[pTofPidTraits.size()-1];
	if(!pSelectedTrait) continue;
	
	ppidTof = dynamic_cast<StBTofPidTraits *>(pSelectedTrait);
	
	if(ppidTof && mUseEventVertex) {
	  ppidTof->setTimeOfFlight((Float_t)tofcorr);
	}
      }
      
      /// PID calculation if the track is a "primary" track.
	Double_t L = -9999.;
	Double_t ptot = -9999.;
	Bool_t doPID = kFALSE;     //! switch indicating to calculate PID or not
	if(mUseEventVertex) {
	  if(!pTrack) {
	    LOG_DEBUG << " The associated track is not a primary one. Skip PID calculation! " << endm;
	  } else {
	    StTrackGeometry *theTrackGeometry = pTrack->geometry();
	    const StVertex *thisVertex = pTrack->vertex();
	    if(!thisVertex) {
	      LOG_DEBUG << " The associated track is not coming from any vertex. Skip PID calculation! " << endm;
	    } else {
	      StThreeVectorF primPos = thisVertex->position();
	      L = tofPathLength(&primPos, &pidTof->position(), theTrackGeometry->helix().curvature());
	      ptot = pTrack->geometry()->momentum().mag();
	      doPID = kTRUE;
	    }
	  }
	  
	} else {
	  
	  StTrackGeometry *theTrackGeometry = gTrack->geometry();
	  StThreeVectorD tofPos =  theTrackGeometry->helix().at(theTrackGeometry->helix().pathLengths(*mBeamHelix).first);
	  StThreeVectorD dcatof = tofPos - mBeamHelix->at(theTrackGeometry->helix().pathLengths(*mBeamHelix).second);
	  if(dcatof.perp()>DCARCUT) {
	    LOG_DEBUG << " The projected position is far from beam line. Skip PID calculation! " << endm;
	  } else if(fabs(tofPos.z()-mVPDVtxZ)>VZDIFFCUT) {
	    LOG_DEBUG << " This track is not coming from the same VPD vertex! Skip PID calculation! " << endm; 
	  } else {
	    L = tofPathLength(&tofPos, &pidTof->position(), theTrackGeometry->helix().curvature());
	    ptot = gTrack->geometry()->momentum().mag();
	    if(gTrack->dcaGeometry()) {
	      ptot = gTrack->dcaGeometry()->momentum().mag();
	    }
	    doPID = kTRUE;
	  }
	  
	}
	
	if(!doPID) continue;
	
	Double_t beta = L/(tofcorr*(C_C_LIGHT/1.e9));
	
	Double_t b_e  = ptot/sqrt(ptot*ptot+M_ELECTRON*M_ELECTRON);
	Double_t b_pi = ptot/sqrt(ptot*ptot+M_PION_PLUS*M_PION_PLUS);
	Double_t b_k  = ptot/sqrt(ptot*ptot+M_KAON_PLUS*M_KAON_PLUS);
	Double_t b_p  = ptot/sqrt(ptot*ptot+M_PROTON*M_PROTON);
	
	float sigmae = -9999.;
	float sigmapi = -9999.;
	float sigmak = -9999.; 
	float sigmap = -9999.; 
	float res = 0.013;  // 0.013 by default - 1/beta resolution
	if(fabs(res)>1.e-5) {
	  sigmae = (Float_t)((1./beta-1./b_e)/res);
	  sigmapi = (Float_t)((1./beta-1./b_pi)/res);
	  sigmak = (Float_t)((1./beta-1./b_k)/res);  
	  sigmap = (Float_t)((1./beta-1./b_p)/res);  
	}
	
	pidTof->setPathLength((Float_t)L);
	pidTof->setBeta((Float_t)beta);
	pidTof->setSigmaElectron(sigmae);
	pidTof->setSigmaPion(sigmapi);
	pidTof->setSigmaKaon(sigmak);
	pidTof->setSigmaProton(sigmap);
	
	LOG_DEBUG << " storing BTofPidTraits for the global track" << endm;
	
	if(mUseEventVertex) {
	  
	  if(ppidTof) {
	    
	    ppidTof->setPathLength((Float_t)L);
	    ppidTof->setBeta((Float_t)beta);
	    ppidTof->setSigmaElectron(sigmae);
	    ppidTof->setSigmaPion(sigmapi);
	    ppidTof->setSigmaKaon(sigmak);
	    ppidTof->setSigmaProton(sigmap);
	    
	    LOG_DEBUG << " storing BTofPidTraits for the primary track" << endm;
	  } // end if ppidTof
	}  // end if mUseEventVertex
	
  }  // end tof hits
  
  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::processMuDst()
{
  if(!mMuDst) {
    LOG_WARN << " No MuDst ... bye-bye" << endm;
    return;
  }
  
  cleanCalibMuDst();
  
  Int_t nhits = mMuDst->numberOfBTofHit();
  LOG_INFO << " Fired TOF cells + upVPD tubes : " << nhits << endm;
  
  if(mUseVpdStart) {
    
    mEvtVtxZ  = -9999.;
    mProjVtxZ = -9999.;
    float dcaRmin = 9999.;
    //    float rankHmax = -1.e9;
    
    if(mUseEventVertex) {
      //      ///
      //      /// select the vertex with highest positive rank within the VPDVtxZ cut range
      //      ///
      //       int nVtx = mMuDst->numberOfPrimaryVertices();
      //       for(int i=0;i<nVtx;i++) {
      //         StMuPrimaryVertex* pVtx = mMuDst->primaryVertex(i);
      //         if(!pVtx) continue;
      // //        if(pVtx->ranking()<0) continue;               //! select positive ranking vertex
      //         if(fabs(pVtx->position().z())>200.) continue;   //! within 200 cm
      //         if(fabs(pVtx->position().z()-mVPDVtxZ)>VZDIFFCUT) continue;  //! VPDVtxZ cut
      //         if(pVtx->ranking()<rankHmax) continue;
      //         mEvtVtxZ = pVtx->position().z();
      //         rankHmax = pVtx->ranking();
      //       }
      /// Select default primary vertex.
      //  (Future version should allow for non-default vertex selections)
      StMuPrimaryVertex* pVtx = mMuDst->primaryVertex();
      if (pVtx){
        mEvtVtxZ = pVtx->position().z();
      }
      else {
	LOG_WARN << "No (default) primary vertex information for this (mudst) event"  << endm;
      }
      
      tstart(mEvtVtxZ, &mTStart, &mTDiff);
      
    } else {
      ///
      /// select the projection position with smallest dcaR within the VPDVtxZ cut range
      ///
      for(int i=0;i<nhits;i++) {
        StMuBTofHit *aHit = (StMuBTofHit*)mMuDst->btofHit(i);
        if(!aHit) continue;
        int trayId = aHit->tray();
        if(trayId>0&&trayId<=St_tofCorrC::mNTray) {
          StMuTrack *gTrack = aHit->globalTrack();
          if(!gTrack) continue;
	  
          StPhysicalHelixD thisHelix = gTrack->helix();
	  
          StThreeVectorD tofPos =  thisHelix.at(thisHelix.pathLengths(*mBeamHelix).first);
          StThreeVectorD dcatof = tofPos - mBeamHelix->at(thisHelix.pathLengths(*mBeamHelix).second);
	  
          /// track projection z should be close to vzvpd
	    if(fabs(tofPos.z()-mVPDVtxZ)>VZDIFFCUT) continue;
	    
	    if(dcaRmin>dcatof.perp()) {
	      mProjVtxZ = tofPos.z();
	      dcaRmin = dcatof.perp();
	    }
        } // end if
      } // end loop tofhits
      
      if(dcaRmin>DCARCUT)  mProjVtxZ = -9999.;  // beam line contrain
      tstart(mProjVtxZ, &mTStart, &mTDiff);
      
    } // end if(mUseEventVertex)
  } else { // don't use vpd as the start time
    
    StMuPrimaryVertex *pVtx = mMuDst->primaryVertex();
    if(!pVtx) {
      LOG_WARN << " No primary vertex ... bye-bye" << endm;
      return;
    }
    mEvtVtxZ = pVtx->position().z();
    
    tstart_NoVpd(mMuDst, pVtx, &mTStart);
  }
  
  LOG_INFO << "primVz = " << mEvtVtxZ << " projVz = " << mProjVtxZ << "  vpdVz = " << mVPDVtxZ  << endm;
  LOG_INFO << "Tstart = " << mTStart << " Tdiff = " << mTDiff << "  NTzero = " << mNTzero << endm;
  LOG_INFO << "NWest = " << mNWest << " NEast = " << mNEast << " TdcSum West = " << mTSumWest << " East = " << mTSumEast << endm;
  
  
  if(mTStart<-1000.) {
    LOG_INFO << " No valid start time for this event. Skip ..." << endm;
    mValidStartTime = kFALSE;
    return;
  } else {
    mValidStartTime = kTRUE;
  }
  
  //---------------------------------------
  // BTof calibration
  //---------------------------------------
  
  for(int i=0;i<nhits;i++) {
    StMuBTofHit *aHit = (StMuBTofHit*)mMuDst->btofHit(i);
    if(!aHit) continue;
    int trayId = aHit->tray();
    if(trayId<=0 || trayId>St_tofCorrC::mNTray) continue;
    
    StMuTrack *gTrack = aHit->globalTrack();
    if(!gTrack) {
      LOG_DEBUG << " No associated Track with this hit." << endm;
      continue;
    }
    
    StMuBTofPidTraits pidTof = gTrack->btofPidTraits();
    
    double tot = aHit->tot(); // ns
    double tdc = aHit->leadingEdgeTime();
    while(tdc>TMAX) tdc -= TMAX;
    double tof = tdc - mTStart;
    Double_t zhit = pidTof.zLocal();
    
    int moduleChan = (aHit->module()-1)*6 + (aHit->cell()-1);
    Double_t tofcorr = tofAllCorr(tof, tot, zhit, trayId, moduleChan);
    if(tofcorr<0.) {
      LOG_DEBUG << " Calibration failed! ... " << endm;
      continue;
    }
    
    /// store the corrected tof information for all global matches
      pidTof.setTimeOfFlight((Float_t)tofcorr);
      
      /// find the primary track if any
	StMuTrack *pTrack = aHit->primaryTrack();
	StMuBTofPidTraits ppidTof;
	if(pTrack) {
	  ppidTof = pTrack->btofPidTraits();
	  if(mUseEventVertex) ppidTof.setTimeOfFlight((Float_t)tofcorr);
	}
	
	/// PID calculation if the track is a "primary" track.
	  Double_t L = -9999.;
	  Double_t ptot = -9999.;
	  Bool_t doPID = kFALSE;
	  if(mUseEventVertex) {
	    if(!pTrack) {
	      LOG_DEBUG << " The associated track is not a primary one. Skip PID calculation! " << endm;
	    } else {
	      int iv = pTrack->vertexIndex();
	      StMuPrimaryVertex *thisVertex = mMuDst->primaryVertex(iv);
	      if(!thisVertex) {
		LOG_DEBUG << " The associated track is not coming from any vertex. Skip PID calculation! " << endm;
	      } else {
		StThreeVectorF primPos = thisVertex->position();
		StPhysicalHelixD thisHelix = pTrack->helix();
		L = tofPathLength(&primPos, &pidTof.position(), thisHelix.curvature());
		ptot = pTrack->momentum().mag();
		doPID = kTRUE;
	      }
	    }
	    
	  } else {
	    
	    StPhysicalHelixD gHelix = gTrack->helix();
	    StThreeVectorD tofPos =  gHelix.at(gHelix.pathLengths(*mBeamHelix).first);
	    StThreeVectorD dcatof = tofPos - mBeamHelix->at(gHelix.pathLengths(*mBeamHelix).second);
	    if(dcatof.perp()>DCARCUT) {
	      LOG_DEBUG << " The projected position is far from beam line. Skip PID calculation! " << endm;
	    } else if(fabs(tofPos.z()-mVPDVtxZ)>VZDIFFCUT) {
	      LOG_DEBUG << " This track is not coming from the same VPD vertex! Skip PID calculation! " << endm;
	    } else {
	      L = tofPathLength(&tofPos, &pidTof.position(), gHelix.curvature());
	      ptot = gTrack->momentum().mag();
	      doPID = kTRUE;
	    }
	  }
	  
	  if(doPID) {
	    Double_t beta = L/(tofcorr*(C_C_LIGHT/1.e9));
	    
	    Double_t b_e  = ptot/sqrt(ptot*ptot+M_ELECTRON*M_ELECTRON);
	    Double_t b_pi = ptot/sqrt(ptot*ptot+M_PION_PLUS*M_PION_PLUS);
	    Double_t b_k  = ptot/sqrt(ptot*ptot+M_KAON_PLUS*M_KAON_PLUS);
	    Double_t b_p  = ptot/sqrt(ptot*ptot+M_PROTON*M_PROTON);
	    
	    float sigmae = -9999.;
	    float sigmapi = -9999.;
	    float sigmak = -9999.;
	    float sigmap = -9999.;
	    float res = 0.013;  // 0.013 by default - 1/beta resolution
	    if(fabs(res)>1.e-5) {
	      sigmae = (Float_t)((1./beta-1./b_e)/res);
	      sigmapi = (Float_t)((1./beta-1./b_pi)/res);
	      sigmak = (Float_t)((1./beta-1./b_k)/res);
	      sigmap = (Float_t)((1./beta-1./b_p)/res);
	    }
	    
	    pidTof.setPathLength((Float_t)L);
	    pidTof.setBeta((Float_t)beta);
	    pidTof.setSigmaElectron(sigmae);
	    pidTof.setSigmaPion(sigmapi);
	    pidTof.setSigmaKaon(sigmak);
	    pidTof.setSigmaProton(sigmap);
	    
	    if(mUseEventVertex && pTrack) {
	      
	      ppidTof.setPathLength((Float_t)L);
	      ppidTof.setBeta((Float_t)beta);
	      ppidTof.setSigmaElectron(sigmae);
	      ppidTof.setSigmaPion(sigmapi);
	      ppidTof.setSigmaKaon(sigmak);
	      ppidTof.setSigmaProton(sigmap);
	    }
	  }
	  
	  gTrack->setBTofPidTraits(pidTof);
	  LOG_DEBUG << " storing BTofPidTraits for the global track" << endm;
	  
	  if(mUseEventVertex && pTrack) {
	    pTrack->setBTofPidTraits(ppidTof);
	    LOG_DEBUG << " storing BTofPidTraits for the primary track" << endm;
	  }
  }  // end tof hits
  
  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::cleanCalibMuDst()
{
  if(!mMuDst) return;
  
  Int_t nPrimary = mMuDst->numberOfPrimaryTracks();
  Int_t nGlobal = mMuDst->numberOfGlobalTracks();
  for(int i=0;i<nPrimary;i++) {
    StMuTrack *pTrack = (StMuTrack *)mMuDst->primaryTracks(i);
    if(!pTrack) continue;
    StMuBTofPidTraits pid = pTrack->btofPidTraits();
    cleanCalib(pid);
    pTrack->setBTofPidTraits(pid);
  }
  for(int i=0;i<nGlobal;i++) {
    StMuTrack *gTrack = (StMuTrack *)mMuDst->globalTracks(i);
    if(!gTrack) continue;
    StMuBTofPidTraits pid = gTrack->btofPidTraits();
    cleanCalib(pid);          
    gTrack->setBTofPidTraits(pid);
  }
  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::cleanCalib(StMuBTofPidTraits& pid)
{
  pid.setTimeOfFlight(-999.);
  pid.setPathLength(-999.);
  pid.setBeta(-999.);
  pid.setSigmaElectron(-999.);
  pid.setSigmaPion(-999.);
  pid.setSigmaKaon(-999.);
  pid.setSigmaProton(-999.);
  pid.setProbElectron(-999.);
  pid.setProbPion(-999.);
  pid.setProbKaon(-999.);
  pid.setProbProton(-999.);
  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::initEvent()
{
  if(mMuDstIn) {
    StMuDstMaker *mMuDstMaker = (StMuDstMaker *)GetMaker("MuDst");
    if(!mMuDstMaker) {
      LOG_WARN << " No MuDstMaker ... bye-bye" << endm;
      return;
    }
    mMuDst = mMuDstMaker->muDst();
    if(!mMuDst) {
      LOG_WARN << " No MuDst ... bye-bye" << endm;
      return;
    }

    mBTofHeader = mMuDst->btofHeader();
  } else {
    mEvent = (StEvent *) GetInputDS("StEvent");

    if(!mEvent) return;
    StBTofCollection *btofColl = mEvent->btofCollection();
    if(!btofColl) return;
    mBTofHeader = btofColl->tofHeader();
  }

  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::loadVpdData()
{
   if(!mBTofHeader) return;

   mTSumWest = 0;
   mTSumEast = 0;
   mVPDHitPatternWest = mBTofHeader->vpdHitPattern(west);
   mVPDHitPatternEast = mBTofHeader->vpdHitPattern(east);
   mNWest = mBTofHeader->numberOfVpdHits(west);
   mNEast = mBTofHeader->numberOfVpdHits(east);
   mVPDVtxZ = mBTofHeader->vpdVz();

   for(int i=0;i<St_tofCorrC::mNVPD;i++) {
     mVPDLeTime[i] = mBTofHeader->vpdTime(west, i+1);
     if(mVPDLeTime[i]>0.) mTSumWest += mVPDLeTime[i];
     if(Debug()) {
       LOG_DEBUG << " loading VPD West tubeId = " << i+1 << " time = " << mVPDLeTime[i] << endm;
     }
   }

   for(int i=0;i<St_tofCorrC::mNVPD;i++) {
     mVPDLeTime[i+St_tofCorrC::mNVPD] = mBTofHeader->vpdTime(east, i+1);
     if(mVPDLeTime[i+St_tofCorrC::mNVPD]>0.) mTSumEast += mVPDLeTime[i+St_tofCorrC::mNVPD];
     if(Debug()) {
       LOG_DEBUG << " loading VPD East tubeId = " << i+1 << " time = " << mVPDLeTime[i+St_tofCorrC::mNVPD] << endm;
     }
   }

   return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::writeStartTime()
{
  if(mBTofHeader) {
    mBTofHeader->setTStart(mTStart);
    mBTofHeader->setTDiff(mTDiff);
    mBTofHeader->setNTzero(mNTzero);
  }

  return;
}

//_____________________________________________________________________________
Double_t StBTofCalibMaker::tofAllCorr(const Double_t tof, const Double_t tot, const Double_t z, const Int_t iTray, const Int_t iModuleChan)
{
  int tray = iTray;
  int module = iModuleChan/6 + 1;
  int cell = iModuleChan%6 + 1;
  //int board = iModuleChan/24 + 1;
  LOG_DEBUG << "\nStBTofCalibMaker::btofAllCorr: BTof calibrating...\n" 
  	     << "\tDoing Calibration in BTOF Tray " << tray << " Module " << module << " Cell " << cell
	     << "\n\tinput tof = " << tof
	     << "  TOT = " << tot << "  Zlocal = " << z << endm;
  Double_t tofcorr = tof;
  tofcorr -= St_tofTOffsetC::instance()->t0(tray,module,cell);
  if(mSlewingCorr) {
    Double_t dcorr = St_tofTotbCorrC::instance()->Corr(tray,module,cell,tot);
    if (dcorr <= -9999.0) {
      LOG_DEBUG << " TOT out of range! EXIT! " << endm;
      return -9999.;
    }
    tofcorr -= dcorr;
    dcorr = St_tofZbCorrC::instance()->Corr(tray,module,cell,z);
    if (dcorr <= -9999.0) {
      LOG_DEBUG << " Z out of range! EXIT! " << endm;
      return -9999.;
    }
    tofcorr -= dcorr;
    LOG_DEBUG << "zHit correction: "<<dcorr<<endm;
  }
  LOG_DEBUG << "  Corrected tof: tofcorr = " << tofcorr << endm;
  return tofcorr;
}

//_____________________________________________________________________________
void StBTofCalibMaker::tstart(const Double_t vz, Double_t *tstart, Double_t *tdiff)
{
  *tstart = -9999.;
  *tdiff = -9999.;

  if(fabs(vz)>200.) {LOG_INFO << "tstart: vz too big" << endm; return;}

  Double_t TSum = mTSumEast + mTSumWest;

  if(mNEast+mNWest>0) {
    *tstart = (TSum-(mNEast-mNWest)*vz/(C_C_LIGHT/1.e9))/(mNEast+mNWest);
  }
  if ( mNEast>=mVPDEastHitsCut && mNWest>=mVPDWestHitsCut ) {
    *tdiff = (mTSumEast/mNEast - mTSumWest/mNWest)/2. - vz/(C_C_LIGHT/1.e9);
  }

  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::tstart_NoVpd(const StBTofCollection *btofColl, const StPrimaryVertex *pVtx, Double_t *tstart)
{
  *tstart = -9999.;
  if(!btofColl) return;

  const StSPtrVecBTofHit &tofHits = btofColl->tofHits();
  Int_t nCan = 0;
  Double_t tSum = 0.;
  Double_t t0[5000];
  memset(t0, 0., sizeof(t0));
  for(size_t i=0;i<tofHits.size();i++) {
    StBTofHit *aHit = dynamic_cast<StBTofHit*>(tofHits[i]);
    if(!aHit) continue;
    int trayId = aHit->tray();
    if(trayId>0&&trayId<=St_tofCorrC::mNTray) {
      StGlobalTrack *gTrack = dynamic_cast<StGlobalTrack*>(aHit->associatedTrack());
      if(!gTrack) continue;
      StPrimaryTrack *pTrack = dynamic_cast<StPrimaryTrack*>(gTrack->node()->track(primary));
      if(!pTrack) continue;
      if(pTrack->vertex() != pVtx) continue;
      StThreeVectorF mom = pTrack->geometry()->momentum();
      double ptot = mom.mag();

      // use lose cut for low energies to improve the efficiency - resolution is not a big issue
      if(ptot<0.2 || ptot>0.6) continue;

      static StTpcDedxPidAlgorithm PidAlgorithm;
      static StPionPlus* Pion = StPionPlus::instance();
      const StParticleDefinition* pd = pTrack->pidTraits(PidAlgorithm);
      double nSigPi = -999.;
      if(pd) {
        nSigPi = PidAlgorithm.numberOfSigma(Pion);
      }

      if( fabs(nSigPi)>2.0 ) continue;

      const StPtrVecTrackPidTraits& theTofPidTraits = pTrack->pidTraits(kTofId);
      if(!theTofPidTraits.size()) continue;

      StTrackPidTraits *theSelectedTrait = theTofPidTraits[theTofPidTraits.size()-1];
      if(!theSelectedTrait) continue;

      StBTofPidTraits *pidTof = dynamic_cast<StBTofPidTraits *>(theSelectedTrait);
      if(!pidTof) continue;

      double tot = aHit->tot(); // ns
      double tof = aHit->leadingEdgeTime();
      double zhit = pidTof->zLocal();

      int moduleChan = (aHit->module()-1)*6 + (aHit->cell()-1);
      Double_t tofcorr = tofAllCorr(tof, tot, zhit, trayId, moduleChan);
      if(tofcorr<0.) continue;

      StThreeVectorF primPos = pVtx->position();
      StPhysicalHelixD helix = pTrack->geometry()->helix();
      double L = tofPathLength(&primPos, &pidTof->position(), helix.curvature());
      double tofPi = L*sqrt(M_PION_PLUS*M_PION_PLUS+ptot*ptot)/(ptot*(C_C_LIGHT/1.e9));

      tSum += tofcorr - tofPi;
      t0[nCan] = tofcorr - tofPi;
      nCan++;
      
    }

  }

  if(nCan<=0) {
    *tstart = -9999.;
    return;
  }

  Int_t nTzero = nCan;
  if(nCan>1) { // remove hits too far from others
    for(int i=0;i<nCan;i++) {
      if (nTzero < 2) break;
      double tdiff = t0[i] - (tSum-t0[i])/(nTzero-1);
      if(fabs(tdiff)>5.0) {
        tSum -= t0[i];
        nTzero--;
      }
    }
  }

  mNTzero = nTzero;

  *tstart = nTzero>0 ? tSum / nTzero : -9999.;

  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::tstart_NoVpd(const StMuDst *muDst, const StMuPrimaryVertex *pVtx, Double_t *tstart)
{
  *tstart = -9999.;
  if(!muDst) return;

  Int_t nBTofHits = muDst->numberOfBTofHit();
  Int_t nCan = 0;
  Double_t tSum = 0.;
  Double_t t0[5000];
  memset(t0, 0., sizeof(t0));
  for(int i=0;i<nBTofHits;i++) {
    StMuBTofHit *aHit = (StMuBTofHit*)muDst->btofHit(i);
    if(!aHit) continue;
    int trayId = aHit->tray();
    if(trayId>0&&trayId<=St_tofCorrC::mNTray) {
      StMuTrack *gTrack = aHit->globalTrack();
      if(!gTrack) continue;
      StMuTrack *pTrack = aHit->primaryTrack();
      if(!pTrack) continue;
      StMuPrimaryVertex *aVtx = muDst->primaryVertex(pTrack->vertexIndex());
      if(aVtx != pVtx) continue;
      StThreeVectorF mom = pTrack->momentum();
      double ptot = mom.mag();

      // For low energies, lose cut to improve the efficiency in peripheral collisions - resolution should be not a big issue
      if(ptot<0.2 || ptot>0.6) continue;
      double nSigPi = pTrack->nSigmaPion();
      if( fabs(nSigPi)>2. ) continue;

      StMuBTofPidTraits pidTof = pTrack->btofPidTraits();

      double tot = aHit->tot(); // ns
      double tof = aHit->leadingEdgeTime();
      double zhit = pidTof.zLocal();

      int moduleChan = (aHit->module()-1)*6 + (aHit->cell()-1);
      Double_t tofcorr = tofAllCorr(tof, tot, zhit, trayId, moduleChan);
      if(tofcorr<0.) continue;

      StThreeVectorF primPos = pVtx->position();
      StPhysicalHelixD helix = pTrack->helix();
      double L = tofPathLength(&primPos, &pidTof.position(), helix.curvature());
      double tofPi = L*sqrt(M_PION_PLUS*M_PION_PLUS+ptot*ptot)/(ptot*(C_C_LIGHT/1.e9));

      tSum += tofcorr - tofPi;
      t0[nCan] = tofcorr - tofPi;
      nCan++;
      
    }

  }

  if(nCan<=0) {
    *tstart = -9999.;
    return;
  }

  Int_t nTzero = nCan;
  if(nCan>1) { // remove hits too far from others
    for(int i=0;i<nCan;i++) {
      double tdiff = t0[i] - (tSum-t0[i])/(nTzero-1);
      if(fabs(tdiff)>5.0) {
        tSum -= t0[i];
        nTzero--;
      }
    }
  }

  mNTzero = nTzero;

  *tstart = nTzero>0 ? tSum / nTzero : -9999.;

  return;
}

//_____________________________________________________________________________
void StBTofCalibMaker::bookHistograms()
{
#if 0
  hEventCounter = new TH1D("eventCounter","eventCounter",20,0,20);
#endif
}

//_____________________________________________________________________________
void StBTofCalibMaker::writeHistograms()
{
#if 0
  // Output file
  TFile *theHistoFile =  new TFile(mHistoFileName.c_str(), "RECREATE");
  LOG_INFO << "StBTofCalibMaker::writeHistograms()"
       << " histogram file " <<  mHistoFileName << endm;

  theHistoFile->cd();

  if(mHisto) {
    hEventCounter->Write();
  }
  return;
#endif
}

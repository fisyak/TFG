// Author : Yuri Fisyak
// $Id: StxMaker.cxx,v 1.6 2013/09/16 19:54:04 fisyak Exp $
//#define __TPC3D__
#include "StxMaker.h"
#include "StEvent/StEvent.h"
#include "StEvent/StGlobalTrack.h"
#include "StEvent/StTrackMassFit.h"
#include "StEvent/StL3Trigger.h"
#include "StEvent/StTrack.h"
#include "StEvent/StTrackNode.h"
#include "StEvent/StTrackDefinitions.h"
#include "StEvent/StTrackMethod.h"
#include "StEvent/StTrackDetectorInfo.h"
#include "StEvent/StHelixModel.h"
#include "StTrackGeometry.h"
#include "StEvent/StTpcHit.h"
#include "StEvent/StBTofHit.h"
#include "StEventUtilities/StEventHelper.h"
#include "StEventUtilities/StTrackUtilities.h"
#include "TRMatrix.h"
#include "TRVector.h"
#include "KFParticle/KFVertex.h"
#include "KFParticle/KFPTrack.h"
#include "StxCAInterface.h"
// GenFit
#include <iostream>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>

#include "GenFit/AbsFinitePlane.h"
#include "GenFit/AbsFitterInfo.h"
#include "GenFit/AbsMeasurement.h"
#include "GenFit/AbsTrackRep.h"
#include "GenFit/ConstField.h"
#include "GenFit/DetPlane.h"
#include "GenFit/Exception.h"
#include "GenFit/FieldManager.h"
#include "GenFit/KalmanFittedStateOnPlane.h"
#include "GenFit/AbsKalmanFitter.h"
#include "GenFit/KalmanFitter.h"
#include "GenFit/KalmanFitterRefTrack.h"
#include "GenFit/KalmanFitterInfo.h"
#include "GenFit/KalmanFitStatus.h"
#include "GenFit/DAF.h"
#include "GenFit/GFGbl.h"
#include "GenFit/MeasurementOnPlane.h"
#include "GenFit/FullMeasurement.h"
#include "GenFit/PlanarMeasurement.h"
#include "GenFit/ProlateSpacepointMeasurement.h"
#include "GenFit/RectangularFinitePlane.h"
#include "GenFit/ReferenceStateOnPlane.h"
#include "GenFit/SharedPlanePtr.h"
#include "GenFit/SpacepointMeasurement.h"
#include "GenFit/StateOnPlane.h"
#include "GenFit/Tools.h"
#include "GenFit/TrackCand.h"
#include "GenFit/TrackCandHit.h"
#include "GenFit/Track.h"
#include "GenFit/TrackPoint.h"
#include "GenFit/WireMeasurement.h"
#include "GenFit/WirePointMeasurement.h"

#include "GenFit/MaterialEffects.h"
#include "GenFit/RKTools.h"
#include "GenFit/RKTrackRep.h"
#include "GenFit/StepLimits.h"
#include "GenFit/TGeoMaterialInterface.h"

#include "GenFit/EventDisplay.h"

#include "GenFit/HelixTrackModel.h"
#include "GenFit/MeasurementCreator.h"
#include "StPlanarMeasurement.h"
#include "StTpcPlanarMeasurement.h"
#include "StTpc3DMeasurement.h"
#include "StBTofPlanarMeasurement.h"
#include "StG2TrackVertexMap.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TDatabasePDG.h"
#include "TEveManager.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TVector3.h"
#include "TStopwatch.h"
#include "TString.h"
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TDatabasePDG.h"
#include "TMath.h"
#include "TString.h"

#include <memory>
//#define VALGRIND

#ifdef VALGRIND
  #include <valgrind/callgrind.h>
#else
#define CALLGRIND_START_INSTRUMENTATION
#define CALLGRIND_STOP_INSTRUMENTATION
#define CALLGRIND_DUMP_STATS
#endif
#include "StarField.h"
//#define __DEBUG__
#ifdef __DEBUG__
#define DEBUG_LEVEL
#define PrPP(A,B)  DEBUG_LEVEL {LOG_INFO << "StxMaker::" << (#A) << "\t" << (#B) << " = \t" << (B) << endm;}
#else
#define PrPP(A,B)
#endif
using namespace  genfit;
using namespace  std;
ClassImp(StxMaker);
//_____________________________________________________________________________
Int_t StxMaker::Make(){
  Int_t ok = kStOK;
  mEvent = dynamic_cast<StEvent*>( GetInputDS("StEvent") );
  if (! mEvent) {return kStWarn;};
  StEventHelper::Remove(mEvent,"StSPtrVecTrackDetectorInfo");
  StEventHelper::Remove(mEvent,"StSPtrVecTrackNode");
  StEventHelper::Remove(mEvent,"StSPtrVecPrimaryVertex");
  StEventHelper::Remove(mEvent,"StSPtrVecV0Vertex");
  StEventHelper::Remove(mEvent,"StSPtrVecXiVertex");
  StEventHelper::Remove(mEvent,"StSPtrVecKinkVertex");
  //  StiKalmanTrackNode::SetExternalZofPVX(0);
  St_g2t_track  *g2t_track  = (St_g2t_track  *) GetDataSet("geant/g2t_track");  
  St_g2t_vertex *g2t_vertex = (St_g2t_vertex *) GetDataSet("geant/g2t_vertex"); 
  StG2TrackVertexMap::instance(g2t_track,g2t_vertex);
 
  StxCAInterface::Instance().SetNewEvent();
  // Run reconstruction by the CA Tracker
  StxCAInterface::Instance().Run();
  // Sort CA track candidate on no. of hits
  const Int_t NRecoTracks = StxCAInterface::Instance().GetTracker()->NTracks();
  TArrayI NoHits(NRecoTracks); Int_t *noHits = NoHits.GetArray();
  for ( Int_t iTr = 0; iTr < NRecoTracks; iTr++ ) {
    noHits[iTr] = StxCAInterface::Instance().GetTracker()->Track( iTr ).NHits();
  }  
  TArrayI Index(NRecoTracks); Int_t *index = Index.GetArray();
  TMath::Sort(NRecoTracks,noHits,index,kTRUE);
  vector<SeedHit_t>        &fSeedHits = StxCAInterface::Instance().GetSeedHits();
  for ( Int_t ITr = 0; ITr < NRecoTracks; ITr++ ) {
    Int_t iTr = index[ITr];
    const AliHLTTPCCAGBTrack &tr = StxCAInterface::Instance().GetTracker()->Track( iTr );
    Int_t NoHitsTotal = 0;
    Int_t NoHitsUsed = 0;
    Int_t NoTpcHitsUsed = 0;
    Int_t NoNonTpcHitsUsed = 0;
    Int_t NHits = tr.NHits();
    for ( Int_t iHit = 0; iHit < NHits; iHit++ ){ 
      const Int_t index = StxCAInterface::Instance().GetTracker()->TrackHit( tr.FirstHitRef() + iHit );
      const Int_t hId   = StxCAInterface::Instance().GetTracker()->Hit( index ).ID();
      const StHit    *hit    = fSeedHits[hId].hit;
      const StTpcHit *tpcHit = dynamic_cast<const StTpcHit *>(hit);
#if 0      
      if (! tpcHit) {
	if (Debug()) {LOG_WARN << "StxMaker::Make pointer to StTpcHit is zero" << endm;}
	continue;
      }
#else
#endif
      NoHitsTotal++;
      if (tpcHit) NoTpcHitsUsed++;
      else        NoNonTpcHitsUsed++;
      if (! hit->usedInFit()) continue;
      NoHitsUsed++;
    }
#if 0
    cout << "Track #" << iTr << "\tNoTpcHitsUsed = " << NoTpcHitsUsed << "\tNoNonTpcHitsUsed = " << NoNonTpcHitsUsed << endl;
#endif
    if (NoHitsTotal < 10) {
      if (Debug()) {LOG_WARN << "StxMaker::Make no. of hits for the track candidate " << NoHitsTotal << " is too low. Reject it." << endm;}
      continue;
    }
    if (NoHitsUsed > 0.1*NoHitsTotal) {
      if (Debug()) {LOG_WARN << "StxMaker::Make no. of reused hits for the track candidate " 
			     << NoHitsTotal << " from total " << NoHitsTotal << " is too high. Reject it." << endm;}
      continue;
    }
    if (! FitTrack(tr)) continue;
    // Create StTrack
  }
  // Find Vertives
  StMaker *KFV = GetMaker("KFVertex");
  if (KFV) {
    ok = KFV->Make();
  }
  //  StTrackUtilities::instance()->FillPrimaryTracks();
  return ok;
}
//________________________________________________________________________________
#ifdef __HANDLER__
void handler(Int_t sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}
#endif
//________________________________________________________________________________
Double_t StxMaker::ConvertCA2Gen(const Double_t alpha, const StxCApar& stxPar, CA2GenState_t& ca2Gen ) {
  // --------------------------------------------------------------------------------
  Float_t _alpha = TMath::Pi()/2 - alpha;
  Double_t ca = cos(_alpha);
  Double_t sa = sin(_alpha);
  Double_t xyzp[6];
  xyzp[0] = ca*stxPar.pars.x() - sa*stxPar.pars.y(); 
  xyzp[1] = sa*stxPar.pars.x() + ca*stxPar.pars.y(); 
  xyzp[2] =    stxPar.pars.z();
  Int_t charge = (stxPar.pars.ptin() > 0.0) ? -1 : 1;
  Double_t pT = 1./TMath::Abs(stxPar.pars.ptin());
  Double_t ce = TMath::Cos(stxPar.pars.eta()+_alpha);
  Double_t se = TMath::Sin(stxPar.pars.eta()+_alpha);
  Double_t px = pT*ce;
  Double_t py = pT*se;
  Double_t pz = pT*stxPar.pars.tanl();
  xyzp[3] = px;
  xyzp[4] = py;
  xyzp[5] = pz;
  Double_t dpTdPti = -pT*pT*TMath::Sign(1.,stxPar.pars.ptin());
  Double_t f[36] = {
    //          x,  y,     z,     eta,               ptin, tanl
    /*  x */  ca, -sa,     0,       0,                  0,    0, 
    /*  y */  sa,  ca,     0,       0,                  0,    0, 
    /*  z */   0,   0,     1,       0,                  0,    0, 
    /* px */   0,   0,     0,     -py,         dpTdPti*ce,    0, 
    /* py */   0,   0,     0,      px,         dpTdPti*se,    0,
    /* pz */   0,   0,     0,       0,dpTdPti*stxPar.pars.tanl(),   pT};
  TRMatrix F(6,6,f);
  TRSymMatrix C(6,stxPar.errs.G());
  TRSymMatrix Cov(F,TRArray::kAxSxAT,C);
  // --------------------------------------------------------------------------------
  ca2Gen.pos = TVector3(xyzp);
  ca2Gen.mom = TVector3(xyzp+3);
  ca2Gen.covM.ResizeTo(6,6);
  for (Int_t i = 0; i < 6; i++) 
    for (Int_t j = 0; j < 6; j++) 
      ca2Gen.covM(i,j) = Cov(i,j);
#ifdef __DEBUG__
  if (Debug()) {
    ca2Gen.covM.Print("");
  }
#endif
  ca2Gen.chi2 = stxPar.chi2;
  ca2Gen.NDF = stxPar.NDF;
  return charge;
}
//________________________________________________________________________________
Int_t StxMaker::FitTrack(const AliHLTTPCCAGBTrack &tr) {
#if 0
  static TStopwatch *watch = new  TStopwatch;
  watch->Start(kTRUE);
#endif
  static genfit::eFitterType          fitterId = genfit::Undefined;
  static genfit::eMultipleMeasurementHandling mmHandling = genfit::undefined;
  static const Int_t nIter = 20; // max number of iterations
  static const Double_t dPVal = 1.E-3; // convergence criterion
#if 0
  static Bool_t resort = kFALSE;
  static Bool_t prefit = kFALSE; // make a simple Kalman iteration before the actual fit
  static Bool_t refit  = kFALSE; // if fit did not converge, try to fit again
  static Bool_t twoReps = kFALSE; // test if everything works with more than one rep in the tracks
#endif
  static const Bool_t matFX = kTRUE; // kFALSE;         // include material effects; can only be disabled for RKTrackRep!
#if 0
  static Bool_t checkPruning = kFALSE;
  static const Bool_t onlyDisplayFailed = kFALSE; // only load non-converged tracks into the display
#endif
  //
  static genfit::AbsKalmanFitter* fitter = 0;
  static Bool_t Initialized = kFALSE;
  if (! Initialized) {
    if      (IAttr("Undefined"))      fitterId = genfit::Undefined;
    else if (IAttr("SimpleKalman"))   fitterId = genfit::SimpleKalman;
    else if (IAttr("DafRef"))         fitterId = genfit::DafRef;
    else if (IAttr("DafSimple"))      fitterId = genfit::DafSimple;
    else if (IAttr("RefKalman"))      fitterId = genfit::RefKalman;
    else                              fitterId = genfit::RefKalman;
    //
    if      (IAttr("weightedAverage"))                   mmHandling = genfit::weightedAverage;
    else if (IAttr("unweightedClosestToReference"))      mmHandling = genfit::unweightedClosestToReference;
    else if (IAttr("unweightedClosestToPrediction"))     mmHandling = genfit::unweightedClosestToPrediction;
    else if (IAttr("weightedClosestToReference"))        mmHandling = genfit::weightedClosestToReference;
    else if (IAttr("weightedClosestToPrediction"))       mmHandling = genfit::weightedClosestToPrediction;
    else if (IAttr("unweightedClosestToReferenceWire"))  mmHandling = genfit::unweightedClosestToReferenceWire;
    else if (IAttr("unweightedClosestToPredictionWire")) mmHandling = genfit::unweightedClosestToPredictionWire;
    else if (IAttr("weightedClosestToReferenceWire"))    mmHandling = genfit::weightedClosestToReferenceWire;
    else if (IAttr("weightedClosestToPredictionWire"))   mmHandling = genfit::weightedClosestToPredictionWire;
    else                                                 mmHandling = genfit::unweightedClosestToPredictionWire;
    //
#if 0
    if      (IAttr("prefit"))  prefit = kTRUE;
    if      (IAttr("resort"))  resort = kTRUE;
    if      (IAttr("refit"))   refit = kTRUE;
#endif
    //  
#if 0
    if (IAttr("twoReps")) twoReps = kTRUE;
    if (IAttr("checkPruning")) checkPruning = kTRUE;
#endif
#ifdef __HANDLER__
    signal(SIGSEGV, handler);   // install our handler
#endif
    switch (fitterId) {
    case genfit::SimpleKalman:
      fitter = new genfit::KalmanFitter(nIter, dPVal);
      fitter->setMultipleMeasurementHandling(mmHandling);
      break;
    case genfit::RefKalman:
      fitter = new genfit::KalmanFitterRefTrack(nIter, dPVal);
      fitter->setMultipleMeasurementHandling(mmHandling);
      break;
    case genfit::DafSimple:
      fitter = new genfit::DAF(kFALSE);
      break;
    case genfit::DafRef:
      fitter = new genfit::DAF();
      break;
    default:
      break;
    }
    if (fitter) {
      fitter->setDebugLvl(Debug());
      if (dynamic_cast<genfit::DAF*>(fitter) != nullptr) {
	static_cast<genfit::DAF*>(fitter)->setAnnealingScheme(100, 0.1, 5);
	static_cast<genfit::DAF*>(fitter)->setConvergenceDeltaWeight(0.0001);
      }
      fitter->setMaxIterations(nIter);
    }
    genfit::FieldManager::getInstance()->init(new genfit::StarField());
    if (IAttr("useCache")) genfit::FieldManager::getInstance()->useCache(kTRUE, 8);
    else                   genfit::FieldManager::getInstance()->useCache(kFALSE, 0);
    genfit::TGeoMaterialInterface *geoMat = new genfit::TGeoMaterialInterface();
    geoMat->setDebugLvl(Debug());
    geoMat->setDebugLvT(Debug());
    genfit::MaterialEffects::getInstance()->init(geoMat);
    if (!matFX) genfit::MaterialEffects::getInstance()->setNoEffects();
    // Set Debug flags
    if (Debug()) {
      if (fitter) fitter->setDebugLvl(10);
      gGeoManager->SetVerboseLevel(5);
#ifndef __TPC3D__ /* ! __TPC3D__ */
      StTpcPlanarMeasurement::SetDebug(1);
#endif /* ! __TPC3D__ */    
      genfit::MaterialEffects::getInstance()->setDebugLvl(2);
      //    genfit::MaterialEffects::getInstance()->setDebugLvT(2);
    } else {
      if (fitter) fitter->setDebugLvl(0);
      gGeoManager->SetVerboseLevel(0);
#ifndef __TPC3D__ /* ! __TPC3D__ */
      StTpcPlanarMeasurement::SetDebug(0);
#endif /* ! __TPC3D__ */ 
      genfit::MaterialEffects::getInstance()->setDebugLvl(0);
      //    genfit::MaterialEffects::getInstance()->setDebugLvT(0);
    }
    Initialized = kTRUE;
  } // end of initialization
  const Int_t pdg = 211; // -13;               // particle pdg code mu+
  //  const Double_t charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/(3.);
  //========== Reference  track ======================================================================
  static StxCApar stxPar;
  Double_t alpha = tr.Alpha();
  Int_t sign  = (tr.InnerParam().GetQPt() > 0.0) ? -1 : 1;
  genfit::AbsTrackRep* rep = new genfit::RKTrackRep(sign*pdg);
#if 0
  static genfit::AbsTrackRep* secondRep = 0;
#endif
  if (Debug()) rep->setDebugLvl();
  // propagation direction. (-1, 0, 1) -> (backward, auto, forward).
  rep->setPropDir(1);
  genfit::MeasuredStateOnPlane stateSeed(rep);
  for (Int_t io = 1; io >= 0 ; io--) {// 0 -> Inner, 1 -> Outer
    if (! io) StxCAInterface::Instance().ConvertPars(tr.InnerParam(), alpha, stxPar);
    else      StxCAInterface::Instance().ConvertPars(tr.OuterParam(), alpha, stxPar);
    ConvertCA2Gen(alpha, stxPar, fCA2Gen[io]);
    stateSeed.setPosMomCov(fCA2Gen[io].pos, fCA2Gen[io].mom, fCA2Gen[io].covM);
    stateSeed.get6DStateCov(fCA2Gen[io].state7, fCA2Gen[io].origCov);
  }
#if 0
  if (twoReps) secondRep = new genfit::RKTrackRep(sign*-211);
  // create track
  //  genfit::Track* secondTrack(nullptr);
  //  genfit::Track  fitTrack(rep, fCA2Gen[0].pos, fCA2Gen[0].mom);
#endif
  stateSeed.get6DStateCov(fCA2Gen[0].state7, fCA2Gen[0].origCov);
  genfit::Track fitTrack(rep, fCA2Gen[0].state7, fCA2Gen[0].origCov);
  // remember original initial state
  const genfit::StateOnPlane stateRefOrig(stateSeed);
  //========== Mesurements ======================================================================
  const Int_t NHits = tr.NHits();
  vector<SeedHit_t>        &fSeedHits = StxCAInterface::Instance().GetSeedHits();
  //  std::vector< std::vector<genfit::AbsMeasurement*> > measurements;
  //  genfit::AbsMeasurement* measurement = 0;
  if (Debug()) {
    cout << "momSeed\t"; fCA2Gen[0].mom.Print("");
    cout << "posSeed\t"; fCA2Gen[0].pos.Print("");
    cout << "NHits = " << NHits << endl;
  }
  StPlanarMeasurement::SetHitId(0);
  for ( Int_t iHit = 0; iHit < NHits; iHit++ ){ 
    const Int_t index = StxCAInterface::Instance().GetTracker()->TrackHit( tr.FirstHitRef() + iHit );
    const Int_t hId   = StxCAInterface::Instance().GetTracker()->Hit( index ).ID();
    const StHit    *hit    = fSeedHits[hId].hit;
    if (! hit) continue;
    const StTpcHit *tpcHit = 0;
    const StBTofHit *tofHit = 0;
    genfit::PlanarMeasurement* measurement = 0;
    switch (hit->detector()) {
    case kTpcId:
      tpcHit = (const StTpcHit *) hit;
#ifndef __TPC3D__ /* ! __TPC3D__ */
      measurement = new StTpcPlanarMeasurement(tpcHit, nullptr);
#else /* __TPC3D__ */
      measurement = new StTpc3DMeasurement(tpcHit, nullptr);
#endif /* ! __TPC3D__ */
      break;
    case kBTofId:
      tofHit = (const StBTofHit*) hit;
      measurement = new StBTofPlanarMeasurement(tofHit, nullptr);
      break;
    default:
      break;
    }
    if ( measurement ) {
      fitTrack.insertPoint(new genfit::TrackPoint(measurement, &fitTrack));
    }
  }
  TVector3 posI = stateRefOrig.getPos();
  //  gGeoManager->SetCurrentPoint(posI.X(), posI.Y(), posI.Z());
  gGeoManager->FindNode(posI.X(), posI.Y(), posI.Z());
  try{
    //check
    fitTrack.checkConsistency();
    
    // do the fit
    if (fitter) {
      fitter->processTrack(&fitTrack);
      // print fit result
      if (Debug()) {
	fitTrack.getFittedState().Print();
      }
      //check
      //      fitTrack.checkConsistency();
    }
  }
  catch(genfit::Exception& e){
    std::cout<<"Exception, next track"<<std::endl;
    std::cout << e.what();
    return kStErr;
  }
  if (fitter) {
    KalmanFitStatus *fitStatus = fitTrack.getKalmanFitStatus();
    if (! fitStatus || !(fitStatus->isFitConvergedFully() || fitStatus->isFitConvergedPartially())) {
      return kStErr;
    }
    //_________ Fill StTrack _______________
    //  UInt_t npoints = fitTrack.getNumPoints();
    if (Debug()) {
      std::cout << "Inner Parameters" << std::endl << "====================" << endl;
      fitTrack.getFittedState().Print();
    }
    //  const AbsTrackRep* rep = fitTrack.getCardinalRep();
    genfit::TrackPoint* point = fitTrack.getPointWithMeasurementAndFitterInfo(0, rep);
    genfit::AbsFitterInfo* fitterInfo = point->getFitterInfo(rep);
    const genfit::MeasuredStateOnPlane& measuredPointStateI = fitterInfo->getFittedState(kTRUE);
    TVector3 posI, momI;
    TMatrixDSym covI(6,6);
    measuredPointStateI.getPosMomCov(posI, momI, covI);
    if (Debug()) {
      std::cout << "Outer Parameters" << std::endl << "====================" << endl;
      fitTrack.getFittedState(-1).Print();
    }
    point = fitTrack.getPointWithMeasurementAndFitterInfo(-1, rep);
    fitterInfo = point->getFitterInfo(rep);
    const genfit::MeasuredStateOnPlane& measuredPointStateO = fitterInfo->getFittedState(kTRUE);
    TVector3 posO, momO;
    TMatrixDSym covO(6,6);
    measuredPointStateO.getPosMomCov(posO, momO, covO);
  }
  try{
    FillGlobalTrack(&fitTrack);
  }
  catch(genfit::Exception& e) {
    std::cout << "Exception, FillGlobalTrack" << std::endl;
    return kStErr;
  }  
#if 0
  watch->Print("");
#endif
  return kStOK;
}
// $Log: StxMaker.cxx,v $
//_____________________________________________________________________________
Bool_t StxMaker::Accept(genfit::Track *kTrack) {
#if 0
  Int_t nFittedPoints   = track->FitPointCount(0);
  if (nFittedPoints  <  5 )   return kFALSE;
  if(track->TrackLength()<=0) return kFALSE; 
#endif
  return kTRUE;
}
//_____________________________________________________________________________
void StxMaker::FillGlobalTrack(genfit::Track *kTrack) {
  if (!Accept(kTrack)) return; // get rid of riff-raff
  // track node where the new StTrack will reside
  StTrackNode* trackNode = new StTrackNode;
  // actual filling of StTrack from genfit::Track 
  StGlobalTrack* gTrack = new StGlobalTrack;
  trackNode->addTrack(gTrack);
#if 0
  trackNode->Print();
#endif
  // filling successful, set up relationships between objects
  StSPtrVecTrackNode& trNodeVec = mEvent->trackNodes(); 
  UShort_t Id = trNodeVec.size() + 1;
  gTrack->setKey(Id);
  // reuse the utility to fill the topology map
  // this has to be done at the end as it relies on
  // having the proper track->detectorInfo() relationship
  // and a valid StDetectorInfo object.
  //cout<<"Tester: Event Track Node Entries: "<<trackNode->entries()<<endl;
  //  mTrkNodeMap.insert(map<StxKalmanTrack*,StTrackNode*>::value_type (kTrack,trNodeVec.back()) );
  if (FillTrack(gTrack,kTrack)) {
#if 0 /* keep rejected tracks */
    //    delete gTrack;
    delete trackNode;
    throw genfit::Exception("Consistency check failed ", __LINE__, __FILE__);
#endif
  }
  trNodeVec.push_back(trackNode);
  return;
}
//_____________________________________________________________________________
Int_t StxMaker::FillTrack(StTrack* gTrack, genfit::Track * kTrack)
{
  //cout << "StxMaker::FillTrack()" << endl;
  // encoded method = 16 bits = 12 fitting and 4 finding, for the moment use:
  // kKalmanFitId
  // bit 15 for finding, (needs to be changed in StEvent).
  // change: make sure bits are ok, are the bits set up one in each position and nothing else?
  // this would mean that the encoded method is wasting space!
  // the problem is that in principle there might be combinations of finders for each tracking detector
  // but the integrated tracker will use only one for all detectors maybe
  // so need this bit pattern:
  // finding 100000000000     
  // fitting             0010 
  //            32768    +    2 = 32770;
  //
  // above is no longer used, instead use kITKalmanfitId as fitter and tpcOther as finding method
  //  gTrack->setEncodedMethod(mStxEncoded);
  FillDetectorInfo(gTrack,kTrack,kTRUE); //3d argument used to increase/not increase the refCount. MCBS oct 04.
  FillGeometry(gTrack, kTrack, kFALSE); // inner geometry
  FillGeometry(gTrack, kTrack, kTRUE ); // outer geometry
  KalmanFitStatus *fitStatus = kTrack->getKalmanFitStatus();
  Double_t tlen = 0;
  if (fitStatus) {
    tlen = kTrack->getTrackLen();
  } else { 
    StThreeVectorD oI(gTrack->geometry()->origin());
    StThreeVectorD oO(gTrack->outerGeometry()->origin());
    StPhysicalHelixD helixO = gTrack->outerGeometry()->helix();
    StPhysicalHelixD helixI = gTrack->geometry()->helix();
    Double_t s1 = helixI.pathLength(oO);
    Double_t s2 = helixO.pathLength(oI);
    tlen = (s1 - s2)/2;
  } 
  assert(tlen >0.0 && tlen<1000.);
  gTrack->setLength(tlen);// someone removed this, grrrr!!!!

  StTrackUtilities::instance()->StFixTopoMap(gTrack);
  StTrackUtilities::instance()->FillFlags(gTrack);
  gTrack->setIdTruth();
#if 0
  if (!gTrack->IsPrimary()) 
#endif
    FillDca(gTrack,kTrack);
  Int_t iok = gTrack->bad();
  return iok;
}
//_____________________________________________________________________________
/// use the vector of StHits to fill the detector info
/// change: currently point and fit points are the same for genfit::Track s,
/// if this gets modified later in ITTF, this must be changed here
/// but maybe use track->PointCount() later?
//_____________________________________________________________________________
Int_t StxMaker::FillDetectorInfo(StTrack *gTrack, genfit::Track * track, Bool_t refCountIncr) {
  //  output array actually is count[maxDetId+1][3] 
  //  count[0] all detectors
  //  count[detId] for particular detector
  //  count[detId][0] == number of possible points
  //  count[detId][1] == number of measured points
  //  count[detId][2] == number of fitted   points
  enum {kPP=0,kMP=1,kFP=2};
  Int_t dets[kMaxDetectorId][3];

  memset(dets,0,sizeof(dets));
  KalmanFitStatus *fitStatus = track->getKalmanFitStatus();
  const AbsTrackRep* rep = track->getCardinalRep();
  StTrackDetectorInfo* detInfo = new StTrackDetectorInfo;
  for (std::vector< genfit::TrackPoint* >::const_iterator it = track->getPointsWithMeasurement().begin(); 
       it != track->getPointsWithMeasurement().end(); ++it) {
    genfit::TrackPoint *tp = *it;
    genfit::AbsFitterInfo* fitterInfo = tp->getFitterInfo();
    for (std::vector< genfit::AbsMeasurement* >::const_iterator im = tp->getRawMeasurements().begin(); 
	 im !=  tp->getRawMeasurements().end(); ++im) {
      dets[0][kPP]++;
      const StPlanarMeasurement *measurement =  dynamic_cast<StPlanarMeasurement *>(*im);
      if (! measurement) continue;
      //      Int_t detId = measurement->getPlaneId()/10000 + 1;
      Int_t detId = measurement->getDetId();
      dets[0][kPP]++; dets[detId][kPP]++;
      StHit *hit = (StHit*) measurement->Hit();
      if (! hit) continue;
      detId = hit->detector();
      dets[0][kMP]++; dets[detId][kMP]++;
      if (fitStatus && ! tp->hasFitterInfo(rep)) continue;
      dets[0][kFP]++; dets[detId][kFP]++;
      detInfo->addHit((StHit *) hit,refCountIncr);
      Int_t used = hit->usedInFit();
      used++;
      hit->setFitFlag(used);
      // dX calculation
      /*
  	     ( 1 -g  b )               (   u_x -g*u_y + b*u_z)                (   v_x -g*v_y + b*v_z) 
	R =  ( g  1 -a ); u' = R * u = ( g*u_x +  u_y - a*u_z);	 v' = R * v = ( g*v_x +  v_y - a*v_z);
	     (-b  a  1 )               (-b*u_x +a*u_y +   u_z) 	              (-b*v_x +a*v_y +   v_z)  

	     (w,u,v)     - original
             (w1,u1,x1)  - new => (R w, R u, R v);
                           ( i   j   k  )
              w = u x v =  ( u_x u_y u_z) = (u_y*v_z - u_z*v_y, v_x*u_y - v_y*u_x, u_x*v_y + v_x*u_y)
                           ( v_x v_y v_z)
                           (   w_x -g*w_y + b*w_z)   (u'_y*v'_z - u'_z*v'_y)        (-g*w_y + b*w_z)
	      w' = R * w = ( g*w_x +  w_y - a*w_z) = (v'_x*u'_y - v'_y*u'_x) = w +  ( g*w_x - a*w_z)
                           (-b*w_x +a*w_y +  w_z)    (u'_x*v'_y + v'_x*u'_y)        (-b*w_x + a*w_y)

                           (( g*u_x +  u_y - a*u_z)*(-b*v_x +a*v_y +   v_z) - (-b*u_x +a*u_y +   u_z)*( g*v_x +  v_y - a*v_z))
		=          ((   v_x -g*v_y + b*v_z)*( g*u_x +  u_y - a*u_z) - ( g*v_x +  v_y - a*v_z)*(   u_x -g*u_y + b*u_z))
 			   ((   u_x -g*u_y + b*u_z)*( g*v_x +  v_y - a*v_z) + (   v_x -g*v_y + b*v_z)*( g*u_x +  u_y - a*u_z))


                           (( g*u_x +  u_y - a*u_z)*(-b*v_x) +( g*u_x +  u_y - a*u_z)(a*v_y) +   ( g*u_x +  u_y - a*u_z)*(v_z) - (-b*u_x +a*u_y +   u_z)*( g*v_x) - (-b*u_x +a*u_y +   u_z*v_y - a*v_z))
              w' = 	   ((   v_x -g*v_y + b*v_z)*( g*u_x +  u_y - a*u_z) - ( g*v_x +  v_y - a*v_z)*(   u_x -g*u_y + b*u_z))
			   ((   u_x -g*u_y + b*u_z)*( g*v_x +  v_y - a*v_z) + (   v_x -g*v_y + b*v_z)*( g*u_x +  u_y - a*u_z))

       */
      if (! fitterInfo) continue;
      genfit::MeasuredStateOnPlane& measuredPointState =  (genfit::MeasuredStateOnPlane&) fitterInfo->getFittedState(kTRUE);
      if (detId == kTpcId) {
	StTpcHit *tpcHit = (StTpcHit *) hit;
	genfit::DetPlane &startPlane = (*(measuredPointState.getPlane()));
	Double_t dx[2] = {0};
	TVector3 O(tpcHit->position().x(),tpcHit->position().y(),tpcHit->position().z());
	TVector3 dO;
	for (Int_t i = 0; i < 2; i++) {
	  if (! i) dO = TVector3(tpcHit->positionU().x(),tpcHit->positionU().y(),tpcHit->positionU().z());
	  else     dO = TVector3(tpcHit->positionL().x(),tpcHit->positionL().y(),tpcHit->positionL().z());
	  dO -= O;
	  genfit::DetPlane *aPlane = new genfit::DetPlane(startPlane);
	  TVector3 o = startPlane.getO();
	  aPlane->setO(o+dO);
	  genfit::StateOnPlane state = *measuredPointState.clone();
	  dx[i] = state.extrapolateToPlane(genfit::SharedPlanePtr(aPlane),i);
	}
	tpcHit->setdX(TMath::Abs(dx[0])+TMath::Abs(dx[1]));
      }
    }
  }
  genfit::TrackPoint *firstTP = track->getPointWithMeasurementAndFitterInfo(0);
  genfit::TrackPoint *lastTP  = track->getPointWithMeasurementAndFitterInfo(-1);
  genfit::TrackPoint *flTP[2] = {firstTP, lastTP};
  for (Int_t i = 0; i < 2; i++) {
    genfit::TrackPoint *tp = flTP[i];
    if (! tp) continue;
    genfit::AbsFitterInfo* fitterInfo = tp->getFitterInfo();
    if (fitterInfo) {
      const genfit::MeasuredStateOnPlane& measuredPointState = fitterInfo->getFittedState(kTRUE);
      TVector3 pos = measuredPointState.getPos();
      StThreeVectorF posF(pos.X(), pos.Y(), pos.Z());
      if (! i) detInfo->setFirstPoint(posF);
      else     detInfo->setLastPoint (posF);
    } else {
      for (std::vector< genfit::AbsMeasurement* >::const_iterator im = tp->getRawMeasurements().begin(); 
	   im !=  tp->getRawMeasurements().end(); ++im) {
	const StPlanarMeasurement *measurement =  dynamic_cast<StPlanarMeasurement *>(*im);
	if (! measurement) continue;
	const StHit *hit = measurement->Hit();
	if (! hit) continue;
	if (! i) detInfo->setFirstPoint(hit->position());
	else     detInfo->setLastPoint (hit->position());
      }
    }
  }
  // fitTraits
  // mass
  // this makes no sense right now... Double_t massHyp = track->getMass();  // change: perhaps this mass is not set right?
  UShort_t geantIdPidHyp = 9999;
  //if (.13< massHyp<.14) 
  geantIdPidHyp = 9;
#if 0
  // chi square and covariance matrix, plus other stuff from the
  // innermost track node
  genfit::Track Node* node = wattrack->InnerMostHitNode(3);
  Float_t x[6],covMFloat[15];
  node->GlobalTpt(x,covMFloat);
#else
  Float_t covMFloat[15] = {0};
#endif
  Float_t chi2[2] = {0, -999};
  if (fitStatus) {
    //get chi2/dof
    chi2[0] = fitStatus->getChi2()/fitStatus->getNdf();
  } else {
    if (fCA2Gen[0].NDF >0)  chi2[0] = fCA2Gen[0].chi2/fCA2Gen[0].NDF;
  }
  // setFitTraits uses assignment operator of StTrackFitTraits, which is the default one,
  // which does a memberwise copy.  Therefore, constructing a local instance of 
  // StTrackFitTraits is fine, as it will get properly copied.
  StTrackFitTraits fitTraits(geantIdPidHyp,0,chi2,covMFloat);
  for (Int_t i=1;i<kMaxDetectorId;i++) {
    if(!dets[i][0]) continue;
    gTrack->setNumberOfPossiblePoints((UChar_t)dets[i][0],(StDetectorId)i);
    if (!dets[i][1]) continue;
    detInfo->setNumberOfPoints(dets[i][1],static_cast<StDetectorId>(i));
    if (!dets[i][2]) continue;
    fitTraits.setNumberOfFitPoints((UChar_t)dets[i][2],(StDetectorId)i);
  }
  StSPtrVecTrackDetectorInfo& detInfoVec = mEvent->trackDetectorInfo(); 
  detInfoVec.push_back(detInfo);
  gTrack->setDetectorInfo(detInfo);
  gTrack->setFitTraits(fitTraits);
  
  return kStOk;
}
//_____________________________________________________________________________
void StxMaker::FillGeometry(StTrack* gTrack, genfit::Track * track, Bool_t outer) {
  assert(gTrack);
  assert(track) ;
  genfit::AbsTrackRep* rep = 0;
  genfit::TrackPoint* point = 0;
  TVector3 pos, mom;
  TMatrixDSym cov(6);
  Double_t charge = 0;
  if (! outer) point = track->getPointWithMeasurementAndFitterInfo(0, rep);
  else         point = track->getPointWithMeasurementAndFitterInfo(-1, rep);
  if ( point) {
    genfit::AbsFitterInfo* fitterInfo = point->getFitterInfo(rep);
    const genfit::MeasuredStateOnPlane& measuredPointState = fitterInfo->getFittedState(kTRUE);
    measuredPointState.getPosMomCov(pos, mom, cov);
    charge = measuredPointState.getCharge();
  } else {
    Int_t io = 0;
    if (outer) io = 1;
#if 0
    const TVectorD  &stateSeed = track->getStateSeed();
    cov  = track->getCovSeed();
#else
    const TVectorD  &stateSeed = fCA2Gen[io].state7;
    cov                        = fCA2Gen[io].origCov;
#endif
    pos = TVector3(stateSeed(0),stateSeed(1),stateSeed(2));
    mom = TVector3(stateSeed(3),stateSeed(4),stateSeed(5));
    rep = track->getCardinalRep();
    if (rep) charge = rep->getPDGCharge();
  }
  TVector3 field = FieldManager::getInstance()->getField()->get(pos);
  StThreeVectorF origin(pos.X(),pos.Y(),pos.Z());
  static const Double_t EC = 2.99792458e-4;
  StThreeVectorF p(mom.X(), mom.Y(), mom.Z());
  Double_t hz = EC*field.Z();
  Double_t qovepT = charge/mom.Pt();
  Double_t curvature = - hz*qovepT;
  Double_t helicity = (curvature < 0) ? -1 : 1;
  StTrackGeometry* geometry = new StHelixModel(short(charge),
					       mom.Phi(),
					       fabs(curvature), 
					       TMath::PiOver2() - mom.Theta(),
					       origin, 
					       p,
					       helicity);
  if (outer)
    gTrack->setOuterGeometry(geometry);
  else
    gTrack->setGeometry(geometry);
#ifdef  __kfpAtFirstHit__
  Double_t xyzp[6] = {pos.X(), pos.Y(), pos.Z(), mom.X(), mom.Y(), mom.Z()};
  Double_t CovXyzp[21];
  Int_t ii = 0;
  for (Int_t i = 0; i < 6; i++)
    for (Int_t j = 0; j < = i; j++)
      CovXyzp[ii] = cov(i,j);
  KFPTrack *KFPTrackAtHit = new KFPTrack();
  KFPTrackAtHit->SetID(gTrack->key());
  KFPTrackAtHit->SetCharge(track->getCharge());
  KFPTrackAtHit->SetParameters(xyzp);
  KFPTrackAtHit->SetCovarianceMatrix(CovXyzp);
  if (outer) gTrack->setKFPTrackatLastHit(KFPTrackAtHit);
  else       gTrack->setKFPTrackatFirstHit(KFPTrackAtHit);
#endif
  return;
}
//_____________________________________________________________________________
void StxMaker::FillDca(StTrack* stTrack, genfit::Track * track) {
  StGlobalTrack *gTrack = dynamic_cast<StGlobalTrack*>(stTrack);
  assert(gTrack);
  Float_t length = gTrack->length();
  const AbsTrackRep* rep = track->getCardinalRep();
  genfit::TrackPoint* point = track->getPointWithMeasurementAndFitterInfo(0, rep);
  genfit::MeasuredStateOnPlane  state;
  if (point) {
    genfit::AbsFitterInfo* fitterInfo = point->getFitterInfo(rep);
    const genfit::MeasuredStateOnPlane& stateI = fitterInfo->getFittedState(kTRUE);
    state = stateI;
  } else {
    length = 100; // TO DO
    gTrack->setLength(length);
    // form state from seed
    const TVectorD  &stateSeed = track->getStateSeed();
    TVector3 pos, mom;
    TMatrixDSym cov(6);
    pos = TVector3(stateSeed(0),stateSeed(1),stateSeed(2));
    mom = TVector3(stateSeed(3),stateSeed(4),stateSeed(5));
    cov  = track->getCovSeed();
    rep = track->getCardinalRep();
    genfit::MeasuredStateOnPlane stateI(rep);
    stateI.setPosMomCov(stateSeed,cov);
    state = stateI;
  }
  Bool_t ok = kTRUE;
  StPhysicalHelixD helixI = gTrack->geometry()->helix();
  Double_t step = helixI.pathLength(0.0,0.0);
  StThreeVectorD dcaH = helixI.at(step);
  // No Dca for tracks outside of IFC
  if (TMath::Abs(dcaH.z()) > 250 || dcaH.perp() > 50) ok = kFALSE;
  Double_t s = 0;
  if (ok) {
    static TVector3 linePoint(0,0,0);
    static TVector3 lineDirection(0,0,1);
    try{
      s = rep->extrapolateToLine(state, linePoint, lineDirection);
    }
    catch(genfit::Exception& e) {
      std::cout << "Exception, fail to make DCA" << std::endl;
      std::cout << "Inner Parameters" << std::endl << "====================" << std::endl;
      track->getFittedState().Print();
      std::cout << "Outer Parameters" << std::endl << "====================" << std::endl;
      track->getFittedState(-1).Print();
      std::cout << "Dca from Helix\t" << dcaH << std::endl;
      s = 0;
      ok = kFALSE;
    }
    if (Debug()) {
      state.Print();
    }
  }
  length -= s;
  KalmanFitStatus *fitStatus = track->getKalmanFitStatus();
  Double_t chi2 = 0;
  Int_t Ndf = -1; 
  if (fitStatus) {
    chi2 = fitStatus->getChi2();
    Ndf = fitStatus->getNdf();
  }
  Double_t charge = state.getCharge();
  TVector3 mom, pos;
  TMatrixDSym cov(6,6);
  state.getPosMomCov(pos, mom, cov);
  // ___  KFParticle at Dca to (0,0) ____
  Int_t kg = gTrack->key();
  static KFParticle fParticle;
  static KFPTrack kfTrack;
  Float_t xyzp[6], CovXyzp[21];
  pos.GetXYZ(xyzp);
  mom.GetXYZ(xyzp+3);
  Int_t ij = 0;
  for (Int_t i = 0; i < 6; i++) {
    for (Int_t j = 0; j <= i; j++) {
      CovXyzp[ij] = cov(i,j);
      ij++;
    }
  }
  kfTrack.SetParameters(xyzp);
  kfTrack.SetCovarianceMatrix(CovXyzp);
  kfTrack.SetNDF(Ndf);
  kfTrack.SetChi2(chi2);
  //  kfTrack.SetId(kg);
  Int_t pdg = rep->getPDG();
  kfTrack.SetCharge(charge);
  fParticle = KFParticle(kfTrack, pdg);
  fParticle.SetPDG(pdg);
  fParticle.SetId(kg);
  fParticle.AddDaughterId(kg);
  StTrackMassFit *dcaTrack = new StTrackMassFit(kg, &fParticle);
  dcaTrack->setLength(length);
  gTrack->node()->addTrack(dcaTrack);
#if 0
  gTrack->node()->Print();
#endif
  if (ok) {
    Double_t pT = mom.Pt();
    TVector3 field = FieldManager::getInstance()->getField()->get(pos);
    static const Double_t EC = 2.99792458e-4;
    Double_t hz = EC*field.Z();
    Double_t qoverpT = charge/mom.Pt();
    Double_t qoverpT2 = qoverpT * qoverpT;
    Double_t curvature = - hz*qoverpT;
    Double_t cosPsi = mom.X()/pT;
    Double_t sinPsi = mom.Y()/pT;
    Double_t Psi = TMath::ATan2(sinPsi, cosPsi);
    Double_t tanL   = mom.Z()/pT;
    Double_t Imp    = -pos.X()*sinPsi + pos.Y()*cosPsi;
    Double_t V = - qoverpT;
    Double_t dImpdPsi = -(pos.X()*cosPsi + pos.Y()*sinPsi);
    Double_t dPsidpx  = -mom.Y()*qoverpT2;
    Double_t dPsidpy  =  mom.X()*qoverpT2;
    Double_t VpT2  = V/(pT*pT);
    Double_t f[30] = {
      //                 x,       y,    z,               px,               py,    pz	
      /* Imp   */  -sinPsi,  cosPsi,    0, dImpdPsi*dPsidpx, dImpdPsi*dPsidpy,     0,
      /* Z     */        0,       0,    1,                0,                0,     0,
      /* Psi   */        0,       0,    0,          dPsidpx,          dPsidpy,     0,
      /* -q/pT */        0,       0,    0,    -VpT2*mom.X(),    -VpT2*mom.Y(),     0,
      /* tanL  */        0,       0,    0,    -tanL*mom.X(),    -tanL*mom.Y(),  1./pT
    };
    TRMatrix F(5,6,f);
    TRSymMatrix C(6,CovXyzp);
    TRSymMatrix Cov(F,TRArray::kAxSxAT,C);
    Float_t setp[6];
    setp[0] = Imp; 
    setp[1] = pos.Z(); 
    setp[2] = Psi; 
    setp[3] = V; 
    setp[4] = tanL; 
    setp[5] = curvature;
    Float_t sete[15];
    for (Int_t i = 0; i < 15; i++) sete[i] = Cov[i];
    StDcaGeometry *dca = new StDcaGeometry;
    gTrack->setDcaGeometry(dca);
    dca->set(setp,sete);
  }
}

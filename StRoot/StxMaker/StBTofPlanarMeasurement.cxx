#include <cassert>
#include "StBTofPlanarMeasurement.h"
#include "GenFit/Exception.h"
#include "GenFit/AbsMeasurement.h"
#include "GenFit/RKTrackRep.h"
#include "GenFit/HMatrixU.h"
#include "GenFit/HMatrixV.h"
#include "GenFit/HMatrixUV.h"
#include "GenFit/StateOnPlane.h"
#include "StEvent/StBTofHit.h"
#include "StEvent/StEnumerations.h"
#include "StDetectorDbMaker/StiBTofHitErrorCalculator.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoPhysicalNode.h"
#include "TGeoBBox.h"
#include "StarVMC/StarVMCApplication/StarVMCDetector.h"
#include "TMath.h"
using namespace genfit;
//________________________________________________________________________________
StBTofPlanarMeasurement::StBTofPlanarMeasurement(int nDim)
  : StPlanarMeasurement(nDim) {}
//________________________________________________________________________________
StBTofPlanarMeasurement::StBTofPlanarMeasurement(const TVectorD& rawHitCoords, const TMatrixDSym& rawHitCov, int detId, int hitId, TrackPoint* trackPoint)
  : StPlanarMeasurement(rawHitCoords, rawHitCov, detId, hitId, trackPoint) {}
//________________________________________________________________________________
StBTofPlanarMeasurement::StBTofPlanarMeasurement(const StBTofHit *hit,TrackPoint* trackPoint) : StPlanarMeasurement(2) {
  fHit = hit;
  detId_ = (Int_t) fHit->detector();
  hitId_ = fHit->id();
  Int_t tray = hit->tray();
  Int_t moduleRC = hit->module();
  fErrCalc = StiBTofHitErrorCalculator::instance(); 
  Int_t planeId = 100000*kBTofId + 100*tray + moduleRC;
  TString path = hit->GetPath(); // 
  if (! gGeoManager->CheckPath(path)) {
    cout << "Illegal path " << path.Data() << endl;
    assert(0);
  }
  TObjArray *nodes = gGeoManager->GetListOfPhysicalNodes();
  TGeoPhysicalNode *nodeP = 0;
  if (nodes) nodeP = (TGeoPhysicalNode *) nodes->FindObject(path);
  if (! nodeP) nodeP =gGeoManager->MakePhysicalNode(path);
  if (! nodeP) {
    cout << "TGeoPhysicalNode with path " << path.Data() << " does not exists" << endl;
    assert(0);
  }
  const TGeoHMatrix &D = *nodeP->GetMatrix();
  //  genfit::eMeasurementType type = genfit::Spacepoint;
  //  measurementTypes.push_back(type);
  Double_t xyzL[3] = {hit->position().x(),hit->position().y(),hit->position().z()};
  Double_t xyzG[3];
  D.LocalToMaster(xyzL,xyzG);
  TGeoHMatrix DT(D);
  if (DT.Determinant() < 0) {
    Double_t *r = DT.GetRotationMatrix();
    //      for (Int_t i = 0; i < 9; i++) r[i] = - r[i];
    r[1] = - r[1];
    r[4] = - r[4];
    r[7] = - r[7];
  }
  TVector3 o(DT.GetTranslation());
  const Double_t *r = DT.GetRotationMatrix();
  TVector3 u(r[1],r[4],r[7]);
  TVector3 v(r[2],r[5],r[8]);
  setPlane(genfit::SharedPlanePtr(new genfit::DetPlane(o,u,v)),planeId);
  rawHitCoords_[0] = xyzL[1];
  rawHitCoords_[1] = xyzL[2];
  static float BTofPadWidth  =   3.45;        //! Pad Width    
  static float BTofPadLength = 2*4.70;        //! Pad Length
  static Double_t sigma_y = BTofPadWidth/TMath::Sqrt(12.);
  static Double_t sigma_z = BTofPadLength/TMath::Sqrt(12.);
  static Double_t ecross = sigma_y*sigma_y, edip = sigma_z*sigma_z;
  rawHitCov_(0,0) = ecross;
  rawHitCov_(1,1) = edip;
  if (Debug()) {
    cout << path.Data() << " local xyz " << xyzL[0] << "/" << xyzL[1] << "/" << xyzL[2] <<  endl;
    getPlane()->Print();
    hit->Print();
  }
}
#if 0
//________________________________________________________________________________
const TVectorD& StBTofPlanarMeasurement::getRawHitCoords(genfit::StateOnPlane *state) const {return genfit::AbsMeasurement::getRawHitCoords(state);}
//________________________________________________________________________________
const TMatrixDSym& StBTofPlanarMeasurement::getRawHitCov(genfit::StateOnPlane *state) const {return genfit::AbsMeasurement::getRawHitCov(state);}
#endif
//________________________________________________________________________________
TVectorD& StBTofPlanarMeasurement::getRawHitCoords(const genfit::StateOnPlane *state) {return genfit::AbsMeasurement::getRawHitCoords(state);}
//________________________________________________________________________________
TMatrixDSym& StBTofPlanarMeasurement::getRawHitCov(const genfit::StateOnPlane *state) {
  Double_t eta  = TMath::ATan(state->getState()(1));
  Double_t tanL = state->getState()(2);
  Double_t Z    = state->getState()(4);
  TMatrixDSym &rawHitCov = genfit::AbsMeasurement::getRawHitCov(state);
  fErrCalc->calculateError(Z,eta,tanL, rawHitCov(0,0), rawHitCov(1,1));
  return rawHitCov;
}
//________________________________________________________________________________

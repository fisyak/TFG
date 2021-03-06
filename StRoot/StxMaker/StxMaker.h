// $Id: StxMaker.h,v 1.2 2013/09/16 19:54:04 fisyak Exp $
#ifndef STAR_StHTLCAMaker
#define STAR_StHTLCAMaker

/*!
 *                                                                     
 * \class  StxMaker
 * \author Yuri Fisyak
 * \date   2013/08/12
 * \brief  
 *
 */                                                                      

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include <vector>
#include "TVector3.h"
#include "TMatrixDSym.h"
#include "GenFit/Track.h"
#include "StThreeVectorD.hh"
#include "GenFit/MeasuredStateOnPlane.h"
#include "TGeoManager.h"
class AliHLTTPCCAGBTrack;
class AliHLTTPCCATrackParam;
class StHit;
class StEvent;
class StTrack;
class StPrimaryTrack;
class StTrackDetectorInfo;
class StxCApar;
class CA2GenState_t  {
 public:
 CA2GenState_t() : pos(), mom(), covM(), chi2(-1), NDF(-1) {}
  TVector3 pos;
  TVector3 mom;
  TMatrixDSym covM;
  Float_t chi2;
  Int_t   NDF;
  TVectorD  state7;
  TMatrixDSym origCov;
};
class StxMaker : public StMaker {
 public: 
  StxMaker(const char *name="Stx") : StMaker(name) {}
  virtual      ~StxMaker() {}
  virtual Int_t InitRun(Int_t runumber);
  virtual Int_t Make();
  vector<const StHit *> *GetHitVector(const AliHLTTPCCAGBTrack *tr, Bool_t reverse = kFALSE);
  void          PrintGBTrack(const AliHLTTPCCAGBTrack *tr, Int_t iTr = 0);
  virtual Int_t FitTrack(const AliHLTTPCCAGBTrack *tr);
  virtual Int_t FitTrack(Double_t alpha, const AliHLTTPCCATrackParam *InnerParam, const AliHLTTPCCATrackParam *OuterParam, vector<const StHit*> *HitVect);
  static Double_t ConvertCA2Gen(const Double_t alpha, const StxCApar* stxPar, CA2GenState_t* ca2Gen );
  virtual void   SetDebugX();          // *MENU*
  Bool_t Accept(genfit::Track *kTrack);
  void   FillGlobalTrack(genfit::Track *kTrack);
  void   FillPrimaryTracks();
  void   FillPrimaryTrack(StPrimaryTrack *pTrack);
  Int_t  FillDetectorInfo(StTrack *gTrack, genfit::Track * track, Bool_t refCountIncr) ;
  void   FillGeometry(StTrack* gTrack, genfit::Track * track, Bool_t outer);
  Int_t  FillTrack(StTrack* gTrack, genfit::Track * track);
  void   FillDca(StTrack* stTrack, genfit::Track * track);
  CA2GenState_t fCA2Gen[2];// 0 -> Inner, 1 -> Outer
  void UseOriginalGeometry() {if (gGeoManagerOrig) gGeoManager = gGeoManagerOrig;}
  void UseAvergadGeometry()  {if (gGeoManagerAver) gGeoManager = gGeoManagerAver;}
  static TGeoManager *OriginalGeometry() {return gGeoManagerOrig;}
  static TGeoManager *AverageGeometry() {return gGeoManagerAver;}
  static void MakeAvergedGeometry();
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StxMaker.h,v 1.2 2013/09/16 19:54:04 fisyak Exp $ built " __DATE__ " " __TIME__ ; 
    return cvs;
  }
 private:
  StEvent *mEvent;
  static TGeoManager *gGeoManagerOrig;
  static TGeoManager *gGeoManagerAver;
  ClassDef(StxMaker,0)   //StAF chain virtual base class for Makers
};

#endif


// $Log: StxMaker.h,v $

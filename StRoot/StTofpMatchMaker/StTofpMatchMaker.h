//! TOFp Match Maker
/*! \class StTofpMatchMaker
 *  \brief Match Maker for the TOFp detector
 *  \author Frank Geurts
 *  \date August 2003
 *
 *  The TOFp MatchMaker matches TPC tracks to the TOFp scintillator
 *  slats. The code expects the raw TOFp data to be available in
 *  tofCollection->tofData() and will store the matches in
 *  tofCollection->tofSlats().
 *
 * $Id: StTofpMatchMaker.h,v 1.1 2003/08/08 18:31:26 geurts Exp $
 */    
/*  -------------------------------------------------------------------------
 * $Log: StTofpMatchMaker.h,v $
 * Revision 1.1  2003/08/08 18:31:26  geurts
 * first release
 *
 *
 * -------------------------------------------------------------------------
 */
#ifndef STTOFPMATCHMAKER_HH     
#define STTOFPMATCHMAKER_HH
#include "StMaker.h"
#include <string>

class StEvent;
class StTrack;
class StHelix;
class StThreeVectorD;
class StTrackGeometry;
class StTofGeometry;
class StTofCollection;
class StTofSlatCollection;
class StSPtrVecTofData;
class TOrdCollection;
class TH1D;
class TH2D;



class StTofpMatchMaker : public StMaker {
  static const Int_t NTOFP = 41;
  static const Int_t NPVPD = 6;
public:
  StTofpMatchMaker(const Char_t *name="tofpMatch");
  ~StTofpMatchMaker();

  void   Clear(Option_t *option="");
  Int_t  Init();
  Int_t  InitRun(int);
  Int_t  FinishRun(int);
  Int_t  Make();
  Int_t  Finish();

  void initLocalDb(Bool_t localDb=kTRUE);
  void createHistograms(Bool_t histos=kTRUE);
  void setOuterTrackGeometry();
  void setStandardTrackGeometry();
  void setValidAdcRange(Int_t, Int_t);
  void setValidTdcRange(Int_t, Int_t);
  void setMinHitsPerTrack(Int_t);
  void setMinFitPointsPerTrack(Int_t);
  void setMaxDCA(Float_t);
  void setHistoFileName(Char_t*);

  Bool_t doPrintMemoryInfo; //! 
  Bool_t doPrintCpuInfo; //!

protected:
  StTrackGeometry* trackGeometry(StTrack*);//!
  Float_t slatPropagationTime(StThreeVectorD*); // calculate hit position correction
  Float_t startTime(const float); // calculated pvpd startTime
  Int_t getTofData(StTofCollection*); // check, remap and fill local arrays with tof and pvpd data
  Int_t storeMatchData(StTofSlatCollection*, StTofCollection*); 
  Float_t mTofpAdc[NTOFP];
  Float_t mTofpTdc[NTOFP];
  Float_t mPvpdAdc[NPVPD];
  Float_t mPvpdAdcLoRes[NPVPD];
  Float_t mPvpdTdc[NPVPD];


private:
  Bool_t strobeEvent(StSPtrVecTofData);// check pVPD data for strobe event
  void bookHistograms();
  void writeHistogramsToFile();
  StTofGeometry *mTofGeom; //! pointer to the TOF geometry utility class

  Bool_t mHisto; //! create, fill and write out histograms
  Bool_t mInitLocalDb; //! initialize from local (true) dbase or STAR dbase (false)
  Bool_t mYear2; //! STAR year2: TOFp+pVPD
  Bool_t mYear3; //! STAR year3: TOFp+pVPD+TOFr
  Bool_t mOuterTrackGeometry; //! use outer track geometry (true) for extrapolation
  string mHistoFileName; //! name of histogram file, if empty no write-out

  Bool_t validAdc(float const);
  Bool_t validTdc(float const);
  Bool_t validEvent(StEvent *);
  Bool_t validTrack(StTrack*);
  Bool_t validTofTrack(StTrack*);

  // event counters
  Int_t mEventCounter;          //! #processed events
  Int_t mAcceptedEventCounter;  //! #events w/ valid prim.vertex
  Int_t mTofEventCounter;       //! #events w/ Tof raw data
  Int_t mTofStrobeEventCounter; //! #(strobe events)
  Int_t mAcceptAndStrobe;       //! #(strobe events) w/ prim.vertex
  Int_t mAcceptAndBeam;         //! #(beam events) w/ prim.vertex

  // various cut-offs and ranges
  Float_t mMinValidTdc; //! lower cut on  TDC value
  Float_t mMaxValidTdc; //! upper cut on TDC value
  Float_t mMinValidAdc; //! lower cut on ADC value
  Float_t mMaxValidAdc; //! upper cut on ADC value
  UInt_t mMinHitsPerTrack; //! lower cut on #hits per track
  UInt_t mMinFitPointsPerTrack; //! lower cut on #fitpoints per track
  Float_t mMaxDCA; //! upper cut (centimeters) on final (global) DCA


  // TOFp histogram collections
  TOrdCollection* mHitPosHistNames;//!
  TOrdCollection* mTrackHistNames; //!
  TOrdCollection* mOccupancyHistNames; //!

  TH2D *hTofpHitMap1; //! tray hit positions -- see SlatId 
  TH2D *hTofpHitMap2; //! tray hit positions
  TH2D *hTofpHitMap3; //! tray hit positions
  TH2D *hTofpHitMap4; //! tray hit positions
  TH1D *hTofpSlatIdA0; //! events per slat
  TH1D *hTofpSlatIdA1; //! valid slat
  TH1D *hTofpSlatIdB1; //! #tracks match  valid slat
  TH1D *hTofpSlatIdD1; //! track match per valid slat
  TH1D *hTofpSlatIdD2; //! single track match per slat
  TH1D *hTofpSlatIdE1; //! one slat for one track match 
  TH1D *hTofpSlatIdE2; //! recovered from hitprof-weight
  TH1D *hTofpSlatIdE3; //! recovered from ss
  TH1D *hTofpSlatIdE4; //! recovered from closest hitplane
  TH1D *hTofpSlatIdE5; //! total recovered slat per track match
  TH1D *hTofpSlatIdF1; //! primary track match per slat
  TH1D *hTofpSlatHitVecSize;     //! slat mult per StTrack 

  TH1D *hTofpNumberOfTrackHits;  //! number of TPC hits per track
  TH1D *hTofpPtTrack;            //! pT per track
  TH1D *hTofpDCATrackprimVertex; //! DCA distribution


  // Occupancy plots
  TH1D *hTofpNumberOfValidAdc;        //! #valid slats w/ valid ADC
  TH1D *hTofpNumberOfValidTdc;        //! #valid slats w/ valid TDC
  TH1D *hTofpNumberOfValidSlats;      //! #valid slats w/ valid TDC and ADC
  TH1D *hTofpNumberOfGlobalTracks;    //! #global extrapolated tracks
  TH1D *hTofpNumberOfHitSlats;        //! #hit slats by global tracks
  TH1D *hTofpNumberOfSingleHitTracks; //! #single hit slats
  TH1D *hTofpNumberOfSingleValidHitTracks;//!#single valid hit slats

  // track matching plots
  TOrdCollection* mMatchHistNames;//!
  TH2D *hTofpMatchHit[NTOFP]; //!
  TH2D *hTofpMatchNoHit[NTOFP]; //!

  virtual const char *GetCVS() const 
    {static const char cvs[]="Tag $Name:  $ $Id: StTofpMatchMaker.h,v 1.1 2003/08/08 18:31:26 geurts Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StTofpMatchMaker,1)
};

inline void StTofpMatchMaker::initLocalDb(Bool_t localDb){mInitLocalDb = localDb;}
inline void StTofpMatchMaker::setValidAdcRange(Int_t min, Int_t max){mMinValidAdc=min; mMaxValidAdc=max;}
inline void StTofpMatchMaker::setValidTdcRange(Int_t min, Int_t max){mMinValidTdc=min; mMaxValidTdc=max;}
inline void StTofpMatchMaker::setOuterTrackGeometry(){mOuterTrackGeometry=true;}
inline void StTofpMatchMaker::setStandardTrackGeometry(){mOuterTrackGeometry=false;}
inline void StTofpMatchMaker::setMinHitsPerTrack(Int_t nhits){mMinHitsPerTrack=nhits;}
inline void StTofpMatchMaker::setMinFitPointsPerTrack(Int_t nfitpnts){mMinFitPointsPerTrack=nfitpnts;}
inline void StTofpMatchMaker::setMaxDCA(Float_t maxdca){mMaxDCA=maxdca;}
inline void StTofpMatchMaker::setHistoFileName(Char_t* filename){mHistoFileName=filename;}
inline void StTofpMatchMaker::createHistograms(Bool_t histos){mHisto = histos;}
inline bool StTofpMatchMaker::validAdc(const float adc){return((adc>=mMinValidAdc) && (adc<=mMaxValidAdc));}
inline bool StTofpMatchMaker::validTdc(const float tdc){return((tdc>=mMinValidTdc) && (tdc<=mMaxValidTdc));}

#endif

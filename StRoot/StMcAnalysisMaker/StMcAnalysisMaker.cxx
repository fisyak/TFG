/*************************************************
 *
 * $Id: StMcAnalysisMaker.cxx,v 1.38 2011/04/03 16:02:40 fisyak Exp $
 * $Log: StMcAnalysisMaker.cxx,v $
 * Revision 1.38  2011/04/03 16:02:40  fisyak
 * Fix effect of constness in StAssociationMaker
 *
 * Revision 1.37  2010/06/22 16:23:25  fine
 * introduce the correct const StMcTarck * pointer cast
 *
 * Revision 1.36  2010/05/10 17:15:34  fine
 * RT # 1932 Remove the redundant correction. Restore 1.34 rev
 *
 * Revision 1.35  2010/05/07 20:17:18  fine
 * Add CPP macro to separate McTracks
 *
 * Revision 1.34  2010/01/28 18:11:46  perev
 * WarnOff
 *
 * Revision 1.33  2007/04/17 05:08:36  perev
 * GetTFile()==>StMaker. Jerome request
 *
 * Revision 1.32  2007/03/21 17:14:36  fisyak
 * Add keys for switch off Tpc, Svt and Ssd NTuples
 *
 * Revision 1.31  2006/06/20 02:39:38  calderon
 * Fixed several changes from the last version which made this package not
 * executable using the default StAssociator.C macro.
 * Removed dependence on StBFChain, so the default StAssociator.C macro can work.
 * Removed #if 0  commands which had taken out the track ntuple.
 * Added back the missing code to produce the first 8 entries of the ntuple.
 * Reverted to the file handling as was done before so that StAssociator.C can work.
 * Kept the ntuples added by Yuri (but did not check them).
 *
 * Revision 1.30  2005/11/22 21:51:53  fisyak
 * Add NTuple for svt and ssd hit
 *
 * Revision 1.29  2005/09/28 22:52:52  calderon
 * Changed access to StMcEvent to use GetDataSet to be consistent with persistent StMcEvent.
 *
 * Revision 1.28  2005/07/19 22:04:49  perev
 * MultiVertex
 *
 * Revision 1.27  2004/03/30 03:11:18  calderon
 * Added information about the matching into the track Ntuple:
 *  - Dominatrack... (track with most common IdTruth, a.k.a. dominant contributor
 *  - nHitsIdTruth (should be very similar to commonTpcHits, but the distance cut
 *    option might make them different)
 *  - n MC hits for the dominatrack
 *  - n Fit points for the reconstructed track
 *  - n Points (from StDetectorInfo).
 *  - "quality" = average hit quality of the reco hits belonging to the dominatrack.
 *
 * Revision 1.26  2004/02/08 00:13:05  calderon
 * Check that the size of the map is non-zero to get the first track.
 *
 * Revision 1.25  2004/02/08 00:10:58  calderon
 * Histogram of rc hits is now not restricted to TPC only.   Histogram of mc hits is
 * done looping over tpc and svt hits.
 * If the first entry of the track nodes is not associated, use the first entry of
 * the map.
 *
 * Revision 1.24  2004/01/24 04:35:53  calderon
 * The last commits using BFChain broke StAssociator.  The histograms were
 * not found and nothing was plotted.  Revert back to the usual mode (this
 * code is not meant to run in bfc anyway, it's meant as a collection of
 * examples).
 *
 * Revision 1.23  2004/01/17 19:12:28  fisyak
 * Add protection when StMcAnalysisMaker is not running within StBFChain and/or TFile is not coming from StBFChain parameters
 *
 * Revision 1.22  2004/01/13 21:06:04  fisyak
 * Add TpcHitNtuple usind IdTruth info
 *
 * Revision 1.21  2003/09/02 17:58:41  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.20  2000/05/11 21:57:34  calderon
 * Book the histograms before creating the file so that Kathy's macros
 * can pick them up.
 *
 * Revision 1.19  2000/05/11 16:21:26  calderon
 * Write only one V0 matched pair to supress the screen output.
 *
 * Revision 1.18  2000/04/20 21:31:52  calderon
 * More checks for the cases where a track has no partner, Thanks Janet.
 *
 * Revision 1.17  2000/04/20 16:59:47  calderon
 * Pick up the makers with the new names
 * Change the name from "McAnalysis" to "StMcAnalysisMaker"
 * No longer use the helix, use the primary track momentum when available
 * (also avoids dependency on StEventSummary)
 * Denominator for mom. resolution histograms is now simulated momentum.
 *
 * Revision 1.16  2000/04/12 21:33:57  calderon
 * check case where no multimaps are found
 *
 * Revision 1.15  2000/04/04 23:18:12  calderon
 * B -> B * kilogauss
 *
 * Revision 1.14  2000/03/28 02:28:04  calderon
 * Return to calculating momentum at the origin, instead of taking the
 * momentum from the first point.
 *
 * Revision 1.13  2000/03/06 21:47:56  calderon
 * Add Lee's V0 example
 *
 * Revision 1.12  2000/02/07 16:43:37  calderon
 * Find the first hit wherever it may be, instead of assuming a sector and padrow.
 *
 * Revision 1.11  2000/01/24 22:22:24  calderon
 * use delete [] for the array of floats
 *
 * Revision 1.10  1999/12/14 07:08:48  calderon
 * First version to work with new StEvent, StMcEvent & StAssociationMaker.
 * Need to add more examples of all the new maps.
 *
 * Revision 1.9  1999/10/01 14:11:15  calderon
 * Chech to see whether StEvent has primary vertex
 * before trying to use it.  If no primary vertex is
 * found, assume its position is (0,0,0).
 *
 * Revision 1.8  1999/09/28 15:03:29  didenko
 * Cleanup dependencies on non existing h-files
 *
 * Revision 1.7  1999/09/23 21:25:48  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 * Revision 1.6  1999/09/10 19:11:15  calderon
 * Write the Ntuple in StMcAnalysisMaker into a file.
 * This way it can be accessed after the macro finishes,
 * otherwise it gets deleted.
 *
 * Revision 1.5  1999/09/09 23:59:37  calderon
 * Made the following changes:
 *
 * -book histograms and ntuple in Init()
 * -do not delete the histograms, they are supposed to be
 *  deleted automatically by the chain
 * -don't create the canvas here, now done in the macro
 *
 * Revision 1.4  1999/07/30 16:19:19  calderon
 * Use value_type typedef for inserting pairs in multimaps, Victor corrected iterators on HP in SL99h, Improved use of const for HP compilation
 *
 * Revision 1.3  1999/07/29 15:08:33  calderon
 * Include Mom. Resolution example (Histograms & Ntuple)
 *
 * Revision 1.2  1999/07/28 20:27:30  calderon
 * Version with SL99f libraries
 *
 *
 * Examples that use the structures of
 * StMcEvent and StAssociationMaker
 *
 *************************************************/
#include <assert.h>
#include <Stiostream.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cmath>
#include "TMath.h"
#include "TStyle.h"
#include "TNtuple.h"
#include "TFile.h"

#include "StMcAnalysisMaker.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"

#include "StMessMgr.h"

#include "StAssociationMaker/StAssociationMaker.h"
#include "StAssociationMaker/StTrackPairInfo.hh"

#include "StThreeVectorF.hh"

#include "StEventTypes.h"

#include "StMcEventTypes.hh"

#include "StMcEvent.hh"

struct TpcHitPair_t {
  Float_t xM, yM, zM, pxM, pyM, pzM, dEM, dSM, nM,
          xR, yR, zR, dER, IdM, IdR, qR, nR;
};
struct TpcHitMRPair_t {
  Float_t sector, row, isDet;
  TpcHitPair_t current;
  TpcHitPair_t previous;
};
static const Char_t *vTpcHitMRPair = "sector:row:isDet:xM:yM:zM:pxM:pyM:pzM:dEM:dSM:nM:xR:yR:zR:dER:IdM:IdR:qR:nR"
  ":xMp:yMp:zMp:pxMp:pyMp:pzMp:dEMp:dSMp:nMp:xRp:yRp:zRp:dERp:IdMp:IdRp:qRp:nRp";
static TpcHitMRPair_t TpcHitMRPair;
static TpcHitPair_t current;
static TpcHitPair_t previous;
struct SvtHitMRPair_t {
  Float_t barrel, layer, ladder, wafer,  hybrid, index,
    xM, yM, zM, pxM, pyM, pzM, dEM, dSM, nM,
    xR, yR, zR, dER, IdM, IdR, qR, nR;
};
static const Char_t *vSvtHitMRPair = "barrel:layer:ladder:wafer:hybrid:index:xM:yM:zM:pxM:pyM:pzM:dEM:dSM:nM:xR:yR:zR:dER:IdM:IdR:qR:nR";
static SvtHitMRPair_t SvtHitMRPair;
struct SsdHitMRPair_t {
  Float_t ladder, wafer,
    xM, yM, zM, pxM, pyM, pzM, dEM, dSM, nM,
    xR, yR, zR, dER, IdM, IdR, qR, nR;
};
static const Char_t *vSsdHitMRPair = "ladder:wafer:xM:yM:zM:pxM:pyM:pzM:dEM:dSM:nM:xR:yR:zR:dER:IdM:IdR:qR:nR";
static SsdHitMRPair_t SsdHitMRPair;
ClassImp(StMcAnalysisMaker)

//_________________________________________________
StMcAnalysisMaker::StMcAnalysisMaker(const char *name, const char *title):
  StMaker(name,title)
{
    //  StMcAnalysisMaker Constructor
    // - zero all pointers defined in the header file
    mTrackNtuple    = 0;
    mTpcHitNtuple   = 0;
    mSvtHitNtuple   = 0;
    mSsdHitNtuple   = 0;
}
//_________________________________________________
Int_t StMcAnalysisMaker::Init()
{
    TFile *f = GetTFile();
    assert(f);
    f->cd();
    const Char_t* vars = "px:py:pz:p:pxrec:pyrec:pzrec:prec:commTpcHits:hitDiffX:hitDiffY:hitDiffZ:mcTrkId:mostCommIdTruth:nHitsIdTruth:nMcHits:nFitPts:nDetPts:quality";
    mTrackNtuple = new TNtuple("TrackNtuple","Track Pair Info",vars);
    mTrackNtuple->SetAutoSave(100000000);
    if (! m_Mode || m_Mode & 0x1) {
      mTpcHitNtuple = new TNtuple("TpcHitNtuple","the TPC hit pairs Info",vTpcHitMRPair);
      mTpcHitNtuple->SetAutoSave(100000000);
    }
    if (! m_Mode || m_Mode & 0x2) {
      mSvtHitNtuple = new TNtuple("SvtHitNtuple","the SVT hit pairs Info",vSvtHitMRPair);
      mSvtHitNtuple->SetAutoSave(100000000);
    }
    if (! m_Mode || m_Mode & 0x4) {
      mSsdHitNtuple = new TNtuple("SsdHitNtuple","the SSD hit pairs Info",vSsdHitMRPair);
      mSsdHitNtuple->SetAutoSave(100000000);
    }
    return StMaker::Init();
}
//_________________________________________________
Int_t StMcAnalysisMaker::Make()
{
  // Get the pointers we need, we have to use the titles we gave them in the
  // macro.  I just used the defaults.
  
  // StEvent
  StEvent* rEvent =  (StEvent*) GetInputDS("StEvent");
  const StMcTrack  *mTrack = 0;
  
  // StMcEvent
  StMcEvent* mEvent = (StMcEvent*) GetDataSet("StMcEvent");
  
  // StAssociationMaker
  StAssociationMaker* assoc = 0;
  assoc = (StAssociationMaker*) GetMaker("StAssociationMaker");
  
  // the Multimaps...
  rcTpcHitMapType* theHitMap   = assoc->rcTpcHitMap();
  mcTpcHitMapType* theMcHitMap = assoc->mcTpcHitMap();
  rcSvtHitMapType* svtHitMap   = assoc->rcSvtHitMap();
  mcSvtHitMapType* svtMcHitMap = assoc->mcSvtHitMap();
  rcSsdHitMapType* ssdHitMap   = assoc->rcSsdHitMap();
  mcSsdHitMapType* ssdMcHitMap = assoc->mcSsdHitMap();
  rcTrackMapType*  theTrackMap = assoc->rcTrackMap();
  
  if (!theHitMap) {
    gMessMgr->Warning() << "----------WARNING----------\n"
			<< "No Hit Map found for this event!" << endm;
    return kStWarn;
  }
  // Example: look at the position of the primary vertex
  //          Map is not needed for this, but it's a good check,
  //          tracking will not be good if primary vertex was not well placed.
  
  // First check whether the Primary Vertex is there at all.
  StThreeVectorD VertexPos(0,0,0);
  if (rEvent->primaryVertex()) {
    VertexPos = rEvent->primaryVertex()->position();
    cout << "Position of Primary Vertex from StEvent:" << endl;
    cout << VertexPos << endl;
  }
  else {
    cout << "----------- WARNING ------------" << endl;
    cout << "No primary vertex from StEvent!!" << endl;
    cout << "Assume it is at (0,0,0)         " << endl;
    
  }
  cout << "Position of Primary Vertex from StMcEvent:" << endl;
  cout << mEvent->primaryVertex()->position() << endl;
  
  // Example: look at hits associated with 1st REC hit in Tpc Hit collection.
  
  StTpcHit*     firstHit;
  Bool_t        gotOneHit;
  StTpcHitCollection* tpcColl = rEvent->tpcHitCollection();
  unsigned int j,k, nhits;
  nhits = tpcColl->numberOfHits();
  if (tpcColl && nhits) {
    gotOneHit = kFALSE;
    for (k=0; !gotOneHit && k<tpcColl->numberOfSectors(); k++)
      for (j=0; !gotOneHit && j<tpcColl->sector(k)->numberOfPadrows(); j++)
	if (tpcColl->sector(k)->padrow(j)->hits().size()) {
	  
	  firstHit = tpcColl->sector(k)->padrow(j)->hits()[0];
	  cout << "First Hit Found in Sector "
	       << firstHit->sector() << " Padrow "
	       << firstHit->padrow() << endl;
	  gotOneHit = kTRUE;
	}
    cout << "This hit has " <<  theHitMap->count(firstHit) << " MC Hits associated with it."<< endl;
    // To get the associated hits of the first hit we use equal_range(key), which returns
    // 2 iterators, the lower bound and upper bound, so that then we can loop over them.
    
    cout << "Position of First Rec. Hit and Associated (if any) MC Hit:" << endl;
    pair<rcTpcHitMapIter,rcTpcHitMapIter> hitBounds = theHitMap->equal_range(firstHit);    
    for (rcTpcHitMapIter it=hitBounds.first; it!=hitBounds.second; ++it) 
      cout << "[" << (*it).first->position() << ", " << (*it).second->position() << "]" << endl;
  }
  else {
    cout << "There are no reconstructed TPC Hits in this event" << endl;
  }
  
  if (theHitMap && mTpcHitNtuple) {// TPC
    StTpcHitCollection* recHits = rEvent->tpcHitCollection();
    StMcTpcHitCollection* mcHits = mEvent->tpcHitCollection();
    assert (recHits || mcHits);
    cout << "Making Hit Resolution Histogram..." << endl;
    // Loop over Rec Hits
    
    for (unsigned int iSector=0; iSector< recHits->numberOfSectors(); iSector++) {
      StTpcSectorHitCollection* sectorCollection = recHits->sector(iSector);
      if (! sectorCollection) continue;
      for (unsigned int iPadrow=0; iPadrow< sectorCollection->numberOfPadrows(); iPadrow++) {
	StTpcPadrowHitCollection *rowCollection = sectorCollection->padrow(iPadrow);
	if (! rowCollection) continue;
	StSPtrVecTpcHit &hits = rowCollection->hits();
	UInt_t NoHits = hits.size();
	if (! NoHits) continue;
	memset (&TpcHitMRPair, 0, sizeof(TpcHitMRPair_t));
	memset (&current, 0, sizeof(TpcHitPair_t));
	memset (&previous, 0, sizeof(TpcHitPair_t));
	// sort hits increasing in Z
	TArrayI idxT(NoHits); Int_t  *idx = idxT.GetArray();
	TArrayD zT(NoHits);   Double_t *z = zT.GetArray();
	Int_t n = 0;
	for (UInt_t k = 0; k < NoHits; k++) {
	  const StTpcHit *tpcHit = dynamic_cast<const StTpcHit   *>(hits[k]);
	  if (! tpcHit) continue;
	  z[n] = TMath::Abs(tpcHit->position().z());
	  n++;
	}
	TMath::Sort(n,z,idx,0);
	for (Int_t k = 0; k < n; k++) {
	  const StTpcHit   *rhit = dynamic_cast<const StTpcHit   *> (hits[idx[k]]);
	  if (rhit->TestBit(StMcHit::kMatched))    {
	    pair<rcTpcHitMapIter,rcTpcHitMapIter>
	      recBounds = theHitMap->equal_range(rhit);
	    for (rcTpcHitMapIter it2=recBounds.first; it2!=recBounds.second; ++it2){
	      const StMcTpcHit *mhit = dynamic_cast<const StMcTpcHit *> ((*it2).second);
	      assert ( mhit);
	      memset (&current, 0, sizeof(TpcHitPair_t));
	      TpcHitMRPair.sector   = rhit->sector();
	      TpcHitMRPair.row      = rhit->padrow();
	      TpcHitMRPair.isDet    = mhit->isDet();
	      current.xR       = rhit->position().x();
	      current.yR       = rhit->position().y();
	      current.zR       = rhit->position().z();
	      current.dER      = rhit->charge();
	      current.IdR      = rhit->idTruth();
	      current.qR       = rhit->qaTruth();
	      current.nR       = theHitMap->count(rhit);
	      current.xM       = mhit->position().x();
	      current.yM       = mhit->position().y();
	      current.zM       = mhit->position().z();
	      current.pxM      = mhit->localMomentum().x();
	      current.pyM      = mhit->localMomentum().y();
	      current.pzM      = mhit->localMomentum().z();
	      current.dEM      = mhit->dE();
	      current.dSM      = mhit->dS();
	      const StMcTrack *mTrack     = mhit->parentTrack();
	      if (mTrack) current.IdM = mTrack->key();
	      else        current.IdM = 0;
	      current.nM     = theMcHitMap->count(mhit);
	      TpcHitMRPair.current = current;
	      if (k > 0) TpcHitMRPair.previous = previous;
	      mTpcHitNtuple->Fill(&TpcHitMRPair.sector);
	    }
	  }
	  else {
	    memset (&current, 0, sizeof(TpcHitPair_t));
	    current.xR       = rhit->position().x();
	    current.yR       = rhit->position().y();
	    current.zR       = rhit->position().z();
	    current.dER      = rhit->charge();
	    current.IdR      = rhit->idTruth();
	    current.qR       = rhit->qaTruth();
	    current.nR       = theHitMap->count(rhit);
	    TpcHitMRPair.current = current;
	    if (k > 0) TpcHitMRPair.previous = previous;
	    mTpcHitNtuple->Fill(&TpcHitMRPair.sector);
	  }
	  previous = current;
	}
      }
    }
    for (unsigned int iSector=0;
	 iSector<mcHits->numberOfSectors(); iSector++) {
      
      if (Debug()) {cout << iSector + 1 << " "; flush(cout);}
      StMcTpcSectorHitCollection* tpcSectHitColl = mcHits->sector(iSector);
      for (unsigned int iPadrow=0;
	   iPadrow<tpcSectHitColl->numberOfPadrows();
	   iPadrow++) {
	StMcTpcPadrowHitCollection* tpcPadRowHitColl = tpcSectHitColl->padrow(iPadrow);
	for (unsigned int iHit=0;
	     iHit<tpcPadRowHitColl->hits().size();
	     iHit++){
	  const StMcTpcHit *mhit = dynamic_cast<const StMcTpcHit *> (tpcPadRowHitColl->hits()[iHit]);
	  assert (mhit);
	  if (mhit->TestBit(StMcHit::kMatched)) continue;
	  memset (&TpcHitMRPair, 0, sizeof(TpcHitMRPair));
	  TpcHitMRPair.sector   = mhit->sector();
	  TpcHitMRPair.row      = mhit->padrow();
	  TpcHitMRPair.isDet    = mhit->isDet();
	  current.xM       = mhit->position().x();
	  current.yM       = mhit->position().y();
	  current.zM       = mhit->position().z();
	  current.pxM      = mhit->localMomentum().x();
	  current.pyM      = mhit->localMomentum().y();
	  current.pzM      = mhit->localMomentum().z();
	  current.dEM      = mhit->dE();
	  current.dSM      = mhit->dS();
	  const StMcTrack *mTrack     = mhit->parentTrack();
	  if (mTrack) current.IdM = mTrack->key();
	  else        current.IdM = 0;
	  TpcHitMRPair.current = current;
	  mTpcHitNtuple->Fill(&TpcHitMRPair.sector);
	}
      }
    }
  }
  if (svtHitMap && svtMcHitMap && mSvtHitNtuple) {  // svt hits
    StSvtHitCollection* recHits = rEvent->svtHitCollection();
    StMcSvtHitCollection* mcHits = mEvent->svtHitCollection();
    if (recHits && mcHits) {
      for (unsigned int iBarrel=0; iBarrel< recHits->numberOfBarrels(); iBarrel++) {
	for (unsigned int iLadder=0; iLadder<recHits->barrel(iBarrel)->numberOfLadders(); iLadder++) {
	  for (unsigned int iWafer = 0; iWafer < recHits->barrel(iBarrel)->ladder(iLadder)->numberOfWafers(); iWafer++) {
	    for (StSvtHitIterator iter = recHits->barrel(iBarrel)->ladder(iLadder)->wafer(iWafer)->hits().begin();
	       iter != recHits->barrel(iBarrel)->ladder(iLadder)->wafer(iWafer)->hits().end();
		 iter++) {
	      const StSvtHit   *rhit = dynamic_cast<const StSvtHit   *> (*iter);
	      assert(rhit);
	      if (rhit->TestBit(StMcHit::kMatched)) 
	      {
		pair<rcSvtHitMapIter,rcSvtHitMapIter> recBounds = svtHitMap->equal_range(rhit);
		for (rcSvtHitMapIter it2=recBounds.first; it2!=recBounds.second; ++it2){
		  const StMcSvtHit *mhit = dynamic_cast<const StMcSvtHit *> ((*it2).second);
		  assert ( mhit);
		  memset (&SvtHitMRPair, 0, sizeof(SvtHitMRPair));
		  SvtHitMRPair.barrel   = rhit->barrel();
		  SvtHitMRPair.ladder   = rhit->ladder();
		  SvtHitMRPair.layer    = rhit->layer();
		  SvtHitMRPair.wafer    = rhit->wafer();
		  SvtHitMRPair.hybrid   = rhit->hybrid();
		  SvtHitMRPair.index    = rhit->index();
		  SvtHitMRPair.xR       = rhit->position().x();
		  SvtHitMRPair.yR       = rhit->position().y();
		  SvtHitMRPair.zR       = rhit->position().z();
		  SvtHitMRPair.dER      = rhit->charge();
		  SvtHitMRPair.IdR      = rhit->idTruth();
		  SvtHitMRPair.qR       = rhit->qaTruth();
		  SvtHitMRPair.nR       = svtHitMap->count(rhit);
		  SvtHitMRPair.xM       = mhit->position().x();
		  SvtHitMRPair.yM       = mhit->position().y();
		  SvtHitMRPair.zM       = mhit->position().z();
		  SvtHitMRPair.pxM      = mhit->localMomentum().x();
		  SvtHitMRPair.pyM      = mhit->localMomentum().y();
		  SvtHitMRPair.pzM      = mhit->localMomentum().z();
		  SvtHitMRPair.dEM      = mhit->dE();
		  SvtHitMRPair.dSM      = mhit->dS();
		  mTrack     = mhit->parentTrack();
		  if (mTrack) SvtHitMRPair.IdM = mTrack->key();
		  else        SvtHitMRPair.IdM = 0;
		  SvtHitMRPair.nM     = svtMcHitMap->count(mhit);
		  mSvtHitNtuple->Fill(&SvtHitMRPair.barrel);
		}
	      }
	      else {
		memset (&SvtHitMRPair, 0, sizeof(SvtHitMRPair));
		SvtHitMRPair.barrel   = rhit->barrel();
		SvtHitMRPair.ladder   = rhit->ladder();
		SvtHitMRPair.layer    = rhit->layer();
		SvtHitMRPair.wafer    = rhit->wafer();
		SvtHitMRPair.hybrid   = rhit->hybrid();
		SvtHitMRPair.index    = rhit->index();
		SvtHitMRPair.xR       = rhit->position().x();
		SvtHitMRPair.yR       = rhit->position().y();
		SvtHitMRPair.zR       = rhit->position().z();
		SvtHitMRPair.dER      = rhit->charge();
		SvtHitMRPair.IdR      = rhit->idTruth();
		SvtHitMRPair.qR       = rhit->qaTruth();
		SvtHitMRPair.nR       = svtHitMap->count(rhit);
		mSvtHitNtuple->Fill(&SvtHitMRPair.barrel);
	      }
	    }
	  }
	}
      }
      for (unsigned int iBarrel=0; iBarrel< mcHits->numberOfBarrels(); iBarrel++) {
	for (unsigned int iLadder=0; iLadder<mcHits->barrel(iBarrel)->numberOfLadders(); iLadder++) {
	  for (unsigned int iWafer = 0; iWafer < mcHits->barrel(iBarrel)->ladder(iLadder)->numberOfWafers(); iWafer++) {
	    for (StMcSvtHitIterator iter = mcHits->barrel(iBarrel)->ladder(iLadder)->wafer(iWafer)->hits().begin();
	       iter != mcHits->barrel(iBarrel)->ladder(iLadder)->wafer(iWafer)->hits().end();
		 iter++) {
	      const StMcSvtHit   *mhit = dynamic_cast<const StMcSvtHit   *> (*iter);
	      assert (mhit);
	      if (mhit->TestBit(StMcHit::kMatched)) continue;
	      memset (&SvtHitMRPair, 0, sizeof(SvtHitMRPair));
	      SvtHitMRPair.barrel   = mhit->barrel();
	      SvtHitMRPair.ladder   = mhit->ladder();
	      SvtHitMRPair.layer    = mhit->layer();
	      SvtHitMRPair.wafer    = mhit->wafer();
	      SvtHitMRPair.hybrid   = mhit->hybrid();
	      SvtHitMRPair.index    = -1;
	      SvtHitMRPair.barrel   = mhit->barrel();
	      SvtHitMRPair.ladder   = mhit->ladder();
	      SvtHitMRPair.xM       = mhit->position().x();
	      SvtHitMRPair.yM       = mhit->position().y();
	      SvtHitMRPair.zM       = mhit->position().z();
	      SvtHitMRPair.pxM      = mhit->localMomentum().x();
	      SvtHitMRPair.pyM      = mhit->localMomentum().y();
	      SvtHitMRPair.pzM      = mhit->localMomentum().z();
	      SvtHitMRPair.dEM      = mhit->dE();
	      SvtHitMRPair.dSM      = mhit->dS();
	      mTrack     = mhit->parentTrack();
	      if (mTrack) SvtHitMRPair.IdM = mTrack->key();
	      else        SvtHitMRPair.IdM = 0;
	      mSvtHitNtuple->Fill(&SvtHitMRPair.barrel);
	    }
	  }
	}
      }
    }
  }
  if (ssdHitMap && ssdMcHitMap && mSsdHitNtuple) {  // ssd hits
    StSsdHitCollection* recHits = rEvent->ssdHitCollection();
    StMcSsdHitCollection* mcHits = mEvent->ssdHitCollection();
    if (recHits && mcHits) {
      for (unsigned int iLadder=0; iLadder<recHits->numberOfLadders(); iLadder++) {
	for (unsigned int iWafer = 0; iWafer < recHits->ladder(iLadder)->numberOfWafers(); iWafer++) {
	  for (StSsdHitIterator iter = recHits->ladder(iLadder)->wafer(iWafer)->hits().begin();
	       iter != recHits->ladder(iLadder)->wafer(iWafer)->hits().end();
	       iter++) {
	    const StSsdHit   *rhit = dynamic_cast<const StSsdHit   *> (*iter);
	    assert(rhit);
	    if (rhit->TestBit(StMcHit::kMatched)) 
	      {
		pair<rcSsdHitMapIter,rcSsdHitMapIter> recBounds = ssdHitMap->equal_range(rhit);
		for (rcSsdHitMapIter it2=recBounds.first; it2!=recBounds.second; ++it2){
		  const StMcSsdHit *mhit = dynamic_cast<const StMcSsdHit *> ((*it2).second);
		  assert ( mhit);
		  memset (&SsdHitMRPair, 0, sizeof(SsdHitMRPair));
		  SsdHitMRPair.ladder   = rhit->ladder();
		  SsdHitMRPair.wafer    = rhit->wafer();
		  SsdHitMRPair.xR       = rhit->position().x();
		  SsdHitMRPair.yR       = rhit->position().y();
		  SsdHitMRPair.zR       = rhit->position().z();
		  SsdHitMRPair.dER      = rhit->charge();
		  SsdHitMRPair.IdR      = rhit->idTruth();
		  SsdHitMRPair.qR       = rhit->qaTruth();
		  SsdHitMRPair.nR       = ssdHitMap->count(rhit);
		  SsdHitMRPair.xM       = mhit->position().x();
		  SsdHitMRPair.yM       = mhit->position().y();
		  SsdHitMRPair.zM       = mhit->position().z();
		  SsdHitMRPair.pxM      = mhit->localMomentum().x();
		  SsdHitMRPair.pyM      = mhit->localMomentum().y();
		  SsdHitMRPair.pzM      = mhit->localMomentum().z();
		  SsdHitMRPair.dEM      = mhit->dE();
		  SsdHitMRPair.dSM      = mhit->dS();
		  mTrack     = mhit->parentTrack();
		  if (mTrack) SsdHitMRPair.IdM = mTrack->key();
		  else        SsdHitMRPair.IdM = 0;
		  SsdHitMRPair.nM     = ssdMcHitMap->count(mhit);
		  mSsdHitNtuple->Fill(&SsdHitMRPair.ladder);
		}
	      }
	    else {
	      memset (&SsdHitMRPair, 0, sizeof(SsdHitMRPair));
	      SsdHitMRPair.ladder   = rhit->ladder();
	      SsdHitMRPair.wafer    = rhit->wafer();
	      SsdHitMRPair.xR       = rhit->position().x();
	      SsdHitMRPair.yR       = rhit->position().y();
	      SsdHitMRPair.zR       = rhit->position().z();
	      SsdHitMRPair.dER      = rhit->charge();
	      SsdHitMRPair.IdR      = rhit->idTruth();
	      SsdHitMRPair.qR       = rhit->qaTruth();
	      SsdHitMRPair.nR       = ssdHitMap->count(rhit);
	      mSsdHitNtuple->Fill(&SsdHitMRPair.ladder);
	    }
	  }
	}
      }
      for (unsigned int iLadder=0; iLadder<mcHits->numberOfLadders(); iLadder++) {
	for (unsigned int iWafer = 0; iWafer < mcHits->ladder(iLadder)->numberOfWafers(); iWafer++) {
	  for (StMcSsdHitIterator iter = mcHits->ladder(iLadder)->wafer(iWafer)->hits().begin();
	       iter != mcHits->ladder(iLadder)->wafer(iWafer)->hits().end();
	       iter++) {
	    const StMcSsdHit   *mhit = dynamic_cast<const StMcSsdHit   *> (*iter);
	    assert (mhit);
	    if (mhit->TestBit(StMcHit::kMatched)) continue;
	    memset (&SsdHitMRPair, 0, sizeof(SsdHitMRPair));
	    SsdHitMRPair.ladder   = mhit->ladder();
	    SsdHitMRPair.wafer    = mhit->wafer();
	    SsdHitMRPair.ladder   = mhit->ladder();
	    SsdHitMRPair.xM       = mhit->position().x();
	    SsdHitMRPair.yM       = mhit->position().y();
	    SsdHitMRPair.zM       = mhit->position().z();
	    SsdHitMRPair.pxM      = mhit->localMomentum().x();
	    SsdHitMRPair.pyM      = mhit->localMomentum().y();
	    SsdHitMRPair.pzM      = mhit->localMomentum().z();
	    SsdHitMRPair.dEM      = mhit->dE();
	    SsdHitMRPair.dSM      = mhit->dS();
	    mTrack     = mhit->parentTrack();
	    if (mTrack) SsdHitMRPair.IdM = mTrack->key();
	    else        SsdHitMRPair.IdM = 0;
	    mSsdHitNtuple->Fill(&SsdHitMRPair.ladder);
	  }
	}
      }
    }
  }
  cout << "Finished Making Histogram." << endl;
  
  if (!theTrackMap) {
    gMessMgr->Warning() << "----------WARNING----------\n"
			<< "No Track Map found for this event!" << endm;
    return kStWarn;
  }
  // Example: look at the magnitude of the momentum of
  //          the MC track associated with first track in track Collection
  
  StSPtrVecTrackNode& rcTrackNodes = rEvent->trackNodes();
  StTrackNode*        firstTrackNode = 0;
  const StGlobalTrack*      firstTrack = 0;
  if (! rcTrackNodes.size()) {
    cout << "Track Nodes List is empty!" << endl;
    return kStWarn;
  }
  firstTrackNode = *(rcTrackNodes.begin());
  if (! firstTrackNode) {
    cout << "No tracks in Track Nodes List!" << endl;
    return kStWarn;
  }
  firstTrack = dynamic_cast<const StGlobalTrack*>(firstTrackNode->track(global));
  if (! firstTrack) {
    cout << "GlobalTrack for first Track Nodehas not been found!" << endl;
    return kStWarn;
  }
  cout << "MC Tracks associated with first Track in collection: " << theTrackMap->count(firstTrack) << endl;
  if (!theTrackMap->count(firstTrack) && theTrackMap->size()) {
      cout << "First track in track node container was not associated.  Pick first track in map." << endl;
      firstTrack = theTrackMap->begin()->first; //first entry in the map is a pair, and first entry of pair is the global track
  }
  pair<rcTrackMapIter,rcTrackMapIter> trackBounds = theTrackMap->equal_range(firstTrack);
  cout << "Momentum of First Track and of Associated Tracks (if there are any):" << endl;
  // Get the momentum of the track and compare it to MC Track.
  // Use primary track if available
  const StPrimaryTrack* pTrk = dynamic_cast<const StPrimaryTrack*>(firstTrack->node()->track(primary));
  StThreeVectorD recMom(0,0,0);
  if (pTrk)
    recMom = pTrk->geometry()->momentum();
  else
    recMom = firstTrack->geometry()->momentum();
  for (rcTrackMapIter trkIt=trackBounds.first; trkIt!=trackBounds.second; ++trkIt) { 
    const StMcTrack*   origTrk = (*trkIt).second->partnerMcTrack();
    cout << "[" << abs(recMom) << ", ";
    cout << abs((*trkIt).second->partnerMcTrack()->momentum()) << "]" << endl;
    cout << "These tracks have : \n";
    cout << (*trkIt).second->commonTpcHits() << " TPC  hits in common, out of " << origTrk->tpcHits().size() << endl;
    cout << (*trkIt).second->commonSvtHits() << " SVT  hits in common, out of " << origTrk->svtHits().size() << endl;
    cout << (*trkIt).second->commonFtpcHits() <<" FTPC hits in common, out of " << origTrk->ftpcHits().size() << endl;
  }
  // Example: Make a histogram of the momentum resolution of the event
  //          Make an Ntuple with rec & monte carlo mom, mean hit difference, and # of common hits
  const StGlobalTrack* recTrack;
  const StPrimaryTrack* primTrk;
  const StMcTrack*     mcTrack;
  StThreeVectorD p(0,0,0);
  StThreeVectorD pmc(0,0,0);
  
  float* values = new float[19];
  
  for (rcTrackMapIter tIter=theTrackMap->begin();
       tIter!=theTrackMap->end(); ++tIter){
    
    recTrack = (*tIter).first;
    //yf    if ((*tIter).second->commonTpcHits()<10) continue;
    const StMcTrack *mcTrack = (*tIter).second->partnerMcTrack();
    pmc = mcTrack->momentum();
    for (int k=0; k<3; k++) values[k] = pmc[k];  	 
    values[3]=pmc.mag(); 	 
    
    primTrk = dynamic_cast<const StPrimaryTrack*>(recTrack->node()->track(primary)); 	 
    if (primTrk) 	 
      p = primTrk->geometry()->momentum(); 	 
    else 	 
      p = recTrack->geometry()->momentum(); 	 
    
    for (int j=0; j<3; j++) values[j+4] = p[j]; 	 
    values[7]=p.mag(); 	 
    values[8]=(*tIter).second->commonTpcHits();
    // Loop to get Mean hit position diff.
    StThreeVectorF rHitPos(0,0,0);
    StThreeVectorF mHitPos(0,0,0);
    
    StPtrVecHit recTpcHits = recTrack->detectorInfo()->hits(kTpcId);
    multimap<int,float> idTruths;
    set<int> uniqueIdTruths;
    for (StHitIterator hi=recTpcHits.begin();
	 hi!=recTpcHits.end(); hi++) {
      StHit* hit = *hi;
      StTpcHit* rHit = dynamic_cast<StTpcHit*>(hit);
      if (!rHit) { cout << "This Hit is not a TPC Hit"<< endl; continue;}
      idTruths.insert( multimap<int,float>::value_type(rHit->idTruth(),rHit->qaTruth()));
      uniqueIdTruths.insert(static_cast<int>(rHit->idTruth()));
      pair<rcTpcHitMapIter,rcTpcHitMapIter> rBounds = theHitMap->equal_range(rHit);
      for (rcTpcHitMapIter hIter=rBounds.first; hIter!=rBounds.second; hIter++) {
	const StMcTpcHit* mHit = (*hIter).second;
	if (mHit->parentTrack() != mcTrack) continue;
	rHitPos += rHit->position();
	mHitPos += mHit->position();
      }// Associated Hits Loop.
      
    } // Hits of rec. Track Loop
    if ( (*tIter).second->commonTpcHits() > 0) 
      rHitPos /=(float) (*tIter).second->commonTpcHits();
    if ( (*tIter).second->commonTpcHits() > 0) 
      mHitPos /=(float) (*tIter).second->commonTpcHits();
    for (int jj=0; jj<3; jj++) values[9+jj] = rHitPos[jj] - mHitPos[jj];
    values[12] = mcTrack->key();
    // Figure out the most common IdTruth; the dominatrix track!
    int mostCommonIdTruth = -9; 
    int cachedNHitsIdTruth = 0;
    for (set<int>::iterator si=uniqueIdTruths.begin(); si!=uniqueIdTruths.end(); ++si) {
      int currentNHitsIdTruth = idTruths.count(static_cast<int>(*si));
      if (currentNHitsIdTruth>cachedNHitsIdTruth) {
	mostCommonIdTruth = *si; 
	cachedNHitsIdTruth = currentNHitsIdTruth;
      }
    }
    // at this point we know the most common IdTruth,
    // now calculate the track "quality" for this track, averaging
    // the hit qualities
    float idQuality = 0;
    
    pair<multimap<int,float>::iterator,multimap<int,float>::iterator> mostCommRange = idTruths.equal_range(mostCommonIdTruth);
    for (multimap<int,float>::iterator mi=mostCommRange.first; mi!=mostCommRange.second; ++mi) {
      idQuality+=mi->second;
    }
    if (cachedNHitsIdTruth)    idQuality/=cachedNHitsIdTruth;
    
    values[13] = mostCommonIdTruth;
    values[14] = cachedNHitsIdTruth;
    values[15] = mcTrack->tpcHits().size();
    values[16] = recTrack->fitTraits().numberOfFitPoints(kTpcId);
    values[17] = recTpcHits.size();
    values[18] = idQuality;
    mTrackNtuple->Fill(values);
  } // Tracks in Map Loop
  cout << "Finished Track Loop, Made Ntuple" << endl;
  //delete vars;
  delete [] values;
  return kStOK;
}

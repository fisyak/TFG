/* an example to calculate feed up to primary vertex track from secondaries (for Shikshit Gupta)
   By default it is supposed that you analyze ./MuDst/*.MuDst.root set. 
   root.exe lMuDst.C MuS.C+
*/
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <assert.h>
#include <map>
#include <utility>
#include "Riostream.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TProfile.h"
#include "TProfile3D.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TClassTable.h"
#include "TFile.h"
#include "TChain.h"
#include "TString.h"
#include "SystemOfUnits.h"
#include "StMuDSTMaker/COMMON/StMuTimer.h"
#include "StMuDSTMaker/COMMON/StMuDebug.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuMcVertex.h"
#include "StMuDSTMaker/COMMON/StMuMcTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryTrackCovariance.h"
#include "StarRoot/TPolynomial.h"
#include "StDcaGeometry.h"
#include "TRSymMatrix.h"
#include "THelixTrack.h"
#include "Names.h"
#include "StBichsel/Bichsel.h"
#define ClassStMessMgr
#define StMessMgr Int_t
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#undef  StMessMgr
#undef ClassStMessMgr
#else
#ifndef __MAKECINT__
#define BIT(n)       (1 << (n))
#define SETBIT(n,i)  ((n) |= (1 << i))
#define CLRBIT(n,i)  ((n) &= ~(1 << i))
#define TESTBIT(n,i) ((Bool_t)(((n) & (1 << i)) != 0))
class StMuDstMaker;
#endif
#endif
StMuDstMaker* maker = 0;
enum TrackMatchType {kPositive, kNegative, kTotalSigns};                                     // switch between charges
struct PlotName_t {
  TrackMatchType    k;
  const Char_t *Name;
  const Char_t *Title;
};
struct VarName_t {
  const Char_t *Name;
  const Char_t *Title;
  Int_t nx;
  Double_t xmin, xmax;
  Int_t ny;
  Double_t ymin, ymax;
  Int_t nz;
  Double_t zmin, zmax;
  Double_t  min,  max; // min and max for plots
};
const Char_t *NameCharge[kTotalSigns] = {"Pos", "Neg"};
const Char_t *TitleCharge[kTotalSigns] = {"(+)", "(-)"};
//________________________________________________________________________________
static Int_t _debug = 0;
void SetDebug(Int_t k) {_debug = k;}
Int_t Debug() {return _debug;}
//________________________________________________________________________________
Bool_t Accept(const StMuTrack *gTrack = 0) {
  if (! gTrack)            return kFALSE;
  //  if (! gTrack->idTruth()) return kFALSE;
  if (! gTrack->charge())  return kFALSE;
  if (  gTrack->flag() < 100 ||  gTrack->flag()%100 == 11) return kFALSE; // bad fit or short track pointing to EEMC
  if (  gTrack->flag() > 1000) return kFALSE;  // pile up track in TPC
  if (  gTrack->nHitsFit() < 10) return kFALSE;
  //  if (  gTrack->qaTruth() < 90) return kFALSE;
  return kTRUE;
}
//________________________________________________________________________________
Bool_t AcceptVX(const StMuPrimaryVertex *Vtx = 0) {
  if (! Vtx) return kFALSE;
  if (! Vtx->idTruth())  return kFALSE;
  //  if (  Vtx->qaTruth() < 90) return kFALSE;
  return kTRUE;
}
//________________________________________________________________________________
void MuS(Long64_t nevent = 9999999,
	 const char* file="./MuDst/*.MuDst.root",
	 //	 const char* file="/star/rcf/test/dev/trs_sl302.ittf/Wed/year_2011/pp500_pileup/rcf10100_90_200evts_Wplus_enu.MuDst.root",
	 //	 const char* file="MuDstSel.lis",
	 const char* filter="st:MuDst.root",
	 const  char* outFile="Mu.root") {
  TFile *fOut = new TFile(outFile,"recreate");
  TH1D *hpT[kTotalSigns];
  for (Int_t s = kPositive; s < kTotalSigns; s++) {
    hpT[s] = new TH1D(Form("pT%s",NameCharge[s]),Form("pT for %s",TitleCharge[s]),200,0,200);
  }

  StMuDebug::setLevel(0);  
  maker = new StMuDstMaker(0,0,"",file,filter,1e9);   // set up maker in read mode
  //                       0,0                        this mean read mode
  //                           dir                    read all files in this directory
  //                               file               bla.lis real all file in this list, if (file!="") dir is ignored
  //                                    filter        apply filter to filenames, multiple filters are separated by ':'
  //                                          10      maximum number of file to read
  maker->SetStatus("*",0);
  const Char_t *ActiveBranches[] = {"MuEvent"
				    ,"PrimaryVertices"
				    ,"PrimaryTracks"
#if 0
				    ,"GlobalTracks"
				    ,"CovPrimTrack"
				    ,"CovGlobTrack"
#endif
				    ,"StStMuMcVertex"
				    ,"StStMuMcTrack"
  };
  Int_t Nb = sizeof(ActiveBranches)/sizeof(Char_t *);
  for (Int_t i = 0; i < Nb; i++) maker->SetStatus(ActiveBranches[i],1); // Set Active braches
  StMuDebug::setLevel(0);  
  TChain *tree = maker->chain();
  if (! tree) return;
  Long64_t nentries = tree->GetEntries();
  nevent = TMath::Min(nevent,nentries);
  cout << nentries << " events in chain " << nevent << " will be read." << endl;
  if (nentries < 100) return;
  tree->SetCacheSize(-1);        //by setting the read cache to -1 we set it to the AutoFlush value when writing
  tree->SetCacheLearnEntries(1); //one entry is sufficient to learn
  tree->SetCacheEntryRange(0,nevent);

  for (Long64_t ev = 0; ev < nevent; ev++) {
    if (maker->Make()) break;
    StMuDst* mu = maker->muDst();   // get a pointer to the StMuDst class, the class that points to all the data
    if (ev%1000 == 0) cout << "Read event\t" << ev << endl;
    StMuEvent* muEvent = mu->event(); // get a pointer to the class holding event-wise information
    // cout << " #" << ev;
    //    Int_t referenceMultiplicity = muEvent->refMult(); // get the reference multiplicity
    // cout << " refMult= "<< referenceMultiplicity;
    //    const Double_t field = muEvent->magneticField()*kilogauss;
    TClonesArray *PrimaryVertices   = mu->primaryVertices(); 
    Int_t NoPrimaryVertices = PrimaryVertices->GetEntriesFast();  // cout << "\tPrimaryVertices " << NoPrimaryVertices;
    TClonesArray *PrimaryTracks    = mu->array(muPrimary);  
    Int_t NoPrimaryTracks = PrimaryTracks->GetEntriesFast();  // cout << "\tPrimaryTracks " << NoPrimaryTracks;
    TClonesArray *MuMcVertices   = mu->mcArray(0); 
    Int_t NoMuMcVertices = MuMcVertices->GetEntriesFast();        if (Debug()) {cout << "\t" << StMuArrays::mcArrayTypes[0] << " " << NoMuMcVertices;}
    TClonesArray *MuMcTracks     = mu->mcArray(1); 
    Int_t NoMuMcTracks = MuMcTracks->GetEntriesFast();            if (Debug()) {cout << "\t" << StMuArrays::mcArrayTypes[1] << " " << NoMuMcTracks;}
    if (Debug())                                                               {cout << endl;}
    if (! NoMuMcVertices || ! NoMuMcTracks) {
      cout << "Ev. " << ev << " has no MC information ==> skip it" << endl;
      continue;
    }
    for (Int_t l = 0; l < NoPrimaryVertices; l++) {
      StMuPrimaryVertex *Vtx = (StMuPrimaryVertex *) PrimaryVertices->UncheckedAt(l);
      if (l) continue; // take only the first one
      if (! Vtx ) continue;
      // Check Mc
      if (Vtx->idTruth() < 0 || Vtx->idTruth() > NoMuMcVertices) {
	cout << "Illegal idTruth " << Vtx->idTruth() << " The track is ignored" << endl;
	Vtx->Print();
	continue;
      }
      StMuMcVertex *mcVertex = (StMuMcVertex *) MuMcVertices->UncheckedAt(Vtx->idTruth()-1);
      if (! mcVertex) continue;
      if (mcVertex->Id() != Vtx->idTruth()) {
	cout << "Mismatched idTruth " << Vtx->idTruth() << " and mcVertex Id " <<  mcVertex->Id() 
	     << " The vertex is ignored" <<  endl;
	mcVertex->Print();
	continue;
      }
      if (mcVertex->Id() != 1) {
	cout << "Best RC Vertex is not MC primary vertex" << endl;
	mcVertex->Print();
	continue;
      }
      for (Int_t k = 0; k < NoPrimaryTracks; k++) {
	StMuTrack *pTrack = (StMuTrack *) PrimaryTracks->UncheckedAt(k);
	if (! pTrack) continue;
        if (pTrack->vertexIndex() != l) continue;
	if (! Accept(pTrack)) continue;
	TrackMatchType s = kPositive;
	if (pTrack->charge() < 0) s = kNegative;
	hpT[s]->Fill(pTrack->pt());
	// Check MC
	if (pTrack->idTruth() <= 0 || pTrack->idTruth() > NoMuMcTracks) {
	  cout << "Illegal idTruth " << pTrack->idTruth() << " The track is ignored" << endl;
	  pTrack->Print();
	  continue;
	}
	StMuMcTrack *mcTrack = (StMuMcTrack *) MuMcTracks->UncheckedAt(pTrack->idTruth()-1);
	if (! mcTrack || mcTrack->Id() != pTrack->idTruth()) {
	  cout << "Mismatched idTruth " << pTrack->idTruth() << " and mcTrack Id " <<  mcTrack->Id() 
	       << " The track is ignored" <<  endl;
	  if (mcTrack) mcTrack->Print();
	}
	Int_t IdVx = mcTrack->IdVx();
	if (IdVx != 1) {
	  cout << "Mc Track is coming from non primary vertex" << endl;
	  cout << "pTrack:"; pTrack->Print();
	  cout << "Mc Tk:"; mcTrack->Print("");
	  cout << "mcVertex:"; mcVertex->Print();
	  mcTrack->Print();
	  StMuMcVertex *secMcVx = (StMuMcVertex *) MuMcVertices->UncheckedAt(IdVx-1);
	  if (secMcVx) {
	    cout << "secMcVX:"; secMcVx->Print();
	    Int_t IdParTrack = secMcVx->IdParTrk();
	    if (IdParTrack > 0 && IdParTrack <= NoMuMcTracks) {
	      StMuMcTrack *mcTrackP = (StMuMcTrack *) MuMcTracks->UncheckedAt(IdParTrack-1);
	      if (mcTrackP) {
		cout << "its parent track"; mcTrackP->Print("");
	      }
	    }
	  }
	}
      }
    }
  }
  if (fOut) fOut->Write();
}




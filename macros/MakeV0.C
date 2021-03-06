#ifndef __CINT__
#include "Riostream.h"
#include "Rtypes.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TProfile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TClassTable.h"
#include "TFileSet.h"
#include "TDataSetIter.h"
#include "StBFChain.h"
#include "StIOMaker.h"
void bfc (const Int_t Last, 
	  const Char_t *Chain,
	  const Char_t *infile, 
	  const Char_t *outfile, 
	  const Char_t *TreeFile);
#else
class StBFChain;
#endif
//________________________________________________________________________________
void MakeV0(Int_t nevents=1, const Char_t *parn = "StK0s", Int_t RunID = 1)
{
  if (gClassTable->GetID("TTable") < 0) {
    gSystem->Load("libTable");
  }
  gROOT->LoadMacro("bfc.C");
  Int_t iD = 0;
  TString ParName(parn);
  Int_t Npart = 10;
  if      (ParName == "K0s"  )      iD = 16;
  else if (ParName == "StK0s"  )   {iD = 16; Npart =   1;}
  else if (ParName == "K0l"  )      iD = 10;
  else if (ParName == "gamma")     {iD =  1; Npart = 100;}
  else if (ParName == "Lambda")     iD = 18;
  else if (ParName == "AntiLambda") iD = 26;
  if (! iD) {
    cout << "nonrecognized particle name " << parn << endl;
    return;
  }
  TString RootFile = ParName;
  RootFile += "_"; RootFile += RunID; 
  TString MainFile = RootFile;
  MainFile += ".root";
  RootFile += ".MuDst.root";
  //   TString Chain("AgML,TpcRS,bbcSim,btofsim,pxlFastSim,istSim,emcY2,eefs,TpxClu,y2014a,useXgeom,FieldOn,NoSsdIt"
  // 		",NoSvtIt,StiHftC,Idst,BAna,tpcDB,TpcHitMover,btofMatch,btofCalib,tags,emcY2,IdTruth,gstar"
  // 		",Corr4,OSpaceZ2,OGridLeakFull"
  // 		",StiCA,beamline,KFVertex,StiHftC,pxlFastSim,ssdfast,useXgeom,VFMCE,noRunco,noHistos,noTags");
  //		",muMc"); 
  TString Chain("MC.2016a,StiCA,-hitfilt,KFVertex,StiHftC,geantOut,VFMCE,noRunco,noHistos,noTags,");
  if ( gClassTable->GetID("TGiant3") >= 0) { // root4star
    Chain += ",gstarLib,Corr4,OSpaceZ2,OGridLeakFull,useXgeom";
  } else {
    Chain += "vmc,CorrX,OSpaceZ2,OGridLeakFull,-useXgeom"; //VMCAlignment,sdt20160301,
  }
  Chain += Form(",rung.%i",RunID);
  chain = bfc(0,Chain.Data(),0,RootFile.Data(),MainFile.Data());
  Double_t pTlow = 0.2;
  Double_t pThigh = 20;
  Double_t Ylow = -1;
  Double_t Yhigh = 1;
  Double_t Philow = 0;
  Double_t Phihigh = 2*TMath::Pi();
  Double_t Zlow = -20;
  Double_t Zhigh = 20;
  if (gClassTable->GetID("St_geant_Maker") >= 0) {
    TString  kine(Form("gkine %i %i %f %f %f %f %f %f %f %f",Npart,iD, pTlow,pThigh,Ylow, Yhigh, Philow, Phihigh, Zlow, Zhigh));
    St_geant_Maker::instance()->Do(kine.Data());
  } else {
    if (StarVMCApplication::Instance()) {
      StarMCSimplePrimaryGenerator *gener = (StarMCSimplePrimaryGenerator *) StarVMCApplication::Instance()->GetPrimaryGenerator();
      if ( gener && ! gener->IsA()->InheritsFrom( "StarMCSimplePrimaryGenerator" ) ) {
	delete gener; gener = 0;
      }
      if (! gener) gener =  new 
	StarMCSimplePrimaryGenerator( Npart, iD, pTlow,pThigh,Ylow, Yhigh, Philow, Phihigh, Zlow, Zhigh, "G");
      else
	gener->SetGenerator( Npart, iD, pTlow,pThigh,Ylow, Yhigh, Philow, Phihigh, Zlow, Zhigh, "G");
      StarVMCApplication::Instance()->SetPrimaryGenerator(gener);
      cout << "Set StarMCSimplePrimaryGenerator" << endl;
    }
  }
  chain->EventLoop(nevents);
}

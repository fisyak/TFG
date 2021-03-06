#if !defined(__CINT__) && !defined(__CLING__)
#include "Riostream.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TProfile.h"
#include "TString.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TClassTable.h"
#include "TFileSet.h"
#include "TDataSetIter.h"
#include "StBFChain.h"
void bfc (const Int_t Last, 
	  const Char_t *Chain,
	  const Char_t *infile, 
	  const Char_t *outfile, 
	  const Char_t *TreeFile);
//R__EXTERN StBFChain *chain;
#else
#define SETBIT(n,i)  ((n) |=  (1 << i))
#define CLRBIT(n,i)  ((n) &= ~(1 << i))
class StBFChain;
StBFChain *chain;
class St_db_Maker;
St_db_Maker *dbMk = 0;
#endif
Bool_t Root4Star = kFALSE;
//________________________________________________________________________________
void SetPartGan(TString RootFile,TString RunOpt, TString Opt) {
  cout << "Call SetPartGen with RootFile = " << RootFile.Data() << "\tRunOpt = " << RunOpt.Data() << "\t Opt = " << Opt.Data() << endl;
  Int_t Ids[15] =            {      5,      6,           3,          2,       8,       9,      11,      12,       14,     15,         45,       46,    49,      47,         9};
  Double_t Masses[15] = {    0.1056584,0.1056584,0.51099907e-3,0.51099907e-3,0.13956995,0.13956995,0.493677,0.493677,0.93827231,0.93827231,1.875613,2.80925,2.80923,3.727417,0.13956995  };
  const Char_t  *Names[15] = {"muon+", "muon-", "electron", "positron", "pion+", "pion-", "kaon+", "kaon-", "proton", "pbar", "deuteron", "triton", "He3", "alpha", "pionMIP"};
  Int_t    NTRACK = 100;
  Int_t    ID = 5;
  Double_t Ylow   =  -1; 
  Double_t Yhigh  =   1;
  Double_t Philow =   0;
  Double_t Phihigh= 2*TMath::Pi();
  Double_t Zlow   =  -50; 
  Double_t Zhigh  =   50; 
  Double_t bgMinL10  = -2; // 3.5;// 1e2; // 1e-2;
  Double_t bgMaxL10  =  6;  // 1e2;// 1e5;
  Double_t mass   = -1;
  for (Int_t i = 0; i < 15; i++) {
    if (RootFile.Contains(Names[i],TString::kIgnoreCase)) {
      ID =Ids[i];
      if (i == 14) {bgMinL10 = 0.544; bgMaxL10 = 0.653;}
      mass = Masses[i];;
      break;
    }
  }
  if (mass < 0) {
    cout << Opt.Data() << " is not identified. Abort" << endl;
    return;
  }
  Double_t pTmin = mass*TMath::Power(10.,bgMinL10);
  Double_t pTmax = mass*TMath::Power(10.,bgMaxL10);
  if (pTmax > 10) {
    pTmax = 10;
    bgMaxL10 = TMath::Log10(pTmax/mass);
    //    cout << "Reduce bgMax10 = " << bgMaxL10 << endl;
  }
  if (Root4Star) {
    cout << "Initialize Root4star starsim " << endl;
    St_geant_Maker *geant = (St_geant_Maker *) chain->GetMakerInheritsFrom("St_geant_Maker");
    //                   NTRACK  ID PTLOW PTHIGH YLOW YHIGH PHILOW PHIHIGH ZLOW ZHIGH
    //    geant->Do("gkine 100  14   0.1    10.  -1     1      0    6.28    0.    0.;");
    cout << "Options: " << Opt.Data() << "=========================================================" << endl;
    if (FileIn == "") {
      if (kuip) {
	TString Kuip(kuip);
	geant->Do(kuip);
      } else if ( RunOpt.Contains("Mickey",TString::kIgnoreCase)) {
      } else if ( Opt.Contains("laser",TString::kIgnoreCase)) {
	gSystem->Load("gstar.so");
	geant->Do("call gstar");
	geant->Do("gkine 1 170   1   1  0   0   0  0    180.00    180.00;");
	geant->Do("gprint kine");
	geant->Do("gvert 0  54   0");
	geant->Do("mode TRAC prin 15");
      } else if (TString(geant->SAttr("GeneratorFile")) == "") {
	if (RunOpt.Contains("gstarLib",TString::kIgnoreCase)) {geant->Do("call gstar");}
	if (pTmin < 0) pTmin = mass*bgMin; if (pTmin <    0.01) pTmin =    0.01;
	if (pTmax < 0) pTmax = mass*bgMax; if (pTmax >   10.00) pTmax =   10.00;
	//	TString Kine(Form("gkine %i %i %f %f -2  2 0 %f -50 50;",NTRACK,ID,pTmin,pTmax,TMath::TwoPi()));
	TString Kine(Form("gkine %i %i %f %f -3  3 0 %f -200 200;",NTRACK,ID,pTmin,pTmax,TMath::TwoPi()));
	cout << "Set kinematics: " << Kine.Data() << endl;
	geant->Do(Kine.Data());
      }
    }
  } else { // VMC
    cout << "Initialize STAR VMC interface" << endl;
    if (! StVMCMaker::instance()) return 0;
    if (! StarVMCApplication::Instance()) return 0;
    StarMCSimplePrimaryGenerator *gener = (StarMCSimplePrimaryGenerator *) StarMCPrimaryGenerator::Instance();
    if ( gener && ! gener->IsA()->InheritsFrom( "StarMCSimplePrimaryGenerator" ) ) {
      delete gener; gener = 0;
    }
    if (! gener) gener =  new 
      StarMCSimplePrimaryGenerator( NTRACK, ID, bgMinL10, bgMaxL10,Ylow, Yhigh, Philow, Phihigh, Zlow, Zhigh, "GBL");
    else
      gener->SetGenerator( NTRACK, ID, bgMinL10, bgMaxL10,Ylow, Yhigh, Philow, Phihigh, Zlow, Zhigh, "GBL");
    StarVMCApplication::Instance()->SetPrimaryGenerator(gener);
    cout << "Set StarMCSimplePrimaryGenerator" << endl;
  }
}
//________________________________________________________________________________
void TpcRS(Int_t First, Int_t Last, const Char_t *Run = "y2011,TpcRS",  
	   const Char_t *fileIn = 0, const Char_t *opt = "Bichsel", const Char_t *kuip = 0,
	   const Char_t *fileOut = 0) {
  if (gClassTable->GetID("TGiant3") >= 0) Root4Star = kTRUE;
  gROOT->LoadMacro("bfc.C"); 
  TString ChainOpt("");
  TString RootFile(fileOut);
  TString Opt(opt);
  TString RunOpt(Run);
  //  RunOpt.ToLower();
  //ChainOpt = "MakeEvent,ITTF,ForceGeometry,NoSsdIt,NoSvtIt,Idst,VFMinuit,analysis,dEdxY2,";
  if (RunOpt.Contains("y2005",TString::kIgnoreCase)) {
    //    ChainOpt = "ry2005b,in,tpcI,svt_daq,SvtD,Physics,Idst,l0,tags,Tree,evout,ssdDb,IAna,fcf,VFMinuit,emcDY2,";
    ChainOpt = "ry2005b,in,tpcI,Physics,Idst,l0,tags,Tree,evout,IAna,fcf,VFMinuit,emcDY2,";
    ChainOpt+= "ftpc,trgd,ZDCvtx,Corr3,DbV20060421,useCDV,ITTF,tofDat,NosstIT,NosvtIT,SCEbyE,OGridLeakFull,OShortR,OSpaceZ2,TpxClu,TpxRaw";//-VFMinuit,";
    ChainOpt+= ",useInTracker";
    ChainOpt += ",McTpcAna,";
  } else if ( RunOpt.Contains("RC.y",TString::kIgnoreCase) ||
	      RunOpt.Contains("MC.y",TString::kIgnoreCase) ||
              RunOpt.Contains("RC20",TString::kIgnoreCase) ||
	      RunOpt.Contains("MC20",TString::kIgnoreCase) ) {
    ChainOpt = RunOpt;
    ChainOpt += ",MakeEvent,ITTF,NoSsdIt,NoSvtIt,Idst,VFMinuit,analysis,dEdxY2";
    //  ChainOpt += "Corr4";// no dynamical distortion ! ,OSpaceZ2,OGridLeakFull,"; // check that StTpcRSMaker::kDistortion bit is set
    //  ChainOpt += "EvOut,MuDST,MiniMcMk,McTpcAna,IdTruth,useInTracker,-hitfilt,";
    //  ChainOpt += ",CMuDst,MiniMcMk,IdTruth,useInTracker,tree,";
    //    ChainOpt += ",CMuDst,McAna,IdTruth,useInTracker,tree,KFVertex,xgeometry,";
    //    ChainOpt += ",CMuDst,IdTruth,useInTracker,tree,StiKFVertex,xgeometry,";
    ChainOpt += ",CMuDst,IdTruth,useInTracker,tree,xgeometry,";
    ChainOpt += "bbcSim,btofsim,btofMatch,btofCalib,";
    ChainOpt += "EvOut,-hitfilt,";
    ChainOpt += "McTpcAna,";
    // ChainOpt += ",tree,";
#if 1
    if (TString(gSystem->Getenv("STAR_VERSION")) == ".DEV2" ||
	TString(gSystem->Getenv("STAR_VERSION")) == "SL11d_embed") ChainOpt += "NoHistos,NoRunco,noTags,";
    else                                                           ChainOpt += "tags,";
#endif
  }
  // ChainOpt += "MiniMcMk,IdTruth,useInTracker,-hitfilt,CMuDst,Tree,tags,evout,";
  if (RunOpt.Contains("fcf",TString::kIgnoreCase)) {
    ChainOpt += ",tpl,tpcI,";
    RunOpt.ReplaceAll("TpcRS,","");
    RunOpt.ReplaceAll("trs,","");
  } else {
    ChainOpt += ",tpcDB,TpcHitMover,TpxClu,";
  }
  //  Bool_t needAlias = kFALSE;
  TString FileIn(fileIn);
  if (FileIn == "") { //  && fileOut == 0) {
    if (RunOpt.Contains("pythia",TString::kIgnoreCase)) {
      RootFile += "pythia";
    } else if (RunOpt.Contains("hijing",TString::kIgnoreCase)) {
      RootFile += "hijing";
    } else {
      
      if (Root4Star) {
	ChainOpt += "gstar,"; // RootFile += "gstar";
      } else {
	ChainOpt += "20Muons,";
      }
    }
    if (!( RunOpt.Contains("hadr_on",TString::kIgnoreCase) ||
	   Opt.Contains("hadr_on",TString::kIgnoreCase))) ChainOpt += "hadr_off,";
    if (! (RunOpt.Contains("phys_on",TString::kIgnoreCase) ||
	   Opt.Contains("phys_of",TString::kIgnoreCase))) ChainOpt += "phys_off,";
    if (RunOpt.Contains("PhysicsOff",TString::kIgnoreCase) ||
	   Opt.Contains("PhysicsOff",TString::kIgnoreCase)) ChainOpt += "phys_off,";
    if      (Opt.Contains("FieldOff" ,TString::kIgnoreCase)) ChainOpt += "FieldOff,";
    else if (Opt.Contains("HalfField",TString::kIgnoreCase)) ChainOpt += "HalfField,";
    else                                                     ChainOpt += "FieldOn,";
  } else {
    RootFile += Form("%s",gSystem->BaseName(FileIn.Data())); 
    if        (FileIn.Contains(".daq",TString::kIgnoreCase)) {
      ChainOpt += "in,TpxRaw,";
      RootFile.ReplaceAll(".daq","");
    } else if (Root4Star && FileIn.Contains(".fz",TString::kIgnoreCase)) {
      ChainOpt += "fzin,";
      RootFile.ReplaceAll(".fzd","");
      RootFile.ReplaceAll(".fz","");
    } else if (Root4Star && FileIn.Contains(".nt",TString::kIgnoreCase)) {
      ChainOpt += "ntin,";
      RootFile.ReplaceAll(".nt","");
      RootFile.ReplaceAll(".","_");
    } else if (FileIn.Contains(".geant.root",TString::kIgnoreCase)) {
      ChainOpt += "in,";
      RootFile.ReplaceAll(".geant.root","");
    } else if (Root4Star && FileIn.Contains(".MuDst",TString::kIgnoreCase)) {
      ChainOpt += "mtin,";
      RootFile.ReplaceAll(".MuDst.root","");
    } else {
      if (gSystem->AccessPathName(FileIn.Data())) FileIn ="";
    }
  }
  ChainOpt = RunOpt + ChainOpt;
  if (FileIn = "") {
    RootFile += Form("_%s_%i_%i",Opt.Data(),First,Last);
  }
  RootFile.ReplaceAll(".root","");
  RootFile.ReplaceAll(",","_");
  if (RootFile.Contains(";")) {
    Int_t index = RootFile.Index(";");
    RootFile = RootFile(0,index);
  }
  RootFile += ".root";
  RootFile.ReplaceAll(" ","");
  cout << "ChainOpt : " << ChainOpt.Data() << "\tOuput file " << RootFile.Data() << endl;
  if (Last < 0) {
    bfc(-1,ChainOpt.Data(),0,0,0);
    return;
  }
  
  TString output = RootFile;
  output.ReplaceAll(".root","O.root");
  output.ReplaceAll("*","");
  if (RunOpt.Contains("devT,",TString::kIgnoreCase)) ChainOpt += ",useXgeom";
  bfc(-1,ChainOpt.Data(),fileIn,output.Data(),RootFile.Data());
  StTpcRSMaker *tpcRS = (StTpcRSMaker *) chain->Maker("TpcRS");
  if (tpcRS) {
    //      if (needAlias) tpcRS->SetInput("geant","bfc/.make/inputStream/.make/inputStream_Root/.data/bfcTree/geantBranch");
    Int_t m_Mode = tpcRS->GetMode();
    //    cout << "========== Opt " << Opt.Data() << endl;
    if (Opt.Contains("heed",TString::kIgnoreCase))        {
      SETBIT(m_Mode,StTpcRSMaker::kHEED); CLRBIT(m_Mode,StTpcRSMaker::kBICHSEL);
      cout << "========== set HEED" << endl;
    }
    if (Opt.Contains("bichsel",TString::kIgnoreCase))     {
      SETBIT(m_Mode,StTpcRSMaker::kBICHSEL); CLRBIT(m_Mode,StTpcRSMaker::kHEED);
      cout << "========== set Bichsel" << endl;
      }
    if (! ChainOpt.Contains("Corr",TString::kIgnoreCase)) {
      CLRBIT(m_Mode,StTpcRSMaker::kDistortion);  // Check that distorton are IN chain
      cout << "========== set Distortion" << endl;
    }
      tpcRS->SetMode(m_Mode);
      //      tpcRS->SetDebug(13);
  }

#if 0 /* not enough memory for dE/dx plots */
  StMaker *dEdxY2 = chain->GetMaker("dEdxY2"); 
  if (dEdxY2) {
    StdEdxY2Maker *dEdx = (StdEdxY2Maker *) dEdxY2;
    Int_t mask = 0;
#if 0
    //     SETBIT(mask,StTpcdEdxCorrection::ktpcPressure); 
    //     SETBIT(mask,StTpcdEdxCorrection::kAdcCorrection); 
    //     SETBIT(mask,StTpcdEdxCorrection::kTpcSecRow); 
    //     SETBIT(mask,StTpcdEdxCorrection::kDrift);
    //     SETBIT(mask,StTpcdEdxCorrection::kzCorrection);
    //     SETBIT(mask,StTpcdEdxCorrection::kdXCorrection);
    //     SETBIT(mask,StTpcdEdxCorrection::kTpcdEdxCor);
    //     SETBIT(mask,StTpcdEdxCorrection::kTpcLengthCorrection);
    // from dEdx 06/20/04
    SETBIT(mask,StTpcdEdxCorrection::ktpcPressure); 
    //  SETBIT(mask,StTpcdEdxCorrection::ktpcMethaneIn); 
    //  SETBIT(mask,StTpcdEdxCorrection::ktpcGasTemperature); 
    //  SETBIT(mask,StTpcdEdxCorrection::ktpcWaterOut); 
    SETBIT(mask,StTpcdEdxCorrection::kAdcCorrection); 
    SETBIT(mask,StTpcdEdxCorrection::kTpcSecRow); 
    SETBIT(mask,StTpcdEdxCorrection::kDrift);
    SETBIT(mask,StTpcdEdxCorrection::kzCorrection);
    SETBIT(mask,StTpcdEdxCorrection::kdXCorrection);
    //  SETBIT(mask,StTpcdEdxCorrection::kTpcdEdxCor);
    //  SETBIT(mask,StTpcdEdxCorrection::kTpcLengthCorrection);
    SETBIT(mask,StTpcdEdxCorrection::kAdcCorrection);
    SETBIT(mask,StTpcdEdxCorrection::kTpcLast);
    //    SETBIT(Mode,StdEdxY2Maker::kOldClusterFinder); 
    //    SETBIT(Mode,StdEdxY2Maker::kDoNotCorrectdEdx);
    
    SETBIT(Mode,StdEdxY2Maker::kPadSelection); 
    SETBIT(Mode,StdEdxY2Maker::kCalibration);
    //    if (TString(gSystem->Getenv("STAR_VERSION")) == ".DEV2") 
    SETBIT(Mode,StdEdxY2Maker::kZBGX);
    SETBIT(Mode,StdEdxY2Maker::kGASHISTOGRAMS);
#endif
    Int_t Mode = 2;
    if (Mode) {
      dEdx->SetDebug(1);
      cout << " set dEdxY2 Mode" << Mode << " =======================================" << endl;
      dEdx->SetMode(Mode); 
    }
    if (mask) {
      cout << " set dEdxY2 mask " << mask << " =======================================" << endl;
      dEdx->SetMask(mask); 
    }
  }
#endif    
  if (Last < 0) return;
  Int_t initStat = chain->Init(); // This should call the Init() method in ALL makers
  if (initStat) {
    cout << "Chain initiation has failed" << endl;
    chain->Fatal(initStat, "during Init()");
  }
  cout << "FileIn = " << FileIn.Data() << " ====================" << endl;
  if (FileIn == "") {
    SetPartGan(RootFile, RunOpt, Opt);
  }
  if (Last > 0)  chain->EventLoop(First,Last);
}
#if 0
//________________________________________________________________________________
void TpcRS(Int_t Last=100,
	   const Char_t *Run = "y2009,TpcRS",//trs,fcf", // "TpcRS,fcf",
	   const Char_t *fileIn = 0,
	   const Char_t *opt = "Bichsel", const Char_t *kuip = 0,
	   const Char_t *fileOut = 0) {
  //  /star/data03/daq/2004/093/st_physics_adc_5093007_raw_2050001.daq
  //  /star/data03/daq/2004/fisyak/st_physics_adc_5114043_raw_2080001.daq
  // nofield /star/data03/daq/2004/076/st_physics_adc_5076061_raw_2060001.daq
  //                                   st_physics_adc_5076061_raw_4050001.daq
  TpcRS(1,Last,Run,fileIn,opt,kuip,fileOut);
}
#else
//________________________________________________________________________________
void TpcRS(Int_t Last=1,
	   const Char_t *Run = "mc2019",
	   const Char_t *fileIn = "./st_cosmic_adc_19151081_raw_0000005.MuDst.root",
	   const Char_t *opt = "Bichsel", const Char_t *kuip = 0,
	   const Char_t *fileOut = 0) {
  //  /star/data03/daq/2004/093/st_physics_adc_5093007_raw_2050001.daq
  //  /star/data03/daq/2004/fisyak/st_physics_adc_5114043_raw_2080001.daq
  // nofield /star/data03/daq/2004/076/st_physics_adc_5076061_raw_2060001.daq
  //                                   st_physics_adc_5076061_raw_4050001.daq
  TpcRS(1,Last,Run,fileIn,opt,kuip,fileOut);
}
#endif

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Macro for running chain with different inputs                        //
// owner:  Yuri Fisyak                                                  //
// Modifications by J. Lauret, V, Prevoztchikov, G.V. Buren, L. Didenko //
//                  and V. Fine                                         //
//                                                                      //
// $Id: bfc.C,v 1.194 2020/04/12 20:24:18 perev Exp $
//////////////////////////////////////////////////////////////////////////
class StBFChain;        
class StMessMgr;
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "St_base/Stiostream.h"
#include "TSystem.h"
#include "TClassTable.h"
#include "TApplication.h"
#include "TInterpreter.h"
#include "StBFChain.h"
#ifndef __CLING__1
#include "StMessMgr.h"
#include "StStarLogger/StLoggerManager.h"
#include "StarRoot/TAttr.h"
#endif
#include "TROOT.h"
#include "TAttr.h"
#include "Rtypes.h"
#endif
StBFChain    *chain=0; 
//_____________________________________________________________________
//_________________ Prototypes _______________________________________________
void Usage();
void Load(const Char_t *options="");
//TString defChain("y2010,gstar,20Muons,Test.default.Fast.ITTF,NosvtIT,NossdIT,-sfs,-ssdFast");
//TString defChain("y2011,gstar,20Muons,TpcRS,TpxClu,bbcSim,btofsim,emcY2,EEfs,pmdSim,IdTruth,MakeEvent,VFMinuit,ITTF,NosvtIT,NossdIT,analysis,BTofIT,KFVertex");//,corr4,KFVertex,sdt20100107.110000");
//TString defChain("MC.y2012a,sdt20120319,gstar,20Muons,StiCA,KFVertex,BEmcChkStat,btof,fmsSim,emcSim,EEss,eemcA2E,Corr4,OSpaceZ2,OGridLeak3D,-hitfilt");//,corr4,KFVertex,sdt20100107.110000");
//TString defChain("MC.y2012a,gstar,20Muons,StiCA,KFVertex,BEmcChkStat,btof,fmsSim,emcSim,EEss,Corr4,OSpaceZ2,OGridLeak3D,-hitfilt");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012a,gstar,20Muons,StiCA,KFVertex,xgeometry");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012a,gstar,20Muons,StiVMC");//,KFVertex");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012a,gstar,20Muons,StiCA,KFVertex,-hitfilt");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,pythia,StiCA,beamline,-hitfilt,KFVertex,CorrX,OSpaceZ2,OGridLeak3D,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,gstar,20Muons,StiCA,beamline,-hitfilt,KFVertex,CorrX,OSpaceZ2,OGridLeak3D,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,pythia,Sti,-hitfilt,Corr4,OSpaceZ2,OGridLeak3D,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,pythia,beamline,StiCA,KFVertex,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,pythia,TpcRS,StiCA,KFVertex,beamline,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,gstar,20Muons,TpcRS,StiCA,KFVertex,beamline,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,gstar,20Muons,TpcRS,Sti,beamline,VFPPVnoCTB,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,gstar,20Muons,TpcRS,StiCA,beamline,KFVertex,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,gstar,20Muons,TpcRS,StiCA,KFVertex,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,gstar,20Muons,TpcRS,Sti,VFMinuit,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2013,pythia,StiCA,-hitfilt,KFVertex,beamline");//eemcA2E,,sdt20100107.110000"); ,KFVertex
//TString defChain("MC.y2012a,gstar,20Muons,StiCA,-hitfilt,KFVertex,beamline,sdt20120408.134653,Corr4,OSpaceZ2,OGridLeak3D");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012a,pythia,StiCA,-hitfilt,KFVertex,beamline,sdt20120408.134653,Corr4,OSpaceZ2,OGridLeak3D");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.y2012,gstar,20Muons,tofsim,TpcRS,StiCA,KFVertex,beamline,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.devTY,gstar,20Muons,tofsim,TpcRS,StiCA,KFVertex,beamline,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2014,AgML,pythia,StiCA,-hitfilt,KFVertex,Corr4,OSpaceZ2,OGridLeak3D");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2014,AgML,pythia,StiCA,-hitfilt,KFVertex,Corr4,OSpaceZ2,OGridLeak3D,StiHftC,pxlFastSim,ssdfast");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2014,pythia,StiCA,-hitfilt,KFVertex,Corr4,OSpaceZ2,OGridLeak3D,StiHftC,pxlFastSim,ssdfast,AgML,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2014,pythia,StiCA,-hitfilt,KFVertex,Corr4,OSpaceZ2,OGridLeak3D,StiHftC,pxlFastSim,ssdfast,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2014,gstar,20Muons,StiCA,beamline,-hitfilt,KFVertex,Corr4,OSpaceZ2,OGridLeak3D,StiHftC,pxlFastSim,ssdfast,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2014,gstar,20Muons,StiCA,-hitfilt,KFVertex,Corr4,OSpaceZ2,OGridLeak3D,StiHftC,pxlFastSim,ssdfast,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("P2014aPxLSim,gstar,20Muons,TpcRS,TpxClu,bbcSim,btofsim,btofMatch,emcY2,emcSim,EEfs,McEvent,StiCA,KFVertex,useXgeom,HftMatTree");
//TString defChain("MC.2014,gstar,20Muons,TpcRS,Sti,KFVertex,beamline,-hitfilt,useXgeom,pxlFastSim,IstSim");
//TString defChain("MC.2014,gstar,20Muons,TpcRS,StiCA,KFVertex,-hitfilt,useXgeom,pxlFastSim,IstSim,sdt20140410.123905,phys_off");
//TString defChain("MC.2014,Pythia,TpcRS,StiCA,KFVertex,beamline,-hitfilt,useXgeom,pxlFastSim,IstSim");
//"P2014a,Stv,mtd,btof,pxlHit,istHit,BEmcChkStat,CorrX,OSpaceZ2,OGridLeak3D,-hitfilt,-SsdIt","/star/rcf/test/daq/2014/164/st_physics_15164004_raw_2000022.daq")'
//TString defChain("MC.2015,gstar,20Muons,phys_off,TpcRS,StiCA,beamline,KFVertex,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2015,gstar,20Muons,TpcRS,StiCA,VFMinuit,-hitfilt,useXgeom");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2015,HijingAuAuFixedTarget19,StiCA,KFVertex,-SstIt,-SstDb");//eemcA2E,,sdt20100107.110000");
//TString defChain("MC.2015,HijingAuAuFixedTarget19,StiCA,KFVertex,-SstIt,-SstDb,-MiniMcMk,tfs");//eemcA2E,,sdt20100107.110000");
//TString defChain("test.RC.AuAu200.y2016,StiCA,KFVertex")
//StBFChain * bfc(Int_t First, Int_t Last,const Char_t *Chain = defChain, // + ",Display",
//	 const Char_t *infile=0, const Char_t *outfile="HijingAuAuFixedTarget19.event.root", const Char_t *TreeFile="HijingAuAuFixedTarget19.root");
//	 const Char_t *Chain="gstar,20Muons,y2005h,MakeEvent,trs,sss,svt,ssd,fss,bbcSim,emcY2,tpcI,fcf,ftpc,SvtCL,svtDb,ssdDb,svtIT,ssdIT,ITTF,genvtx,Idst,event,analysis,EventQA,tags,Tree,EvOut,McEvOut,GeantOut,IdTruth,miniMcMk,StarMagField,FieldOn,McAna,Display",//,,NoSimuDb, display, //McQa, 
StBFChain * bfc(Int_t First, Int_t Last,const Char_t *Chain = "", // + ",Display",
	 const Char_t *infile=0, const Char_t *outfile=0, const Char_t *TreeFile=0, const Char_t *chainName=0);
StBFChain *bfc(Int_t First, const Char_t *Chain = "MC2016,StiCA,20Muons,vmc,Rung.1",
 	       const Char_t *infile=0, const Char_t *outfile=0, const Char_t *TreeFile=0, const Char_t *chainName = "");
//	 const Char_t *Chain="gstar,20Muons,y2005h,tpcDb,trs,tpc,Physics,Cdst,Kalman,tags,Tree,EvOut,McEvOut,IdTruth,miniMcMk,StarMagField,FieldOn,McAna", // McQA
//_____________________________________________________________________
void Load(const Char_t *options)
{
  cout << "Load system libraries\t";
  int nodefault = TString(options).Contains("nodefault",TString::kIgnoreCase);

  if ( TString(gProgName)!="root4star") { // ! root4star
    if (!nodefault || TString(options).Contains("mysql",TString::kIgnoreCase)) {
      const Char_t *mysql = "libmysqlclient";
      //Char_t *mysql = "libmimerS"; // just to test it picks from OPTSTAR

      //
      // May use USE_64BITS - the x8664 work fine too
      //
      const Char_t *libsLocal[]= {"",
	                    "$OPTSTAR/lib/",
			    "$OPTSTAR/lib/mysql/",
			    "/usr/lib/", 
			    "/usr/lib/mysql/", 
			    "/usr/mysql/lib/",
			    "/sw/lib/",
			    NULL}; 
      const Char_t *libsGlbal[]= {"", 
			    "/usr/lib/", 
			    "/usr/lib/mysql/", 
			    "/usr/mysql/lib/",
			    "$OPTSTAR/lib/",
			    "$OPTSTAR/lib/mysql/",
			    "/sw/lib/",
			    NULL}; 

      const Char_t **libs;

      if ( gSystem->Getenv("USE_LOCAL_MYSQL") ){
	libs = libsLocal;
      } else {
	libs = libsGlbal;
      }


      TString Arch( gSystem->GetBuildArch() );
      Bool_t i64 = kFALSE;
      if ( gSystem->Getenv("USE_64BITS") || Arch.Contains("x8664")) i64 = kTRUE;

      Int_t i = 0;
      while ((libs[i])) {
	TString lib(libs[i]);
	//cout << "Found " << lib << endl;
	if (i64) lib.ReplaceAll("/lib","/lib64");
	lib += mysql;
	lib = gSystem->ExpandPathName(lib.Data());
	if (gSystem->DynamicPathName(lib,kTRUE)) {
	  gSystem->Load(lib.Data()); 
	  cout << " + " << mysql << " from " << lib.Data();
	  break;
	}
	i++;
      }
    }
    cout << endl;
  }
  gSystem->Load("libSt_base");                                        //  StMemStat::PrintMem("load St_base");
#ifndef __CLING__1
  // Look up for the logger option
  Bool_t needLogger  = kFALSE;
  if (gSystem->Load("liblog4cxx") >=  0) {             //  StMemStat::PrintMem("load log4cxx");
    cout << " + liblog4cxx";
    if(gSystem->Load("libStStarLogger") >= 0) {              //  StMemStat::PrintMem("load log4cxx");
      cout << " + libStStarLogger";
      //      gROOT->ProcessLine("StLoggerManager::StarLoggerInit();"); 
      StLoggerManager::StarLoggerInit();
      if (gROOT->IsBatch())  StLoggerManager::setColorEnabled(kFALSE);
    }
  }
#endif
  //  gSystem->Load("libHtml");
  gSystem->Load("libStChain");                                        //  StMemStat::PrintMem("load StChain");
  gSystem->Load("libStUtilities");                                    //  StMemStat::PrintMem("load StUtilities");
#if 0
  gSystem->Load( "libVMC.so");
  gSystem->Load( "libsim_Tables");
  gSystem->Load( "libgen_Tables");
  gSystem->Load( "StarGeneratorUtil.so" );
  gSystem->Load( "StarGeneratorEvent.so" );
  gSystem->Load( "StarGeneratorBase.so" );

  gSystem->Load( "libMathMore.so"   );  
  gSystem->Load( "libStarGenEventReader.so" );
#endif  
  gSystem->Load("libStBFChain");                                      //  StMemStat::PrintMem("load StBFChain");
  cout << endl;
}
#ifndef __CLING__
//#define __V0Filter__
#ifdef __V0Filter__
//_____________________________________________________________________
void V0Filter() {
  St_geant_Maker *geant = (St_geant_Maker *) chain->Maker("geant");
  if (geant) {
    // Filter
    //    geant->Do("gfilter v0");
    geant->Do("gfilter example");
  }
}
#endif /* __V0Filter__ */
#endif
//_____________________________________________________________________
StBFChain *bfc(Int_t First, Int_t Last,
	       const Char_t *Chain,
	       const Char_t *infile,
	       const Char_t *outfile,
	       const Char_t *TreeFile, 
	       const Char_t *chainName)
{ // Chain variable define the chain configuration 
  // All symbols are significant (regardless of case)
  // "-" sign before requiest means that this option is disallowed
  // Chain = "gstar,20Muons" run GEANT on flight with 10 muons in range |eta| < 1 amd pT = 1GeV/c (default)
  // Dynamically link some shared libs
  // disable cint exeption catching
#ifndef __CLING__
  gROOT->ProcessLine(".exception");
#endif
  TString tChain(Chain);
  if (tChain == "") {
    if (Last == -2 && tChain.CompareTo("ittf",TString::kIgnoreCase)) Usage();
    return chain;
  } else {
#if 0
    // Predefined test chains
    Int_t typeC = 0;
    const Char_t *predChains[4] = {"MC2016","MC2017","MC2018","MC2019"};
    for (Int_t i = 0; i < 4; i++) {
      TString PredChain(predChains[i]);
      if (TString(Chain) == PredChain) {typeC = i+1; break;}
      PredChain += ".Ideal";
      if (TString(Chain) == PredChain) {typeC = -(i+1); break;}
    }
    if (typeC) {
      if ( TString(gProgName) == "root4star") {
	tChain += ".Ideal,gstar,useXgeom,CorrX"; 
      } else                                   {
	tChain += ",vmc,CorrX";
      }
      tChain += ",RunG.1";
    }
#endif
  }
  if (gClassTable->GetID("StBFChain") < 0) Load(tChain.Data());
  chain = (StBFChain *) StMaker::New("StBFChain", chainName);
  cout << "Create chain " << chain->GetName() << endl;
  chain->cd();
  chain->SetDebug(1);
  if (Last < -3) return chain;
  chain->SetFlags(tChain);
#ifndef __CLING__
  gMessMgr->QAInfo() << Form("Process [First=%6i/Last=%6i/Total=%6i] Events",First,Last,Last-First+1) << endm;
#else
  cout <<  Form("QA :INFO  - Process [First=%6i/Last=%6i/Total=%6i] Events",First,Last,Last-First+1) << endl;
#endif
  if (Last < -2) return chain;
  if (chain->Load() > kStOk) {
#ifndef __CLING__
    gMessMgr->Error() << "Problems with loading of shared library(ies)" << endm;
#else
    cout << "QA :ERROR - Problems with loading of shared library(ies)" << endl;
#endif
    gSystem->Exit(1);
  }
  chain->Set_IO_Files(infile,outfile);
  if (TreeFile) chain->SetTFile(new TFile(TreeFile,"RECREATE"));
  if (Last < -1) return chain;
  if (chain->Instantiate() > kStOk)  { 
#ifndef __CLING__
    gMessMgr->Error() << "Problems with instantiation of Maker(s)" << endm;
#else
    cout << "QA :ERROR - Problems with instantiation of Maker(s)" << endl;
#endif
    gSystem->Exit(1);
  }
#ifdef __V0Filter__
  gSystem->Load("StMCFilter");
#endif
  StMaker::lsMakers(chain);
  if (Last < 0) return chain;
  StMaker *dbMk = chain->GetMaker("db");
  if (dbMk) dbMk->SetDebug(1);
  StMaker *sti  = chain->GetMaker("Sti");
  if (sti) sti->SetAttr("useTiming",1);
#if 0
  StMaker *btofSim = chain->GetMaker("BTofSim");
  if (btofSim) btofSim->SetAttr("UseMCTstart",1);
#endif
#if 0
  // Insert your maker before "tpc_hits"
  Char_t *myMaker = "St_TLA_Maker";
  if (gClassTable->GetID(myMaker) < 0) {
	  gSystem->Load(myMaker);//  TString ts("load "; ts+=myMaker; StMemStat::PrintMem(ts.Data());
  }
  StMaker *myMk = chain->GetMaker(myMaker);
  if (myMk) delete myMk;
  myMk = chain->New(myMaker,"before");
  if (myMk) {
    Char_t *before = "tpc_hits";
    StMaker *tclmk = chain->GetMaker(before);
    if (tclmk) chain->AddBefore(before,myMk);
  }
  // Insert your maker after "tpc_hits"
  myMk = chain->New(myMaker,"after");
  if (myMk) {
    Char_t *after = "tpc_hits";
    StMaker *tclmk = chain->GetMaker(after);
    if (tclmk) chain->AddAfter(after,myMk);
  }
  // this block is meant as an example ONLY
  // The default values are set in StRoot/StPass0CalibMaker/ StTpcT0Maker 
  // constructor and are suitable for production. You can change it here
  // for test purposes.
  if (chain->GetOption("TpcT0")) {
    StTpcT0Maker *t0mk = (StTpcT0Maker *) chain->GetMaker("TpcT0");
    if (t0mk) t0mk->SetDesiredEntries(10);
  }
#endif
  {
    TDatime t;
#ifndef __CLING__
    gMessMgr->QAInfo() << Form("Run is started at Date/Time %i/%i",t.GetDate(),t.GetTime()) << endm;
#else
    cout << Form("QA :INFO  - Run is started at Date/Time %i/%i",t.GetDate(),t.GetTime()) << endl;
#endif
  }
#ifndef __CLING__
  gMessMgr->QAInfo() << Form("Run on %s in %s",gSystem->HostName(),gSystem->WorkingDirectory()) << endm;
  gMessMgr->QAInfo() << Form("with %s", chain->GetCVS()) << endm;
#else
  cout << Form("QA :INFO  - Run on %s in %s",gSystem->HostName(),gSystem->WorkingDirectory()) << endl;
  cout << Form("QA :INFO  - with %s", chain->GetCVS()) << endl;
#endif
  // Init the chain and all its makers
  TAttr::SetDebug(0);
#ifndef __CLING__
  chain->SetAttr(".Privilege",0,"*"                ); 	  //All  makers are NOT priviliged
  chain->SetAttr(".Privilege",1,"StIOInterFace::*" ); 	  //All IO makers are priviliged
  chain->SetAttr(".Privilege",1,"St_geant_Maker::*"); 	  //It is also IO maker
  chain->SetAttr(".Privilege",1,"StTpcDbMaker::*"); 	  //It is also TpcDb maker to catch trips
  chain->SetAttr(".Privilege",1,"*::tpc_hits"); //May be allowed to act upon excessive events
  chain->SetAttr(".Privilege",1,"*::tpx_hits"); //May be allowed to act upon excessive events
  chain->SetAttr(".Privilege",1,"StTpcHitMover::*"); //May be allowed to act upon corrupt events
  chain->SetAttr(".Privilege",1,"*::tpcChain"); //May pass on messages from sub-makers
  chain->SetAttr(".Privilege",1,"StTriggerDataMaker::*"); //TriggerData could reject event based on corrupt triggers
  chain->SetAttr(".Privilege",1,"StEandBDirMaker::*"); // just for debuggin purpose
  chain->SetAttr(".Privilege",1,"StEventMaker::*"); //May be allowed to act upon trigger IDs (filtering)
#endif
  Int_t iInit = chain->Init();
  if (iInit >=  kStEOF) {chain->FatalErr(iInit,"on init"); return chain;}
  if (Last == 0) return chain;
#ifdef __V0Filter__
  V0Filter();
#endif
  StEvtHddr *hd = (StEvtHddr*)chain->GetDataSet("EvtHddr");
  if (hd) hd->SetRunNumber(-2); // to be sure that InitRun calls at least once
    // skip if any
  chain->EventLoop(First,Last,0);
#ifndef __CLING__
  gMessMgr->QAInfo() << "Run completed " << endm;
#else
  cout << "QA :INFO  - Run completed " << endl;
#endif
  return chain;
}
//_____________________________________________________________________
StBFChain *bfc(Int_t Last, 
	       const Char_t *Chain,
	       const Char_t *infile, 
	       const Char_t *outfile, 
	       const Char_t *TreeFile,
	       const Char_t *chainName) {
  return bfc(1,Last,Chain,infile,outfile,TreeFile,chainName);
}
//____________________________________________________________
void Usage() {
  printf ("============= \t U S A G E =============\n");
  printf ("bfc(Int_t First,Int_t Last,const Char_t *Chain,const Char_t *infile,const Char_t *outfile,const Char_t *TreeFile)\n");
  printf ("bfc(Int_t Last,const Char_t *Chain,const Char_t *infile,const Char_t *outfile,const Char_t *TreeFile)\n");
  printf ("bfc(const Char_t *ChainShort,Int_t Last,const Char_t *infile,const Char_t *outfile)\n");
  printf ("where\n");
  printf (" First     \t- First event to process\t(Default = 1)\n");
  printf (" Last      \t- Last  event to process\t(Default = 1)\n");
  printf (" Chain     \t- Chain specification   \t(without First &  Last: Default is \"\" which gives this message)\n");
  printf ("           \t                        \t with    First || Last: Default is \"gstar,20Muons\")\n");
  printf (" infile    \t- Name of Input file    \t(Default = 0, i.e. use preset file names depending on Chain)\n"); 
  printf (" outfile   \t- Name of Output file   \t(Default = 0, i.e. define Output file name from Input one)\n");
  printf (" outfile   \t- Name of Tree File     \t(Default = 0, i.e. define Output file name from Input one (tags TNtuple))\n");
  printf (" ChainShort\t- Short cut for chain   \t(Default = \"\" -> print out of this message)\n");
  gSystem->Exit(1);
}
//_____________________________________________________________________
StBFChain *bfc(const Char_t *Chain="ittf") {
  return bfc(-2,Chain);
}

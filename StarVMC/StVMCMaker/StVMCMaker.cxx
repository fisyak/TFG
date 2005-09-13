//*-- Author : Yuri Fisyak
// 
// $Id: StVMCMaker.cxx,v 1.4 2005/09/13 21:34:29 fisyak Exp $
// $Log: StVMCMaker.cxx,v $
// Revision 1.4  2005/09/13 21:34:29  fisyak
// Move initialization from Init to InitRun, add conversion TGeoVolume to TVolume for StEventDisplayMaker and TofMatchers
//
// Revision 1.3  2005/06/17 18:35:45  fisyak
// Add flow diagram
//
// Revision 1.2  2005/06/09 20:14:40  fisyak
// Set Run number (=1 D)
//
// Revision 1.1  2005/05/24 22:58:08  fisyak
// The first version
//
//
/* Flow diagram:
   Load(); // shared libraries
   GetVMC(); // define gGeoManager
------------------
StVMCMaker::Init()
------------------

   StarVMCApplication *appl = new StarVMCApplication("StarVMC", "The STAR VMC application");
   Geant3TGeo* geant3 = new TGeant3TGeo("C++ Interface to Geant3"); // gMC
   StarMCPrimaryGenerator *generator = new StarMCHBPrimaryGenerator(fInputFile,m_DataSet); // a generator
   appl->SetPrimaryGenerator(generator);
   StarMCHits *hits = StarMCHits::instance(); 
   appl->SetStepping(hits);
   hits->SetHitHolder(m_DataSet); // set hit storage
   appl->InitMC();
   ->    gMC->SetStack(fStack);
   ->    gMC->Init();
   ->                  DefineParticles();
   ->                  appl->AddParticles();
   ->                  appl->ConstructGeometry();
   ->                  FinishGeometry();
   ->                  appl->InitGeometry();
   ->                        fMcHits->Init(); // hit description
   ->    gMC->BuildPhysics(); 
----------------
StVMCMaker::Make
----------------
   appl->RunMC(1);
   ->    gMC->ProcessRun(1)
   ->                  appl->BeginEvent();
   ->                  ProcessEvent();
   ->                    Gtrigi();
   ->                    Gtrigc();
   ->                    Gtrig();
   ->                      appl->GeneratePrimaries();             // gukine
   ->                      gtreveroot();
   ->                               appl->Field(xdouble,bdouble); // gufld
   ->                               appl->PreTrack();
   ->                               g3track();
   ->                                   appl->Stepping();         // gustep
   ->                                       hits->Step();
   ->                                         fill fHit
   ->                                         FillG2Table();
   ->                                            fCurrentDetector->GetChair()->Fill(fHit);
   ->                                                   G2TBook[Track]Hit();
   ->                                                   G2TFill[Track]Hit();
   ->                               appl->PostTrack();
   ->                  appl->FinishEvent();
   ->                      hits->FinishEvent(); // fill run,event,track tables
*/

#include <assert.h>
#include "TGeometry.h"
#include "TGeoManager.h"
#include "TObjectSet.h"
#include "StVMCMaker.h"
#include "StChain.h"
#include "Stiostream.h"
#include "StarMagField.h"
#include "StarMCHits.h"
#include "StarMCSimplePrimaryGenerator.h"
#include "StarMCHBPrimaryGenerator.h"
#include "TGeoDrawHelper.h"
#include "StMessMgr.h"

ClassImp(StVMCMaker);

StarVMCApplication* StVMCMaker::fgStarVMCApplication = 0;
TGeant3TGeo*        StVMCMaker::fgGeant3 = 0;
//_____________________________________________________________________________
Int_t StVMCMaker::InitRun  (Int_t runumber){
  TObjectSet *geom = (TObjectSet *) GetDataBase("VmcGeometry");
  assert(geom);
  if (!fVolume) {
    TGeoDrawHelper Helper;
    fVolume = (TDataSet *) Helper.GetVolume();
  }
  if (fVolume) { 
     if (gGeometry) {
        TList *listOfVolume = gGeometry->GetListOfNodes();

        // Remove hall from the list of ROOT nodes to make it free of ROOT control
        listOfVolume->Remove(fVolume);
        listOfVolume->Remove(fVolume);
     }
     // Add "hall" into ".const" area of this maker
     ((StVMCMaker *)this)->AddConst(fVolume);
     if (Debug()) fVolume->ls(3);
  }
  
  if (fgStarVMCApplication) return kStOK;
  fgStarVMCApplication = new StarVMCApplication("StarVMC", "The STAR VMC application");
  if ( fgGeant3) return kStOK;
  fgGeant3 = new TGeant3TGeo("C++ Interface to Geant3");//, 1, 200000); 
  gMessMgr->Info() << "StVMCMaker::InitRun Geant3 has been created." << endm;
  
  gMessMgr->Info() << "StVMCMaker::InitRun SetMagField set as StarMagField" 
		   << " with Map: " << StarMagField::Instance()->GetMap()
		   << ",Factor: " << StarMagField::Instance()->GetFactor() 
		   << ",Rescale: " << StarMagField::Instance()->GetRescale() <<endm;
  fgStarVMCApplication->SetMagField(StarMagField::Instance());

  if ((m_Mode/10)%10 == 1) {gMessMgr->Info() << "StVMCMaker::InitRun Passive mode" << endm;} 
  else                     {gMessMgr->Info() << "StVMCMaker::InitRun Active  mode" << endm;
    if ( m_Mode%10 == 1)   {gMessMgr->Info() << "StVMCMaker::InitRun Mixer   mode" << endm;}
    else {
      fEvtHddr = (StEvtHddr*)GetDataSet("EvtHddr");
      if (!fEvtHddr) {                            // Standalone run
	fEvtHddr = new StEvtHddr(m_ConstSet);
	SetOutput(fEvtHddr);	                //Declare this "EvtHddr" for output
      }
      fEvtHddr->SetRunNumber(fRunNo);
      fEvtHddr->SetEventNumber(0);
      fEvtHddr->SetEventType("VMC");
      fEvtHddr->SetProdDateTime();
    }
    StarMCPrimaryGenerator *generator = 0;
    if (fInputFile != "") generator = new StarMCHBPrimaryGenerator(fInputFile,m_DataSet);
    //                                                             Ntrack Id Ptmin Ptmax Ymin Ymax Phimin Phimax Zmin Zmax
    //  else              generator = new StarMCSimplePrimaryGenerator( 1, 5,    1.,   1.,0.1, 0.1, 0.57,  0.57,  0.,   0., "G");
    else                  generator = new StarMCSimplePrimaryGenerator(80, 5,    1.,   1.,-4.,  4.,    0,  6.28,  0.,   0., "G");
    assert(generator);
    if (Debug()) generator->SetDebug(1);
    fgStarVMCApplication->SetPrimaryGenerator(generator);
    StarMCHits *hits = StarMCHits::instance();
    hits->SetHitHolder(m_DataSet);
    fgStarVMCApplication->SetStepping(hits);
  }
  //  fgStarVMCApplication->SetStepping(new StMCSteppingHist("tgeom"));
  //  fgStarVMCApplication->SetStepping(new StMCStepping);
  // The "Init" method in the gMC object causes the geometry to be cosntructed
  if ((m_Mode/100)%10 == 1) {// switch off physics 
    gMessMgr->Info() << "StVMCMaker::InitRun switch off physics" << endm;
    gMC->SetProcess("DCAY", 0);
    gMC->SetProcess("ANNI", 0);
    gMC->SetProcess("BREM", 0);
    gMC->SetProcess("COMP", 0);
    gMC->SetProcess("HADR", 0);
    gMC->SetProcess("MUNU", 0);
    gMC->SetProcess("PAIR", 0);
    gMC->SetProcess("PFIS", 0);
    gMC->SetProcess("PHOT", 0);
    gMC->SetProcess("RAYL", 0);
    gMC->SetProcess("LOSS", 4); // no fluctuations 
    //  gMC->SetProcess("LOSS 1"); // with delta electron above dcute
    gMC->SetProcess("DRAY", 0);
    gMC->SetProcess("MULS", 0);
    gMC->SetProcess("STRA", 0);
    gMC->SetCut("CUTGAM",	1e-3  );
    gMC->SetCut("CUTELE", 	1e-3  );
    gMC->SetCut("CUTHAD", 	.001  );
    gMC->SetCut("CUTNEU", 	.001  );
    gMC->SetCut("CUTMUO", 	.001  );
    gMC->SetCut("BCUTE", 	.001  );
    gMC->SetCut("BCUTM", 	.001  );
    gMC->SetCut("DCUTE", 	1e-3  );
    gMC->SetCut("DCUTM", 	.001  );
    gMC->SetCut("PPCUTM", 	.001  );
    gMC->SetCut("TOFMAX", 	50.e-6);
  }
  
  fgStarVMCApplication->InitMC();
  if (Debug() > 1) {
    fgGeant3->SetDEBU(1,1,100);
    fgGeant3->SetSWIT(1,2);
    fgGeant3->SetSWIT(2,2);
  } else {
    fgGeant3->SetSWIT(4,0);
  }
  fInitRun = 1;
  return kStOK;
}
//_____________________________________________________________________________
Int_t StVMCMaker::Make(){
  if (! fInitRun) InitRun(fRunNo);
  fEventNo++;
  if (fEvtHddr) {
    fEvtHddr->SetRunNumber(fRunNo);
    fEvtHddr->SetEventNumber(fEventNo);
    fEvtHddr->SetEventType("VMC");
    fEvtHddr->SetProdDateTime();
    //    SetDateTime();
  }  
  if ((m_Mode/10)%10 != 1) {// Active  mode 
    TStopwatch sw;
    fgStarVMCApplication->RunMC(1);
    if (Debug())   sw.Print();
  }
  return kStOK;
}
//_____________________________________________________________________________
void StVMCMaker::Clear(Option_t *option){
  StMaker::Clear();
}
//_____________________________________________________________________________
Int_t StVMCMaker::Finish(){
  //  StMCSteppingHist::Instance()->Finish();
  return StMaker::Finish();
}
//_____________________________________________________________________________
void StVMCMaker::SetDateTime(Int_t id, Int_t it) {
  if ( m_Mode%10 != 1 && (m_Mode/10)%10 != 1 && fEvtHddr ) {
    fEvtHddr->SetDateTime(id,it);
  }  
 }
//_____________________________________________________________________________
void StVMCMaker::Skip(Int_t nskip) {
}
//_____________________________________________________________________________
TDataSet  *StVMCMaker::FindDataSet (const char* logInput,const StMaker *uppMk,
                                        const StMaker *dowMk) const {
  TDataSet *ds = StMaker::FindDataSet(logInput,uppMk,dowMk);
  
  if (ds || strcmp(logInput,"HALL")) return ds;
  return fVolume;

}

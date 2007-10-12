//////////////////////////////////////////////////////////////////////////
//
//
// StTriggerSimuMaker R.Fatemi, Adam Kocoloski , Jan Balewski  (Fall, 2007)
//
// Goal: generate trigger response based on ADC
// implemented BEMC,EEMC,....
// >StTriggerSimu/*SUB*/St*SUB*TriggerSimu.h
// >where *SUB* are the subsystems: Eemc, Bemc, Bbc,.... 
// > L2 is served by a separate maker
//
//////////////////////////////////////////////////////////////////////////


#include <Stiostream.h>
#include "StChain.h"
#include "TFile.h"
#include <math.h>

//#include "StEEmcPool/StL2Emulator/StL2EmulatorMaker.h"
#include "St_db_Maker/St_db_Maker.h" // just for time stamp
#include "StEEmcUtil/EEdsm/EMCdsm2Tree.h"// to access Etot

//StEvent
#include "StEvent/StEvent.h"

//get  EEMC
#include "Eemc/StEemcTriggerSimu.h"
#include "Eemc/EemcHttpInfo.h"
#include "StEEmcUtil/EEdsm/EEfeeTPTree.h" // for printouts only
#include "StEEmcUtil/EEdsm/EEfeeTP.h"  // for printouts only

//get BEMC
#include "StTriggerUtilities/Bemc/StBemcTriggerSimu.h"
#include "StEmcRawMaker/StBemcTables.h"

//get BBC
#include "Bbc/StBbcTriggerSimu.h"

//get L2
#include "L2Emulator/StL2TriggerSimu.h"

//get HEAD Maker
#include "StTriggerSimuMaker.h"


ClassImp(StTriggerSimuMaker)

StTriggerSimuMaker::StTriggerSimuMaker(const char *name):StMaker(name) {
  mDbMk=0;
  mYear=-1;
  mMCflag=0;
  eemc=0;
  bbc=0;
  bemc=0;
  lTwo=0;
  config = new TString("offline");
}

//____________________________________________________________________________
StTriggerSimuMaker::~StTriggerSimuMaker(){
}

//________________________________________________
void 
StTriggerSimuMaker::useEemc(int flag){
  eemc=new StEemcTriggerSimu();
  eemc->setConfig(flag);
  mSimulators.push_back(eemc);
}

//________________________________________________
void 
StTriggerSimuMaker::useBbc(){
  bbc=new StBbcTriggerSimu;
  mSimulators.push_back(bbc);
}

//________________________________________________
void
StTriggerSimuMaker::useBemc(){
  bemc=new StBemcTriggerSimu;
  mSimulators.push_back(bemc);
}

//________________________________________________
void
StTriggerSimuMaker::useL2(){
  lTwo=new StL2TriggerSimu;
  mSimulators.push_back(lTwo);
}

//_____________________________________________________________________________
Int_t 
StTriggerSimuMaker::Init() {
  LOG_INFO <<Form("StTriggerSimuMaker::Init(), MCflag=%d",mMCflag)<<endm;
  
  if(eemc) {
    eemc->setHList(mHList);
  }

  if(bemc) {
    mTables=new StBemcTables();
    event = NULL;
    bemc->setBemcConfig(config);
  }
  
  for(unsigned i=0; i<mSimulators.size(); i++) {
    mSimulators[i]->setMC(mMCflag);
    mSimulators[i]->Init();
  }

 return StMaker::Init();
}

//________________________________________________
void 
StTriggerSimuMaker::Clear(const Option_t*){

  LOG_INFO<<"StTriggerSimuMaker::Clear()"<<endm;
  
  mTriggerList.clear();
  
  for(unsigned i=0; i<mSimulators.size(); i++) {
    mSimulators[i]->Clear();
  }

  for (int tpid=0;tpid<kNPatches;tpid++) {
    BEMC_L0_HT_ADC[tpid]=0;
    BEMC_L0_TP_ADC[tpid]=0;
  }

}


//________________________________________________

Int_t
StTriggerSimuMaker::InitRun  (int runNumber){
  LOG_INFO<<"::InitRun()="<<runNumber<<endm;

  //if(eemc) eemc->InitRun();
  if(bemc) {
    bemc->setBemcDbMaker(mDbMk);
    mTables->loadTables(this);
    bemc->setTableMaker(mTables);
    //bemc->InitRun();
  }
  
  for(unsigned i=0; i<mSimulators.size(); i++) {
    mSimulators[i]->InitRun(runNumber);
  }

  assert(mDbMk);
  mYear=mDbMk->GetDateTime().GetYear();
  int yyyymmdd=mDbMk->GetDateTime().GetDate(); //form of 19971224 (i.e. 24/12/1997)
  int hhmmss=mDbMk->GetDateTime().GetTime(); //form of 123623 (i.e. 12:36:23)
  
  LOG_INFO<<Form("InitRun() run=%d yyyymmdd=%d  hhmmss=%06d\n",runNumber, yyyymmdd, hhmmss )<<endm;

  return kStOK;
}

//_____________________________________________________________________________
Int_t 
StTriggerSimuMaker::Make(){
  LOG_INFO<<"::Make()"<<endm;

  if(bemc) 
  {
    mTables->loadTables(this);
    bemc->setTableMaker(mTables);

    event=(StEvent*)GetInputDS("StEvent");
    if(!event) return kStOk;
    bemc->setEvent(event);
  }

  for(unsigned i=0; i<mSimulators.size(); i++) {
    mSimulators[i]->Make();
  }

  if(bemc) {
    int *BEMC_HT_Holder=bemc->getBEMC_L0_HT_ADC();
    int *BEMC_TP_Holder=bemc->getBEMC_L0_TP_ADC();
    for (int tpid=0;tpid<kNPatches;tpid++)  {
      BEMC_L0_HT_ADC[tpid]=BEMC_HT_Holder[tpid];
      BEMC_L0_TP_ADC[tpid]=BEMC_TP_Holder[tpid];
    }
  }

  // add L2 triggers
  //.....

  // now both E+B EMC response has been processed by the trigger logic, below only get-methods are called.
  /*
  if(bemc) bemc->addTriggerList(&mTriggerList);
  if(eemc) eemc->addTriggerList(&mTriggerList);
  addTriggerList(); //  final decisions, involve L2 


  // all code below is for Pibero for testing

  int tpId;
  EemcHttpInfo httpInfo;
  for (tpId=0;tpId<90;tpId++){
    bool found=eemc->getHttpInfo(tpId,httpInfo);
    printf("http(%d) ok=%d ",tpId,found);
    if(!found) { printf("\n"); continue;}
    httpInfo.print();
       // details of this TP
    eemc->feeTPTreeADC->TP(tpId)->print(3);
      // do your job
      // break;// ???
  } 
  cout<<Form(" 4Pibero HTTP bit=%d  bbc=%d",eemc->dsm2TreeADC->getOutEndcapHTTP1bit(), bbc->getEandW())<<endl;
  */
  return kStOK;
}


//========================================
//========================================
void
StTriggerSimuMaker::addTriggerList() { // not working yet
#if 0

  StL2EmulatorMaker *L2Mk=(StL2EmulatorMaker *)GetMaker("L2TrigEmul");
  assert(L2Mk);
  if(mYear==2006) {

    bool mb=true; // fake bit
    bool bjp0=isTrigger(127501);
    bool ejp0=isTrigger(127551);
    bool Etot=StEemcTrigger::dsm2TreeADC->getOutEtot1bit();
    bool L2jet=L2Mk->isTrigger(127652) || L2Mk->isTrigger(127622);
    printf("zz mb=%d bjp0=%d ejp0=%d etot=%d L2jet=%d\n",mb,  bjp0, ejp0,Etot,L2jet);

    if(L2jet && Etot && mb) {
      if( ejp0) mTriggerList.push_back(127652);
      if( bjp0) mTriggerList.push_back(127622);
    }    
  } // end of 2006
#endif
}
//========================================
    
bool StTriggerSimuMaker::isTrigger(int trigId) {
//  uint j;
//  for(j=0; j<mTriggerList.size();j++) {
//    //  printf("aa j=%d,  %d %d  ret=%d\n",j, trigId, mTriggerList[j],trigId==mTriggerList[j]);
//    if(trigId==mTriggerList[j]) return true;
//  }
//  return false;

  for(unsigned i=0; i<mSimulators.size(); i++) {
    if(mSimulators[i]->isTrigger(trigId) == 0) return false;
  }
  return true;
}

//========================================

Int_t 
StTriggerSimuMaker::Finish() {
  return StMaker::Finish();
}

void StTriggerSimuMaker::setTableMaker(StBemcTables *bemcTab){
  bemc->setTableMaker(bemcTab);
}

// $Id: StTriggerSimuMaker.cxx,v 1.14 2007/10/12 14:36:00 balewski Exp $
//
// $Log: StTriggerSimuMaker.cxx,v $
// Revision 1.14  2007/10/12 14:36:00  balewski
// added L2 interface
//
// Revision 1.13  2007/10/11 00:32:56  balewski
// L2algo added
//
// Revision 1.12  2007/09/25 18:19:35  rfatemi
// Update for TP work
//
// Revision 1.11  2007/09/24 18:08:11  kocolosk
// some code restructuring
//
// Revision 1.10  2007/09/21 18:45:40  rfatemi
// End of week update
//
// Revision 1.9  2007/08/13 02:51:29  rfatemi
// update before rcf goes offline
//
// Revision 1.8  2007/08/12 01:03:22  rfatemi
// Added flag for offline/online/expert settings
//
// Revision 1.7  2007/08/07 15:48:20  rfatemi
// Added BEMC access
//
// Revision 1.6  2007/07/24 01:32:43  balewski
// *** empty log message ***
//
// Revision 1.5  2007/07/23 02:59:48  balewski
// cleanup, bbc for M-C still not working
//
// Revision 1.4  2007/07/22 23:09:34  rfatemi
// Add Bbc access
//
// Revision 1.2  2007/07/21 23:35:24  balewski
// works for M-C
//
// Revision 1.1  2007/07/20 21:03:18  balewski
// start
//

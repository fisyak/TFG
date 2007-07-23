//////////////////////////////////////////////////////////////////////////
//
//
// StTriggerSimuMaker R.Fatemi, Adam Kocoloski , Jan Balewski  (Fall, 2007)
//
// Goal: generate trigger response based on ADC
// implemented BEMC,EEMC,....
// >StTriggerSimu/*SUB*/St*SUB*TriggerSimu.h
// >where *SUB* are the subsystems: Eemc, Bemc, Bbc, L2,.... 
//
//////////////////////////////////////////////////////////////////////////


#include <Stiostream.h>
#include "StChain.h"
#include "TFile.h"
#include <math.h>

//#include "StEEmcPool/StL2Emulator/StL2EmulatorMaker.h"
#include "St_db_Maker/St_db_Maker.h" // just for time stamp
#include "StEEmcUtil/EEdsm/EMCdsm2Tree.h"// to access Etot

//get  EEMC
#include "Eemc/StEemcTriggerSimu.h"
//get BBC
#include "Bbc/StBbcTriggerSimu.h"
//get HEAD Maker
#include "StTriggerSimuMaker.h"


ClassImp(StTriggerSimuMaker)

StTriggerSimuMaker::StTriggerSimuMaker(const char *name):StMaker(name) {
  mDbMk=0;
  mYear=-1;
  mMCflag=0;
  eemc=0;
  bbc=0;
}

//____________________________________________________________________________
StTriggerSimuMaker::~StTriggerSimuMaker(){
}

//________________________________________________
void 
StTriggerSimuMaker::useEemc(){
  eemc=new StEemcTriggerSimu;
}

//________________________________________________
void 
StTriggerSimuMaker::useBbc(){
  bbc=new StBbcTriggerSimu;
}

//_____________________________________________________________________________
Int_t 
StTriggerSimuMaker::Init() {
  LOG_INFO <<Form("StTriggerSimuMaker::Init(), MCflag=%d",mMCflag)<<endm;
  
  if(eemc) {
    eemc->setHList(mHList);
    eemc->setMC(mMCflag);
    eemc->Init();
  }

  if(bbc) {
    bbc->setMC(mMCflag);
    bbc->Init();
  }

 return StMaker::Init();
}

//________________________________________________
void 
StTriggerSimuMaker::Clear(const Option_t*){
  
  mTriggerList.clear();
  if(eemc) eemc->Clear();
  if(bbc)  bbc ->Clear();
  
  LOG_DEBUG<<"::Clear()"<<endm;
}


//________________________________________________

Int_t
StTriggerSimuMaker::InitRun  (int runNumber){
  LOG_INFO<<"::InitRun()="<<runNumber<<endm;

  if(eemc) eemc->InitRun();
  
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

    LOG_DEBUG<<"::Make()"<<endm;

    if(bbc) bbc->Make();
    cout<<"BBC Trigger = "<<bbc->bbcTrig<<endl;
    if(eemc) eemc->Make();
 
    // add L2 triggers
    //.....

    // now both E+B EMC response has been processed by the trigger logic, below only get-methods are called.
    
    // StBemcTrigger::addTriggerList(&mTriggerList);
    if(eemc) eemc->addTriggerList(&mTriggerList);
    addTriggerList(); //  final decisions, involve L2 


    cout<<Form(" 4Pibero HTTP bit=%d  bbc=%d",eemc->dsm2TreeADC->getOutEndcapHTTP1bit(), bbc->getEandW())<<endl;
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
  uint j;
  for(j=0; j<mTriggerList.size();j++) {
    //  printf("aa j=%d,  %d %d  ret=%d\n",j, trigId, mTriggerList[j],trigId==mTriggerList[j]);
    if(trigId==mTriggerList[j]) return true;
  }
  return false;
}
    
    
//========================================

Int_t 
StTriggerSimuMaker::Finish() {
    return StMaker::Finish();
}

// $Id: StTriggerSimuMaker.cxx,v 1.5 2007/07/23 02:59:48 balewski Exp $
//
// $Log: StTriggerSimuMaker.cxx,v $
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

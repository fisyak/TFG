// $Id: StSsdDbMaker.cxx,v 1.8 2006/09/18 16:40:14 fisyak Exp $
//
// $Log: StSsdDbMaker.cxx,v $
// Revision 1.8  2006/09/18 16:40:14  fisyak
// Add sim flag for ssdWafersPosition
//
// Revision 1.7  2005/06/03 21:30:41  perev
// Move configuration Init()==>InitRun()
//
// Revision 1.6  2005/05/10 12:48:06  reinnart
// The new StSsdDbMaker without DirectDataBase Access
// 

/***************************************************************************
 * Author: christelle roy
 * Description: SSD DB access Maker
 **************************************************************************/

#include "StSsdDbMaker.h"

#include "StChain.h"
#include "TDataSetIter.h"
#include "St_ObjectSet.h"
#include "StMessMgr.h"

#include "StDbLib/StDbManager.hh"                      // Database Libraries
#include "StDbLib/StDbConfigNode.hh"                   //
#include "StDbLib/StDbTable.h"                         //

#include "StSsdDbWriter.hh"
#include "St_SsdDb_Reader.hh"
#include "St_db_Maker/St_db_Maker.h"
#include "StSsdUtil/StSsdGeometry.hh"
#include "StSsdUtil/StSsdEnumerations.hh"
#include "tables/St_ssdWafersPosition_Table.h"
#include "tables/St_ssdConfiguration_Table.h"
#include "tables/St_ssdDimensions_Table.h"
StSsdDbMaker* gStSsdDbMaker=NULL; 
St_ObjectSet *ssdSetConfig;
St_ObjectSet *ssdSetGeom;
St_ObjectSet *wafersPositionSetGeom;
St_ObjectSet *dimensions;

ClassImp(StSsdDbMaker)
//_____________________________________________________________________________
StSsdDbMaker::StSsdDbMaker(const char *name):StMaker(name)
{
  // mTimeStamp    = "2004-07-10 00:00:00";

  m_Reader      = NULL;
  gStSsdDbMaker = this;
  mWriter       = NULL;
}
//_____________________________________________________________________________
StSsdDbMaker::~StSsdDbMaker()
{

 gStSsdDbMaker = NULL; 
 //delete m_Reader;
}

//_____________________________________________________________________________
Int_t StSsdDbMaker::Init()
{
  if (Debug()) gMessMgr->Debug() << "StSsdDbMaker::Init - Start - " << endm;
  if( m_Mode == 1) {
    gMessMgr->Message() << 
      "StSsdDbMaker::Init setting WafersPostions to sim" << endm;
    SetFlavor("sim","ssdWafersPosition");   
  }
  gMessMgr->Info() << "StSsdDbMaker::Init() - Done - "<<endm;
  return StMaker::Init();

}
//_____________________________________________________________________________
Int_t StSsdDbMaker::InitRun(int runumber)
{
static int once = 0;
  if (!once) { //May be this "once" must be removed later (Victor)
    once = 2005;
    setSsdDb_Reader();
    setSsdConfig();
    readSsdConfig();   // read!
    //  setSsdDimensions();
    //  readSsdDimensions();
    setSsdGeometry();
    readSsdGeometry(); // read!
  }
  
  gMessMgr->Info() << "StSsdDbMaker::InitRun" << endm;
  gMessMgr->Info() << "StSsdDbMaker::readSsdGeometry done here " << endm;

  return kStOk;
}

//_____________________________________________________________________________
void StSsdDbMaker::setSsdDb_Reader() // called in INIT  
{
  if (Debug()) gMessMgr->Debug() << "StSsdDbMaker::set_SsdDirectDb_Reader" << endm;

  m_Reader = new St_SsdDb_Reader();
  m_Reader->setDataBase(GetDataBase("Geometry/ssd"),0);


  gMessMgr->Info() << " Finishing setSsdDirectDb_Reader..........."<<endm;
}
//_____________________________________________________________________________
void StSsdDbMaker::setSsdConfig()
{    
  ssdSetConfig = new St_ObjectSet("StSsdConfig");
  AddConst(ssdSetConfig);  
}
//_____________________________________________________________________________
void StSsdDbMaker::readSsdConfig()
{      
  gMessMgr->Info() << " StSsdDbMaker::readSsdConfig - Start - " <<endm;

  TObject* o=0;
  if (m_Reader)
  {
    o = (TObject*)m_Reader->getConfiguration();
  }
  else
    gMessMgr->Error()<<" Pb in StSsdDbMaker::readSsdConfig : m_Reader Not Defined "<<endm;

  if (o != ssdSetConfig->GetObject()) ssdSetConfig->SetObject(o);
  gMessMgr->Info() << " StSsdDbMaker::readSsdConfig - End - " <<endm;
}
//_____________________________________________________________________________
void StSsdDbMaker::setSsdDimensions()
{
  dimensions = new St_ObjectSet("StSsdDimensions");
  AddConst(dimensions);
}
//_____________________________________________________________________________
void StSsdDbMaker::readSsdDimensions()
{
  gMessMgr->Info() <<" SsdDbMaker::readSsdDimensions - Start - "<<endm;

  TObject* o;
  if (m_Reader) 
    o = (TObject*)m_Reader->getDimensions();
  else
    gMessMgr->Error()<<" Pb in StSsdDbMaker::readSsdDimensions : m_Reader Not Defined "<<endm;

  if (o != dimensions->GetObject()) dimensions->SetObject(o);
  gMessMgr->Info() <<" SsdDbMaker::readSsdDimensions - End - "<<endm;
}
//_____________________________________________________________________________
void StSsdDbMaker::setSsdGeometry()
{
  wafersPositionSetGeom = new St_ObjectSet("StSsdGeometry");
  AddConst(wafersPositionSetGeom);
}
//_____________________________________________________________________________
void StSsdDbMaker::readSsdGeometry()
{
  gMessMgr->Info() <<" SsdDbMaker::readSsdDirectGeometry - Start - "<<endm;

  TObject* o;
  if (m_Reader) 
    o = (TObject*)m_Reader->getGeometry();
  else
    gMessMgr->Error()<<" Pb in StSsdDbMaker::readSsdDGeometry : m_Reader Not Defined "<<endm;

  if (o != wafersPositionSetGeom->GetObject()) wafersPositionSetGeom->SetObject(o);

  gMessMgr->Info() <<" SsdDbMaker::readSsdDirectGeometry - End - "<<endm;
}
//_____________________________________________________________________________
Int_t StSsdDbMaker::Make()
{
  if (Debug()) gMessMgr->Debug() << "StSsdDbMaker::Make" << endm;

  return kStOK;
}

//_____________________________________________________________________________
void StSsdDbMaker::Clear(const char*)
{
  if (Debug()) gMessMgr->Debug() << "StSsdDbMaker::Clear" << endm;
  StMaker::Clear();
}

//_____________________________________________________________________________
Int_t StSsdDbMaker::Finish()
{
  if (Debug()) gMessMgr->Debug() << "StSsdDbMaker::Finish" << endm;
  return kStOK;
}

/***************************************************************************
 *
 * $Id: StSvtOnlineSeqAdjSimMaker.cxx,v 1.3 2004/01/27 02:45:10 perev Exp $
 *
 * Author: Petr Chaloupka
 ***************************************************************************
 *
 * Description: Simulates online seqence adjusting
 *
 ***************************************************************************/


#include "StSvtOnlineSeqAdjSimMaker.h"
#include "StSvtClassLibrary/StSvtHybridPixelsC.hh"
#include "StSvtClassLibrary/StSvtHybridPixelsD.hh"
#include "StSvtClassLibrary/StSvtHybridCollection.hh"
#include "StSvtClassLibrary/StSvtData.hh"
#include "StSvtClassLibrary/StSvtHybridBadAnodes.hh"
#include "StSvtClassLibrary/StSvtHybridData.hh"
#include "StSvtClassLibrary/StSvtConfig.hh"
#include "StSequence.hh"
#include "StMessMgr.h"
#include "StSvtConversionTable.h"

ClassImp(StSvtOnlineSeqAdjSimMaker)

StSvtOnlineSeqAdjSimMaker::StSvtOnlineSeqAdjSimMaker(const char* name):StMaker(name)
{
  mRawData=NULL;
  m8bitPixelColl=NULL;
  mPixelColl=NULL;
  mSvtBadAnodes=NULL;

  //here are the default settings, you can override them in Init()
  SetKillBadAnodes(kTRUE);
  SetNumberTBinsToClear(2); 

  //defaults is no raw anodes
  SetSaveAnode2Raw(kFALSE);
  SetSaveAnode239Raw(kFALSE);
  
  //default is no extra anodes
  SetExtraPixelsBefore(1);
  SetExtraPixelsAfter(4);

  SetPedOffset(20);
  //these settings are lower then those usualy used
  Set_thresh_hi(7);
  Set_n_seq_hi(0);

  Set_thresh_lo(2);  
  Set_n_seq_lo(2);

  //This is because of some Makers downd the chain
  GetConfig();
  SetRawData(); 
}

//____________________________________________________________________________
StSvtOnlineSeqAdjSimMaker::~StSvtOnlineSeqAdjSimMaker()
{
}

//____________________________________________________________________________
Int_t StSvtOnlineSeqAdjSimMaker::GetConfig()
{
  mConfig=NULL;
  St_DataSet *dataSet = NULL;
  dataSet = GetDataSet("StSvtConfig");
  
  if (!dataSet)
    {
      gMessMgr->Warning() << " No SvtConfig  data set" << endm;
      dataSet = new St_ObjectSet("StSvtConfig");                                                               
      AddConst(dataSet);
      mConfig=NULL;
    }
  
  mConfig=((StSvtConfig*)(dataSet->GetObject()));
 
  if (!mConfig) {
    gMessMgr->Warning() << "SvtConfig data set is empty- seting default full configuration" << endm;
    mConfig=new StSvtConfig();
    mConfig->setConfiguration("FULL");
    dataSet->SetObject(mConfig);
  }
  
  return kStOk;
}

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::GetPixelData()
{
  St_DataSet* dataSet=NULL;
  dataSet = GetDataSet("StSvtPixelData");
  if (!dataSet) {
    gMessMgr->Error()<<"no StSvtPixelData dataset"<<endm;
    assert(dataSet);
    }
  mPixelColl=(StSvtData*)dataSet->GetObject();
  if (!mPixelColl){
      gMessMgr->Error()<<"StSvtPixelData is empty"<<endm;
      assert(mPixelColl);
      }
      
  dataSet=NULL;
  
  dataSet = GetDataSet("StSvt8bitPixelData");
  if (!dataSet) {
    gMessMgr->Error()<<"no StSvt8bitPixelData dataset"<<endm;
    assert(dataSet);
    }
  m8bitPixelColl=(StSvtData*)dataSet->GetObject();
  if (!m8bitPixelColl){
      gMessMgr->Error()<<"StSvt8bitPixelData is empty"<<endm;
      assert(m8bitPixelColl);
      }
}  

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::GetBadAnodes()
{
  St_DataSet *dataSet;
  
  dataSet = GetDataSet("StSvtBadAnodes");
  if( !dataSet) {
    gMessMgr->Warning() << "StSvtOnlineSeqAdjSimMaker: No Svt Bad Anodes data set" << endm;
    return;
  }

  mSvtBadAnodes = (StSvtHybridCollection*)(dataSet->GetObject());
  if( !mSvtBadAnodes) {
    gMessMgr->Warning() << "StSvtOnlineSeqAdjSimMaker: No Svt Bad Anodes data " << endm;
    return;
  }

  /*if (Debug())*/  gMessMgr->Info()<<"StSvtOnlineSeqAdjSimMaker:Svt Bad Anode list found"<<endm;
}


//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::SetRawData()
{  //this makes new or replaces raw data pro SvtDaqMaker

  St_ObjectSet* set=(St_ObjectSet*)GetDataSet("StSvtRawData");
  
  if (!set) {
    set = new St_ObjectSet("StSvtRawData");
    AddData(set);
  }
  mRawData = new StSvtData(mConfig->getConfiguration());
  set->SetObject(mRawData);
}

//____________________________________________________________________________
Int_t StSvtOnlineSeqAdjSimMaker::Init()
{
  // these are user settings - they should be read from database - so far here
 /* 
  SetNumberTBinsToClear(4);
  SetExtraPixelsBefore(1);
  SetExtraPixelsAfter(4);
  SetAdjParams(3,2,7,0);
 */
 return  StMaker::Init();
}

//____________________________________________________________________________
Int_t StSvtOnlineSeqAdjSimMaker::InitRun(int runumber)
{
  if (Debug()) gMessMgr->Info()<<"StSvtOnlineSeqAdjSimMaker::InitRun"<<endm;	
  GetConfig();
  GetBadAnodes();
  if (Debug()) gMessMgr->Info()<<"StSvtOnlineSeqAdjSimMaker::InitRun...END"<<endm;	
  return  StMaker::InitRun(runumber);
}

//____________________________________________________________________________
Int_t StSvtOnlineSeqAdjSimMaker::Finish()
{
  return kStOK;
}

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::Clear(const char*)
{
  mRawData=NULL;
  StMaker::Clear();
}

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::SetAdjParams(int thresh_lo,int n_seq_lo,int thresh_hi,int n_seq_hi)
{
m_thresh_lo =thresh_lo;
m_n_seq_lo =n_seq_lo;
m_thresh_hi =thresh_hi;
m_n_seq_hi =n_seq_hi;
}

//____________________________________________________________________________
Int_t  StSvtOnlineSeqAdjSimMaker::Make()
{
  if (Debug()) gMessMgr->Info()<<"StSvtOnlineSeqAdjSimMaker::Make"<<endm;	
  SetRawData();
  GetPixelData();
  
   for(int Barrel = 1;Barrel <= mPixelColl->getNumberOfBarrels();Barrel++) {
     for (int Ladder = 1;Ladder <= mPixelColl->getNumberOfLadders(Barrel);Ladder++) {
       for (int Wafer = 1;Wafer <= mPixelColl->getNumberOfWafers(Barrel);Wafer++) {
         for( int Hybrid = 1;Hybrid <= mPixelColl->getNumberOfHybrids();Hybrid++){
           
           mCurrentIndex = mPixelColl->getHybridIndex(Barrel,Ladder,Wafer,Hybrid);
           if( mCurrentIndex < 0) continue; 
            //cout<<"index:"	<<mCurrentIndex<<endl;
	   
           mCurrentPixelData  = (StSvtHybridPixelsD*)mPixelColl->at(mCurrentIndex);
           mCurrent8bitPixelData = (StSvtHybridPixelsC*)m8bitPixelColl->at(mCurrentIndex);

	   if(!mCurrent8bitPixelData) {
             mCurrent8bitPixelData = new StSvtHybridPixelsC(Barrel, Ladder, Wafer, Hybrid);
             m8bitPixelColl->put_at(mCurrent8bitPixelData,mCurrentIndex);
	   }

           if(!mCurrentPixelData) { //no data from simulation Maker
              mCurrent8bitPixelData->Reset();
           }
	   
	   //No we have the pixel data sructures,turn it now into the real data
           //ie. simulate the DAQ
	   
	   Conversion10to8bit();
	   ClearMask();
	   if (mKillBadAnodes) KillBadAnodes();	 
	   if (mNumberTBinsToClear>0) ClearFirstTbins();
	   RawAnodes();
           SequenceSearch();
	   WriteMask();
	   FillRawData();
	       
         }
       }
     }
   }
   
   if (Debug()) gMessMgr->Info()<<"StSvtOnlineSeqAdjSimMaker::Make...END"<<endm;	
   return kStOK;
}

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::Conversion10to8bit()
{
  double *fromArray=mCurrentPixelData->GetArray();
  Char_t *toArray=mCurrent8bitPixelData->GetArray();

  for (int i=0;i<mCurrentPixelData->getTotalNumberOfPixels();i++)
    {
      double adc=fromArray[i];
      if (adc<=0) adc=0.;
      if (adc>=1023) adc=1023.;
      unsigned int adc1=(unsigned int)adc; //conversion to 10 bits from double - ?is it "compiler" reliable?
      toArray[i]=(Char_t)StSvt10to8ConversionTable[adc1]; //conversion to 8 bits
    }
}

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::FillRawData()
{
  StSvtHybridData *hybridData;
  hybridData = new StSvtHybridData(mCurrentPixelData->getBarrelID(), mCurrentPixelData->getLadderID(), mCurrentPixelData->getWaferID(),mCurrentPixelData->getHybridID());
  mRawData->put_at(hybridData,mCurrentIndex);
  
  int anodes=0;  //number of anodes with some sequences
  
  //raw anodes need to checked if they are not flagged bad
  StSvtHybridBadAnodes*  badAnode =NULL;
  if (mKillBadAnodes && mSvtBadAnodes)  badAnode = (StSvtHybridBadAnodes*)mSvtBadAnodes->at(mCurrentIndex);
  
  Char_t *mAdcArray=mCurrent8bitPixelData->GetArray(); // array of [128*240]  
 
  StSequence tmpSeq[128];  //buffer for sequences on one anode
  for (int ianode=0;ianode<240;ianode++)
    {  
       int seqCount=0; //number of sequences on current anode

      //first check for raw anodes
      if ((ianode==1)&&mSaveAnode2Raw){
        if (badAnode->isBadAnode(2)) continue; //don't write out zeros if bad
	tmpSeq[0].startTimeBin =0;
	tmpSeq[0].firstAdc=(unsigned char*)(mAdcArray+ianode*128);
	tmpSeq[0].length = 128;
	seqCount=1;
	hybridData->setListSequences(anodes,ianode+1, seqCount, tmpSeq);
	anodes++;
	continue;
      }

      if ((ianode==238)&&mSaveAnode239Raw){
        if (badAnode->isBadAnode(239)) continue; //don't write out zeros if bad
	tmpSeq[0].startTimeBin =0;
	tmpSeq[0].firstAdc=(unsigned char*)(mAdcArray+ianode*128);
	tmpSeq[0].length = 128;
	seqCount=1;
	hybridData->setListSequences(anodes,ianode+1, seqCount, tmpSeq);
	anodes++;
	continue;
      }
  
     
      int pixCount=0; ///number of pixels in current sequence
      for(int tim = 0; tim <= 128; tim++)
	{//loop over time bins in one anode
	  unsigned char adc; 
	  if (tim==128)  adc=0; // make an artificial end of time sequence
	  else adc= (unsigned char)mAdcArray[ianode*128 + tim];
	  
	  if (adc>0)
	    {
	      if (pixCount==0){ //starting new sequence
		tmpSeq[seqCount].startTimeBin = tim;
		tmpSeq[seqCount].firstAdc=(unsigned char*)(mAdcArray+ianode*128 + tim);
	      }
	      pixCount++;
	    }
	  else
	    {
	      if(pixCount>0){//end of sequence
		tmpSeq[seqCount].length = pixCount;
		seqCount++;
		pixCount=0;
	      }
	    }
	  
	  
	}
   
      if(seqCount>0){ //save found sequences
	//cout<<"found sequences:"<<seqCount<<endl;
	hybridData->setListSequences(anodes,ianode+1, seqCount, tmpSeq);
	anodes++;
      }
      
      
    }   

  hybridData->setAnodeList();
}

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::WriteSequence(int anode,int begins, int ends, int NumOfHigh)
{
  //check the proper size
  //cout<<"anode:"<<anode<<"start:"<<begins<<"ends:"<<ends<<"hi:"<<NumOfHigh<<endl;
  if (NumOfHigh<=m_n_seq_hi) return;
  if ((ends-begins+1)<=m_n_seq_lo) return;
  
  //extra anodes
  begins= begins-mExtraBefore;
  if (begins<0) begins=0;
  
  ends=ends+mExtraAfter;
  if (ends>127) ends=127;

  for (int i=begins;i<=ends;i++) mMask[anode*128 + i]=kTRUE;
}

//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::SequenceSearch()
{
  unsigned char adc;
  int loCount;
  int hiCount;
  int SeqBegins=0;

  int HiTresh=mPedOffset+m_thresh_hi;
  int LoTresh=mPedOffset+m_thresh_lo;

  
  Char_t *mAdcArray=mCurrent8bitPixelData->GetArray(); // array of [128*240]

  for(int an = 0; an < 240; an++){
    //get ready for new anode
    loCount=0;hiCount=0; //just for safety

    for(int tim = 0; tim <= 128; tim++)
      {//loop over time bins in one anode
	
	if (tim==128)  adc=0; // make an artificial end of time sequence
	else adc=(unsigned char)mAdcArray[an*128 + tim];
	
	if (adc>HiTresh) hiCount++;

	if (adc>LoTresh)
	  { //inside of sequence or at the beginning
	    if (loCount==0) SeqBegins=tim; //it is the beginning of the sequence
	    loCount++;  
	  }
	else
	  { //ouside or at the end of the sequence
	    if(loCount!=0) //end of sequence
	      {	
	        WriteSequence(an,SeqBegins,tim-1,hiCount);
		loCount=0;hiCount=0;	
	      }
	  }
	
	
      }
  }

}



//____________________________________________________________________________
void  StSvtOnlineSeqAdjSimMaker::KillBadAnodes()
{

  if (!mSvtBadAnodes){
    cout<<"Warning: cannot simulate bad anodes in online sequence adjusting - no anode list"<<endl;
    return;
  }
    StSvtHybridBadAnodes*  BadAnode = (StSvtHybridBadAnodes*)mSvtBadAnodes->at(mCurrentIndex);
  if (!BadAnode) return;
  
  Char_t *mAdcArray=mCurrent8bitPixelData->GetArray(); // array of [128*240]  
  
  for (int an=0;an<240;an++)
    if (BadAnode->isBadAnode(an+1))
      { //now I've found bad anode and I'm going to delete it
	for(int tim = 0; tim < 128; tim++) mAdcArray[an*128 + tim]=0;
      }
  
}


//____________________________________________________________________________
void StSvtOnlineSeqAdjSimMaker::RawAnodes()
{
  int anode;
  int tb;

  if(mSaveAnode2Raw)
    {
      anode=1;
      for(tb = 0; tb < 128; tb++) mMask[anode*128 + tb]=kTRUE;
    }

 if(mSaveAnode239Raw)
    {
      anode=238;
      for(tb = 0; tb < 128; tb++) mMask[anode*128 + tb]=kTRUE;
    } 

}

//____________________________________________________________________________
void  StSvtOnlineSeqAdjSimMaker::ClearMask()
{
  for (int  i=0;i<128*240;i++)mMask[i]=kFALSE;
}

//____________________________________________________________________________
void  StSvtOnlineSeqAdjSimMaker::ClearFirstTbins()
{
 Char_t *mAdcArray=mCurrent8bitPixelData->GetArray(); // array of [128*240]
  
 for(int tim = 0; tim < mNumberTBinsToClear; tim++){
     for(int an = 0; an < 240; an++){
       mAdcArray[an*128 + tim]=0;
	  
    }
  }
}

//____________________________________________________________________________
void  StSvtOnlineSeqAdjSimMaker::WriteMask()
{
  Char_t *mAdcArray=mCurrent8bitPixelData->GetArray(); // array of [128*240]
  
  for(int tim = 0; tim < 128; tim++){
    for(int an = 0; an < 240; an++){
      if (mMask[an*128 + tim]==kFALSE)	mAdcArray[an*128 + tim]=0;
    }
  }
}



/***************************************************************************
 *
 * $Id: StSvtEmbeddingMaker.cxx,v 1.4 2004/01/22 16:30:47 caines Exp $
 *
 * Author: Selemon Bekele
 ***************************************************************************
 *
 * Description: Svt Embedding Maker class
 *
 ***************************************************************************
 *
 * $Log: StSvtEmbeddingMaker.cxx,v $
 * Revision 1.4  2004/01/22 16:30:47  caines
 * Getting closer to a final simulation
 *
 * Revision 1.3  2003/11/30 20:51:46  caines
 * New version of embedding maker and make OnlSeqAdj a stand alone maker
 *
 * Revision 1.2  2003/09/07 03:49:06  perev
 * gcc 3.2 + WarnOff
 *
 * Revision 1.1  2003/07/31 19:18:09  caines
 * Petrs improved simulation code
 *
 **************************************************************************/

#include "Stiostream.h"
#include <math.h>

#include "TH2.h"
#include "TFile.h"

#include "StChain.h"
#include "St_DataSetIter.h"
#include "St_ObjectSet.h"
#include "StSequence.hh"
#include "StSvtEmbeddingMaker.h"
#include "StMessMgr.h"

#include "StSvtClassLibrary/StSvtHybridCollection.hh"
#include "StSvtClassLibrary/StSvtHybridData.hh"
#include "StSvtClassLibrary/StSvtData.hh"
#include "StSvtClassLibrary/StSvtHybridPixelsD.hh"
#include "StSvtClassLibrary/StSvtHybridPixels.hh"
#include "StSvtClassLibrary/StSvtConfig.hh"
#include "StSvtClassLibrary/StSvtHybridPed.hh"

ClassImp(StSvtEmbeddingMaker)

//____________________________________________________________________________
StSvtEmbeddingMaker::StSvtEmbeddingMaker(const char *name):StMaker(name)
{
  mDoEmbedding=kTRUE;     
  setBackGround();        //sets to default true and sigma 1.8
 
  mSimPixelColl = NULL;
  mRealDataColl = NULL;
  mPedColl = NULL;
  mPedRMSColl = NULL;
}

//____________________________________________________________________________
StSvtEmbeddingMaker::~StSvtEmbeddingMaker()
{

}

//____________________________________________________________________________
Int_t StSvtEmbeddingMaker::Init()
{
  //bookHistograms();

  return StMaker::Init();
}

//____________________________________________________________________________
Int_t StSvtEmbeddingMaker::InitRun(int runumber)
{
  GetPedRMS();
  //bookHistograms();

  return StMaker::InitRun(runumber);
}

//____________________________________________________________________________
Int_t StSvtEmbeddingMaker::Make()
{
  GetSvtData();
  ClearMask(); //it has to be cleared here - needed for plain simulation

  for(int Barrel = 1;Barrel <= mSimPixelColl->getNumberOfBarrels();Barrel++) {
    for (int Ladder = 1;Ladder <= mSimPixelColl->getNumberOfLadders(Barrel);Ladder++) {
      for (int Wafer = 1;Wafer <= mSimPixelColl->getNumberOfWafers(Barrel);Wafer++) {
	for( int Hybrid = 1;Hybrid <= mSimPixelColl->getNumberOfHybrids();Hybrid++){
	  
	  mCurrentIndex = mSimPixelColl->getHybridIndex(Barrel,Ladder,Wafer,Hybrid);
	  if( mCurrentIndex < 0) continue; 
	  
	  mCurrentPixelData  = (StSvtHybridPixelsD*)mSimPixelColl->at(mCurrentIndex);
	    
	  if(!mCurrentPixelData) { //no data from simulation Maker
	    gMessMgr->Info()<<"Error  in StSvtEmbeddingMaker::Make(): Something is wrong, no data from simulator for hybrid index:"<<mCurrentIndex<<endm;
	    mCurrentPixelData = new StSvtHybridPixelsD(Barrel, Ladder, Wafer, Hybrid);
	    mSimPixelColl->put_at(mCurrentPixelData,mCurrentIndex);
           }
	   
	   if (mRealDataColl){  //now decide if it's true embedding into raw data or just simple background
	    ClearMask();
	    AddRawData();
	   }
	   
           if (mBackGrOption) CreateBackground();
	}
      }
    }
  }
 
  //  fillHistograms();
  return kStOK;
}

//____________________________________________________________________________
void  StSvtEmbeddingMaker::GetPedRMS()
{
  mPedRMSColl=NULL;
  mPedColl=NULL;

  St_DataSet* dataSet=NULL;
  dataSet = GetDataSet("StSvtRMSPedestal");
  if (dataSet)  mPedRMSColl= (StSvtHybridCollection*)dataSet->GetObject();
  if (mPedRMSColl) cout<<"StSvtEmbeddingMaker: Found RMS values for individual pixels."<<endl;
    else cout<<"StSvtEmbeddingMaker: NO RMS values for individual pixels."<<endl;
      
  dataSet=NULL;
  dataSet = GetDataSet("StSvtPedestal");
  if (dataSet) mPedColl= (StSvtHybridCollection*)dataSet->GetObject();
  if (mPedColl) cout<<"StSvtEmbeddingMaker: Found RMS values for individual hybrids."<<endl;
    else cout<<"StSvtEmbeddingMaker: NO RMS values for individual hybrids."<<endl;

  if ((!mPedRMSColl)&&(!mPedColl))
    cout<<"Warning: no SVT RMS information available from Chain - using default backgroung:"<<mBackGSigma<<endl;
}


//____________________________________________________________________________
void StSvtEmbeddingMaker::GetSvtData()
{
  //EmbeddingMaker requires some data(at least empty) from the SimulationMaker
  St_DataSet* dataSet = GetDataSet("StSvtPixelData");
  assert(dataSet); 
  mSimPixelColl= (StSvtData*)(dataSet->GetObject());
  assert(mSimPixelColl);
  
  mRealDataColl=NULL;
  if (!mDoEmbedding) return; //in case it's forbiden to embed
  dataSet = GetDataSet("StSvtRawData");
  if (dataSet) mRealDataColl= (StSvtData*)(dataSet->GetObject());
  if (!mRealDataColl)      //switching to plain simulation, because there is no raw data
     gMessMgr->Info()<<"Note: StSvtEmbeddingMaker is set to do embbeding, but found no raw data -SWITCHING TO PLAIN SIMULATION"<<endm;
}

//____________________________________________________________________________
void StSvtEmbeddingMaker::AddRawData()
{ //mixes raw data into pixel data nad set mask for background
  
  StSequence* Sequence;
  
  int numOfSeq;
  int* anolist;
  
  StSvtHybridData* realData = (StSvtHybridData *)mRealDataColl->at(mCurrentIndex); 
    
  double *adcArray=mCurrentPixelData->GetArray();      
  double offset =  mCurrentPixelData->getPedOffset(); //this could be problem if offset differs between real and simulated data!!!

  anolist = NULL;
  if (realData)
  for (int iAnode= 0; iAnode<realData->getAnodeList(anolist); iAnode++){
              
    int Anode = anolist[iAnode]; //goes from 1
    realData->getSequences(Anode,numOfSeq,Sequence);
    
    for (int nSeq=0; nSeq< numOfSeq ; nSeq++){ 
      unsigned char* adc=Sequence[nSeq].firstAdc;
      int length = Sequence[nSeq].length;
      int startTimeBin=Sequence[nSeq].startTimeBin;
      
      for(int t = 0; t<length; t++){
	int time = startTimeBin + t;
	unsigned char adcVal = adc[t];
	//pixelIndex = mSimPixelData->getPixelIndex(Anode,time);
	//less clean but faster
	int pixelIndex=(Anode-1)*128+time;
	//there already is pedestal offset
	adcArray[pixelIndex]=adcArray[pixelIndex]+(double)adcVal-offset;

	//don't do noise here
	mMask[pixelIndex]=kFALSE;
      }
    }
  }
  
  //now clear rest of the mask
  for(int an = 0; an < 240; an++){
      for(int tim = 0; tim < 128; tim++){
	int pIndex=an*128 + tim;
	if (adcArray[pIndex]==offset) mMask[pIndex]=kFALSE; //don't make extra noise outside of hits
      }
  }
  
}

//____________________________________________________________________________
double StSvtEmbeddingMaker::MakeGaussDev(double sigma)
{

 static int iset = 0;
 static double gset;
 double fac,rsq,v1,v2;

 //if(*idum < 0) iset = 0;
 if(iset == 0)
   {
     do {
       v1 = 2.0*((float)rand()/(float)RAND_MAX) - 1.0;
       v2 = 2.0*((float)rand()/(float)RAND_MAX) - 1.0;
       rsq = v1*v1 + v2*v2;
       
     } while(rsq >= 1.0 || rsq == 0.0);
     
     fac = sigma*::sqrt(-2.0*::log(rsq)/rsq);
     
     gset = v1*fac;  // gset = 3.0*::sqrt(-2.0*::log(rsq))*(v1/::sqrt(rsq))
     iset = 1;
     return v2*fac;
   }
 else
   {
     iset = 0;
     return gset;
   }
}

//____________________________________________________________________________
void StSvtEmbeddingMaker::CreateBackground()
{
  const double rmsScale=16.;
  double *adcArray=mCurrentPixelData->GetArray(); // array of [128*240]
  
  //find out what background to use
  StSvtHybridPixels* pedRms=NULL;
  if (mPedRMSColl)
     { 
     pedRms = (StSvtHybridPixels*)mPedRMSColl->at(mCurrentIndex);
     if (pedRms == NULL) cout<<"Warning: Individual pixel RMS info is empty for hybrid "<<mCurrentIndex<<" =>have to use other method "<<endl;
  }
  
  if(pedRms)
    {// I have rms for each pixel
      for(int an = 0; an < 240; an++)  for(int tim = 0; tim < 128; tim++){
	//cout<<pedRms<<"indiv rms="<<pedRms->At(pedRms->getPixelIndex(an+1,tim))/rmsScale<<endl;
	int pIndex=an*128 + tim;
	if (mMask[pIndex]) adcArray[pIndex]+=MakeGaussDev(pedRms->At(pedRms->getPixelIndex(an+1,tim))/rmsScale);// !! mAdcArray already contains PedOffset
      }
    }
  else {
    //one value for hybrid
    double backgsigma;
    StSvtHybridPed *ped=NULL;
    if (mPedColl){ 
	ped=(StSvtHybridPed *)mPedColl->at(mCurrentIndex);
	if (ped == NULL) cout<<"Warning: hybrid  RMS info is empty for hybrid "<<mCurrentIndex<<" =>using default value "<<mBackGSigma<<endl;
	}
    if (ped) backgsigma=ped->getRMS(); else  backgsigma=mBackGSigma; //the default value
    if ((backgsigma<=0.)||(backgsigma>=6.)){ //check for obviously bad values 
      cout<<"Warnig for index "<<mCurrentIndex<<" pedestal RMS is:"<<backgsigma<<" => seting to default "<<mBackGSigma<<endl;
      backgsigma=mBackGSigma;
    }
    if (Debug()) cout<<"for index "<<mCurrentIndex<<" pedestal RMS is:"<< backgsigma<<endl;
    //t->Fill(backgsigma);
    for(int an = 0; an < 240; an++){
      for(int tim = 0; tim < 128; tim++){
	int pIndex=an*128 + tim;
	if (mMask[pIndex]) adcArray[pIndex]+=MakeGaussDev(backgsigma);// !! mAdcArray already contains PedOffset             
      }
    }
  }
}

//____________________________________________________________________________
Int_t StSvtEmbeddingMaker::Finish()
{ 
  
  //  mFile->Write();
  //mFile->Close();

  return kStOK;
}

//____________________________________________________________________________
void  StSvtEmbeddingMaker::setDoEmbedding(Bool_t doIt){
  mDoEmbedding = doIt;
}

//____________________________________________________________________________
void  StSvtEmbeddingMaker::setBackGround(Bool_t backgr,double backgSigma){
  mBackGrOption = backgr;
  mBackGSigma = backgSigma;
}

//____________________________________________________________________________
void  StSvtEmbeddingMaker::ClearMask()
{
  for (int  i=0;i<128*240;i++)mMask[i]=kTRUE;
}

//____________________________________________________________________________
void StSvtEmbeddingMaker::bookHistograms()
{ 
  mFile = new TFile("embedding.root","RECREATE","Embedding");
  
  char index[10];
  char preTitle[25]; 
  char* title; 
  int indexHyb;

  mDataHist = new TH2F*[mRealDataColl->getTotalNumberOfHybrids()];
  
  for(int Barrel = 1;Barrel <= mRealDataColl->getNumberOfBarrels();Barrel++) {    
    for (int Ladder = 1;Ladder <= mRealDataColl->getNumberOfLadders(Barrel);Ladder++) {      
      for (int Wafer = 1;Wafer <= mRealDataColl->getNumberOfWafers(Barrel);Wafer++) {	
	for( int Hybrid = 1;Hybrid <= mRealDataColl->getNumberOfHybrids();Hybrid++){
	  {
	    indexHyb = mRealDataColl->getHybridIndex(Barrel,Ladder,Wafer,Hybrid);
	    if( indexHyb < 0) continue; 
	 
	    sprintf(preTitle,"embeddedData");
	    sprintf(index,"b%dl%dw%dh%d",Barrel, Ladder, Wafer, Hybrid);
      
	    title = strcat(preTitle,index);
	    
	    mDataHist[indexHyb] = new TH2F(title,"ADC vs time and anode",240,0.5,240.5,128,0.0,127.0);
	  }
	}
      }
    }
  }
}

/* not implemented im this version /____________________________________________________________________________
void StSvtEmbeddingMaker::fillHistograms()
{
  
  int* anolist; 
  int mSequence,index;
  int stTimeBin,len,status;
  unsigned char* adc;

 StSequence* svtSequence;

 for(int Barrel = 1;Barrel <= mRealDataColl->getNumberOfBarrels();Barrel++) {    
   for (int Ladder = 1;Ladder <= mRealDataColl->getNumberOfLadders(Barrel);Ladder++) {      
     for (int Wafer = 1;Wafer <= mRealDataColl->getNumberOfWafers(Barrel);Wafer++) {	
       for( int Hybrid = 1;Hybrid <= mRealDataColl->getNumberOfHybrids();Hybrid++){
	 
	 index = mRealDataColl->getHybridIndex(Barrel,Ladder,Wafer,Hybrid);
	 
	 if( index < 0) continue; 
	 
	 mSvtEmbeddedData = (StSvtHybridData *)mRealDataColl->at(index);
	 if(!mSvtEmbeddedData)
	   continue;
	 
	 mDataHist[index]->Reset();
	 for(int ianode = 0; ianode < mSvtEmbeddedData->getAnodeList(anolist); ianode++)
	   {
	     int anode = anolist[ianode];
	     status = mSvtEmbeddedData->getSequences(anode,mSequence,svtSequence);
	     
	     for(int mSeq = 0; mSeq < mSequence; mSeq++) 
	       {
		 stTimeBin = svtSequence[mSeq].startTimeBin; 
		 len = svtSequence[mSeq].length;
		 adc = svtSequence[mSeq].firstAdc;
		 for(int j = 0 ; j < len; j++)
		   {
		     float c = (float) adc[j];
		     
		     mDataHist[index]->Fill(anode,stTimeBin + j,c);
		   }
	       }
	   }
       }
     }
   }
 }
}
*/

//////////////////////////////////////////////////////////////////////
//
// $Id: StFlowTagMaker.cxx,v 1.4 1999/11/11 23:12:59 posk Exp $
//
// Author: Raimond Snellings and Art Poskanzer, LBNL, Jun 1999
// Description:  Maker to fill the Flow EbyE Tag database
//
//////////////////////////////////////////////////////////////////////
//
// $Log: StFlowTagMaker.cxx,v $
// Revision 1.4  1999/11/11 23:12:59  posk
// Rearrangement of files.
//
// Revision 1.3  1999/08/09 21:43:05  snelling
// removed parameters from cxx file
//
// Revision 1.2  1999/07/26 23:39:34  snelling
// added histograms for FlowTag QA
//
// FlowTagMaker: fills FlowTags defined by EbE workgroup
//
//  
//////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include "StFlowTagMaker.hh"
//#include "StMaker.h"
#include "../StFlowMaker/StFlowMaker.hh"
#include "../StFlowMaker/StFlowEvent.hh"
//#include "../StFlowMaker/StFlowTrackCollection.hh"
//#include "../StFlowMaker/StFlowCutEvent.hh"
#include "../StFlowMaker/StFlowCutTrack.hh"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
//#include "StThreeVector.hh"
//#include "StLorentzVector.hh"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#define PR(x) cout << (#x) << " = " << (x) << endl;

ClassImp(StFlowTagMaker)

//-------------------------------------------------------------

StFlowTagMaker::StFlowTagMaker(const Char_t* name) : StFlowMaker(name)
{
}

//-------------------------------------------------------------

StFlowTagMaker::~StFlowTagMaker() 
{
  //delete mFlowTag; //clean up
}

//-------------------------------------------------------------

Int_t StFlowTagMaker::Make() 
{
  StFlowMaker* flowMaker = (StFlowMaker*)GetMaker("Flow");
  mFlowTag = flowMaker->TagPointer();
  mFlowEvent = flowMaker->FlowEventPointer();
  // fill the Flow Tag 
  if (mFlowEvent && mFlowTag) {
    fillFlowTag();    // fill the tag database
  } else {
    return kStOK;     // no StFlowEvent or no Tag pointer
  }

  // fill histograms from Flow Tag
  makeHistograms();

  // print pointer to flowtag 
  cout << "TagMaker TagPointer: " << TagPointer() << endl;

  return kStOK;
}

//-------------------------------------------------------------

void StFlowTagMaker::PrintInfo() 
{
  cout << "$Id: StFlowTagMaker.cxx,v 1.4 1999/11/11 23:12:59 posk Exp $" << endl;
  //if (Debug()) StMaker::PrintInfo();
}

//-------------------------------------------------------------

void StFlowTagMaker::printTag(ostream& os) 
{
  os << "--- Event-by-Event Flow Tag Table ---" << endl; 
  if (!mFlowTag) 
    os << "(empty FlowTag)" << endl;
  else {
    int i;
    os <<  "qxa";
    for(i=0; i<4; i++) os << "[" << i << "] =" << mFlowTag->qxa[i] << ' ';
    os << endl;
  }
}

//-------------------------------------------------------------

Int_t StFlowTagMaker::Finish() {
  StFlowCutTrack::PrintCutList();
  return kStOK;
}

//-------------------------------------------------------------

Int_t StFlowTagMaker::Init()
{

  const Float_t PhiMin   = -pi;
  const Float_t PhiMax   =  pi; 
  const Float_t EtaMin   = -6.;
  const Float_t EtaMax   =  6.;
  const Float_t PtMin    =  0.;
  const Float_t PtMax    =  4.;
  const Float_t PsiMin   =  0.;
  const Float_t PsiMax   =  twopi; 
  const Float_t SumPtMin =  0.;
  const Float_t SumPtMax =  4.;
  const Float_t MultMin  =  0.;
  const Float_t MultMax  =  10000.;
  const Float_t qMin     =  0.;
  const Float_t qMax     =  2.;

  for (int i = 0; i < nSubEvents; i++) {
    TString *mHistTitle;
    char mCountSubEvents[5];
    sprintf(mCountSubEvents,"%d",i);

    mHistTitle = new TString("HistPhiSubevent");
    mHistTitle->Append(*mCountSubEvents);
    histSubEvents[i].mHistPhi =
      new TH1F(mHistTitle->Data(), mHistTitle->Data(), nPhiBins, PhiMin, PhiMax);
    histSubEvents[i].mHistPhi->SetXTitle("Phi Angle (radians)");
    histSubEvents[i].mHistPhi->SetYTitle("Counts");
    delete mHistTitle;

    mHistTitle = new TString("HistPseudoRapiditySubevent");
    mHistTitle->Append(*mCountSubEvents);
    histSubEvents[i].mHistPseudoRapidity =
      new TH1F(mHistTitle->Data(), mHistTitle->Data(), nEtaBins, EtaMin, EtaMax);
    histSubEvents[i].mHistPseudoRapidity->SetXTitle("Pseudo Rapidity");
    histSubEvents[i].mHistPseudoRapidity->SetYTitle("Counts");
    delete mHistTitle;

    mHistTitle = new TString("HistPtSubevent");
    mHistTitle->Append(*mCountSubEvents);
    histSubEvents[i].mHistPt =
      new TH1F(mHistTitle->Data(), mHistTitle->Data(), nPtBins, PtMin, PtMax);
    histSubEvents[i].mHistPt->SetXTitle("Pt (GeV)");
    histSubEvents[i].mHistPt->SetYTitle("Counts");
    delete mHistTitle;

    mHistTitle = new TString("ProfHistResolution");
    mHistTitle->Append(*mCountSubEvents);
    histSubEvents[i].mHistResolution =
      new TProfile(mHistTitle->Data(), mHistTitle->Data(), nHarmonics, 1., 
		   (float)(nHarmonics + 1), -10., 10., "");
    histSubEvents[i].mHistResolution->SetXTitle("Harmonic");
    histSubEvents[i].mHistResolution->SetYTitle("Resolution");
    delete mHistTitle;

    for (int j = 0; j < nHarmonics; j++) {
      char mCountHarmonics[5];
      sprintf(mCountHarmonics,"%d",j);

      mHistTitle = new TString("HistPsiSubevent");
      mHistTitle->Append(*mCountSubEvents);
      mHistTitle->Append("Harmonic");
      mHistTitle->Append(*mCountHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHistPsi =
	new TH1F(mHistTitle->Data(), mHistTitle->Data(), nPsiBins,PsiMin,
		 (PsiMax / (float)(j + 1)));
      histSubEvents[i].histHarmonics[j].mHistPsi->SetXTitle("Psi");
      histSubEvents[i].histHarmonics[j].mHistPsi->SetYTitle("Counts");
      delete mHistTitle;

      mHistTitle = new TString("HistFlowTagSumPt");
      mHistTitle->Append(*mCountSubEvents);
      mHistTitle->Append("Harmonic");
      mHistTitle->Append(*mCountHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHistFlowTagSumPt =
	new TH1F(mHistTitle->Data(), mHistTitle->Data(), nSumPtBins,SumPtMin,SumPtMax);
      histSubEvents[i].histHarmonics[j].mHistFlowTagSumPt->SetXTitle("Sum Pt / Multiplicity");
      histSubEvents[i].histHarmonics[j].mHistFlowTagSumPt->SetYTitle("Counts");
      delete mHistTitle;

      mHistTitle = new TString("HistFlowTagMult");
      mHistTitle->Append(*mCountSubEvents);
      mHistTitle->Append("Harmonic");
      mHistTitle->Append(*mCountHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHistFlowTagMult =
	new TH1D(mHistTitle->Data(), mHistTitle->Data(), nMultBins,MultMin,MultMax);
      histSubEvents[i].histHarmonics[j].mHistFlowTagMult->SetXTitle("Multiplicity");
      histSubEvents[i].histHarmonics[j].mHistFlowTagMult->SetYTitle("Counts");
      delete mHistTitle;

      mHistTitle = new TString("Hist_q");
      mHistTitle->Append(*mCountSubEvents);
      mHistTitle->Append("Harmonic");
      mHistTitle->Append(*mCountHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHist_q =
	new TH1F(mHistTitle->Data(), mHistTitle->Data(), n_qBins,qMin,qMax);
      histSubEvents[i].histHarmonics[j].mHist_q->SetXTitle("q (|Q|/sqrt N)");
      histSubEvents[i].histHarmonics[j].mHist_q->SetYTitle("Counts");
      delete mHistTitle;

    }
  }

  return kStOK;
}

//-------------------------------------------------------------

Int_t StFlowTagMaker::makeHistograms()
{
  for (int j = 0; j < nHarmonics; j++) {

    histSubEvents[0].histHarmonics[j].mHistFlowTagSumPt->
      Fill(mFlowTag->spta[j] / (float)mFlowTag->na[j]);
    histSubEvents[0].histHarmonics[j].mHistFlowTagMult->Fill(mFlowTag->na[j]);
    histSubEvents[0].histHarmonics[j].mHist_q->
      Fill(sqrt(mFlowTag->qxa[j]*mFlowTag->qxa[j] + mFlowTag->qya[j]*mFlowTag->qya[j])
	   / sqrt((float)mFlowTag->na[j]));
    histSubEvents[1].histHarmonics[j].mHistFlowTagSumPt->
      Fill(mFlowTag->sptb[j] / (float)mFlowTag->nb[j]);
    histSubEvents[1].histHarmonics[j].mHistFlowTagMult->Fill(mFlowTag->nb[j]);
    histSubEvents[1].histHarmonics[j].mHist_q->
      Fill(sqrt(mFlowTag->qxb[j]*mFlowTag->qxb[j] + mFlowTag->qyb[j]*mFlowTag->qyb[j])
	   / sqrt((float)mFlowTag->nb[j]));
    histSubEvents[2].histHarmonics[j].mHistFlowTagSumPt->
      Fill(mFlowTag->sptc[j] / (float)mFlowTag->nc[j]);
    histSubEvents[2].histHarmonics[j].mHistFlowTagMult->Fill(mFlowTag->nc[j]);
    histSubEvents[2].histHarmonics[j].mHist_q->
      Fill(sqrt(mFlowTag->qxc[j]*mFlowTag->qxc[j] + mFlowTag->qyc[j]*mFlowTag->qyc[j])
	   / sqrt((float)mFlowTag->nc[j]));
    histSubEvents[3].histHarmonics[j].mHistFlowTagSumPt->
      Fill(mFlowTag->sptd[j] / (float)mFlowTag->nd[j]);
    histSubEvents[3].histHarmonics[j].mHistFlowTagMult->Fill(mFlowTag->nd[j]);
    histSubEvents[3].histHarmonics[j].mHist_q->
      Fill(sqrt(mFlowTag->qxd[j]*mFlowTag->qxd[j] + mFlowTag->qyd[j]*mFlowTag->qyd[j])
	   / sqrt((float)mFlowTag->nd[j]));

    // calculate the resolution (sqrt of hist still has to be implemented also *sqrt 2)
    Float_t EventPlaneAngle1 = atan2(mFlowTag->qya[j], mFlowTag->qxa[j]) / (float)(j+1);
    if (EventPlaneAngle1 < 0.) {
      EventPlaneAngle1 += twopi / (float)(j+1);
    }
    Float_t EventPlaneAngle2 = atan2(mFlowTag->qyb[j], mFlowTag->qxb[j]) / (float)(j+1);
    if (EventPlaneAngle2 < 0.) {
      EventPlaneAngle2 += twopi / (float)(j+1);
    }
    histSubEvents[0].mHistResolution->
      Fill((float)(j+1), (float) cos((float)(j+1) * (EventPlaneAngle1 - EventPlaneAngle2)));
    histSubEvents[1].mHistResolution->
      Fill((float)(j+1), (float) cos((float)(j+1) * (EventPlaneAngle2 - EventPlaneAngle1)));
    
    Float_t EventPlaneAngle3 = atan2(mFlowTag->qyc[j], mFlowTag->qxc[j]) / (float)(j+1);
    if (EventPlaneAngle3 < 0.) {
      EventPlaneAngle3 += twopi / (float)(j+1);
    }
    Float_t EventPlaneAngle4 = atan2(mFlowTag->qyd[j], mFlowTag->qxd[j]) / (float)(j+1);
    if (EventPlaneAngle4 < 0.) {
      EventPlaneAngle4 += twopi / (float)(j+1);
    }
    histSubEvents[2].mHistResolution->
      Fill((float)(j+1), (float) cos((float)(j+1) * (EventPlaneAngle3 - EventPlaneAngle4)));
    histSubEvents[3].mHistResolution->
      Fill((float)(j+1), (float) cos((float)(j+1) * (EventPlaneAngle4 - EventPlaneAngle3)));
    
  }

  return kStOK;
}

//-------------------------------------------------------------

void StFlowTagMaker::fillFlowTag() 
{  
  // Initialize Iterator, loop variables
  StFlowTrackCollection* tracks = mFlowEvent->TrackCollection();
  StFlowTrackIterator    itr;
  Int_t TrackCount = tracks->size();

  // Make random subevents routine looks translated from Fortran so 1..n
  Float_t *mRandomVector = new Float_t[TrackCount + 1];
  long  *mIndexVector = new long[TrackCount + 1];
  int i;
  // Fill array with random numbers
  for (i = 0; i <= TrackCount ; i++) 
    {mRandomVector[i] = rand()/(float)RAND_MAX;}
  // get an Index Vector which sorts the array  
  indexx(TrackCount, mRandomVector, mIndexVector);


  // Make subevent arrays
  long TrackCountSub1 = (long) ceil(TrackCount/2.);
  Float_t *mPhiAngleSub1 = new Float_t[TrackCountSub1];
  Float_t *mPseudoRapiditySub1 = new Float_t[TrackCountSub1];

  Float_t *mPtSub1 = new Float_t[TrackCountSub1];
  long TrackCountSub2 = (long) floor(TrackCount/2.);
  Float_t *mPhiAngleSub2 = new Float_t[TrackCountSub2];
  Float_t *mPseudoRapiditySub2 = new Float_t[TrackCountSub2];
  Float_t *mPtSub2 = new Float_t[TrackCountSub2];

  long Sub1Counter   =0;
  long Sub2Counter   =0;
  Double_t SumPtSub1 =0.;
  Double_t SumPtSub2 =0.;

  // Fill the ararys and histograms without track selection
  for (itr = tracks->begin(), i=0; itr != tracks->end(); itr++, i++) {
    StFlowTrack* mTrack = *itr;
    if (mIndexVector[i + 1] <= TrackCountSub1) {
      mPhiAngleSub1[Sub1Counter] = mTrack->Phi();
      histSubEvents[0].mHistPhi->Fill(mPhiAngleSub1[Sub1Counter]);
      mPseudoRapiditySub1[Sub1Counter] = mTrack->Eta();
      histSubEvents[0].mHistPseudoRapidity->Fill(mPseudoRapiditySub1[Sub1Counter]);
      mPtSub1[Sub1Counter] = mTrack->Pt();
      histSubEvents[0].mHistPt->Fill(mPtSub1[Sub1Counter]);
      SumPtSub1 += mTrack->Pt();
      Sub1Counter++;
    }
    else {
      mPhiAngleSub2[Sub2Counter] = mTrack->Phi();
      histSubEvents[1].mHistPhi->Fill(mPhiAngleSub2[Sub2Counter]);
      mPseudoRapiditySub2[Sub2Counter] = mTrack->Eta();
      histSubEvents[1].mHistPseudoRapidity->Fill(mPseudoRapiditySub2[Sub2Counter]);
      mPtSub2[Sub2Counter] = mTrack->Pt();
      histSubEvents[1].mHistPt->Fill(mPtSub2[Sub2Counter]);
      SumPtSub2 += mTrack->Pt();
      Sub2Counter++;
    }
  }

//   cout << "TrackCountSub1, Sub1Counter, TrackCountSub2, Sub2Counter" << endl;
//   cout << TrackCountSub1 << " " << Sub1Counter << " " 
//        << TrackCountSub2 << " " << Sub2Counter << endl; 


  for (i = 0; i < 5 ; i++) {
    Double_t Qx, Qy, EventPlaneAngle;

    // calculate plane and Q vectors first subevent
    eventPlane(TrackCountSub1, mPseudoRapiditySub1, mPhiAngleSub1, 
	       &Qx, &Qy, &EventPlaneAngle, i+1, 0);
    histSubEvents[0].histHarmonics[i].mHistPsi->Fill(EventPlaneAngle);
    // fill tags
    mFlowTag->qxa[i] = Qx;
    mFlowTag->qya[i] = Qy;
    mFlowTag->na[i] = TrackCountSub1;
    mFlowTag->spta[i] = SumPtSub1;

    // calculate plane and Q vectors second subevent
    eventPlane(TrackCountSub2, mPseudoRapiditySub2, mPhiAngleSub2, 
	       &Qx, &Qy, &EventPlaneAngle, i+1, 1);
    histSubEvents[1].histHarmonics[i].mHistPsi->Fill(EventPlaneAngle);
    // fill tags
    mFlowTag->qxb[i] = Qx;
    mFlowTag->qyb[i] = Qy;
    mFlowTag->nb[i] = TrackCountSub2;
    mFlowTag->sptb[i] = SumPtSub2;

    // calculate plane and Q vectors first subevent
    eventPlane(TrackCountSub1, mPseudoRapiditySub1, mPhiAngleSub1, 
	       &Qx, &Qy, &EventPlaneAngle, i+1, 2);
    histSubEvents[2].histHarmonics[i].mHistPsi->Fill(EventPlaneAngle);
    // fill tags
    mFlowTag->qxc[i] = Qx;
    mFlowTag->qyc[i] = Qy;
    mFlowTag->nc[i] = TrackCountSub1;
    mFlowTag->sptc[i] = SumPtSub1;

    // calculate plane and Q vectors second subevent
    eventPlane(TrackCountSub2, mPseudoRapiditySub2, mPhiAngleSub2, 
	       &Qx, &Qy, &EventPlaneAngle, i+1, 3);
    histSubEvents[3].histHarmonics[i].mHistPsi->Fill(EventPlaneAngle);
    // fill tags
    mFlowTag->qxd[i] = Qx;
    mFlowTag->qyd[i] = Qy;
    mFlowTag->nd[i] = TrackCountSub2;
    mFlowTag->sptd[i] = SumPtSub2;
  }

  delete [] mRandomVector;
  delete [] mIndexVector;
  delete [] mPhiAngleSub1;
  delete [] mPseudoRapiditySub1;
  delete [] mPtSub1;
  delete [] mPhiAngleSub2;
  delete [] mPseudoRapiditySub2;
  delete [] mPtSub2;

}

//-------------------------------------------------------------

Int_t StFlowTagMaker::eventPlane(long Multiplicity, Float_t *mPseudoRapidity, 
  Float_t *mPhiAngle, Double_t *mQx, Double_t *mQy, Double_t *mEventPlaneAngle, 
  Int_t OrderParameter, Int_t eventN)
{
  Int_t harN = OrderParameter - 1;
  // will go to header file
  const Int_t MinimumMultiplicity = 1;
  const Int_t MinimumOrder = 1;
  const Int_t MaximumOrder = nHarmonics;
  //initialize return variables
  *mQx = 0.;
  *mQy = 0.;
  *mEventPlaneAngle = 0.;
  
  if (Multiplicity <= MinimumMultiplicity) {
    cout <<  
      "Selected multiplicity not usefull to calculate event plane" 
	 << endl;
    return 0;
  }
  if (OrderParameter < MinimumOrder || OrderParameter > MaximumOrder) {
    cout <<  
      "Selected order not valid to calculate event plane" 
	 << endl;
    return 0;
  }

  for (long i=0; i < Multiplicity; i++) {
    Double_t phiWgt = PhiWeight(mPhiAngle[i], eventN, harN);
    if (mPseudoRapidity[i] < 0 && OrderParameter % 2 == 1) {
      phiWgt *= -1.;
    }
    *mQx += phiWgt * cos(mPhiAngle[i] * OrderParameter);
    *mQy += phiWgt * sin(mPhiAngle[i] * OrderParameter);
  }
    *mEventPlaneAngle = atan2(*mQy,*mQx) / (float)OrderParameter;
  if (*mEventPlaneAngle < 0.) {
    *mEventPlaneAngle += twopi / (float)OrderParameter;
  }
  return kStOK;
}

//-------------------------------------------------------------

void StFlowTagMaker::swap(long &a,long &b)
{
  long itemp = a;
  a = b;
  b = itemp;
}

void StFlowTagMaker::indexx(long n,float arr[], long indx[])
{
  //
  //routine from Numerical Recipes in C pag. 338
  // Indexes an array arr[1..n], i.e. outputs the array indx[1..n]
  // such that arr[indx[j]] is in ascending order for j = 1,2..,N
  // The input quantities n and arr are not changed
  //

  const int M=7, NSTACK=50;
  long i, indxt, ir=n, j, k, l=1;
  int jstack=0;
  int *istack;
  float a;
  
  istack = new int[NSTACK+1];
  for (j=1;j<=n;j++) indx[j]=j;
  for (;;) {
    if (ir-l < M) {
      for (j=l+1;j<=ir;j++) {
	indxt=indx[j];
	a=arr[indxt];
	for (i=j-1;i>=1;i--) {
	  if (arr[indx[i]] <= a) break;
	  indx[i+1]=indx[i];
	}
	indx[i+1]=indxt;
      }
      if (jstack == 0) break;
      ir=istack[jstack--];
      l=istack[jstack--];
    } 
    else {
      k=(l+ir) >> 1;
      swap(indx[k],indx[l+1]);
      if (arr[indx[l+1]] > arr[indx[ir]]) {
	swap(indx[l+1],indx[ir]);
      }
      if (arr[indx[l]] > arr[indx[ir]]) {
	swap(indx[l],indx[ir]);
      }
      if (arr[indx[l+1]] > arr[indx[l]]) {
	swap(indx[l+1],indx[l]);
      }
      i=l+1;
      j=ir;
      indxt=indx[l];
      a=arr[indxt];
      for (;;) {
	do i++; while (arr[indx[i]] < a);
	do j--; while (arr[indx[j]] > a);
	if (j < i) break;
	swap(indx[i],indx[j]);
      }
      indx[l]=indx[j];
      indx[j]=indxt;
      jstack += 2;
      if (jstack > NSTACK) {cout <<"NSTACK too small in indexx." << endl;}
      if (ir-i+1 >= j-l) {
	istack[jstack]=ir;
	istack[jstack-1]=i;
	ir=j-1;
      } 
      else {
	istack[jstack]=j-1;
	istack[jstack-1]=l;
	l=i;
      }
    }
  }
  delete [] istack;
}

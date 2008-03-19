/**********************************************************************
 *
 * $Id: StEStructCutBin.cxx,v 1.11 2008/03/19 22:06:01 prindle Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  Cut-bins for building histograms based on kinematic selections
 *               Singleton class with several implementations based on
 *               a mode ID
 *
 ***********************************************************************/
#include "StEStructCutBin.h"
#include "StEStructPairCuts.h"

ClassImp(StEStructCutBin)


StEStructCutBin* StEStructCutBin::mInstance=0;

StEStructCutBin* StEStructCutBin::Instance(){
  if(!mInstance) mInstance=new StEStructCutBin();    
  return mInstance;
}


StEStructCutBin::~StEStructCutBin(){};

void StEStructCutBin::setMode(int mode){

  /* if(mcutModeName && mode!=mcutMode){
    cout<<" Changing cut mode from mode="<<mcutMode<<" to mode="<<mode<<endl;
    delete [] mcutModeName;
    }*/

  bool silent = false;  // do we need to output cutbin info?
  if (mcutModeName && mode==mcutMode) silent = true;
  if (!silent && mode!=mcutMode) cout<<" Changing cut mode from mode="<<mcutMode<<" to mode="<<mode<<endl;
 
  if(mcutModeName) delete [] mcutModeName; 
  mcutModeName=new char[128];

  switch(mode){
  case 0:
    {
      mnumBins=1;
      mnumParentBins=1;
      strcpy(mcutModeName," No Cut Binning ");
      initPtBinMode0();
      break;
    }
  case 1:
    { 
      mnumBins=27;
      mnumParentBins=1;
      strcpy(mcutModeName," yt1 x yt2 Cut Binning, 27 bins ");
      initPtBinMode1();
      break;
    }
  case 2:
    {
      mnumBins=6;
      mnumParentBins=1;
      strcpy(mcutModeName,"Simple soft/hard same-side/away-side, 6 bins");   // *** new change
      initPtBinMode2();
      break;
    }
  case 3:
    {
      mnumBins=16;
      mnumParentBins=4;
      strcpy(mcutModeName," yt_sum, yt_delta, same-side, away-side Cut Binning, 16 bins");
      initPtBinMode3();
      break;
    }
  case 4:
    {
      mnumBins=32;
      mnumParentBins=1;
      strcpy(mcutModeName," yt_sum, yt_delta, same-side, away-side, minijet fine binning, 16 bins");
      initPtBinMode4();
      break;
    }
  case 5:
    {
      // mnumBins=14;
      mnumBins=10;
      mnumParentBins=4;
      // strcpy(mcutModeName," same-side, away-side, identified particles, 14 bins");
      strcpy(mcutModeName," identified particles, 10 bins");
      initPtBinMode5();
      break;
    }
  case 6:
    {
      mnumBins=10;
      mnumParentBins=1;
      strcpy(mcutModeName," event-wise z-vertex binning, 10 bins");
      initPtBinMode6();
      break;
    }
  case 7:
    {
      mnumBins=60;
      mnumParentBins=1;
      strcpy(mcutModeName," event-wise z-vertex binning & soft/hard SS/AS, 60 bins");
      initPtBinMode7();
      break;
    }
  default:
    {
      cout<<"Warning: cut bin mode="<<mode<<" not defined "<<endl;
      break;
    }
  }

  mcutMode=mode;
  if (!silent) cout<<"  Cut Bin Mode = "<<printCutBinName()<<endl;
}
int StEStructCutBin::getMode() {
    return mcutMode;
}
void StEStructCutBin::setCutBinHistMode(int mode) {
    mcutBinHistMode = mode;
}
int StEStructCutBin::getCutBinHistMode() {
    return mcutMode;
}
//------------------------- Mode=0 ----------------------------------------
// no cut

void StEStructCutBin::initPtBinMode0(){
  mPtBins[0]=0;
  mPtBins[1]=-1;
  mPtBinMin[0]=0.;
  mPtBinMax[0]=9999.;
}

//------------------------ Mode=1 -------------------------------------------

//  ytyt space in even bins: 
//  - 7 bins from 1.0-4.5 (>4.5 included in last bin)
//  - diagonal symmetry
//  - 6 bins (7-12), 5 bins (13-17), 4 bins( 19-22), 2 bins (23-25), 1 bin (27)

static int __yt1_x_yt2_bin[7]={0,7,13,18,22,25,27};

int StEStructCutBin::getCutBinMode1(StEStructPairCuts* pc){

  int imin,imax,istore;
  imin=((StEStructTrack*)pc->Track1())->getYtBin();
  imax=((StEStructTrack*)pc->Track2())->getYtBin();

  if( imin > imax ){
    istore=imin;
    imin=imax;
    imax=istore;
  }
  return __yt1_x_yt2_bin[imin]+imax-imin;
}
void StEStructCutBin::initPtBinMode1(){
  // everyone is 0-6

  for(int i=0;i<7;i++){
    mPtBinMin[i]=0.;
    mPtBinMax[i]=999.;
  }
  mPtBinMin[7]=0.139*sinh(1.5);
  mPtBinMax[7]=0.139*sinh(2.0);
  mPtBinMin[8]=mPtBinMin[13]=mPtBinMax[7];
  mPtBinMax[8]=mPtBinMax[13]=0.139*sinh(2.5);

  mPtBinMin[9]=mPtBinMin[14]=mPtBinMin[18]=mPtBinMax[8];
  mPtBinMax[9]=mPtBinMax[14]=mPtBinMax[18]=0.139*sinh(3.0);

  mPtBinMin[10]=mPtBinMin[15]=mPtBinMin[19]=mPtBinMin[22]=mPtBinMax[9];
  mPtBinMax[10]=mPtBinMax[15]=mPtBinMax[19]=mPtBinMax[22]=0.139*sinh(3.5);

  mPtBinMin[11]=mPtBinMin[16]=mPtBinMin[20]=mPtBinMin[23]=mPtBinMin[25]=mPtBinMax[10];
  mPtBinMax[11]=mPtBinMax[16]=mPtBinMax[20]=mPtBinMax[23]=mPtBinMax[25]=0.139*sinh(4.0);

  mPtBinMin[12]=mPtBinMin[17]=mPtBinMin[21]=mPtBinMin[24]=mPtBinMin[26]=mPtBinMin[27]=mPtBinMax[11];
  mPtBinMax[12]=mPtBinMax[17]=mPtBinMax[21]=mPtBinMax[24]=mPtBinMax[26]=mPtBinMax[27]=999.;

}

//------------------------ Mode=2 -------------------------------------------

// now trying simple soft/hard SS/AS binning
//   using cut from pp ytxyt paper with soft == yt_sum < 3.3
//   should also require yt > 2 for hard, but I don't want to make a third case right now
//   0 = soft SS;  1 = hard SS;  2 = other SS;  3 = soft AS;  4 = hard AS;  5 = other AS;

int StEStructCutBin::getCutBinMode2(StEStructPairCuts* pc){

  int retVal;
  int iyt, idp;
  
  float yt1=pc->Track1()->Yt();
  float yt2=pc->Track2()->Yt();

  iyt = 2;
  if (yt1+yt2 <= 3.3) iyt = 0;
  if (yt1>=2 && yt2>=2) iyt = 1;

  if (pc->sameSide()) idp = 0;
  else idp = 1;

  retVal = iyt + 3*idp;

  return retVal;

}

void StEStructCutBin::initPtBinMode2(){ 
  // check these in qa spectra

  mPtBinMin[0]=mPtBinMin[3]=0;  // soft: min = 0; max = 3.3
  mPtBinMax[0]=mPtBinMax[3]=0.139*sinh(3.3);  

  mPtBinMin[1]=mPtBinMin[4]=0.139*sinh(3.3);  // hard: min = 3.3; max = 999.
  mPtBinMax[1]=mPtBinMax[4]=999.;

  mPtBinMin[2]=mPtBinMin[5]=0;  // other:  min = 0; max = 999.
  mPtBinMax[2]=mPtBinMax[5]=999.;

} 



//------------------------ Mode=3 -------------------------------------------
//
// --> now modified for 
//        soft= yt<1.8
//        neck=1.8<=yt<=2.2
//        hard=yt>2.2
//        rest .. is the rest in the 2pt space ... all pt's satisfy it

// ytyt plot deta,dphi
// 0-3   = soft
// 4-7   = 'neck'
// 8-11  = hard
// 12-15 = rest
// 0,4,8,12 away-side large deta
// 1,5,9,13  away-side small deta
// 2,6,10,14 same-side small deta
// 3,7,11,15 same-side large deta

//static int __yt_deta_dphi_bin[4][4]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
static int __yt_deta_dphi_bin[4][4]={ {0,1,2,3}, {4,5,6,7}, {8,9,10,11}, {12,13,14,15} };  // remove compiler warning

int StEStructCutBin::getCutBinMode3(StEStructPairCuts* pc){

  int iyt;
  
  float yt1=pc->Track1()->Yt();
  float yt2=pc->Track2()->Yt();

  // These numbers are also used in StEStructCutBin::getParentBin (change both)
  if(yt1<1.8 && yt2<1.8){
    iyt=0;
//  } else if(yt1<2.2 && yt2<2.2){
// This was getting the entire yt square below 2.2 minus the soft.
// I thought we only wanted the case with both tracks below 2.2 AND ABOVE 1.8
  } else if( ((1.8<yt1) && (yt1<2.2)) && ((1.8<yt2) && (yt2<2.2))) {
    iyt=1;
  } else if(yt1>=2.2 && yt2>=2.2){
    iyt=2;
  } else {
    iyt=3;
  }

  float deta=fabs(pc->DeltaEta());
  //  float dphi=fabs(pc->DeltaPhi());

  int idedp;

  if(deta<1.0) { 
    if(pc->sameSide()){ // dphi<M_PI/2.0 || dphi> 1.5*M_PI){
      idedp=2;
    } else {
      idedp=1;
    }
  } else {    
    if(pc->sameSide()){  // dphi<M_PI/2. || dphi> 1.5*M_PI){
      idedp=3;
    } else {
      idedp=0;
    }
  }   

  return  __yt_deta_dphi_bin[iyt][idedp];
}
void StEStructCutBin::initPtBinMode3(){

    mPtBinMin[0]=mPtBinMin[1]=mPtBinMin[2]=mPtBinMin[3]=0.;
    mPtBinMax[0]=mPtBinMax[1]=mPtBinMax[2]=mPtBinMax[3]=0.139*sinh(1.8);

    mPtBinMin[4]=mPtBinMin[5]=mPtBinMin[6]=mPtBinMin[7]=mPtBinMax[0];
    mPtBinMax[4]=mPtBinMax[5]=mPtBinMax[6]=mPtBinMax[7]=0.139*sinh(2.2);

    mPtBinMin[8]=mPtBinMin[9]=mPtBinMin[10]=mPtBinMin[11]=mPtBinMax[4];
    mPtBinMax[8]=mPtBinMax[9]=mPtBinMax[10]=mPtBinMax[11]=999.;

    mPtBinMin[12]=mPtBinMin[13]=mPtBinMin[14]=mPtBinMin[15]=0.;
    mPtBinMax[12]=mPtBinMax[13]=mPtBinMax[14]=mPtBinMax[15]=999.;

}

//------------------------ Mode=4 -------------------------------------------

/*
   ns, as:

   0,16 = soft (yt<1.8) 
   1-13,17-29 = dyt<1.8, syt>=4.0, binned in 0.25 up to 6.75 then by 0.5
   14,30 = yt>2.0, dyt>2.0
   15,31 = rest

*/

int StEStructCutBin::getCutBinMode4(StEStructPairCuts* pc){

  float yt1=((StEStructTrack*)pc->Track1())->Yt();
  float yt2=((StEStructTrack*)pc->Track2())->Yt();

  //  float dphi=fabs(pc->DeltaPhi());
  int iside=0;

  if( !pc->sameSide() ) iside+=16; // away-side
  if(yt1<=1.8 && yt2<=1.8) return iside;    

  float ytsum=yt1+yt2;
  float ytdel=fabs(yt1-yt2);
  int ival=0;
  if(ytdel<1.8 && ytsum>4.0){
    ival=(int)floor((ytsum-4.0)/0.25)+1;
    if(ival>11){
      if(ival==12 || ival==13){
	ival=12;
      } else {
	ival=13;
      }
    }
  } else if(yt1>2.0 && yt2>2.0){
    ival=14;
  } else {
    ival=15;
  }

  ival+=iside;
  return ival;
}
void StEStructCutBin::initPtBinMode4(){

  /***********************************************
      
  I don't recomment this cut-selecction for pt-correlations ...
  but for completeness I define the pt-binning here  

  *************************************************/

    mPtBinMin[0]=mPtBinMin[16]=0.;
    mPtBinMax[0]=mPtBinMax[16]=0.139*sinh(1.8);

    mPtBinMin[1]=mPtBinMin[17]=0.139*sinh(1.1); // min for yt+yt=4
    mPtBinMax[1]=mPtBinMax[17]=0.139*sinh(3.025); // max for yt+yt=4.25

    mPtBinMin[2]=mPtBinMin[18]=0.139*sinh(1.125); // min for yt+yt=4.25
    mPtBinMax[2]=mPtBinMax[18]=0.139*sinh(3.15); // max for yt+yt=4.5

    mPtBinMin[3]=mPtBinMin[19]=0.139*sinh(1.25); // min for yt+yt=4.5
    mPtBinMax[3]=mPtBinMax[19]=0.139*sinh(3.275); // max for yt+yt=4.75

    //

    mPtBinMin[4]=mPtBinMin[20]=0.139*sinh(1.275); // min for yt+yt=4.75
    mPtBinMax[4]=mPtBinMax[20]=0.139*sinh(3.4); // max for yt+yt=5.

    mPtBinMin[5]=mPtBinMin[21]=0.139*sinh(1.4); // min for yt+yt=5.
    mPtBinMax[5]=mPtBinMax[21]=0.139*sinh(3.525); // max for yt+yt=5.25

    mPtBinMin[6]=mPtBinMin[22]=0.139*sinh(1.525); // min for yt+yt=5.25
    mPtBinMax[6]=mPtBinMax[22]=0.139*sinh(3.65); // max for yt+yt=5.5

    //

    mPtBinMin[7]=mPtBinMin[23]=0.139*sinh(1.65); // min for yt+yt=5.5
    mPtBinMax[7]=mPtBinMax[23]=0.139*sinh(3.775); // max for yt+yt=5.75

    mPtBinMin[8]=mPtBinMin[24]=0.139*sinh(1.775); // min for yt+yt=5.75
    mPtBinMax[8]=mPtBinMax[24]=0.139*sinh(3.9); // max for yt+yt=6.

    mPtBinMin[9]=mPtBinMin[25]=0.139*sinh(1.9); // min for yt+yt=6.
    mPtBinMax[9]=mPtBinMax[25]=0.139*sinh(4.025); // max for yt+yt=6.25

    //

    mPtBinMin[10]=mPtBinMin[26]=0.139*sinh(2.025); // min for yt+yt=6.25
    mPtBinMax[10]=mPtBinMax[26]=0.139*sinh(4.15); // max for yt+yt=6.5

    mPtBinMin[11]=mPtBinMin[27]=0.139*sinh(2.15); // min for yt+yt=6.5
    mPtBinMax[11]=mPtBinMax[27]=0.139*sinh(4.275); // max for yt+yt=6.75

    mPtBinMin[12]=mPtBinMin[28]=0.139*sinh(2.275); // min for yt+yt=6.75
    mPtBinMax[12]=mPtBinMax[28]=0.139*sinh(4.525); // max for yt+yt=7.25

    mPtBinMin[13]=mPtBinMin[29]=0.139*sinh(2.525); // min for yt+yt=7.25
    mPtBinMax[13]=mPtBinMax[29]=999.;              // max for yt+yt=...


    // -- never use these odd regions for pt-correlations in this cut-selection
    mPtBinMin[14]=mPtBinMin[30]=mPtBinMin[15]=mPtBinMin[31]=9999.;
    mPtBinMax[14]=mPtBinMax[30]=mPtBinMax[15]=mPtBinMax[31]=-1.;

    

};


//------------------------ Mode=5 -------------------------------------------
//
// Particle id using dE/dx identification.
//    Use dE/dx to identify pi, K, p within momentum ranges where this
//    is possible. 
// No yt cut for since dEdx works over a limited momentum range.
//
// The cut values and their meanings are:
//   0  pi-o  pair (where o is not pi, K or p)
//   1  pi-pi pair.
//   2  pi-K  pair
//   3  pi-p  pair
//   4  K-o   pair (where o is not pi, K or p)
//   5  K-K   pair
//   6  K-p   pair
//   7  p-o   pair (where o is not pi, K or p)
//   8  p-p   pair
//   9  o-o   pair (where o is not pi, K or p)
//
// To check on charge symmetry we have split out the -+ from the +-
// in the other parts of the Correlation code.

int StEStructCutBin::getCutBinMode5(StEStructPairCuts* pc, int pairCase) {

    int mode[4][4] = {{9, 0, 4, 7}, {0, 1, 2, 3}, {4, 2, 5, 6}, {7, 3, 6, 8}};

    int it1 = pc->getdEdxPID( pc->Track1() );
    int it2 = pc->getdEdxPID( pc->Track2() );
    if (it1 < 0 || 3 < it1) {
        return -1;
    }
    if (it2 < 0 || 3 < it2) {
        return -2;
    }
    int iBin = mode[it1][it2];

    if (!mcutBinHistMode) {
        return iBin;
    }

    // Might want to make invariant mass cuts.
    double e, e1, e2, p1, p2, p[3], m, m1, m2;
    p1   = pc->Track1()->Ptot();
    p2   = pc->Track2()->Ptot();
    p[0] = pc->Track1()->Px() + pc->Track2()->Px();
    p[1] = pc->Track1()->Py() + pc->Track2()->Py();
    p[2] = pc->Track1()->Pz() + pc->Track2()->Pz();
    float Mass[]  = {0.1396,  0.1396,  0.497, 0.9383};
    float Mass2[] = {0.01949, 0.01949, 0.247, 0.880};
    if (9 == iBin) {
        // For o-o try using m1 = m2 = 0.
        m1 = 0;
        m2 = 0;
        e1 = p1;
        e2 = p2;
    } else {
        m1 = Mass[it1];
        m2 = Mass[it2];
        e1 = sqrt(p1*p1 + Mass2[it1]);
        e2 = sqrt(p2*p2 + Mass2[it2]);
    }
    e = e1 + e2;
    m = sqrt(e*e - p[0]*p[0] - p[1]*p[1] - p[2]*p[2]);
/*
 * Moved the invariant mass cuts to StEStructPairCuts code.
 *
    // Cut on invariant mass to keep or exclude resonances.
    // (Not sure how to bring these in through a cuts file.)
    int resType = 0;
    switch(iBin) {
        case 1: {  // K_short in pi^+ pi^-
            if ((0.47 < m) && (m < 0.51)) {
                resType = 1;
            }
            break;
        }
        case 2: {  // K^* in pi^- K^+
            if ((0.85 < m) && (m < 0.93)) {
                resType = 2;
            }
            break;
        }
        case 3: {  // Lambda in pi^- p
            if ((1.10 < m) && (m < 1.125)) {
                resType = 3;
            }
            break;
        }
        case 5: {  // phi in K^+K^-
            if ((1.01 < m) && (m < 1.04)) {
                resType = 4;
            }
            break;
        }
        case 6: {  // Lambda^* in K^- p
            if ((1.49 < m) && (m < 1.55)) {
                resType = 5;
            }
            break;
        }
        case 8: {  // Unknown threshold thing in pp (NOT in \bar p\bar p
            if ((1.87 < m) && (m < 1.92)) {
                resType = 6;
            }
            break;
        }
    }
 */
    mHCutBinHists[pairCase][iBin]->Fill(m - m1 - m2 + 0.1);

    e1 = sqrt(p1*p1 + Mass2[1]);
    e2 = sqrt(p2*p2 + Mass2[1]);
    e = e1 + e2;
    m = sqrt(e*e - p[0]*p[0] - p[1]*p[1] - p[2]*p[2]) - Mass[1] - Mass[1] + 0.1;
    mHCutBinHists[pairCase][10]->Fill(m);

    return iBin;
}
int StEStructCutBin::ignorePair5(StEStructPairCuts* pc) {

 /*
  * Accept pair if charges are the same.
  */
    int ic1 = pc->Track1()->Charge();
    int ic2 = pc->Track2()->Charge();
    if ( ic1 == ic2 ) {
        return 0;
    }

 /*
  * Ignore particles with the same pid and opposite charge when the first
  * charge is negative.
  * In the main track pair loop every pair will come up twice, the
  * second time in reversed order and we only want it one time.
  *
  * If both particles are un-identified we put pair in ipid=9
  * bin and treat as identical.
  */
    int ip1 = pc->getdEdxPID( pc->Track1() );
    int ip2 = pc->getdEdxPID( pc->Track2() );
    if (ip1 == ip2) {
        if (-1 == ic1) {
            return 1;
        } else {
            return 0;
        }
    }
 /*
  * For particles with different pid and opposite charge we only keep if
  * o < pi < K < p
  */
    if (ip1 < ip2) {
        return 0;
    }
    return 1;
}
int StEStructCutBin::symmetrizeYt5(StEStructPairCuts* pc) {

 /*
  * If particle types and charges are the same we symmetrize.
  * If both particles are un-identified the pair will be treated
  * as identical.
  */
    if ( pc->Track1()->Charge() != pc->Track2()->Charge() ) {
        return 0;
    }
    int ip1 = pc->getdEdxPID( pc->Track1() );
    int ip2 = pc->getdEdxPID( pc->Track2() );
    if (ip1 != ip2) {
        return 0;
    }
    return 1;
}
int StEStructCutBin::switchYt5(StEStructPairCuts* pc) {

 /*
  * For different pid order as 0 < pi < K < p.
  * For same pid want + before -.
  */
    int ipid1 = pc->getdEdxPID( pc->Track1() );
    int ipid2 = pc->getdEdxPID( pc->Track2() );
    if (ipid1 == ipid2) {
        if ( (-1 == pc->Track1()->Charge()) &&
             (+1 == pc->Track2()->Charge()) ) {
            return 1;
        }
        return 0;
    }
    if (ipid2 < ipid1) {
        return 1;
    }
    return 0;
}

/*
  Use mPtMin and mPtMax as limits of where we trust pid.
  Index 1 for pi
        2 for K
        3 for p
  Would be nice to initialize these in some way we didn't have to
  recompile to change them.
 */
void StEStructCutBin::initPtBinMode5(){
  // For Hijing we have perfect pid at all pts.
  mPtBinMin[0]=0.;
  mPtBinMax[0]=9999.;
  mPtBinMin[1]=0.;
  mPtBinMax[1]=9999.;
  mPtBinMin[2]=0.;
  mPtBinMax[2]=9999.;
  mPtBinMin[3]=0.;
  mPtBinMax[3]=9999.;
  // For data we want to exclude relativistic rise region which
  // might possibly give us a few tracks.
  mPtBinMin[0] =    0.0;
  mPtBinMax[0] = 9999.0;
  mPtBinMin[1] =    0.0;
  mPtBinMax[1] =    1.0;
  mPtBinMin[2] =    0.0;
  mPtBinMax[2] =    1.0;
  mPtBinMin[3] =    0.0;
  mPtBinMax[3] =    1.5;


    if (mcutBinHistMode) {
        TString hname;
        char *types[] = {"Sibpp", "Sibpm", "Sibmp", "Sibmm",
                         "Mixpp", "Mixpm", "Mixmp", "Mixmm"};
        char *bases[] = {"piAll", "pipi", "piK", "pip",
                         "KAll",  "KK",   "Kp",  "pAll",
                         "pp",    "OO",   "All"};
        for (int pairCase=0;pairCase<8;pairCase++) {
            mHCutBinHists[pairCase] = new TH1D*[11];
            for (int it=0;it<11;it++) {
                hname  = "Mass";
                hname += bases[it];
                hname += types[pairCase];
                mHCutBinHists[pairCase][it] = new TH1D(hname.Data(),hname.Data(),500,0.0,3.0);
//                cout << "Creating histogram for " << hname.Data() << endl;
            }
        }
    }
}
void StEStructCutBin::writeCutBinHists5() {
    if (mcutBinHistMode) {
        for (int pairCase=0;pairCase<8;pairCase++) {
            if (mHCutBinHists[pairCase]) {
                for (int it=0;it<11;it++) {
                    mHCutBinHists[pairCase][it]->Write();
//                    cout << "Deleting histogram [" << pairCase << "][" << it << "]" << endl;
                    if (mHCutBinHists[pairCase][it]) {
                        delete mHCutBinHists[pairCase][it];
                        mHCutBinHists[pairCase][it] = 0;
                    }
                }
//                cout << "Deleting array [" << pairCase << "]" << endl;
                delete [] mHCutBinHists[pairCase];
                mHCutBinHists[pairCase] = 0;
            }
        }
    }
}


//------------------------ Mode=6 -------------------------------------------
//  Event-wise z-vertex binning
//  This mode breaks the model of everything else, so it is a hack.
//  pc object doesn't have event level info, so cutbin number is set in
//    2ptanalysis by looking at mixing event buffer index.

int StEStructCutBin::getCutBinMode6(StEStructPairCuts*, int zbin){
  // This function should never be used, can't access z-vertex position from here...
  return zbin;
}
void StEStructCutBin::initPtBinMode6(){
  for(int i=0;i<10;i++){
    mPtBinMin[i]=0.;
    mPtBinMax[i]=999.;
  }
}
  
//------------------------ Mode=7 -------------------------------------------     
//  Combines modes 2 and 6:
//  Event-wise z-vertex binning WITH soft/hard SS/AS binning

int StEStructCutBin::getCutBinMode7(StEStructPairCuts* pc, int zbin){

  if (zbin<0 || zbin>9) return 0;

  int retVal;
  int iyt, idp;

  float yt1=pc->Track1()->Yt();
  float yt2=pc->Track2()->Yt();

  iyt = 2;
  if (yt1+yt2 <= 3.3) iyt = 0;
  if (yt1>=2 && yt2>=2) iyt = 1;

  if (pc->sameSide()) idp = 0;
  else idp = 1;

  retVal = zbin*6 + iyt + 3*idp;

  return retVal;

}
void StEStructCutBin::initPtBinMode7(){
  // check these in qa spectra 

  for(int i=0; i<10; i++) {
    mPtBinMin[i*6 + 0]=mPtBinMin[i*6 + 3]=0;  // soft: min = 0; max = 3.3
    mPtBinMax[i*6 + 0]=mPtBinMax[i*6 + 3]=0.139*sinh(3.3);
    mPtBinMin[i*6 + 1]=mPtBinMin[i*6 + 4]=0.139*sinh(3.3);  // hard: min = 3.3; max = 999. 
    mPtBinMax[i*6 + 1]=mPtBinMax[i*6 + 4]=999.;
    mPtBinMin[i*6 + 2]=mPtBinMin[i*6 + 5]=0;  // other:  min = 0; max = 999.
    mPtBinMax[i*6 + 2]=mPtBinMax[i*6 + 5]=999.;
  }
}
  

/***********************************************************************
 *
 * $Log: StEStructCutBin.cxx,v $
 * Revision 1.11  2008/03/19 22:06:01  prindle
 * Added doInvariantMass flag.
 * Added some plots in pairDensityHistograms.
 * SetZOffset used to only be done when doPairDensity was true.
 * Moved creating/copying pairDensity histograms to same place as other histograms.
 * Added cutBinHistMode
 * mode3 neck was defined as yt1<2.2 && yt2<2.2 (and not soft)
 *            now is        1.8<yt1<2.2  && 1.8<yt2<2.2
 * Added gooddzdxy, Merging2 and Crossing2 to pair cuts.
 *
 * Revision 1.10  2007/11/26 19:55:24  prindle
 * In 2ptCorrelations: Support for keeping all z-bins of selected centralities
 *                     Change way \hat{p_t} is calculated for parent distributions in pid case.
 *    Binning          Added parent binning (for \hat{p_t}
 *    CutBin:          Mode 5 extensively modified.
 *                     Added invariant mass cuts (probably a bad idea in general.)
 *
 * Revision 1.9  2007/05/27 22:45:02  msd
 * Added new cut bin modes 2 (soft/hard SS/AS), 6 (z-vertex binning), and 7 (modes 2*6).
 * Fixed bug in merging cut.
 * Added a few histograms to 2pt corr.
 *
 * Revision 1.8  2007/01/26 17:17:09  msd
 * Implemented new binning scheme: dEta stored in array with bin centered at zero, dPhi array has bins centered at zero and pi.  Final DEtaDPhi has 25x25 bins with dPhi bin width of pi/12 so all major angles are centered in bins.
 *
 * Revision 1.7  2006/10/02 22:21:00  prindle
 * Store only quadrant of eta_Delta - phi_Delta array/histogram.
 * Store half of eta_Sigma - phi_Delta array/histogram.
 * This required modifications in Binning.
 * I had a bug in the pair loop (which left +- not fully symmetrized)
 * and had to make changes in cut bins for mode 5 (and 3 I think)
 * when I fixed this.
 * Also change crossing cut to use only two parameters, the sign of
 * the magnetic field being taken from the MuDst.
 *
 * Revision 1.6  2006/04/10 23:42:32  porter
 * Added sameSide() & awaySide() methods to PairCut (so only defined in 1 place)
 * and added the eta_delta weighting as a binned correctin defined by the eta-limits in
 * the StEStructBinning object
 *
 * Revision 1.5  2006/04/06 01:01:19  prindle
 *
 *   New mode in CutBin, 5, to do pid correlations. There is still an issue
 * of how to set the pt ranges allowed for the different particle types.
 * For data we probably want to restrict p to below 1GeV for pi and K, but
 * for Hijing and Pythia we can have perfect pid. Currently cuts are type
 * into the code (so you have to re-compile to change them.)
 *
 *   In the Correlations code I split -+ from +- and am keeping track of
 * pt for each cut bin. These required changes in the Support code.
 *
 * Revision 1.4  2006/02/22 22:05:16  prindle
 * Removed all references to multRef (?)
 * Added cut mode 5 for particle identified correlations.
 * Other cut modes should be same as before
 *
 * Revision 1.3  2005/09/14 17:14:23  msd
 * Large update, added new pair-cut system, added pair density plots for new analysis mode (4), added event mixing cuts (rewrote buffer for this)
 *
 * Revision 1.2  2005/03/03 01:30:44  porter
 * updated StEStruct2ptCorrelations to include pt-correlations and removed
 * old version of pt-correlations from chunhuih (StEStruct2ptPtNbar)
 *
 * Revision 1.1  2004/06/25 03:11:49  porter
 * New cut-binning implementation and modified pair-cuts for chunhui to review
 *
 *
 *********************************************************************/

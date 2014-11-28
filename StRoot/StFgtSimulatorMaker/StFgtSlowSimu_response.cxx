// *-- Author : J.Balewski
// 
// \date   July 2007
// $Id: StFgtSlowSimu_response.cxx,v 1.1 2012/06/06 20:35:09 jeromel Exp $

#include <TRandom3.h>
#include <TH2.h>
#include <TF1.h>
#include <TVector2.h>


#include "StFgtSlowSimuMaker.h"


#include "StFgtDbMaker/StFgtDbMaker.h"

//--------------------------------------------
//--------------------------------------------
//--------------------------------------------
// http://drupal.star.bnl.gov/STAR/system/files/2001-F.Simon-diploma.pdf
// chapter 3. The GEM Concept  
void
StFgtSlowSimuMaker::responseMipModel(TVector3 Rloc, TVector3 Dloc){


  /* in order to simulate the energy loss for individual ionizing collisions a table from Bichsel is used. 
     This is given in figure 9 in NIM A562, 154 (2006)
     This table gives the energy loss for a given random number (from 0 to 1 in steps of 10^-4)
     Two files, low and high beta gamma:
     file Low: beta gamma .31623      1.00000      3.16228     10.00000     31.62278
     file High: beta gamma 100.00000    316.22777   1000.00000   3162.27766  10000.00000

     here use column 2 for High file
  */

  Double_t zLocEnd=Rloc.z()+Dloc.z(); // for transverse diffusion
  Double_t pathLength=Dloc.Mag(); // convert from cm to mm
  TVector3 rv=Dloc; rv=rv.Unit(); // unit vector along the path
//    printf("|v|=%.3f  x,y,z=%.3f,%.3f,%.3f, pT=%.3f\n",rv.Mag(), rv.x(),rv.y(),rv.z(),rvT);

  Double_t totalEnergy_eV = 0;
  Double_t path = 0; // in mm, traveled so far
  Int_t nPrimPair = 0; //# primary pairs  for this path
  Int_t nTotPair = 0; //# any pairs  for this path
  
  Double_t sum1=0,sum2=0; // to calculate average charge position along the tracks
  while (1) {
    // make a random step
    Double_t stepLength = - TMath::Log(mRnd->Uniform()) / par_pairsPerCm;
    path += stepLength;
    if (path > pathLength) break;
    nPrimPair++;
    // additional weight according to secondary energy distribution, according to Bichsel dist
    Int_t rndBin = ((int) (10000.0 * mRnd->Uniform()));
    // Cutoff at 9992  WMZ
    if(rndBin > par_cutoffOfBichel) rndBin = par_cutoffOfBichel;
    //eLossTab removed from StFgtDbMaker in review
    Double_t eL_eV=fgtDb->getEloss(rndBin);
    //jjassert(eL_eV);
    Int_t nAnyPair = 1 + ((int) ((eL_eV-15.4)/26.)); // # of pairs for this sub-step, includes amplification
    if (nAnyPair < 0)   continue; // skip electron inf absorbed
    totalEnergy_eV += eL_eV;
    // add this electron as hit
    TVector3 r=Rloc+ path*rv; // here we use cm as in the whole GSTAR
    // printf("Loc  %f %f %f  Rxy=%f  path=%f ns=%d\n", r.x(),r.y(),r.z(),r.Perp(),path,nAnyPair);
    nTotPair+= nAnyPair;

    //............... transverse difusion in drift gap
    if(par_transDiffusionPerPath>0.001) {
      Double_t zDrift=zLocEnd-r.z(); // distance to 1st GEM foil
      /* WMZ 10/13/09
	 Negative zDrift would happen in some events. The next line is added
	 to avoid it temporarily. Negative zDrift of hits in FGT disks (Z>0)
	 with a negative P_z is probably caused by "back-scattering" tracks.
	 Ignoring the sign of zDrift here does not seem to affect the estimation
	 of PERPENDICULAR diffusion to a hit. But, a better understanding of the
	 "back-scattering" is needed to make a permanent solution for it.
      */
      if(zDrift < 0) zDrift *= -1.0;
//      cout << "Debug: zDrift = " << zDrift << endl;
      Double_t perpDiffRms=par_transDiffusionPerPath*sqrt(zDrift);
      Double_t phi=mRnd->Uniform(TMath::TwoPi());
      Double_t perp=mRnd->Gaus(0,perpDiffRms);
      TVector3 dR; dR.SetPtThetaPhi(perp,0.,phi);
      r+=dR; // add diffusion to current hit location
      // printf("aaa/um %.1f %.1f --> %.1f %.1f %.1f\n", zDrift*1e4,  perpDiffRms*1e4, dR.x()*1e4, dR.y()*1e4, dR.z()*1e4);
#ifdef __FGT_QA_HISTO__
      hA[27]->Fill(zDrift*10); // histo is in mm
      hA[28]->Fill( dR.x()*1e4, dR.y()*1e4);
#endif
    }

    addHit(r,nAnyPair); //position & amplitude of total ionisation caused by this primary pair   

#ifdef __FGT_QA_HISTO__
    hA[20]->Fill(eL_eV);    
#endif
    sum1+=nAnyPair;
    sum2+=nAnyPair*path;    
  }

#ifdef __FGT_QA_HISTO__
  hA[21]->Fill(nPrimPair );
  hA[22]->Fill(totalEnergy_eV/ 1000.);
  hA[23]->Fill(nTotPair );
  hA[24]->Fill(path*10 ); // convert to mm
  Double_t meanPath=sum2/sum1;
  Double_t rvT=rv.Perp(); // only for QA histos

  Double_t meanTpath=meanPath*rvT;
  //  printf("nAnyEle weighted meanL/mm=%.3f , rvT=%4f, meanLT/mm=%3f\n",meanPath*10, rvT,meanTpath*10);
  hA[25]->Fill(meanPath*10);
  hA[26]->Fill(meanTpath*10);
#endif
} 




//--------------------------------------------
//--------------------------------------------
//--------------------------------------------
void
StFgtSlowSimuMaker::addHit(TVector3 rLoc, Double_t ampl) {


  //printf("addH %f %f %f  Rxy=%f  ampl=%f\n", rLoc.x(),rLoc.y(),rLoc.z(),rLoc.Perp(), ampl);
  Float_t  xH=fabs(rLoc.x()); // hit centroid in local ref frame
  Float_t  yH=fabs(rLoc.y());
  //jjassert(xH>0);
  //jjassert(yH>0);
  //digXY->Fill(xH,yH); // store just one value per hit

  TAxis *axX=quadDigitizationXY->GetXaxis();
  Int_t ixH=axX->FindFixBin(xH);
  Int_t mxX=axX->GetNbins();
  Int_t ix1=ixH-par_binStep,ix2=ixH+par_binStep;
  if(ix1<1) ix1=1;
  if(ix1>mxX) ix1=mxX;
  //  printf("hh2x %f %d   %d %d\n",xH,ixH,ix1,ix2);
  TAxis *axY=quadDigitizationXY->GetYaxis();
  Int_t iyH=axY->FindFixBin(yH);
  Int_t mxY=axY->GetNbins();
  Int_t iy1=iyH-par_binStep,iy2=iyH+par_binStep;
  if(iy1<1) iy1=1;
  if(iy1>mxY) iy1=mxY;
  // printf("hh2y %f %d   %d %d\n",yH,iyH,iy1,iy2);
  
  Float_t  valMax=0;
  Int_t ix,iy;
  for(ix=ix1;ix<=ix2;ix++) {
    Float_t  x=axX->GetBinCenter(ix);
    Float_t  val_x=amplFunc(x-xH);
    // printf("hh3 ix=%d x=%f dx=%f, ampl_x=%f\n",ix,x,x-xH,val_x);
    for(iy=iy1;iy<=iy2;iy++) {
      Float_t  y=axY->GetBinCenter(iy);
      Float_t  val_y=amplFunc(y-yH);
      Float_t  val2D=ampl*val_x*val_y;
      //  printf("hh4 iy=%d y=%f dy=%f, ampl_y=%f  ampl_xy=%f\n",iy,y,y-yH,val_y,val2D);
      quadDigitizationXY->Fill(x,y,val2D);
#ifdef __FGT_QA_HISTO__
      digXYAll->Fill(x,y,val2D);
#endif
      if(valMax<val2D) valMax=val2D;
    }
  }
  // printf("hh5 valMax=%f\n",valMax);


}




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// $Log: StFgtSlowSimu_response.cxx,v $
// Revision 1.1  2012/06/06 20:35:09  jeromel
// Code  review closed (requested Anselm/Jan; reviewed Jonathan/Jason)
//
// Revision 1.9  2012/05/08 16:40:26  avossen
// prepare for review
//
// Revision 1.8  2012/03/15 19:27:15  avossen
// added getEloss to StFgtDb
//
// Revision 1.7  2012/03/10 01:57:05  rfatemi
// disable access to eLossTab
//
// Revision 1.6  2011/12/02 01:10:46  balewski
// add back fgtDb->eLosTab(..)
//
// Revision 1.5  2011/12/01 00:58:01  avossen
// changed the use of the naive maker to use of StFgtDb, replaced geom-> with StFgtGeom::
//
// Revision 1.4  2011/10/06 19:05:56  balewski
// Elos table is now read in from STAR DB
//
// Revision 1.3  2011/10/05 18:04:33  balewski
// storing of FGT in StEvent is almost working
//
// Revision 1.2  2011/09/29 21:36:17  balewski
// now 2D distribution of charge & fiducial cuts are workimng properly
//
// Revision 1.1  2011/09/28 20:57:37  balewski
// merging private code
//
// Revision 1.1  2011/04/07 19:31:22  balewski
// start
//


 



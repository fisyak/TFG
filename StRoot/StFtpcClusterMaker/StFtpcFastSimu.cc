// $Id: StFtpcFastSimu.cc,v 1.14 2000/11/24 15:02:33 hummler Exp $
//
// $Log: StFtpcFastSimu.cc,v $
// Revision 1.14  2000/11/24 15:02:33  hummler
// commit changes omitted in last commit
//
// Revision 1.12  2000/09/18 14:26:48  hummler
// expand StFtpcParamReader to supply data for slow simulator as well
// introduce StFtpcGeantReader to separate g2t tables from simulator code
// implement StFtpcGeantReader in StFtpcFastSimu
//
// Revision 1.11  2000/08/03 14:39:00  hummler
// Create param reader to keep parameter tables away from cluster finder and
// fast simulator. StFtpcClusterFinder now knows nothing about tables anymore!
//
// Revision 1.10  2000/02/04 13:49:40  hummler
// upgrade ffs:
// -remove unused fspar table
// -make hit smearing gaussian with decent parameters and static rand engine
// -separate hit smearing from cluster width calculation
//
// Revision 1.9  2000/02/02 15:40:05  hummler
// make hit smearing gaussian instead of box-shaped
//
// Revision 1.8  2000/02/02 15:20:36  hummler
// correct acceptance at sector boundaries,
// take values from fcl_det
//
// Revision 1.7  2000/01/27 14:48:06  hummler
// correct hit smearing
//
// Revision 1.6  2000/01/27 09:47:18  hummler
// implement raw data reader, remove type ambiguities that bothered kcc
//
// Revision 1.5  2000/01/05 13:23:40  hummler
// make cc5 happy
//
// Revision 1.4  2000/01/03 12:48:57  jcs
// Add CVS Id strings
//

#include "StFtpcFastSimu.hh"
#include "StFtpcParamReader.hh"
#include "StFtpcGeantReader.hh"
#include "math_constants.h"
#include <iostream.h>
#include <stdlib.h>
#include <cmath>
#include "Random.h"
#include "RanluxEngine.h"
// random number engines from StarClassLibrary
#include "RandGauss.h"
#include "StFtpcTrackMaker/StFtpcPoint.hh"

static RanluxEngine engine;

StFtpcFastSimu::StFtpcFastSimu(StFtpcGeantReader *geantReader,
			       StFtpcParamReader *paramReader,
			       TClonesArray *pointarray,
			       TClonesArray *geantarray)
{
  //-----------------------------------------------------------------------
  
  // store Readers in data members
  mParam=paramReader;
  mGeant=geantReader;

  // allocate memory for local storage
  // we need local arrays here that we can mess around in
  nPoints=mGeant->numberOfHits();
  mPoint=new StFtpcReducedPoint[nPoints];
  mGeantPoint=new StFtpcGeantPoint[nPoints];

  //    check that fppoint and gepoint are large enough to hold g2t_ftp_hit

  //  Read paramenter tables and inititialize  
  ffs_ini();
  
  // hh Transfer the usable g2t_ftp_hit-data into fppoint and gepoint
  ffs_hit_rd();
  
  // jr   mark each hit with a row-number and a individual id
  ffs_tag();   
  
  //       generate pad response and spatial resolutions
  // mk  in the routine FFS_GEN_PADRES the routine FFS_HIT_SMEAR is called
  ffs_gen_padres();
  
  // Check for hit-merging
  
  ffs_merge_tagger();
  
  TClonesArray &point = *pointarray;
  TClonesArray &gpoint = *geantarray;
  
  for (Int_t i = 0; i < nPoints; i++) {
    // use (default) copy constructor for StFtpcGeantPoint
    new(gpoint[i]) StFtpcGeantPoint(mGeantPoint[i]);
    // as StFtpcPoint is in different package, we have to copy data 
    // from StFtpcReducedPoint
    // hgrrrumpf!!!
    new(point[i]) StFtpcPoint();
    ((StFtpcPoint *)pointarray->At(i))->SetX(mPoint[i].GetX());
    ((StFtpcPoint *)pointarray->At(i))->SetY(mPoint[i].GetY());
    ((StFtpcPoint *)pointarray->At(i))->SetZ(mPoint[i].GetZ());
    ((StFtpcPoint *)pointarray->At(i))->SetXerr(mPoint[i].GetXerr());
    ((StFtpcPoint *)pointarray->At(i))->SetYerr(mPoint[i].GetYerr());
    ((StFtpcPoint *)pointarray->At(i))->SetZerr(mPoint[i].GetZerr());
    ((StFtpcPoint *)pointarray->At(i))->SetPadRow(mPoint[i].GetPadRow());
    ((StFtpcPoint *)pointarray->At(i))->SetSector(mPoint[i].GetSector());
    ((StFtpcPoint *)pointarray->At(i))->SetNumberPads(mPoint[i].GetNumberPads());
    ((StFtpcPoint *)pointarray->At(i))->SetNumberBins(mPoint[i].GetNumberBins());
    ((StFtpcPoint *)pointarray->At(i))->SetMaxADC(mPoint[i].GetMaxADC());
    ((StFtpcPoint *)pointarray->At(i))->SetCharge(mPoint[i].GetCharge());
    ((StFtpcPoint *)pointarray->At(i))->SetFlags(mPoint[i].GetFlags());
    ((StFtpcPoint *)pointarray->At(i))->SetSigmaPhi(mPoint[i].GetSigmaPhi());
    ((StFtpcPoint *)pointarray->At(i))->SetSigmaR(mPoint[i].GetSigmaR());
  }

  
  delete[] mGeantPoint;
  delete[] mPoint;
}

StFtpcFastSimu::~StFtpcFastSimu()
{
  cout << "StFtpcFastSimu destructed" << endl;
}

int StFtpcFastSimu::ffs_gen_padres()
  {
    // Local Variables:
    float check1, check2;
    float xi, yi, zi, phi, Rh, Vh, Timeb;
    float sigTimeb, sigPhi, sigma_tr;
    float sigma_l, sigma_z;
    float alpha, lambda;
    float r, pt; 
    float twist_cosine, twist, theta, cross_ang;
    
    // Variables to call ffs_hit_smear
    
    float xo, yo, zo, sigma_x, sigma_y;
    
    // Loop Variables

    int k;

    //------------------------------------------------------------------------
    // Parameters

    const float l=2.0;

    //-----------------------------------------------------------------------


//     HepJamesRandom engine;
    RandGauss quasiRandom(engine);


    //     loop over tracks

    //mk Check: is s_rad=0 and s_azi=0 then no hit-shifting is wanted. 

    check1 = abs((int)s_rad[0])+abs((int)s_rad[1])+abs((int)s_rad[2])+abs((int)s_rad[3]);
    check2 = abs((int)s_azi[0])+abs((int)s_azi[1])+abs((int)s_azi[2])+abs((int)s_azi[3]);
	
    if(check1==0. && check2 == 0. ) 
      {
           return FALSE;
      }

    //mk end of check

    for(k = 0; k<nPoints; k++)
      {
	//            get space point

	xi = mPoint[k].GetX();
	yi = mPoint[k].GetY();
	zi = mPoint[k].GetZ();

	//             calculate spatial resolution along the padrow

	//       Azimuthal angle Phi (in radians)
	phi = atan2((double) yi,(double) xi);
	if(phi<0)
	  phi += C_2PI;

	//       Radius of Hit
	Rh = sqrt(xi*xi + yi*yi);

	//       Drift velocity at hit [cm/microsec]
	Vh = Vhm[0] + Vhm[1]*Rh + Vhm[2]*Rh*Rh + Vhm[3]*Rh*Rh*Rh;

	//       Arrival time at Readout-Chambers in microsec    
	Timeb = Tbm[0] + Tbm[1]*Rh + Tbm[2]*Rh*Rh + Tbm[3]*Rh*Rh*Rh;

	// Angle-Determination:
	// Calculate Dip- and Crossing-Angle
	// For low-momenta particles the ionization is sometimes pointlike;
	// then at least one of the momentum-components is 0; therefore set the
	// angles to 0

	if((mGeantPoint[k].GetLocalMomentum(0)==0.)||
	   (mGeantPoint[k].GetLocalMomentum(0)==0.)||
	   (mGeantPoint[k].GetLocalMomentum(0)==0.))
	  {
	    alpha  = 0.;
	    lambda = 0.;
	  }
	else
	  {
	    // twist-angle:
	    r  = sqrt ( sqr(mGeant->x(k)) + 
			sqr(mGeant->y(k)));
	    pt = sqrt ( sqr(mGeant->pLocalX(k)) + 
			sqr(mGeant->pLocalY(k)));
	    twist_cosine=(mGeant->pLocalX(k)*mGeant->x(k)+
			  mGeant->pLocalY(k)*mGeant->y(k))/(r*pt);
	    if ( twist_cosine > 1.0 ) 
	      twist_cosine = 1.0;
	    if ( twist_cosine < -1.0 ) 
	      twist_cosine = -1.0;
	    twist = C_DEG_PER_RAD*acos(twist_cosine);

	    // dip-angle:
            theta = C_DEG_PER_RAD*
	      atan2((double) (pt*cos(twist*C_RAD_PER_DEG)),
		    (double) ((mGeant->z(k)
			       /fabs(mGeant->z(k)))*
			      mGeant->pLocalZ(k)));
	    
	    // crossing-angle: 
            cross_ang = C_DEG_PER_RAD*
	      atan2((double) (pt*cos(fabs(90.-twist)*C_RAD_PER_DEG)),   
		    (double) ((mGeant->z(k)/fabs(mGeant->z(k)))*
			      mGeant->pLocalZ(k)));
	    alpha  = fabs(cross_ang*C_RAD_PER_DEG);
            if(alpha>(C_PI_2))
	      alpha=C_PI-alpha;
	    
	    lambda = fabs(theta*C_RAD_PER_DEG);
            if(lambda>(C_PI_2)) 
	      lambda=C_PI-lambda;

	  }

	//>>>>>>>>>>>>>>> AZIMUTHAL Direction>>>>>>>>>>>>>>>>>>>>>>

	//   error sigma in azimuthal-direc. (microns)
	sigPhi = err_azi[0]+err_azi[1]*Rh+err_azi[2]*sqr(Rh)+err_azi[3]*sqr(Rh)*Rh;

	//   Sigma_tr response
	sigma_tr = sqrt(sqr(sigPhi)+(sqr(l*tan(alpha))));

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//>>>>>>>>>>>>>>> RADIAL Direction>>>>>>>>>>>>>>>>>>>>>>>>>

	//       error sigma in r-direc. (microns)
	sigTimeb = err_rad[0] + err_rad[1]*Rh + err_rad[2]*sqr(Rh) + 
	  err_rad[3]*sqr(Rh)*Rh;

	//mk Sigma longitudinal at anode [micron] 
	sigma_l = sqrt(sqr(sigTimeb)+sqr(l*tan(lambda)));

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//>>>>>>>>>>>>>>> Z Direction>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	//   Sector width
	sigma_z = 0.;

	//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//mk  Va = velocity at readout-ch. in [cm/microsec] 
	//              sigma_l = sigTimeb * Va

	//-> Smearing

	ffs_hit_smear( phi, xi, yi, zi, &xo, &yo, &zo,
		       sigma_l, sigma_tr,&sigma_z,&sigma_x,&sigma_y,
		       &quasiRandom);

	mPoint[k].SetX(xo);
	mPoint[k].SetY(yo);
	mPoint[k].SetZ(zo);
	mPoint[k].SetXerr(sigma_x);
	mPoint[k].SetYerr(sigma_y);
	mPoint[k].SetZerr(sigma_z);
      }
    return TRUE;
  }

int StFtpcFastSimu::ffs_hit_rd()
  {
    int ih, ih_max;
    float phi , dphi; 
    
    //-----------------------------------------------------------------------

    //     set ih_max 
      
    ih_max = mGeant->numberOfHits();

    //     loop over MC hits and extract sector-row information
    
    for(ih= 0; ih<ih_max; ih++)
      {
	// specification variables:
	// changed to new structures (gepoint) 01/29/98 hh
	mGeantPoint[ih].SetGeantPID(mGeant->trackPid(ih));
	mGeantPoint[ih].SetTrackPointer(mGeant->track(ih)+1);
	mGeantPoint[ih].SetPrimaryTag(mGeant->trackType(ih));
	if(mGeant->trackCharge(ih) < 0.0)
	  {
	    mGeantPoint[ih].SetPrimaryTag(-1*mGeantPoint[ih].GetPrimaryTag());
	  }
	int temp=mGeant->geantVolume(ih) - 100;
	if (temp > 10)
	  {
	    temp -= 90;
	  }
	mPoint[ih].SetPadRow(temp);

	// Vertex-Momenta
	mGeantPoint[ih].SetVertexMomentum(mGeant->pVertexX(ih),mGeant->pVertexY(ih),mGeant->pVertexZ(ih));
	
	// Vertex position
        mGeantPoint[ih].SetVertexPosition(mGeant->vertexX(ih),mGeant->vertexY(ih),mGeant->vertexZ(ih));

	// Secondary production process
        mGeantPoint[ih].SetGeantProcess(mGeant->productionProcess(ih));

	//local momentum
        mGeantPoint[ih].SetLocalMomentum(mGeant->pLocalX(ih),mGeant->pLocalY(ih),mGeant->pLocalZ(ih));

        mPoint[ih].SetX(mGeant->x(ih));
        mPoint[ih].SetY(mGeant->y(ih));
        mPoint[ih].SetZ(mGeant->z(ih));

	//sector number 
	phi = atan2((double) mGeant->y(ih),
                    (double) mGeant->x(ih));
	if ( phi < 0.0 ) 
	  phi += C_2PI;
	dphi = myModulo((phi-phimin+C_2PI), C_2PI);
        mPoint[ih].SetSector( int ( dphi/phisec ) + 1);  
	
	//de/dx
	mPoint[ih].SetMaxADC(int( 8000000.0 * mGeant->energyLoss(ih) ));
	mPoint[ih].SetCharge(6*mPoint[ih].GetMaxADC());
	// for de/dx simulations, introduce de/dx smearing + adjust factors! hh
	mPoint[ih].SetNumberPads(4);
	mPoint[ih].SetNumberBins(3);
	// possibly make n_pads, n_bins dependent on exact position, charge
      }

    //     set the row counter
    nPoints=ih_max;

    return TRUE;
  }

int StFtpcFastSimu::ffs_hit_smear(float phi, 
				  float xi, 
				  float yi, 
				  float zi, 
				  float *xo, 
				  float *yo, 
				  float *zo,
				  float st_dev_l, 
				  float st_dev_t,
				  float *st_dev_z,
				  float *st_dev_x,
				  float *st_dev_y,
				  RandGauss *quasiRandom)
  {
    // Local Variables

    float err_pad;		//ERROR ALONG PAD ROW FOR SPACE POINT 
    float err_drft;		//ERROR ALONG DRIFT DIRECTION FOR SPACE POINT
    float smear;
    float err_x, err_y;// err_z;

    //-----------------------------------------------------------------------

    // Evaluate the sigmas in x- and y-direction out of the sigmas in long. 
    // and transverse direction
    
    *st_dev_x = sqr(cos(phi)) * sqr(st_dev_l) + sqr(sin(phi)) * sqr(st_dev_t);
    *st_dev_x = (sqrt(*st_dev_x))/10000.;  // microns -> cm

    *st_dev_y = sqr(sin(phi)) * sqr(st_dev_l) + sqr(cos(phi)) * sqr(st_dev_t);
    *st_dev_y = sqrt (*st_dev_y)/10000.;   // microns -> cm

    smear=(float) quasiRandom->shoot();
    err_pad = st_dev_t*smear; // box->sigma
    
    smear=(float) quasiRandom->shoot();
    err_drft = st_dev_l*smear;
    
    //        err_Z = st_dev_Z*SMEAR

    // Evaluate hit-shift in x- and y-direction as well as the new points xo and yo

    err_x = cos(phi) * err_drft - sin(phi) * err_pad;
    *xo = xi + (err_x/10000);  

    err_y = sin(phi) * err_drft + cos(phi) * err_pad;
    *yo = yi + (err_y/10000);
    
    //        ZO = ZI + err_Z
    *zo = zi;

    return TRUE;
  }

int StFtpcFastSimu::ffs_ini()
  {
    //------   TEMPORARY:  put in parameter table   ------------------
    const float phi_origin=90.;
    const float phi_sector=360/mParam->numberOfSectors();
    //------   TEMPORARY:  put in parameter table   ------------------
    //-----------------------------------------------------------------------
    // mk
    ri = mParam->sensitiveVolumeInnerRadius()+0.25;
    ra = mParam->sensitiveVolumeOuterRadius()-0.25;
    padrows = mParam->numberOfPadrowsPerSide();

    //mk Drift-Velocity:
    Vhm[0]  = mParam->vDriftEstimates(0);
    Vhm[1]  = mParam->vDriftEstimates(1);
    Vhm[2]  = mParam->vDriftEstimates(2);
    Vhm[3]  = mParam->vDriftEstimates(3);

    //mk Drift_Time
    Tbm[0] = mParam->tDriftEstimates(0);
    Tbm[1] = mParam->tDriftEstimates(1);
    Tbm[2] = mParam->tDriftEstimates(2);
    Tbm[3] = mParam->tDriftEstimates(3);
    
    // upper entries of sig_arrays temporarily used to store error parameters

    //mk Radial Sigma
    s_rad[0] = mParam->sigmaRadialEstimates(0);
    s_rad[1] = mParam->sigmaRadialEstimates(1);
    s_rad[2] = 0;
    s_rad[3] = 0;

    //mk Azimuthal Sigma
    s_azi[0] = mParam->sigmaAzimuthalEstimates(0);
    s_azi[1] = mParam->sigmaAzimuthalEstimates(1);
    s_azi[2] = 0;
    s_azi[3] = 0;
    
    //Radial Error
    err_rad[0] = mParam->sigmaRadialEstimates(2);
    err_rad[1] = mParam->sigmaRadialEstimates(3);
    err_rad[2] = 0;
    err_rad[3] = 0;

    //Azimuthal Error
    err_azi[0] = mParam->sigmaAzimuthalEstimates(2);
    err_azi[1] = mParam->sigmaAzimuthalEstimates(3);
    err_azi[2] = 0;
    err_azi[3] = 0;
    
    cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    cout << "Parametrization for vd, Td, sig_rad and sig_azi:" << endl;
    cout << "vd=" << Vhm[0]<<"+"<<Vhm[1]<<"x+"<<Vhm[2]<<"xx+"
	 <<Vhm[3]<<"xxx" << endl;
    cout << "Td="<<Tbm[0]<<"+"<<Tbm[1]<<"x+"<<Tbm[2]<<"xx+"<<Tbm[3]
	 <<"xxx" << endl;
    cout << "sig_rad="<<s_rad[0]<<"+"<<s_rad[1]<<"x+"<<s_rad[2]
	 <<"xx+"<<s_rad[3]<<"xxx" << endl;
    cout << "sig_azi="<<s_azi[0]<<"+"<<s_azi[1]<<"x+"<<s_azi[2]
	 <<"xx+"<<s_azi[3]<<"xxx" << endl;
    cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" << endl;
    
    // if slong <0, use built-in longitudinal resolution from radial diffusion 
    // if slong >=0, longintudinal resolution is constant and equal to slong
    slong  = -1;

    // Drift velocity at anode (Ranode = ra from/ftpc_params/ ) [cm/microsec]
    Va = Vhm[0] + Vhm[1]*ra + Vhm[2]*sqr(ra) + Vhm[3]*ra*sqr(ra);

    //     phi of sector number 1 origin
    phimin = C_RAD_PER_DEG * phi_origin;

    //     size of one sector in phi
    phisec = C_RAD_PER_DEG * phi_sector;

    //     a cluster is too close to lower sector boundary if it is
    //     not more than 2 pads away 
    sector_phi_min = mParam->radiansPerBoundary()/2 + 2*mParam->radiansPerPad();

    //     a cluster is too close to upper sector boundary if it is
    //     not more than 2 pads away 
    sector_phi_max = phisec-sector_phi_min;

    return TRUE;
  }

int StFtpcFastSimu::ffs_merge_tagger()
  {
    // Local Variables:
    int id_1, id_2, rem_count1, rem_count2, n_gepoints;
    float sig_azi_1, v1, sig_rad_1;
    float dist_rad_in, dist_rad_out;
//     float r2, phi2;
    float delta_azi, delta_r;
    float sigazi[MXMROW], sigrad[MXMROW];
    float r1[MXMROW], phi1[MXMROW];

    // Loop Variables
    int h,i,j,k;

    //-----------------------------------------------------------------------

    k=0;
    for(i=0; i<nPoints; i++)
      {
	mPoint[i].SetFlags(0);
	
	// azimuthal direction
	r1[i] = sqrt(sqr(mPoint[i].GetX()) + sqr(mPoint[i].GetY()));
	phi1[i] = atan2((double) mPoint[i].GetY(),
                        (double) mPoint[i].GetX());
	if ( phi1[i] < 0.0 ) 
	  phi1[i] += C_2PI;
	
	sig_azi_1 = s_azi[0] + s_azi[1]*r1[i] + 
	  s_azi[2]*sqr(r1[i]) + s_azi[3]*sqr(r1[i])*r1[i];
	mPoint[i].SetSigmaPhi(sig_azi_1/10000*(r1[i]/ra));

	sig_azi_1 = (2.5*sig_azi_1)/10000; // micron -> cm
	sigazi[i] = sig_azi_1*(r1[i]/ra);

	// radial direction
	v1 = Vhm[0]+Vhm[1]*r1[i] + Vhm[2]*sqr(r1[i])+
	  Vhm[3]*sqr(r1[i])*r1[i];
	sig_rad_1 = s_rad[0] + s_rad[1]*r1[i] +  
	  s_rad[2]*sqr(r1[i]) + s_rad[3]*sqr(r1[i])*r1[i];

	mPoint[i].SetSigmaR(sig_rad_1/10000*(v1/Va));

	sig_rad_1 = (2.5*sig_rad_1)/10000; // micron -> cm
	sigrad[i] = sig_rad_1*(v1/Va);

      }

    for(h=0;h<SIZE;h++)
      {
	if(nrowmax[h]==0) 
	  continue;
	
        for(i=0;i<nrowmax[h];i++)
	  {
	    id_1 = nrow[h][i]-1;
	    
	    for(j=i+1; j<nrowmax[h]; j++)
	      {
		id_2 = nrow[h][j]-1;
 		if((mPoint[id_2].GetFlags()==1000) || 
 		   (mPoint[id_2].GetSector()!=mPoint[id_1].GetSector()))
 		  continue;
		
		delta_azi = fabs(phi1[id_1]-phi1[id_2])
		  *((r1[id_1]+r1[id_2])/2);
		delta_r = fabs(r1[id_1]-r1[id_2]);
		
		if((delta_r < (2 * sigrad[id_1])) &&
		   (delta_azi < (2 * sigazi[id_1])))
		  {
		    // mark clusters as unfolded 
		    if(mPoint[id_1].GetFlags() != 1000)
		      {
			mPoint[id_1].SetFlags(1);
		      }
		    mPoint[id_2].SetFlags(1);
		  }
		
		if((delta_r<sigrad[id_1]) &&
		   (delta_azi<sigazi[id_1]))
		  {
		    k++;
		    
		    // merge clusters, mark second for removal
		    if(mPoint[id_1].GetFlags() != 1000)
		      {
			mPoint[id_1].SetFlags(8);
		      }
		    mPoint[id_2].SetFlags(1000);
		    mPoint[id_1].SetMaxADC(mPoint[id_1].GetMaxADC()+
					   mPoint[id_2].GetMaxADC() / 2);
		      // maxadc adds up somehow, maybe more, maybe less
		    mPoint[id_1].SetCharge(mPoint[id_1].GetCharge() +
					   mPoint[id_2].GetCharge());
		    // charge adds up exactly
		    mPoint[id_1].SetX((mPoint[id_1].GetX() +
				       mPoint[id_2].GetX()) / 2);
		    mPoint[id_1].SetY((mPoint[id_1].GetY() +
				       mPoint[id_2].GetY()) / 2);
		    mPoint[id_1].SetZ((mPoint[id_1].GetZ() +
				       mPoint[id_2].GetZ()) / 2);
		    // positions average more or less
		    mPoint[id_1].SetSigmaPhi(mPoint[id_1].GetSigmaPhi()+
					     mPoint[id_2].GetSigmaPhi() / 2);
		    mPoint[id_1].SetSigmaR(mPoint[id_1].GetSigmaR()+
					   mPoint[id_2].GetSigmaR() / 2);
		    //widths add up somehow...
		  }
	      }
	  }
      }
    
    rem_count1=0;
    rem_count2=0;

    // now remove merged clusters and those on sector border
    cout << "remove merged and cut-off hits" << endl;
    id_1 = 0;
    id_2 = 0;
    n_gepoints = 0;
      
    dist_rad_in = s_rad[0] + s_rad[1]*ri + s_rad[2]*sqr(ri) + 
      s_rad[3]*ri*ri*ri;
    dist_rad_out = s_rad[0] + s_rad[1]*ra + s_rad[2]*sqr(ra) + 
      s_rad[3]*ra*ra*ra;
    // minimum distance in cm = 2*cluster sigma in microns
    dist_rad_in /= 5000;
    dist_rad_out /= 5000;
      
    while(id_2 < nPoints)
      {
	delta_azi = phi1[id_2] 
	  -myModulo(((mPoint[id_2].GetSector()-1)*phisec+phimin),(C_2PI));
	if (delta_azi<0.0) 
	  delta_azi += C_2PI;

	if((delta_azi < sector_phi_min) || 
           (delta_azi > sector_phi_max) ||
	   (r1[id_2] < ri+dist_rad_in) ||
	   (r1[id_2] > ra-dist_rad_out) ||
           (mPoint[id_2].GetFlags() > 256))
	  {
            if(mPoint[id_2].GetFlags() > 256)
	      {
		rem_count1++;
	      }
            else
	      {
		rem_count2++;
	      }
	  }
	else
	  {
	    if ( id_2 == id_1 )
	      {
		id_1++;
		n_gepoints++;
	      }
	    else
	      {
		mPoint[id_1].SetPadRow(mPoint[id_2].GetPadRow());
		mPoint[id_1].SetSector(mPoint[id_2].GetSector());
		mPoint[id_1].SetNumberPads(mPoint[id_2].GetNumberPads());
		mPoint[id_1].SetNumberBins(mPoint[id_2].GetNumberBins());
		mPoint[id_1].SetMaxADC(mPoint[id_2].GetMaxADC());
		mPoint[id_1].SetCharge(mPoint[id_2].GetCharge());
		mPoint[id_1].SetFlags(mPoint[id_2].GetFlags());
		mGeantPoint[id_1].SetTrackPointer(mGeantPoint[id_2].GetTrackPointer());
		mGeantPoint[id_1].SetGeantPID(mGeantPoint[id_2].GetGeantPID());
		mGeantPoint[id_1].SetPrimaryTag(mGeantPoint[id_2].GetPrimaryTag());
		mPoint[id_1].SetX(mPoint[id_2].GetX());
		mPoint[id_1].SetY(mPoint[id_2].GetY());
		mPoint[id_1].SetZ(mPoint[id_2].GetZ());
		mPoint[id_1].SetSigmaPhi(mPoint[id_2].GetSigmaPhi());
		mPoint[id_1].SetSigmaR(mPoint[id_2].GetSigmaR());
		mGeantPoint[id_1].SetVertexMomentum(mGeantPoint[id_2].GetVertexMomentum(0),
						    mGeantPoint[id_2].GetVertexMomentum(1),
						    mGeantPoint[id_2].GetVertexMomentum(2));
		mGeantPoint[id_1].SetLocalMomentum(mGeantPoint[id_2].GetLocalMomentum(0),
						   mGeantPoint[id_2].GetLocalMomentum(1),
						   mGeantPoint[id_2].GetLocalMomentum(2));
		mGeantPoint[id_1].SetVertexPosition(mGeantPoint[id_2].GetVertexPosition(0),
						    mGeantPoint[id_2].GetVertexPosition(1),
						    mGeantPoint[id_2].GetVertexPosition(2));
		mGeantPoint[id_1].SetGeantProcess(mGeantPoint[id_2].GetGeantProcess());
		id_1++;
		n_gepoints++;
	      }
	  }
	id_2++;
      }
	
    nPoints = n_gepoints;
      
    cout << "Deleted " << rem_count1 << " merged clusters." << endl;
    cout << "Deleted " << rem_count2 << " clusters on sector limit." << endl;
      
    return TRUE;
  }

int StFtpcFastSimu::ffs_tag()
  {
      int i, k, num_nok;
      //-----------------------------------------------------------------------
      //     Tag hits according to row. Up to maximum row number=20

      //   nrowmax(k) is the #of hits in hitplane k
      //   k is the hitplane-# given by gstar

      for(k=0; k<SIZE; k++)
	{
	  nrowmax[k] = 0;
	}

      num_nok=nPoints;

      if (num_nok > MXMROW )
	{
	  cout << "FFS WARNING: number of Points (" << nPoints
	       << ") greater than mxmrow ("<< MXMROW<<")"<< endl;
	  cout << "              Setting num_nok =  mxmrow ("
	       << MXMROW <<")" << endl;
	  num_nok=MXMROW;
	}
 
      for(i = 0; i< num_nok; i++)
	{
	  k = mPoint[i].GetPadRow();
	  nrowmax[k-1]++;
	  nrow[k-1][nrowmax[k-1]-1] = i+1;
	}

      return TRUE;

  }

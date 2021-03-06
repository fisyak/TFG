/*
 *
 * Author: M. Mustafa
 *
 *
 **********************************************************
 * $Log: StPxlDigmapsSim.cxx,v $
 * Revision 1.1.2.1  2017/09/11 20:15:14  dongx
 * Pxl slow simulator added
 *
 */

#include <stdio.h>

#include "StMessMgr.h"
#include "Stypes.h"
#include "StPxlDigmapsSim.h"
#include "StMcEvent/StMcPxlHit.hh"
#include "StMcEvent/StMcPxlHitCollection.hh"
#include "StPxlDbMaker/StPxlDb.h"
#include "tables/St_pxlControl_Table.h"
#include "tables/St_pxlDigmapsSim_Table.h"
#include "tables/St_HitError_Table.h"
#include "StPxlRawHitMaker/StPxlRawHitCollection.h"
#include "StPxlRawHitMaker/StPxlRawHit.h"

#include "TDataSet.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TDataSet.h"
#include "TObjectSet.h"
#include "TF1.h"
#include "TVector3.h"

#include "DIGMAPS/digplane.h"
#include "DIGMAPS/digadc.h"
#include "DIGMAPS/digtransport.h"
#include "DIGMAPS/digparticle.h"
#include "DIGMAPS/digevent.h"

StPxlDigmapsSim::StPxlDigmapsSim(const Char_t *name): StPxlISim(name),
  mRndGen(nullptr), mDigPlane(nullptr), mDigAdc(nullptr), mDigTransport(nullptr),
  mPxlDb(nullptr)
{}

StPxlDigmapsSim::~StPxlDigmapsSim()
{
  if (mRndGen) delete mRndGen;
  if (mDigAdc) delete mDigAdc;
  if (mDigPlane) delete mDigPlane;
  if (mDigTransport) delete mDigTransport;
  if (mPxlDb) delete mPxlDb;
  if (mdEdxvsBGNorm) delete mdEdxvsBGNorm;
}

Int_t StPxlDigmapsSim::initRun(const TDataSet& calib_db, const TObjectSet* pxlDbDataSet, const Int_t run)
{
  LOG_INFO << "StPxlDigmapsSim::init()" << endm;

  mRndGen = new TRandom3(time(NULL));

  if (pxlDbDataSet != 0)
  {
    mPxlDb = (StPxlDb *)pxlDbDataSet->GetObject();
    if (!mPxlDb)
    {
      LOG_ERROR << "StPxlDigmapsSim - E - mPxlDb is not available" << endm;
      return kStErr;
    }
    else
    {
      LOG_INFO << "StPxlDigmapsSim - Using geometry from pxlDB" << endm;
    }
  }
  else
  {
    LOG_INFO << "StPxlDigmapsSim - Using ideal geometry" << endm;
  }

  pxlControl_st const* const pxlControlTable = mPxlDb->pxlControl();
  mSensorGoodStatusMin = pxlControlTable[0].sensorGoodStatusMin;
  mSensorGoodStatusMax = pxlControlTable[0].sensorGoodStatusMax;
  mRowColumnGoodStatus = pxlControlTable[0].rowColumnGoodStatus;

  pxlDigmapsSim_st const* const pxlDigmapsSimTable = mPxlDb->pxlDigmapsSim();
  // set ADC threshold(s)
  Int_t nAdcBits = 1;
  Int_t nAdcThresholds = int(TMath::Power(2.0, nAdcBits) - 1);
  Bool_t adcLinear = 0;
  Float_t adcElectronConversion = -999;
  Float_t adcThresholds[] = {pxlDigmapsSimTable[0].adcThreshold}; // one threshold only
  Float_t adcLsb = adcThresholds[0];

  if (!mDigAdc) mDigAdc = new DIGADC();
  //  cout << mDigAdc << endl;
  mDigAdc->SetNbits(nAdcBits);
  mDigAdc->SetNThresholds(nAdcThresholds);
  mDigAdc->SetADC_linear(adcLinear);
  mDigAdc->SetLSB(adcLsb);
  mDigAdc->SetElectron_Conversion(adcElectronConversion);
  mDigAdc->SetADC_thresholds(adcThresholds, nAdcThresholds);

  // set transport
  Int_t transportChargeModel = (Int_t)pxlDigmapsSimTable[0].transportChargeModel;
  Float_t   transportRangeLimit_InPitchUnit = pxlDigmapsSimTable[0].transportRangeLimit;
  Float_t   transport_l1dimgauslor_Norm_g_1st = pxlDigmapsSimTable[0].transport_Norm_g_1st;
  Float_t   transport_l1dimgauslor_x0_g_1st = pxlDigmapsSimTable[0].transport_x0_g_1st;
  Float_t   transport_l1dimgauslor_sigma_g_1st = pxlDigmapsSimTable[0].transport_sigma_g_1st;
  Float_t   transport_l1dimgauslor_Gamma_lor_1st = pxlDigmapsSimTable[0].transport_Gamma_lor_1st;
  Float_t   transport_l1dimgauslor_x0_lor_1st = pxlDigmapsSimTable[0].transport_x0_lor_1st;
  Float_t   transport_l1dimgauslor_norm_lor_1st = pxlDigmapsSimTable[0].transport_norm_lor_1st;
  Float_t   transport_l1dimgauslor_Norm_g_2nd = pxlDigmapsSimTable[0].transport_Norm_g_2nd;
  Float_t   transport_l1dimgauslor_x0_g_2nd = pxlDigmapsSimTable[0].transport_x0_g_2nd;
  Float_t   transport_l1dimgauslor_sigma_g_2nd = pxlDigmapsSimTable[0].transport_sigma_g_2nd;
  Float_t   transport_l1dimgauslor_Gamma_lor_2nd = pxlDigmapsSimTable[0].transport_Gamma_lor_2nd;
  Float_t   transport_l1dimgauslor_x0_lor_2nd = pxlDigmapsSimTable[0].transport_x0_lor_2nd;
  Float_t   transport_l1dimgauslor_norm_lor_2nd = pxlDigmapsSimTable[0].transport_norm_lor_2nd;

  if (!mDigTransport) mDigTransport = new DIGTransport();
  mDigTransport->SetChargeModel(transportChargeModel);
  mDigTransport->SetRangeLimit_InPitchUnit(transportRangeLimit_InPitchUnit);
  mDigTransport->Setf1dimgauslor_Norm_g_1st(transport_l1dimgauslor_Norm_g_1st);
  mDigTransport->Setf1dimgauslor_x0_g_1st(transport_l1dimgauslor_x0_g_1st);
  mDigTransport->Setf1dimgauslor_sigma_g_1st(transport_l1dimgauslor_sigma_g_1st);
  mDigTransport->Setf1dimgauslor_Gamma_lor_1st(transport_l1dimgauslor_Gamma_lor_1st);
  mDigTransport->Setf1dimgauslor_x0_lor_1st(transport_l1dimgauslor_x0_lor_1st);
  mDigTransport->Setf1dimgauslor_norm_lor_1st(transport_l1dimgauslor_norm_lor_1st);
  mDigTransport->Setf1dimgauslor_Norm_g_2nd(transport_l1dimgauslor_Norm_g_2nd);
  mDigTransport->Setf1dimgauslor_x0_g_2nd(transport_l1dimgauslor_x0_g_2nd);
  mDigTransport->Setf1dimgauslor_sigma_g_2nd(transport_l1dimgauslor_sigma_g_2nd);
  mDigTransport->Setf1dimgauslor_Gamma_lor_2nd(transport_l1dimgauslor_Gamma_lor_2nd);
  mDigTransport->Setf1dimgauslor_x0_lor_2nd(transport_l1dimgauslor_x0_lor_2nd);
  mDigTransport->Setf1dimgauslor_norm_lor_2nd(transport_l1dimgauslor_norm_lor_2nd);

  // set plane
  Float_t planePitchX = StPxlConsts::kPixelSize * 1e4;
  Float_t planePitchY = StPxlConsts::kPixelSize * 1e4;
  Float_t planeEpitaxialThickness = pxlDigmapsSimTable[0].planeEpitaxialThickness;
  Float_t planeNoiseElectrons = pxlDigmapsSimTable[0].planeNoiseElectrons;
  Int_t planeNpixelsX = kNumberOfPxlRowsOnSensor;   // row - local X - DIGMAPS X
  Int_t planeNpixelsY = kNumberOfPxlColumnsOnSensor; // column - local z - DIGMAPS Y
  Float_t planeTemprature = pxlDigmapsSimTable[0].planeTemprature;
  Float_t planeIonizationEnergy = pxlDigmapsSimTable[0].planeIonizationEnergy;
  Float_t planeSegmentSize = pxlDigmapsSimTable[0].planeSegmentSize;
  Float_t planeMaximumSegmentSize = pxlDigmapsSimTable[0].planeMaxSegmentSize;
  Float_t planeMaximumChargePerSegment = pxlDigmapsSimTable[0].planeMaxChargePerSegment;
  Float_t planeDiffusionMaximumRangeInX = pxlDigmapsSimTable[0].planeDiffusionMaxX;
  Float_t planeDiffusionMaximumRangeInY = pxlDigmapsSimTable[0].planeDiffusionMaxY;
  Float_t planeReflexionCoefficient = pxlDigmapsSimTable[0].planeReflexCoefficient;
  Float_t planeBasicModel_SigmaTenMicrons = pxlDigmapsSimTable[0].planeMod_SigTenMicrons;

  if (!mDigPlane) mDigPlane = new DIGPlane();
  mDigPlane->SetPitch(planePitchX, planePitchY);
  mDigPlane->SetNpixels(planeNpixelsX, planeNpixelsY);
  mDigPlane->SetDimensions(planePitchX * planeNpixelsX, planePitchY * planeNpixelsY, planeEpitaxialThickness);
  mDigPlane->SetNoiseElectrons(planeNoiseElectrons);
  mDigPlane->SetTemperature(planeTemprature);
  mDigPlane->SetIonizationEnergy(planeIonizationEnergy);
  mDigPlane->SetSegmentSize(planeSegmentSize);
  mDigPlane->SetMaximumSegmentSize(planeMaximumSegmentSize);
  mDigPlane->SetMaximumChargePerSegment(planeMaximumChargePerSegment);
  mDigPlane->SetDiffusionMaximumRange(planeDiffusionMaximumRangeInX, planeDiffusionMaximumRangeInY);
  mDigPlane->SetReflexionCoefficient(planeReflexionCoefficient);
  mDigPlane->SetBasicModel_SigmaTenMicrons(planeBasicModel_SigmaTenMicrons);

  mEnergyLandauMean = pxlDigmapsSimTable[0].energyLandauMean;
  mEnergyLandauSigma = pxlDigmapsSimTable[0].energyLandauSigma;
  for(int i=0;i<10;i++) {
    mScalePar[i] = (double)(pxlDigmapsSimTable[0].par[i]);
  }
  mdEdxvsBGNorm = new TF1("dEdxvsBGNorm",this,&StPxlDigmapsSim::dEdxvsBGNorm,0.1,1e5,6);
  mdEdxvsBGNorm->SetParameters(&mScalePar[0]);

  mResAddX = pxlDigmapsSimTable[0].resAddX;
  mResAddZ = pxlDigmapsSimTable[0].resAddZ;
  
  // initialize the sensor offset values due to other contributions
  for(int i=0;i<kNumberOfPxlSectors;i++) {
    for(int j=0;j<kNumberOfPxlLaddersPerSector;j++) {
      for(int k=0;k<kNumberOfPxlSensorsPerLadder;k++) {
        mOffsetX[i][j][k] = mRndGen->Gaus(0.,mResAddX);
        mOffsetZ[i][j][k] = mRndGen->Gaus(0.,mResAddZ);
      }
    }
  }
  return kStOk;
}
//____________________________________________________________
Int_t StPxlDigmapsSim::addPxlRawHits(const StMcPxlHitCollection& mcPxlHitCol,
                                     StPxlRawHitCollection& pxlRawHitCol)
{
  for (unsigned int iSec = 0; iSec < mcPxlHitCol.numberOfSectors(); ++iSec)
  {
    const StMcPxlSectorHitCollection* mcPxlSectorHitCol = mcPxlHitCol.sector(iSec);
    if (!mcPxlSectorHitCol) continue;

    for (unsigned int iLad = 0; iLad < mcPxlSectorHitCol->numberOfLadders(); ++iLad)
    {
      const StMcPxlLadderHitCollection* mcPxlLadderHitCol = mcPxlSectorHitCol->ladder(iLad);
      if (!mcPxlLadderHitCol) continue;

      for (unsigned int iSen = 0; iSen < mcPxlLadderHitCol->numberOfSensors(); ++iSen)
      {
        const StMcPxlSensorHitCollection* mcPxlSensorHitCol = mcPxlLadderHitCol->sensor(iSen);
        if (!mcPxlSensorHitCol) continue;

        if (!goodSensor(iSec+1, iLad+1, iSen+1))
        {
          LOG_DEBUG << " ##Skip bad sensor " << iSec << "/" << iLad << "/" << iSen << " StatusCode = " << mPxlDb->sensorStatus(iSec + 1, iLad + 1, iSen + 1) << endm;
          continue;
        }

        UInt_t nSenHits = mcPxlSensorHitCol->hits().size();
        LOG_DEBUG << "Sector/Ladder/Sensor = " << iSec+1 << "/" << iLad+1 << "/" << iSen+1 << ". Number of sensor hits = " << nSenHits << endm;

        // Loop over hits in the sensor
        for (unsigned int iHit = 0; iHit < nSenHits; ++iHit)
        {
          StMcPxlHit const* const mcPix = mcPxlSensorHitCol->hits()[iHit];
          if (!mcPix) continue;

          int sensorId = ( iSec * kNumberOfPxlLaddersPerSector + iLad ) * kNumberOfPxlSensorsPerLadder + iSen + 1;
          DIGEvent fdigevent{};
          fillDigmapsEvent(sensorId, mcPix, fdigevent);
          
          int n_pxl = 0; // number of pixels passing digQ threshold
          int n_pxl_wmask = 0; // number of pixels passing digQ threshold and with mask
          for (int j = 0; j < fdigevent.GetReadoutmap()->GetNpixels(); ++j)
          {
            if(fdigevent.GetReadoutmap()->GetDigitalCharge()[j] > 0)
            {
              ++n_pxl;
              int const Npixel = fdigevent.GetReadoutmap()->GetPixelMap()[j];
              int const iy = Npixel / mDigPlane->GetNpixelsX();
              int const ix = (mDigPlane->GetNpixelsX()-1) - Npixel % mDigPlane->GetNpixelsX();  // local X direction goes from row number MAX->0

              if (goodPixel(iSec+1 , iLad+1, iSen+1, ix, iy))
              {
                ++n_pxl_wmask;
                int const idTruth = mcPix->parentTrack()? mcPix->parentTrack()->key() : -1;
                LOG_DEBUG << "  adding a new pxlRawHit  sector/ladder/sensor = " << iSec + 1 << "/" << iLad + 1 << "/" << iSen + 1 << " ix/iy=" << ix << "/" << iy << " idTruth=" << idTruth << endm;

                StPxlRawHit* pxlRawHit = new StPxlRawHit(iSec+1, iLad+1, iSen+1, ix, iy, idTruth);
                pxlRawHitCol.addRawHit(*pxlRawHit);
                delete pxlRawHit;
                // pxlRawHitCol.addRawHit(StPxlRawHit(iSec + 1, iLad + 1, iSen + 1, ix, iy, idTruth));
              }
            }
          }

          LOG_DEBUG << " ReadoutMap Npixels = " << fdigevent.GetReadoutmap()->GetNpixels() << "\t Npixels Dig Cut = " << n_pxl << "\t Npixels Mask Cut = " << n_pxl_wmask << endm;
        }  // end for hits
      } // end sensor loop
    } // end ladder loop
  } // end sector loop

  return kStOK;
}

void StPxlDigmapsSim::fillDigmapsEvent(int sensorId, StMcPxlHit const* const mcPix, DIGEvent& fdigevent) const
{
  TVector3 inPos{};
  TVector3 outPos{};
  calculateIncidencePositions(sensorId, mcPix, inPos, outPos);
  float const betagamma = betaGamma(mcPix->parentTrack());
  float const depositedEnergy = calculateDepositedEnergy((inPos-outPos).Mag(), betagamma);
  LOG_DEBUG << " Energy deposit for this hit = " << depositedEnergy << "\t totalLength = " << (inPos-outPos).Mag() << "\t betagamma = " << betagamma << endm;

  DIGParticle fdigparticle(inPos.X(), inPos.Y(), inPos.Z(), outPos.X(), outPos.Y(), outPos.Z(), depositedEnergy);

  //---------charge generation
  fdigparticle.ComputeChargeDeposition(mDigPlane->GetSegmentSize(), mDigPlane->GetMaximumSegmentSize(), mDigPlane->GetMaximumChargePerSegment());
  //---------charge transport
  fdigparticle.ComputeChargeTransport(mDigPlane, mDigTransport);
  //---------random noise (should be removed if one wants to avoid double noise on double hit pixels)
  fdigparticle.AddRandomNoise(mDigPlane);
  //---------ADC (stored only for reference)
  fdigparticle.AnalogToDigitalconversion(mDigAdc, mDigPlane);

  fdigevent.AddParticle(fdigparticle);
  auto chargevector = fdigparticle.GetAnalogCharge();
  auto pixmapvector = fdigparticle.GetPixelMap();

  for (int ipix = 0 ; ipix < fdigparticle.GetNpixels() ; ++ipix)
  {
    (fdigevent.GetReadoutmap())->UpdatePixel(chargevector[ipix], pixmapvector[ipix]);
  }

  //---------Build readout map:
  (fdigevent.GetReadoutmap())->AnalogToDigitalconversion(mDigAdc, mDigPlane);

  LOG_DEBUG << " DigPlane NpixX = " << mDigPlane->GetNpixelsX() << "\t DigPlane NpixY = " << mDigPlane->GetNpixelsY() << endm;
}

float StPxlDigmapsSim::calculateDepositedEnergy(float const totalLength, float const betagamma) const
{
  float const energyMPV = mEnergyLandauMean * totalLength;
  float const energySigma = mEnergyLandauSigma * totalLength;
  float energy = mRndGen->Landau(energyMPV, energySigma) * mdEdxvsBGNorm->Eval(betagamma);
  LOG_DEBUG << " energyMPV/Sigma = " << energyMPV << " " << energySigma << "\t dEdx correction = " << mdEdxvsBGNorm->Eval(betagamma) << endm;

  int count=0;
  while (energy > 50000 && count < 50) // count to avoid infinite loop in case of large energy deposit
  {
    LOG_WARN << "Energy too high -> Energy regenerated " << energy << " MPV/sigma= " << energyMPV << " " << energySigma << "  Seed = " << mRndGen->GetSeed() << endm;
    energy = mRndGen->Landau(energyMPV, energySigma) * mdEdxvsBGNorm->Eval(betagamma);
    count++;
  }

  return energy;
}

Double_t StPxlDigmapsSim::dEdxvsBGNorm(Double_t *x, Double_t *par)
{
  double beta2 = x[0]*x[0]/(1+x[0]*x[0]);
  double delta = TMath::Log(x[0])+par[2];
  if(x[0]<=10.) {
    return par[0]/beta2*(0.5*TMath::Log(x[0]*x[0]*par[1])-beta2-delta/2.);
  } else {
    return par[3] + par[4]*TMath::Log(x[0])+par[5]*TMath::Log(x[0])*TMath::Log(x[0]);
  }
}

void StPxlDigmapsSim::calculateIncidencePositions(int sensorId, StMcPxlHit const* const mcPix, TVector3& inPos, TVector3& outPos) const
{
  int iSec = (sensorId - 1) / (kNumberOfPxlLaddersPerSector * kNumberOfPxlSensorsPerLadder);
  int iLad = (sensorId - 1) % (kNumberOfPxlLaddersPerSector * kNumberOfPxlSensorsPerLadder) / kNumberOfPxlSensorsPerLadder;
  int iSen = (sensorId - 1) % kNumberOfPxlSensorsPerLadder;
  
  double localPixHitPos[3] = {mcPix->position().x() + mOffsetX[iSec][iLad][iSen], mcPix->position().y(), mcPix->position().z() + mOffsetZ[iSec][iLad][iSen]};
  double localPxlMom[3] = {mcPix->localMomentum().x(), mcPix->localMomentum().y(), mcPix->localMomentum().z()};

  // convert to um (all DIGMAPS units are in um)
  localPixHitPos[0] *= 10000.0;
  localPixHitPos[1] *= 10000.0;
  localPixHitPos[2] *= 10000.0;

  //               LOG_DEBUG << "globalPixHitPos = " << globalPixHitPos[0] << " " << globalPixHitPos[1] << " " << globalPixHitPos[2] << endm;
  LOG_DEBUG << "localPixHitPos = " << localPixHitPos[0] << " " << localPixHitPos[1] << " " << localPixHitPos[2] << "\n";
  LOG_DEBUG << "localPxlMom    = " << localPxlMom[0] << " " << localPxlMom[1] << " " << localPxlMom[2] << "\n";
  LOG_DEBUG << " DigPlane dimensions " << mDigPlane->GetXdimension() << " " << mDigPlane->GetYdimension() << " " << mDigPlane->GetZdimension() << endm;

  inPos.SetX(localPixHitPos[0] + mDigPlane->GetXdimension() / 2.0 + (mDigPlane->GetZdimension() / 2.0 - localPixHitPos[1]) * localPxlMom[0] / localPxlMom[1]);
  inPos.SetY(localPixHitPos[2] + mDigPlane->GetYdimension() / 2.0 + (mDigPlane->GetZdimension() / 2.0 - localPixHitPos[1]) * localPxlMom[2] / localPxlMom[1]);
  inPos.SetZ(mDigPlane->GetZdimension() / 2.0);

  outPos.SetX(localPixHitPos[0] + mDigPlane->GetXdimension() / 2.0 + (-mDigPlane->GetZdimension() / 2.0 - localPixHitPos[1]) * localPxlMom[0] / localPxlMom[1]);
  outPos.SetY(localPixHitPos[2] + mDigPlane->GetYdimension() / 2.0 + (-mDigPlane->GetZdimension() / 2.0 - localPixHitPos[1]) * localPxlMom[2] / localPxlMom[1]);
  outPos.SetZ(-mDigPlane->GetZdimension() / 2.0);

  LOG_DEBUG << "inHitPos = " << inPos.X() << " " << inPos.Y() << " " << inPos.Z() << "\n";
  LOG_DEBUG << "outHitPos = " << outPos.X() << " " << outPos.Y() << " " << outPos.Z() << endm;
}

float StPxlDigmapsSim::betaGamma(StMcTrack const* const mcTrk) const
{
  if (!mcTrk) return 1.;

  float betagamma = 1.;
  float const m = mcTrk->fourMomentum().m();
  if(m>0) betagamma = mcTrk->momentum().mag()/m;
  LOG_DEBUG << " track info: " << mcTrk->momentum().mag() << " " << m << " " << betagamma << endm;
  if(m>1.0) LOG_WARN << "  large mass particle " << mcTrk->geantId() << " " << mcTrk->pdgId() << endm;

  return betagamma;
}

bool StPxlDigmapsSim::goodPixel(int const sec, int const lad, int const sen, int const ix, int const iy) const
{
  // check raw and column status and hot pixels
  return (mPxlDb->rowStatus(sec, lad, sen, ix) == mRowColumnGoodStatus) &&
         (mPxlDb->columnStatus(sec, lad, sen, iy) == mRowColumnGoodStatus) &&
         (!mPxlDb->pixelHot(sec, lad, sen, ix, iy));
}

bool StPxlDigmapsSim::goodSensor(int const sec, int const lad, int const sen) const
{
  return (mPxlDb->sensorStatus(sec, lad, sen) >= mSensorGoodStatusMin) &&
         (mPxlDb->sensorStatus(sec, lad, sen) <= mSensorGoodStatusMax);
}

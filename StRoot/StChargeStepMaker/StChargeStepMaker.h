// $Id: StChargeStepMaker.h,v 1.1 2000/07/12 18:43:11 hardtke Exp $
// $Log: StChargeStepMaker.h,v $
// Revision 1.1  2000/07/12 18:43:11  hardtke
// initial version -- very slow, but seems to work
//
#ifndef StChargeStepMaker_H
#define StChargeStepMaker_H



//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StChargeStepMaker: Maker that calculates drift velocity using charge step//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "StMaker.h"
class St_type_shortdata;

class St_tpg_detector;
class St_tss_tsspar;

class St_tcl_sector_index;
class St_tpc_DriftVelocity;
class St_tfc_adcxyz;
class StTpcDb;

class TH1F;
enum eRegions {innerWest, outerWest, innerEast, outerEast};

class StChargeStepMaker : public StMaker {

 public: 

  StChargeStepMaker(const char *name="tpc_charge");
  virtual       ~StChargeStepMaker(); 
  virtual void   tclPixTransOn() {tclPixTrans(kTRUE);}
  virtual void   tclPixTransOff(){tclPixTrans();} 
  
  virtual Int_t  Init();
  virtual Int_t  Make();
  virtual Int_t  Finish();
  virtual void   Clear(const char *opt);
  virtual void   PrintInfo();
  virtual const char *GetCVS() const
    {static const char cvs[]="Tag $Name:  $ $Id: StChargeStepMaker.h,v 1.1 2000/07/12 18:43:11 hardtke Exp $ built "__DATE__" "__TIME__ ; return cvs;}
  TH1S* step[4];  //!1=west,inner:2=west,outer:3=east,inner:4=east,outer
  TH1S* derivative[4];
  TH1F* result[4];
  float GetWeightedMean(TH1S* inputHist);
  
 private:

  Bool_t                 m_tclPixTransOn;       // switch for pixel translation evaluation
  Bool_t                 m_raw_data_tpc;        // bool used to check if there is pixel data

  // define the tables used
  St_tpg_detector*       m_tpg_detector;  	//! TPC geometry parameters 
  St_tcl_sector_index*   m_tcl_sector_index; 	//! Current sector
	                                     	//  for processing
  St_tss_tsspar*         m_tsspar;        	//! parameters for slow simulator running.
  St_tfc_adcxyz*         adcxyz;                //! raw pixel table

  void   tclPixTrans(Bool_t flag=kFALSE){m_tclPixTransOn=flag;}
  void   MakeHistograms(); 
  void   InitHistograms(); 
  StTpcDb*  theDb; //!
  int       theGuess; //!


 protected:

  
  ClassDef(StChargeStepMaker, 1)       //Cint definition
};

#endif

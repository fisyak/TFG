// $Id: St_ems_Maker.cxx,v 1.8 1999/03/03 04:12:14 fisyak Exp $
// $Log: St_ems_Maker.cxx,v $
// Revision 1.8  1999/03/03 04:12:14  fisyak
// replace kStErr to kStWarn
//
// Revision 1.7  1999/02/17 15:54:18  fisyak
// Adjust geometry paameters from GEANT
//
// Revision 1.6  1999/02/16 18:15:44  fisyak
// Check in the latest updates to fix them
//
// Revision 1.5  1998/12/15 22:38:45  akio
// Add some comments
//
// Revision 1.4  1998/12/06 10:25:45  akio
// re-commit
//
// Revision 1.3  1998/12/06 09:57:13  akio
// put histgrams
//
// Revision 1.2  1998/12/03 22:30:09  akio
// Include dep_e_toadc and emc_adc_sim
//
// Revision 1.1  1998/11/30 21:18:30  fisyak
// ems raw data Maker
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_ems_Maker is class for begin_html <FONT COLOR="RED">EMc Simulation</FONT> end_html dataset//
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include "St_ems_Maker.h"
#include "StChain.h"
#include "St_DataSetIter.h"
#include "emc/St_ems_interface2_Module.h"
#include "emc/St_dep_e_toadc_Module.h"
#include "emc/St_emc_adc_sim_Module.h"
#include "emc/St_dep_e_toadc_Module.h"
ClassImp(St_ems_Maker)

//_____________________________________________________________________________
St_ems_Maker::St_ems_Maker(const char *name, const char *title):StMaker(name,title){
// Use Constructor from StMaker
   drawinit=kFALSE;
}
//_____________________________________________________________________________
St_ems_Maker::~St_ems_Maker(){
}
//_____________________________________________________________________________
Int_t St_ems_Maker::Init(){
   St_DataSetIter       local(gStChain->DataSet("params"));
   m_ems_control     = (St_ems_control *)      local("emc/ems/ems_control");
   m_control_toadc   = (St_control_toadc *)    local("emc/ems/control_toadc");
   m_ems_cal_control = (St_ems_cal_control *)  local("emc/cal/ems_cal_control");
   m_org_ped_bemc_h  = (St_emc_calib_header *) local("emc/cal/org_ped_bemc_h");
   m_org_ped_bemc    = (St_emc_pedestal *)     local("emc/cal/org_ped_bemc");
   m_org_slp_bemc_h  = (St_emc_calib_header *) local("emc/cal/org_slp_bemc_h");
   m_org_slp_bemc    = (St_emc_adcslope *)     local("emc/cal/org_slp_bemc");
   //Just to chek the contents!
   //   emc_calib_header_st *b = m_org_ped_bemc_h->GetTable();
   //cout << b[0].nmodule    << "   "<< b[0].neta    << "   "<< b[0].nsub    <<endl;
   //ems_control_st *a = m_ems_control->GetTable();
   //cout << a->nmodule[0] << "   "<< a->nmodule[1] << "   "<< a->nmodule[2] <<endl;
   //m_ems_control->ls("*");
   //m_ems_control->Print(0,1);
   return StMaker::Init();
}
//_____________________________________________________________________________
Int_t St_ems_Maker::Make(){
  if (gStChain->DataSet("geom")){
    St_DataSetIter       geom(gStChain->DataSet("geom"));
    m_calb_calg   = (St_calb_calg   *) geom("calb_calg");
    if (!m_DataSet->GetList())  {    //if DataSet is empty, create table and fill it
      St_ems_hits *ems_hits_bemc = new St_ems_hits("ems_hits_bemc", 9600);m_DataSet->Add(ems_hits_bemc);
      St_ems_hits *ems_hits_bsmd = new St_ems_hits("ems_hits_bsmd",38000);m_DataSet->Add(ems_hits_bsmd);
      St_ems_hits *ems_hits_eemc = new St_ems_hits("ems_hits_eemc",10000);m_DataSet->Add(ems_hits_eemc);
      St_ems_hits *ems_hits_esmd = new St_ems_hits("ems_hits_esmd",20000);m_DataSet->Add(ems_hits_esmd);
      
      St_emc_hits *emc_hits_bemc  = new St_emc_hits("emc_hits_bemc",  4800);m_DataSet->Add(emc_hits_bemc);
      St_emc_hits *emc_hits_bprs  = new St_emc_hits("emc_hits_bprs",  4800);m_DataSet->Add(emc_hits_bprs);
      St_emc_hits *emc_hits_bsmde = new St_emc_hits("emc_hits_bsmde",18000);m_DataSet->Add(emc_hits_bsmde);
      St_emc_hits *emc_hits_bsmdp = new St_emc_hits("emc_hits_bsmdp",18000);m_DataSet->Add(emc_hits_bsmdp);
      St_emc_hits *emc_hits_eemc  = new St_emc_hits("emc_hits_eemc",  1440);m_DataSet->Add(emc_hits_eemc);
      St_emc_hits *emc_hits_eprs  = new St_emc_hits("emc_hits_eprs",  1440);m_DataSet->Add(emc_hits_eprs);
      St_emc_hits *emc_hits_esmde = new St_emc_hits("emc_hits_esmde",10000);m_DataSet->Add(emc_hits_esmde);
      St_emc_hits *emc_hits_esmdp = new St_emc_hits("emc_hits_esmdp",10000);m_DataSet->Add(emc_hits_esmdp);
      
      St_DataSetIter geant(gStChain->DataSet("geant"));
      St_g2t_event  *g2t_event  = (St_g2t_event  *) geant("g2t_event");
      St_g2t_vertex *g2t_vertex = (St_g2t_vertex *) geant("g2t_vertex");
      St_g2t_track  *g2t_track  = (St_g2t_track  *) geant("g2t_track");
      St_g2t_emc_hit *g2t_emc_hit = (St_g2t_emc_hit *) geant("g2t_emc_hit");
      St_g2t_emc_hit *g2t_smd_hit = (St_g2t_emc_hit *) geant("g2t_smd_hit");
      St_g2t_emc_hit *g2t_eem_hit = (St_g2t_emc_hit *) geant("g2t_eem_hit");
      St_g2t_emc_hit *g2t_esm_hit = (St_g2t_emc_hit *) geant("g2t_esm_hit");
      
      //if there is no g2t_hit table, create dummy one 
      int i1=0, i2=0, i3=0, i4=0;
      if (!g2t_emc_hit) {g2t_emc_hit = new St_g2t_emc_hit("g2t_emc_hit",1); i1=1;}
      if (!g2t_smd_hit) {g2t_smd_hit = new St_g2t_emc_hit("g2t_smd_hit",1); i2=1;}
      if (!g2t_eem_hit) {g2t_eem_hit = new St_g2t_emc_hit("g2t_eem_hit",1); i3=1;}
      if (!g2t_esm_hit) {g2t_esm_hit = new St_g2t_emc_hit("g2t_esm_hit",1); i4=1;}
      
      //calling emc_interface2 which get hit from g2t_hit, store in ems_hit
      Int_t Res_ems =  ems_interface2 (g2t_event, g2t_vertex, g2t_track,
				       g2t_emc_hit, g2t_smd_hit, g2t_eem_hit, g2t_esm_hit,
				       m_calb_calg, m_ems_control,
				       ems_hits_bemc, ems_hits_bsmd, ems_hits_eemc, ems_hits_esmd);
      if(!i1){delete g2t_emc_hit;}
      if(!i2){delete g2t_smd_hit;}
      if(!i3){delete g2t_eem_hit;}
      if(!i4){delete g2t_esm_hit;}
      if (Res_ems != kSTAFCV_OK) {
	cout << "***** Problem with ems_interface2 *****" << endl; return kStWarn;
      }
      
      //calling dep_e_toadc which convert geant energy to ADC
      Res_ems = dep_e_toadc(m_ems_control,m_control_toadc,ems_hits_bemc,emc_hits_bemc,emc_hits_bprs);
      if (Res_ems != kSTAFCV_OK) {
	cout << endl << "***** Problem with dep_e_toadc (BEMC/BPRS) *****" << endl; return kStWarn;
      }
      Res_ems = dep_e_toadc(m_ems_control,m_control_toadc,ems_hits_bsmd,emc_hits_bsmde,emc_hits_bsmdp);
      if (Res_ems != kSTAFCV_OK) {
	cout << endl << "***** Problem with dep_e_toadc (BSMD) *****" << endl; return kStWarn;
      } 
      Res_ems = dep_e_toadc(m_ems_control,m_control_toadc,ems_hits_eemc,emc_hits_eemc,emc_hits_eprs);
      if (Res_ems != kSTAFCV_OK) {
	cout << endl << "***** Problem with dep_e_toadc (EEMC/EPRS) *****" << endl; return kStWarn;
      }
      Res_ems = dep_e_toadc(m_ems_control,m_control_toadc,ems_hits_esmd,emc_hits_esmde,emc_hits_esmdp);
      if (Res_ems != kSTAFCV_OK) {
	cout << endl << "***** Problem with dep_e_toadc (ESMD) *****" << endl;  return kStWarn;
      }
      
      //calling emc_adc_sim which gives pedestal and alope variations.
      //for the moment, this is only for BEMC
      Res_ems = emc_adc_sim(m_ems_control, m_ems_cal_control,
			    m_org_ped_bemc_h, m_org_ped_bemc, 
			    m_org_slp_bemc_h, m_org_slp_bemc, emc_hits_bemc);
      if (Res_ems != kSTAFCV_OK) {
	cout << endl << "***** Problem with emc_adc_sim (BEMC) *****" << endl;  return kStWarn;
      }
    }
  }
  return kStOK;
}
//_____________________________________________________________________________
void St_ems_Maker::PrintInfo(){
  printf("**************************************************************\n");
  printf("* $Id: St_ems_Maker.cxx,v 1.8 1999/03/03 04:12:14 fisyak Exp $\n");
  printf("**************************************************************\n");
  if (gStChain->Debug()) StMaker::PrintInfo();
}



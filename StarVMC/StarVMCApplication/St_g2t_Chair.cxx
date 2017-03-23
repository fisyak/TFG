#include "Stiostream.h"
#include "StarMCHits.h"
#include "St_g2t_Chair.h"
ClassImp(St_g2t_Chair);
ClassImp(St_g2t_ctf_hitC)
ClassImp(St_g2t_emc_hitC)
ClassImp(St_g2t_epd_hitC)
ClassImp(St_g2t_fgt_hitC)
ClassImp(St_g2t_ftp_hitC)
ClassImp(St_g2t_gem_hitC)
ClassImp(St_g2t_ist_hitC)
ClassImp(St_g2t_mwc_hitC)
ClassImp(St_g2t_pix_hitC)
ClassImp(St_g2t_pmd_hitC)
ClassImp(St_g2t_rch_hitC)
ClassImp(St_g2t_ssd_hitC)
ClassImp(St_g2t_svt_hitC)
ClassImp(St_g2t_tpc_hitC)
ClassImp(St_g2t_mtd_hitC)
ClassImp(St_g2t_etr_hitC)
ClassImp(St_g2t_vpd_hitC)
ClassImp(St_g2t_fst_hitC)
ClassImp(St_g2t_fts_hitC)
ClassImp(St_g2t_hpd_hitC)
ClassImp(St_g2t_ist_hitC)
Int_t St_g2t_Chair::fDebug = 0;
#define G2TBookTrackHit(A) \
  static g2t_ ## A ## _hit_st g2t_ ## A ## _hit;\
  memset(&g2t_ ## A ## _hit, 0, sizeof(g2t_ ## A ## _hit_st));		\
  St_g2t_ ## A ## _hit *table = (St_g2t_ ## A ## _hit*) GetThisTable(); \
  Int_t  nok = table->GetNRows()+1; 
#define G2TFillNextTrack(A) \
  g2t_ ## A ## _hit_st *row = table->GetTable(); \
  for (Int_t i = nok - 2; i >= 0; i--) if ((row+i)->track_p == vect.iTrack) {g2t_ ## A ## _hit.next_tr_hit_p = i+1; break;} 
#define G2TFillTrackHitB(A) \
  g2t_ ## A ## _hit.id            = nok;				\
  g2t_ ## A ## _hit.x[0]          = vect.Middle.Global.xyzT.X();	\
  g2t_ ## A ## _hit.x[1]          = vect.Middle.Global.xyzT.Y();	\
  g2t_ ## A ## _hit.x[2]          = vect.Middle.Global.xyzT.Z();	\
  g2t_ ## A ## _hit.p[0]          = vect.Middle.Global.pxyzE.X();	\
  g2t_ ## A ## _hit.p[1]          = vect.Middle.Global.pxyzE.Y();	\
  g2t_ ## A ## _hit.p[2]          = vect.Middle.Global.pxyzE.Z();	\
  g2t_ ## A ## _hit.tof           = vect.Middle.Global.xyzT.T();	\
  g2t_ ## A ## _hit.de            = vect.AdEstep;			\
  g2t_ ## A ## _hit.ds            = vect.AStep;				\
  g2t_ ## A ## _hit.track_p       = vect.iTrack;			\
  g2t_ ## A ## _hit.volume_id     = vect.VolumeId;
#define G2TFillTrackHitLocal(A) \
  g2t_ ## A ## _hit.id            = nok;				\
  g2t_ ## A ## _hit.x[0]          = vect.Middle.Local.xyzT.X();	\
  g2t_ ## A ## _hit.x[1]          = vect.Middle.Local.xyzT.Y();	\
  g2t_ ## A ## _hit.x[2]          = vect.Middle.Local.xyzT.Z();	\
  g2t_ ## A ## _hit.p[0]          = vect.Middle.Local.pxyzE.X();	\
  g2t_ ## A ## _hit.p[1]          = vect.Middle.Local.pxyzE.Y();	\
  g2t_ ## A ## _hit.p[2]          = vect.Middle.Local.pxyzE.Z();	\
  g2t_ ## A ## _hit.tof           = vect.Middle.Local.xyzT.T();	\
  g2t_ ## A ## _hit.de            = vect.AdEstep;			\
  g2t_ ## A ## _hit.ds            = vect.AStep;				\
  g2t_ ## A ## _hit.track_p       = vect.iTrack;			\
  g2t_ ## A ## _hit.volume_id     = vect.VolumeId;
#define G2TFillTrackHit(A)			\
  G2TFillTrackHitB(A)				\
  G2TFillNextTrack(A)				\
  table->AddAt(&g2t_ ## A ## _hit);
#define G2TFillTrackRCHit(A)			\
  G2TFillTrackHitB(A)				\
  table->AddAt(&g2t_ ## A ## _hit);
#define G2TFillTrackHitNoTOF(A)						\
  g2t_ ## A ## _hit.id            = nok;				\
  g2t_ ## A ## _hit.x[0]          = vect.Middle.Global.xyzT.X();	\
  g2t_ ## A ## _hit.x[1]          = vect.Middle.Global.xyzT.Y();	\
  g2t_ ## A ## _hit.x[2]          = vect.Middle.Global.xyzT.Z();	\
  g2t_ ## A ## _hit.p[0]          = vect.Middle.Global.pxyzE.X();	\
  g2t_ ## A ## _hit.p[1]          = vect.Middle.Global.pxyzE.Y();	\
  g2t_ ## A ## _hit.p[2]          = vect.Middle.Global.pxyzE.Z();	\
  g2t_ ## A ## _hit.de            = vect.AdEstep;			\
  g2t_ ## A ## _hit.track_p       = vect.iTrack;			\
  g2t_ ## A ## _hit.volume_id     = vect.VolumeId;			\
  table->AddAt(&g2t_ ## A ## _hit);
#define G2TTrackHit(A)				\
void St_g2t_## A ##_hitC::Fill(GHit_t &vect) {	\
  G2TBookTrackHit(A);				\
  G2TFillTrackHit(A);				\
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_hitsC::Fill(GHit_t &vect) {}
//________________________________________________________________________________
void St_g2t_ctf_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(ctf);
  g2t_ctf_hit.s_track       = vect.Sleng;	\
  G2TFillTrackHit(ctf);
 }
//________________________________________________________________________________
void St_g2t_emc_hitC::Fill(GHit_t &vect) {
  static g2t_emc_hit_st g2t_emc_hit;
  memset(&g2t_emc_hit, 0, sizeof(g2t_emc_hit_st));
  St_g2t_emc_hit *table = (St_g2t_emc_hit*) GetThisTable(); if (Debug()) table->Print(0,5);
  Int_t nok = table->GetNRows()+1;
  g2t_emc_hit.id            = nok;
  g2t_emc_hit.de            = vect.birk;// AdEstep;
  g2t_emc_hit.track_p       = vect.iTrack;
  g2t_emc_hit.volume_id     = vect.VolumeId;
  g2t_emc_hit.x             = vect.Middle.Global.xyzT.X();
  g2t_emc_hit.y             = vect.Middle.Global.xyzT.Y();
  g2t_emc_hit.z             = vect.Middle.Global.xyzT.Z();
  g2t_emc_hit_st *emc = table->GetTable();
  for (Int_t i = 0; i < nok - 1; i++, emc++) {
    if (emc->volume_id == g2t_emc_hit.volume_id &&
	emc->track_p   == g2t_emc_hit.track_p) {
      emc->de += g2t_emc_hit.de;
      if (emc->de > 0) {
	Float_t *xOld = &emc->x;
	Float_t *xNew = &g2t_emc_hit.x;
	for (Int_t i = 0; i < 3; i++) 
	  xNew[i] = ((emc->de - g2t_emc_hit.de)*xNew[i] +  g2t_emc_hit.de*xOld[i])/emc->de;
      }
      return;
    }
  }
  table->AddAt(&g2t_emc_hit);    
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_epd_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(epd);
  G2TFillTrackHitNoTOF(epd);
  if (Debug()) table->Print(nok-1,2);}
//________________________________________________________________________________
G2TTrackHit(fgt);
G2TTrackHit(ftp);
G2TTrackHit(gem);
//________________________________________________________________________________
void St_g2t_ist_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(ist);
  G2TFillTrackHitLocal(ist);
  table->AddAt(&g2t_ist_hit);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_mwc_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(mwc);
  g2t_mwc_hit.s_track       = vect.Sleng;				\
  G2TFillTrackHit(mwc);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_pix_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(pix);
  G2TFillTrackHitLocal(pix);
  table->AddAt(&g2t_pix_hit);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_pmd_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(pmd);
  G2TFillTrackHitNoTOF(pmd);
  if (Debug()) table->Print(nok-1,2);
}
//________________________________________________________________________________
void St_g2t_rch_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(rch);
  G2TFillTrackRCHit(rch);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
//G2TTrackHit(ssd);
void St_g2t_ssd_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(ssd);
  G2TFillTrackHitLocal(ssd);
  table->AddAt(&g2t_ssd_hit);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
G2TTrackHit(svt);
//________________________________________________________________________________
void St_g2t_tpc_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(tpc);
  
  Double_t GeKin            = vect.Middle.Global.pxyzE.E() - vect.Mass;
  Double_t lgam             = 0;
  if (vect.Mass > 0 && GeKin > 0 && vect.Charge != 0) 
    lgam = TMath::Log10(GeKin/vect.Mass);
  g2t_tpc_hit.lgam          = lgam;
  G2TFillTrackHit(tpc);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_etr_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(etr);
  
  Double_t GeKin            = vect.Middle.Global.pxyzE.E() - vect.Mass;
  Double_t lgam             = 0;
  if (vect.Mass > 0 && GeKin > 0 && vect.Charge != 0) 
    lgam = TMath::Log10(GeKin/vect.Mass);
  g2t_etr_hit.lgam          = lgam;
  G2TFillTrackHit(etr);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_mtd_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(mtd);
  G2TFillTrackHitLocal(mtd);
  g2t_mtd_hit.s_track       = vect.Sleng;	
  table->AddAt(&g2t_mtd_hit);
  if (Debug()) table->Print(nok-1,2);		\
}
//________________________________________________________________________________
void St_g2t_vpd_hitC::Fill(GHit_t &vect) {
  G2TBookTrackHit(vpd);
  g2t_vpd_hit.s_track       = vect.Sleng;				
  G2TFillTrackHit(vpd);
}
//________________________________________________________________________________
G2TTrackHit(fst);
G2TTrackHit(fts);
G2TTrackHit(hpd);
G2TTrackHit(igt);

#include <stdio.h>
#include <stdlib.h>

#include "Jevp/StJevpPlot/JevpPlotSet.h"
#include "DAQ_READER/daqReader.h"
#include <DAQ_READER/daq_dta.h>
#include <DAQ_L3/daq_l3.h>
#include <DAQ_L4/daq_l4.h>
#include <TStyle.h>
#include "TVector3.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <TH1I.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TFile.h> 
#include <TProfile.h>
#include "TStopwatch.h"
#include <math.h>
#include <DAQ_HLT/daq_hlt.h>
#include "RTS/include/HLT/HLTFormats.h"

enum {
  e,
  Pi,
  K,
  P,
  D,
  T,
  He3,
  He4
};

class l4Builder : public JevpPlotSet {
 public:
    
/**
 * Struct
 * @param 
 * @return 
 * @exception 
 * @see 
 * @author 
 */
  struct hbt_event_info {
    int mult;
    float zvertex;
    int ntracks; 
    float track[10000][5];
  };
        
  struct eventCut_info {
    float zvertexMax;   float zvertexMin;
    float rvertexMax;  	float rvertexMin;
    float BEMCeastEnergyMax;	float BEMCeastEnergyMin;
    float BEMCwestEnergyMax;	float BEMCwestEnergyMin;
    float BEMC_EastWestDiffMax;	float BEMC_EastWestDiffMin;
    float multMax;      float multMin;
  };

  struct trackCut_info {
    int nHitsMin;       int nHitsMax;
    float dcaMin;       float dcaMax;
    float ptMin;        float ptMax;
    float etaMin;       float etaMax;
    float rapMin;       float rapMax;
  };

  struct hlt_diPionPair {
    int dau1NodeSN;
    int dau2NodeSN;
    float invariantMass;
    float pt;
    float psi;
    float tanl;
    float deltphi; // phi difference between two dau tracks
    //int dau1SelectionBit;
    //int dau2SelectionBit;
  };

  struct hlt_diElectronPair {
    int dau1NodeSN;
    int dau2NodeSN;
    float invariantMass;
    float pt;
    float psi;
    float tanl;
    int dau1SelectionBit;
    int dau2SelectionBit;
  };

  // UPC rho bank (pion pair)
  struct HLT_RHO {
    unsigned int nRhos;
    struct hlt_diPionPair PionPair[1000];
  };

  struct HLT_MTDDIMU {
    unsigned int      nEPairs; 
    struct hlt_diElectronPair ePair[1000];
  };

/**
 * Plots, Functions, Histograms.
 * @param 
 * @return 
 * @exception 
 * @see 
 * @author 
 */  
  JevpPlot *HltPlots[41];
  JevpPlot *BesGoodPlots[2];
  JevpPlot *BesMontinorPlots[2];
  JevpPlot *FixedTargetPlots[2];
  JevpPlot *FixedTargetMonitorPlots[2];
  JevpPlot *HeavyFragmentPlots[1];
  JevpPlot *JPsiPlots[14];
  JevpPlot *HltPlots_UPC[36];
  PlotHisto *ph;

  l4Builder(JevpServer *parent=NULL) : JevpPlotSet(parent) {
    plotsetname = (char *)"l4";
  }
 
  void initialize(int argc, char *argv[]);   
  void startrun(daqReader *rdr);
  void stoprun(daqReader *rdr); 
  void event(daqReader *rdr);
  static void main(int argc, char *argv[]);

  int eventCounter;
  char Currentrun[256];
  char CurrentNtuple[256];
  char Currentdir[256];
  char Destindir[256];
  char dEdxTheoDir[256];
  char dEdxMeanFiles[8][256];

  TStopwatch timer;

 private:
		
  void inputDedx();
  double getDedx(double p, const int name);
  void defineHltPlots();
  void defineBesGoodPlots();
  void defineBesMontinorPlots();
  void defineFixedTargetPlots();
  void defineFixedTargetMonitorPlots();
  void defineHeavyFragmentPlots();
  void defineJPsiPlots();
  void defineHltPlots_UPC();
  void setAllPlots();
  void writeHistogram();
  
  TF1 *fTheoDedx_e_pos;
  TF1 *fTheoDedx_e_neg; 
  TF1 *fTheoDedx_Pi_pos;
  TF1 *fTheoDedx_Pi_neg;
  TF1 *fTheoDedx_K_pos; 
  TF1 *fTheoDedx_K_neg; 
  TF1 *fTheoDedx_P_pos;
  TF1 *fTheoDedx_P_neg;
  TF1 *fTheoDedx_D_pos;
  TF1 *fTheoDedx_D_neg;
  TF1 *fTheoDedx_T_pos;
  TF1 *fTheoDedx_T_neg;
  TF1 *fTheoDedx_He3_pos;
  TF1 *fTheoDedx_He3_neg;
  TF1 *fTheoDedx_He4_pos;
  TF1 *fTheoDedx_He4_neg;

  int index;
  int primaryTracks ;
  int primaryTracks_UPC ;
  int runnumber;
  int iBin;
  int switch_BesGood;
  int switch_BesMonitor;
  int switch_FixedTarget;
  int switch_FixedTargetMonitor;
  int switch_HeavyFragment;  
  int switch_jpsi;
  int switch_upc;
  double innerGainPara;
  double outerGainPara;
  double BeamX;
  double BeamY;
  double pi;
  double twopi;
  double A;

  TH1I *hEvtsAccpt;

  // track
  TH1I *hnhits;
  TH1I *hnDedx; 
  TH1D *hDcaXy;
  TH1D *hDcaZ ;
  TH1D *hLn_dEdx;
  TH1D *hGlob_Pt;
  TH1D *hGlob_Phi;
  TH1D *hGlob_Eta;
  TH2F *hGlob_dEdx;
  TH1D *hPrim_Pt;
  TH1D *hPrim_Phi;
  TH1D *hPrim_Eta;
  TH2F *hPrim_dEdx;	
  TH1I *hnhits_UPC;
  TH1I *hnDedx_UPC; 
  TH1D *hDcaXy_UPC;
  TH1D *hDcaZ_UPC;
  TH1D *hLn_dEdx_UPC;
  TH1D *hGlob_Pt_UPC;
  TH1D *hGlob_Phi_UPC;
  TH1D *hGlob_Eta_UPC;
  TH2F *hGlob_dEdx_UPC;
  TH1D *hPrim_Pt_UPC;
  TH1D *hPrim_Phi_UPC;
  TH1D *hPrim_Eta_UPC;
  TH2F *hPrim_dEdx_UPC;

  // event
  TH1D *hVertexX; 
  TH1D *hVertexY;
  TH1D *hVertexZ;
  TH1D *hLm_VertexX;
  TH1D *hLm_VertexY;
  TH1D *hLm_VertexZ;
  TH1I *hglobalMult;
  TH1I *hprimaryMult;
  /*   TH1I *hLmPrimaryMult; */

  TH1D *hVertexX_UPC; 
  TH1D *hVertexY_UPC;
  TH1D *hVertexZ_UPC;
  TH1D *hLm_VertexX_UPC;
  TH1D *hLm_VertexY_UPC;
  TH1D *hLm_VertexZ_UPC;
  TH1I *hglobalMult_UPC;
  TH1I *hprimaryMult_UPC;

  // EMC
  TH1D *hMatchPhi_Diff;
  TH1D *hTowerEnergy ;
  TH1I *hTowerDaqId; 
  TH1I *hTowerSoftId;
  TH1D *hzEdge;
  TH2F *hTowerEtaPhi;

  TH1D *hMatchPhi_Diff_UPC;
  TH1D *hTowerEnergy_UPC;
  TH1I *hTowerDaqId_UPC; 
  TH1I *hTowerSoftId_UPC;
  TH1D *hzEdge_UPC;
  TH2F *hTowerEtaPhi_UPC;

  // di-e
  TH1D *hDiElectronInvMassTpxEmc;
  TH1D *hDiElectronInvMassTpxEmcBG;
  TH1D *hDiElectronInvMassFullRange;
  TH1D *hDiElectronInvMassFullRangeBG;
  TH1D *hDiElectronInvMassCut;
  TH1D *hDiElectronInvMassCutBG;
  TH2F *hdEdx_P1; 
  TH1D *hDaughter1P_TowerEnergy;
  TH1D *hDaughter1TpxEmcInverseBeta;
  TH2F *hdEdx_P2;
  TH1D *hDaughter2P_TowerEnergy;
  TH1D *hDaughter2TpxEmcInverseBeta; 
  TH1D *hDiLeptonRapidity;

  TH1D *hDiElectronInvMassFullRange_UPC;
  TH1D *hDiElectronInvMassFullRangeBG_UPC;
  TH2F *hdEdx_P1_UPC; 
  TH1D *hDaughter1P_TowerEnergy_UPC;
  TH2F *hdEdx_P2_UPC;
  TH1D *hDaughter2P_TowerEnergy_UPC;
  TH1D *hDiLeptonRapidity_UPC;

  // di-muon
  TH1D *hDiMuonInvMassFullRange;
  TH1D *hDiMuonInvMassFullRangeBG;
  TH1D *hDiMuonInvMassTpxCut;
  TH1D *hDiMuonInvMassTpxCutBG;

  // di-pion
  TH1D *hDiPionInvMassFullRange;
  TH1D *hDiPionInvMassFullRangeBG;
  TH1D *hDiPionDeltphi;

  // ToF
  TH1D *hLocalZ;
  TH1D *hLocalY;
  /*   TH2F *hTofprimaryMult; */
  TH2F *hInverseBeta;
  TH2F *hMatchId_fiberId;
  TH2F *hTrayID_TrgTime;
  TH1D *hchannelID;
  TH2F *hVzvpd_Vz ;
  TH1D *hVzDiff;
  /*   TH3D *hMatchannel3D ; */
	
  TH2F *hVzvpd_Vz_UPC ;
  TH1D *hVzDiff_UPC;

  // heavy fragments
  TH2F *hdEdx;
  TH2F *hdEdx_HeavyFragment;
  TH2F *hHFM_dEdx;
  TH2F *hdEdx_UPC;
  TH2F *hHFM_dEdx_UPC;

  // different event tpye
  TH1I *hUpc ;
  TH1I *hMtd ;
  TH1I *hNpeHt_25_NoZdc;
  TH1I *hVpdMb; 
  TH1I *hCentral;
  TH1I *hNpeHt_25;
  TH1I *hNpe;
  TH1I *hAtomcule ;
		
  // run-by-run display
  TH1D *hBeamX;
  TH1D *hBeamY; 
  TH1D *hInnerGain;
  TH1D *hOuterGain;
  TH1D *hMeanDcaXy;

  TH2D *hBesGoodVertexXY;
  TH1D *hBesGoodVr;
  TH2D *hBesMonitorVertexXY;
  TH1D *hBesMonitorVr;
  TH2D *hFixedTargetVertexXY;
  TH1D *hFixedTargetVr;
  TH2D *hFixedTargetMonitorVertexXY;
  TH1D *hFixedTargetMonitorVr;
  TH2D *hVertexXY;
  TH1D *hVertexR;

  ClassDef(l4Builder, 1);
};

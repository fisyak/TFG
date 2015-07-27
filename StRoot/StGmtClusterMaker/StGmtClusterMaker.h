//
// First Cluster Maker
// \class StGmtClusterMaker
// \authors K.S. Engle and Richard Witt (witt@usna.edu)
// based on StFgtClusterMaker
#ifndef STAR_StGmtClusterMaker_HH
#define STAR_StGmtClusterMaker_HH
#include "StMaker.h"
#include "StGmtTrivia.h"
#include "TTree.h"
#include "TFile.h"
#include "StRoot/StChain/StRTSBaseMaker.h"
class StGmtTrivia;
class StGmtSimpleClusterAlgo;
class StGmtClusterMaker :  public StRTSBaseMaker {
  //omitted assignment operator and copy constructor on purpose
 public:
  StGmtClusterMaker( const Char_t* name="GmtCluster");
  ~StGmtClusterMaker() {}
  Int_t Init();
  Int_t Make();
  /**sets the clustering algorithm. Currently there is the simple Clustering algorithm and the max cluster algorithm. 
     The simple cluster algorithm is the default one. The max cluster only selects one hit stip per plane, the one with the highest charge
  */
  Int_t setClusterAlgo(StGmtSimpleClusterAlgo*);

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StGmtClusterMaker.h,v 1.1.1.1 2013/09/02 15:01:02 fisyak Exp $ built " __DATE__ " " __TIME__ ; return cvs;}
  static Int_t gmtStat;
 protected:
  StGmtSimpleClusterAlgo* mClusterAlgoPtr;
  TTree * ftriviatree;
  StGmtTrivia * ftriviahit;
  ClassDef(StGmtClusterMaker,1)
};
#endif

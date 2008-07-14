// -*- mode: c++;-*-
// $Id: StBET4pMakerImp.h,v 1.62 2008/07/14 21:02:01 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STBET4PMAKERIMP_HH
#define STBET4PMAKERIMP_HH


#include "CorrectTowerEnergyForTracks.h"

#include "StJetFinder/AbstractFourVec.h"

#include <Rtypes.h>
#include <TLorentzVector.h>

#include <utility>

namespace StSpinJet {
  class StMuTrackEmu;
  class StJetTPC;
  class StJetBEMC;
  class StJetEEMC;
  class StJetTPCTrackCut;
  class StJetBEMCEnergyCut;
  class TrackListToFourList;
  class EnergyListToFourList;
}


typedef std::vector<AbstractFourVec*> FourList;

class StBET4pMakerImp {

public:
    
  StBET4pMakerImp(StSpinJet::StJetTPC* tpc,
		  StSpinJet::StJetTPCTrackCut* tpcCut,
		  StSpinJet::StJetBEMC* bemc,
		  StSpinJet::StJetBEMCEnergyCut* bemcCut,
		  StSpinJet::CorrectTowerEnergyForTracks* correctTowerEnergyForTracks,
		  StSpinJet::StJetEEMC* eemc);
    
  virtual ~StBET4pMakerImp() { };

  void Init();
  void Make();
    

  std::pair<StSpinJet::TrackList, StSpinJet::TowerEnergyList> getTrackAndEnergyList();

  FourList &getTracks() { return _tracks; };
  Int_t numTracks(void) { return _tracks.size(); };

  StSpinJet::StJetTPC*  TPC()  { return _tpc;  }
  StSpinJet::StJetBEMC* BEMC() { return _bemc; }
  StSpinJet::StJetEEMC* EEMC() { return _eemc; }

  StSpinJet::StJetTPCTrackCut* getTPCTrackCut()     { return _tpcCut; }
  StSpinJet::StJetBEMCEnergyCut* getBEMCEnergyCut() { return _bemcCut; }

private:

  void Clear(Option_t* opt = "");

  FourList _tracks;

  StSpinJet::StJetTPC*  _tpc;
  StSpinJet::StJetBEMC* _bemc;
  StSpinJet::StJetEEMC* _eemc;

  StSpinJet::StJetTPCTrackCut* _tpcCut;
  StSpinJet::StJetBEMCEnergyCut* _bemcCut;

  StSpinJet::CorrectTowerEnergyForTracks* _correctTowerEnergyForTracks;

  StSpinJet::TrackListToFourList& _track2four;
  StSpinJet::EnergyListToFourList& _energy2four;
};

#endif // STBET4PMAKERIMP_HH

/*
  FPE_OFF
  root.exe -q -b -x femtoDst.C
*/
class StGoodTrigger;
//void femtoDst(Int_t N = 10000000, const Char_t *input = "/net/l404/data/fisyak/Pico/2016/125/17125034/st_physics_17125034_raw_5500079.picoDst.root", const Char_t *output = "", const Char_t *tiggerSet = "y2016") {
void femtoDst(Int_t N = 10000000, const Char_t *input = "st_physics_adc_17125034_raw_1000007.picoDst.root", const Char_t *output = "", const Char_t *triggerSet = "y2016") {
#if !defined(__CINT__)
  std::cout << "This code cannot be compiled" << std::endl;
#else
  //  gSystem->SetFPEMask(kInvalid | kDivByZero | kOverflow );
  gROOT->LoadMacro("bfc.C");
  bfc(0,"ry2014,RpicoDst,FemtoDst,mysql,nodefault,quiet",input,output);
  //  bfc(0,"ry2016,RpicoDst,FemtoDst,mysql,nodefault,quiet",input,output);
  StKFParticleInterface::instance()->SetTriggerMode();
  StKFParticleInterface::instance()->SetSoftKaonPIDMode();
  StKFParticleInterface::instance()->SetSoftTofPidMode();
  StKFParticleInterface::instance()->SetChiPrimaryCut(8);
  
  StKFParticleInterface::instance()->SetPtCutCharm(0.5);
  StKFParticleInterface::instance()->SetChiPrimaryCutCharm(8);
  StKFParticleInterface::instance()->SetLdLCutCharmManybodyDecays(3);
  StKFParticleInterface::instance()->SetChi2TopoCutCharmManybodyDecays(10);
  StKFParticleInterface::instance()->SetChi2CutCharmManybodyDecays(3);
  StKFParticleInterface::instance()->SetLdLCutCharm2D(3);
  StKFParticleInterface::instance()->SetChi2TopoCutCharm2D(10);
  StKFParticleInterface::instance()->SetChi2CutCharm2D(3);

  StKFParticleInterface::instance()->AddDecayToReconstructionList(  421);
  StKFParticleInterface::instance()->AddDecayToReconstructionList( -421);
  StKFParticleInterface::instance()->AddDecayToReconstructionList(  426);
  StKFParticleInterface::instance()->AddDecayToReconstructionList(  429);
  StKFParticleInterface::instance()->AddDecayToReconstructionList( -429);
  StKFParticleInterface::instance()->AddDecayToReconstructionList(  411);
  StKFParticleInterface::instance()->AddDecayToReconstructionList( -411);
  StKFParticleInterface::instance()->AddDecayToReconstructionList(  431);
  StKFParticleInterface::instance()->AddDecayToReconstructionList( -431);
  StKFParticleInterface::instance()->AddDecayToReconstructionList( 4122);
  StKFParticleInterface::instance()->AddDecayToReconstructionList(-4122);
  maker = (StPicoDstMaker *) StMaker::GetTopChain()->Maker("PicoDst");
  if (! maker) return;
#if 0
  new StGoodTrigger(triggerSet);
  maker->SetAttr(".Privilege",1);
  maker->SetStatus("*",1);
#endif
  TChain *tree = maker->chain();
  Long64_t nentries = tree->GetEntries();
  if (nentries <= 0) return;
  Long64_t nevent = N;
  nevent = TMath::Min(nevent,nentries);
  cout << nentries << " events in chain " << nevent << " will be read." << endl;
  //  TTree::SetMaxTreeSize(1000000000); // 1.0GB
  ((StBFChain *) StMaker::GetTopChain())->EventLoop(nevent);
#endif
  
}
/*
  368257423 Nov 22 22:40 /net/l404/data/fisyak/Pico/2016/125/17125034/st_physics_17125034_raw_5500079.picoDst.root
  SetChiPrimaryCut(8)   48219329 Feb  5 14:52 st_physics_17125034_raw_5500079.femtoDst.root.8 => 7.64
  SetChiPrimaryCut(10)  36044394 Feb  5 15:14 st_physics_17125034_raw_5500079.femtoDst.root.10 => 10.22
  SetChiPrimaryCut(12)  27451026 Feb  5 15:19 st_physics_17125034_raw_5500079.femtoDst.root => 13.4
  SetChiPrimaryCut(8)   48196298 Feb  6 12:41 st_physics_17125034_raw_5500079.femtoDst.root.8.1e4 => 7.64
chiMax < 1e4
  SetChiPrimaryCut(8)   31186265 Feb  6 15:32 st_physics_17125034_raw_5500079.femtoDst.root => 11.81 
chiMax < 1e4
  SetChiPrimaryCut(10)  23326256 Feb  6 15:43 st_physics_17125034_raw_5500079.femtoDst.root => 15.78
 */

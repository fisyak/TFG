/*
  root.exe makeHftPlots.C
 */
void makeHftPlots(const Char_t *treefiles = ""){//"dEdx") {
  gROOT->LoadMacro("bfc.C");
  TString Chain("tpcDb,istDb,pxlDb,StEvent,detDb,HftMatTree,nodefault");
  bfc(-1,Chain,0,0,0);
  TString TreeFiles(treefiles);
  if (TreeFiles == "") {TreeFiles = "./*.tree.root";}
  TDirIter Dir(TreeFiles.Data());
  tchain = new TChain("t");
  Int_t NFiles = 0;
  ULong64_t nEvents = 0;
  ULong64_t nEvTot = 0;
  Char_t *file = 0;
  while ((file = (Char_t *) Dir.NextFile())) {
    TFile *f = new TFile(file);
    if (! f) continue;
    TTree *tree = (TTree *) f->Get("t");
    if (tree) {
      NFiles++;
      nEvents = tree->GetEntries();
      cout << "#\t" << NFiles << "\t" << f->GetName() << "\t" << nEvents << endl;
      nEvTot += nEvents;
      tchain->Add(f->GetName());
    } else {
      cout << "#\t" << NFiles << "\t" << f->GetName() << "\t Chain is missing" << endl;
    }
    delete f;
  }
  cout << "chained " << NFiles  << " files\t" 
       << "\twith total\t" << nEvTot << " events" << endl;
  HftT t(tchain);
  TString Out(gSystem->BaseName(TreeFiles));
  if (Out == "*.tree.root") {
    Out = "Out.plot.root";
  } else {
    Out.ReplaceAll(".tree.root",".plot.root");
  }
  cout << " ===> " << Out << endl;
  t.SetOutFileName(Out);
  t.Loop(0);
}

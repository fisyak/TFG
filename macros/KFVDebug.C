void KFVDebug() {
  StKFVertexMaker *kfv = (StKFVertexMaker *) chain->Maker("KFVertex");
  if (! kfv) {
    kfv = (StKFVertexMaker *) chain->Maker("StiKFVertex");
    if (!kfv) return;
  }
  kfv->SetDebug(3);
  TCanvas *c1 = new TCanvas("c1","c1",1400,500);
  kfv->SetCanvas(c1);
#if 0
  StMaker *stx = chain->Maker("Stx");
  if (stx) stx->SetDebug(13);
#endif
  chain->MakeEvent();
}

TGeoVolume *itsp() {
TGeoVolume *ITSP = gGeoManager->MakePcon("ITSP",GetMed("ITSP_AIR"),0,360,7); ITSP->SetTitle("the Inner Tracker SuPport cone mother volume");
  ((TGeoPcon*)ITSP->GetShape())->DefineSection(0,52.23,15.67,15.83);
  ((TGeoPcon*)ITSP->GetShape())->DefineSection(1,55.4,15.67,15.83);
  ((TGeoPcon*)ITSP->GetShape())->DefineSection(2,55.4,15.67,21.83);
  ((TGeoPcon*)ITSP->GetShape())->DefineSection(3,55.56,15.67,21.83);
  ((TGeoPcon*)ITSP->GetShape())->DefineSection(4,55.56,21.67,24.05);
  ((TGeoPcon*)ITSP->GetShape())->DefineSection(5,150,37.4,39.78);
  ((TGeoPcon*)ITSP->GetShape())->DefineSection(6,229.36,37.4,39.78);
TGeoVolume *SGRA = gGeoManager->MakePcon("SGRA",GetMed("ITSP_CARBON"),0,360,7); SGRA->SetTitle("the graphite/epoxy support cone");
  ((TGeoPcon*)SGRA->GetShape())->DefineSection(0,55.4,15.67,15.83);
  ((TGeoPcon*)SGRA->GetShape())->DefineSection(1,55.4,15.67,21.83);
  ((TGeoPcon*)SGRA->GetShape())->DefineSection(2,55.56,15.67,21.83);
  ((TGeoPcon*)SGRA->GetShape())->DefineSection(3,55.56,21.67,21.83);
  ((TGeoPcon*)SGRA->GetShape())->DefineSection(4,150,37.4,37.56);
  ((TGeoPcon*)SGRA->GetShape())->DefineSection(5,229.36,37.4,37.56);
  ((TGeoPcon*)SGRA->GetShape())->DefineSection(6,229.36,37.4,39.78);
  SGRA->SetLineColor(6);
TGeoVolume *STAP = gGeoManager->MakePcon("STAP",GetMed("ITSP_CH2"),0,360,3); STAP->SetTitle("the plastic part of the twin-ax cable layer (guess polyethylene)");
  ((TGeoPcon*)STAP->GetShape())->DefineSection(0,55.56,21.83,21.935);
  ((TGeoPcon*)STAP->GetShape())->DefineSection(1,150,37.56,37.665);
  ((TGeoPcon*)STAP->GetShape())->DefineSection(2,229.36,37.56,37.665);
  STAP->SetLineColor(3);
TGeoVolume *SCMY = gGeoManager->MakePcon("SCMY",GetMed("ITSP_MYLAR"),0,360,3); SCMY->SetTitle("a mylar wrap around the support cone");
  ((TGeoPcon*)SCMY->GetShape())->DefineSection(0,55.56,23.94,24.04);
  ((TGeoPcon*)SCMY->GetShape())->DefineSection(1,150,39.67,39.77);
  ((TGeoPcon*)SCMY->GetShape())->DefineSection(2,229.36,39.67,39.77);
  SCMY->SetLineColor(3);
TGeoVolume *SCAL = gGeoManager->MakePcon("SCAL",GetMed("ITSP_ALUMINIUM"),0,360,3); SCAL->SetTitle("the aluminization on the mylar wrap around the support cone");
  ((TGeoPcon*)SCAL->GetShape())->DefineSection(0,55.56,24.04,24.05);
  ((TGeoPcon*)SCAL->GetShape())->DefineSection(1,150,39.77,39.78);
  ((TGeoPcon*)SCAL->GetShape())->DefineSection(2,229.36,39.77,39.78);
  SCAL->SetLineColor(2);
TGeoVolume *ITSP_ITS1 = gGeoManager->MakePcon("ITSP",GetMed("ITSP_AIR"),0,360,7); ITSP_ITS1->SetTitle("the Inner Tracker SuPport cone mother volume");
  ((TGeoPcon*)ITSP_ITS1->GetShape())->DefineSection(0,52.23,7.835,7.995);
  ((TGeoPcon*)ITSP_ITS1->GetShape())->DefineSection(1,55.4,7.835,7.995);
  ((TGeoPcon*)ITSP_ITS1->GetShape())->DefineSection(2,55.4,7.835,10.995);
  ((TGeoPcon*)ITSP_ITS1->GetShape())->DefineSection(3,55.56,7.835,10.995);
  ((TGeoPcon*)ITSP_ITS1->GetShape())->DefineSection(4,55.56,10.835,13.215);
  ((TGeoPcon*)ITSP_ITS1->GetShape())->DefineSection(5,150,18.7,21.08);
  ((TGeoPcon*)ITSP_ITS1->GetShape())->DefineSection(6,229.36,18.7,21.08);
TGeoVolume *SGRA_SGR1 = gGeoManager->MakePcon("SGRA",GetMed("ITSP_CARBON"),0,360,7); SGRA_SGR1->SetTitle("the graphite/epoxy support cone");
  ((TGeoPcon*)SGRA_SGR1->GetShape())->DefineSection(0,55.4,7.835,7.995);
  ((TGeoPcon*)SGRA_SGR1->GetShape())->DefineSection(1,55.4,7.835,10.995);
  ((TGeoPcon*)SGRA_SGR1->GetShape())->DefineSection(2,55.56,7.835,10.995);
  ((TGeoPcon*)SGRA_SGR1->GetShape())->DefineSection(3,55.56,10.835,10.995);
  ((TGeoPcon*)SGRA_SGR1->GetShape())->DefineSection(4,150,18.7,18.86);
  ((TGeoPcon*)SGRA_SGR1->GetShape())->DefineSection(5,229.36,18.7,18.86);
  ((TGeoPcon*)SGRA_SGR1->GetShape())->DefineSection(6,229.36,18.7,21.08);
  SGRA_SGR1->SetLineColor(6);
TGeoVolume *STAP_STA1 = gGeoManager->MakePcon("STAP",GetMed("ITSP_CH2"),0,360,3); STAP_STA1->SetTitle("the plastic part of the twin-ax cable layer (guess polyethylene)");
  ((TGeoPcon*)STAP_STA1->GetShape())->DefineSection(0,55.56,10.995,11.1);
  ((TGeoPcon*)STAP_STA1->GetShape())->DefineSection(1,150,18.86,18.965);
  ((TGeoPcon*)STAP_STA1->GetShape())->DefineSection(2,229.36,18.86,18.965);
  STAP_STA1->SetLineColor(3);
TGeoVolume *SCMY_SCM1 = gGeoManager->MakePcon("SCMY",GetMed("ITSP_MYLAR"),0,360,3); SCMY_SCM1->SetTitle("a mylar wrap around the support cone");
  ((TGeoPcon*)SCMY_SCM1->GetShape())->DefineSection(0,55.56,13.105,13.205);
  ((TGeoPcon*)SCMY_SCM1->GetShape())->DefineSection(1,150,20.97,21.07);
  ((TGeoPcon*)SCMY_SCM1->GetShape())->DefineSection(2,229.36,20.97,21.07);
  SCMY_SCM1->SetLineColor(3);
TGeoVolume *SCAL_SCA1 = gGeoManager->MakePcon("SCAL",GetMed("ITSP_ALUMINIUM"),0,360,3); SCAL_SCA1->SetTitle("the aluminization on the mylar wrap around the support cone");
  ((TGeoPcon*)SCAL_SCA1->GetShape())->DefineSection(0,55.56,13.205,13.215);
  ((TGeoPcon*)SCAL_SCA1->GetShape())->DefineSection(1,150,21.07,21.08);
  ((TGeoPcon*)SCAL_SCA1->GetShape())->DefineSection(2,229.36,21.07,21.08);
  SCAL_SCA1->SetLineColor(2);
   TGeoVolumeAssembly *SCON = new TGeoVolumeAssembly("SCON");
   SCON->AddNode(ITSP,1,GetRot("180R"));
//  CAVE->AddNode(ITSP,1,GetRot("180R"));
   ITSP->AddNode(SGRA,1,gGeoIdentity);
   ITSP->AddNode(STAP,1,gGeoIdentity);
   ITSP->AddNode(SCMY,1,gGeoIdentity);
   ITSP->AddNode(SCAL,1,gGeoIdentity);
   SCON->AddNode(ITSP_ITS1,2,gGeoIdentity);
//  CAVE->AddNode(ITSP_ITS1,2,gGeoIdentity);
   ITSP_ITS1->AddNode(SGRA_SGR1,1,gGeoIdentity);
   ITSP_ITS1->AddNode(STAP_STA1,1,gGeoIdentity);
   ITSP_ITS1->AddNode(SCMY_SCM1,1,gGeoIdentity);
   ITSP_ITS1->AddNode(SCAL_SCA1,1,gGeoIdentity);
	
  return SCON;
}

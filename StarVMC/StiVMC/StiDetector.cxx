#include <math.h>
#include "Stiostream.h"
#include <map>
#include "StiDetectorContainer.h"
#include "StiDetector.h"
#include "StiMapUtilities.h"
#include "TGeoMatrix.h"
#include "TVector3.h"
#include "TMath.h"
//________________________________________________________________________________ 
ostream& operator<<(ostream& os, const TGeoMatrix& r) {
// print the matrix in 4x4 format
   const Double_t *rot = r.GetRotationMatrix();
   const Double_t *tr  = r.GetTranslation();
   os << Form("matrix %s - tr=%d  rot=%d  refl=%d  scl=%d", r.GetName(),(Int_t)r.IsTranslation(),
          (Int_t)r.IsRotation(), (Int_t)r.IsReflection(), (Int_t)r.IsScale()) << endl;
   os << Form("%10.6f%12.6f%12.6f    Tx = %10.6f", rot[0], rot[1], rot[2], tr[0]) << endl; 
   os << Form("%10.6f%12.6f%12.6f    Ty = %10.6f", rot[3], rot[4], rot[5], tr[1]) << endl; 
   os << Form("%10.6f%12.6f%12.6f    Tz = %10.6f", rot[6], rot[7], rot[8], tr[2]) << endl; 
   if (r.IsScale()) {
      const Double_t *scl  = r.GetScale();
      os << Form("Sx=%10.6fSy=%12.6fSz=%12.6f", scl[0], scl[1], scl[2]) << endl;
   }         
   return os;
}
//________________________________________________________________________________ 
ostream& operator<<(ostream& os, const StiDetector& d) {
  os << d.GetName() << *d.GetMatrix();
  return os;
} // operator<<
//________________________________________________________________________________ 
void  StiDetector::SetPhysicalNode(TGeoPhysicalNode *nodeP) {
  // from GEANT local system (x_L, y_L=0, z_L) to Sti Local System (R + y_L, -x_L, z_L)
  static TGeoRotation SWAP("SWAP", -90.0,   90.0,   90.0,    0.0,    0.0,    0.0); // "Z180"  (x,y,z) = > ( y,-x, z)
  _nodeP = nodeP;
  TGeoHMatrix *rotP = _nodeP->GetMatrix();
  Double_t *r = rotP->GetRotationMatrix();
  Double_t *t = rotP->GetTranslation();
  TVector3 normal(r[1],r[4],r[7]);
  TVector3 center(t);
  Double_t prod = center*normal;
  if (prod < 0) normal *= -1;
  // keep for LocalSeedFinder
  Double_t layerAngle  = center.Phi();
  Double_t normalRefAngle = normal.Phi();
  Double_t layerRadius = center.Perp();
  normalRadius  = layerRadius*TMath::Cos(layerAngle-normalRefAngle);
  TGeoHMatrix rotI = rotP->Inverse();
  TGeoHMatrix rot  = SWAP * rotI;
  SafeDelete(_rotm);
  _rotm = new TGeoHMatrix(rot.Inverse());
  _rotm->SetName(GetName());
}

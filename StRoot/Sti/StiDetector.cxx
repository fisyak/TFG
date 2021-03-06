#include <math.h>
#include "Stiostream.h"
#include <string>
#include <map>
#include "TError.h"
#include "TVector3.h"
#include "StiMaterial.h"
#include "StiShape.h"
#include "StiPlanarShape.h"
#include "StiCylindricalShape.h"
#include "StiPlacement.h"
#include "StiDetectorContainer.h"
#include "StiDetector.h"
#include "Sti/StiToolkit.h"
#include "StiUtilities/StiDebug.h"
#include "StiMapUtilities.h"
#include "TMath.h"
#include "TString.h"
int    StiDetector::mgIndex=0;
double StiDetector::mgValue[3]={0};


//______________________________________________________________________________
StiDetector::StiDetector()
{
  reset();
}
//______________________________________________________________________________
void StiDetector::reset()
{
  setName("");
  memset(mBeg,0,mEnd-mBeg+1);
  _key[0] = _key[1] = -1;
}

//______________________________________________________________________________
StiDetector::~StiDetector()
{}

//______________________________________________________________________________
void StiDetector::copy(StiDetector &detector){

  isActiveFunctor = detector.isActiveFunctor;

  gas = detector.getGas();
  material = detector.getMaterial();
  shape = detector.getShape();
  placement = detector.getPlacement();
  _cos  = detector._cos;
  _sin  = detector._sin;
  setName(detector.getName());
}
 
//______________________________________________________________________________
ostream& operator<<(ostream& os, const StiDetector& d)
{
  os   << d.getName()
       << Form("\tR: %7.3f cm ",d.getPlacement()->getNormalRadius()) 
       << Form("\tA: %7.3f degree", TMath::RadToDeg()*d.getPlacement()->getNormalRefAngle());
  return os;
}
//______________________________________________________________________________
int StiDetector::splitIt(StiDetVect &vect,double dXdY,int nMax)
{
static int nCall=0; nCall++;


  vect.resize(1);
  vect[0]=this;
  assert(shape);
  int iShape = shape->getShapeCode();
  float deltaX = shape->getThickness();
  float halfZ  = shape->getHalfDepth(); 
  float halfY  = shape->getHalfWidth(); 
  float angle  = shape->getOpeningAngle(); 
  float nRadius = placement->getNormalRadius();
  if (iShape >= kCylindrical)  nRadius = shape->getOuterRadius()-deltaX/2;

  if (nRadius < deltaX/2) {		// non splitable
    printf("StiDetector::splitIt %s Non splitable Rnormal < thickness/2 %g %g\n"
          ,getName().c_str(),nRadius,deltaX/2);
    return 1;
  }
  int ny = deltaX/(halfY*2*dXdY)+0.5;
  int nz = deltaX/(halfZ*2*dXdY)+0.5;
  int nSplit = (ny>nz)? ny:nz;
  if (nSplit<=1) return 1;
  if (nSplit>nMax) nSplit=nMax;

//		OK, we mast split it.

   vect.clear();
   float dX = deltaX/nSplit;
   double sumWeight = 0;
   for (int iSplit=0; iSplit<nSplit; iSplit++) 
   {
     float xc = -deltaX/2 +dX/2+iSplit*dX;  
//		Create small part of  detector
     StiDetector *det = StiToolkit::instance()->getDetectorFactory()->getInstance();
     det->copy(*this);
     TString ts(getName());
     if (iSplit) { ts+="_"; ts+=iSplit;} 
     det->setName(ts.Data());
//		Create shape
     ts = shape->getName();
     if (iSplit) { ts+="_"; ts+=iSplit;} 
     StiShape *myShape =0;
     float myRadius = nRadius+xc;
assert(myRadius>1e-2 && myRadius < 1e3);
     if (iShape==kPlanar) 	{//Planar shape
       myShape = new StiPlanarShape(ts.Data(),halfZ,dX,halfY);

     } else if (iShape>=kCylindrical) {//Cylinder shape
       myShape = new StiCylindricalShape(ts.Data(),halfZ,dX,myRadius+dX/2,angle);

     } else { assert(0 && "Wrong shape type");}

//		Create placement
     StiPlacement *place = new StiPlacement;
     *place = *placement;
     place->setNormalRep(placement->getNormalRefAngle(),myRadius,placement->getNormalYoffset());
     det->setShape(myShape);
     place->setLayerRadius(myRadius);
     det->setPlacement(place);
     sumWeight += det->getWeight();
     vect.push_back(det);
   }
   this->copy(*vect[0]); 
   this->setName(vect[0]->getName());
//   delete vect[0];
    StiToolkit::instance()->getDetectorFactory()->free(vect[0]);
    vect[0] = this;
//    if (vect.size()>1) {
//      printf("StiDetector::splitIt %s is splitted into %d peaces\n",getName().c_str(),vect.size());}
//    assert(fabs(startWeight-sumWeight)<1e-3*startWeight);


   return vect.size();
}
//______________________________________________________________________________
double StiDetector::getVolume() const
{
return shape->getVolume();
}
//______________________________________________________________________________
    double StiDetector::getWeight() const
{
return shape->getVolume()*material->getDensity();
}
//______________________________________________________________________________
int StiDetector::insideL(const double xl[3],int mode,double fakt) const 
{
double rN = placement->getNormalRadius();
double myErr = rN*(fakt-1);
if (myErr<0.1) myErr = 0.1;
if (myErr>10.) myErr = 10.;
double thick = shape->getThickness();
do {
 if (shape->getShapeCode()==1) { //Planar
   if (mode&1) { 
     mgIndex = 1;
     mgValue[1] = thick/2*fakt;
     mgValue[0] = fabs(xl[0]-rN)-mgValue[1]-myErr;
     if (mgValue[0]>0) break;
   }
   if (mode&2) {
     mgIndex = 2;
     double y = xl[1]-placement->getNormalYoffset();
     mgValue[1] = shape->getHalfWidth()*fakt;
     mgValue[0]  = fabs(y)-mgValue[1]-myErr;
     if (mgValue[0]>0) break;
   }
 } else {
   if (mode&1) {
     mgIndex = 1;
     mgValue[1] = thick/2*fakt;
     double rxy = sqrt(xl[0]*xl[0]+xl[1]*xl[1]);
     mgValue[0] = (fabs(rxy-rN)-mgValue[1]-myErr);
     if (mgValue[0]>0) break;
   }

   if (mode&2) {
     mgIndex = 2;
     double ang = atan2(xl[1],xl[0]);
     if (ang<-M_PI) ang +=M_PI*2;
     if (ang> M_PI) ang -=M_PI*2;
     mgValue[1] = shape->getOpeningAngle()/2 *fakt;
     mgValue[0] = (fabs(ang)-mgValue[1]-myErr/rN);
     if (mgValue[0]>0)	break;
   }
 } 
   if (mode&4) {
     mgIndex = 3;
     mgValue[1] = shape->getHalfDepth()*fakt;
     double z = xl[2]-placement->getZcenter();  
     mgValue[0] = (fabs(z)-mgValue[1]-myErr);
     if (mgValue[0]>0)	break;
   }
   mgIndex = 0;
   return 1;
 } while(0);
  ::Error("StiDetector::insideL","Det=%s XYZ=(%g %g %g)"
         ,getName().c_str(),xl[0],xl[1],xl[2]);
  ::Error("StiDetector::insideL","idx=%d val=%g %g"
         ,mgIndex,mgValue[0],mgValue[1]);

  return 0;
}
//______________________________________________________________________________

/**
 * A setter for most of the detector properties. We do not pass arguments in
 * a constructor because StiDetector-s are normally created by an StiFactory.
 *
 * \author Dmitri Smirnov, BNL
 */
void StiDetector::setProperties(std::string name, StiIsActiveFunctor* activeFunctor,
   StiShape* shape, StiPlacement* placement, StiMaterial* gas, StiMaterial* material)
{
   setName(name.c_str());
   setIsActive(activeFunctor);
   setShape(shape);
   setPlacement(placement);
   setGas(gas);
   setMaterial(material);
}
//______________________________________________________________________________
int StiDetector::insideG(const double xl[3],int mode,double fakt) const 
{
  TVector3 xg(xl);
  double alfa = getPlacement()->getNormalRefAngle();
  xg.RotateZ(-alfa);
  return insideL(&xg[0],mode,fakt);
}
//______________________________________________________________________________
void StiDetector::getDetPlane(double plane[4]) const 
{
  plane[0] = - getPlacement()->getNormalRadius();
  plane[1] = _cos;
  plane[2] = _sin;
  plane[3] = 0.;
}

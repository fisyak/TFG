// $Id: StMCSimplePrimaryGenerator.h,v 1.2 2010/10/26 19:39:57 jwebb Exp $
// $Log: StMCSimplePrimaryGenerator.h,v $
// Revision 1.2  2010/10/26 19:39:57  jwebb
// Changes to interface, mostly.
//
// Revision 1.1  2010/02/11 19:50:55  jwebb
// Code for StarBASE:  Star Baseline Analysis of the Simulation Environment
// ========================================================================
//
// StarBASE provides the capability to generate QA histograms of the detector
// geometries in STAR.  Depth (in units of radiation length) vs pseudo-
// rapidity histograms are created for all detector volumes defined in the
// requested geometry.  A set of python (PyROOT) scripts is used to organize
// the plots into PDF files, comparing a specified volume and its contents.
//
// StarBASE is based on an early Star implementation of VMC, which uses the
// root geometry (TGeo) classes to construct the geometry model, and the
// geant3 fortran codes to provide particle transport.
//
// Revision 1.2  2009/08/29 21:20:08  perev
// No I/O for StMCSimplePrimaryGenerator
//
// Revision 1.1  2009/03/25 23:15:10  perev
// New VMC maker
//
// Revision 1.1.1.1  2008/12/10 20:45:52  fisyak
// Merge with macos version
//
// Revision 1.1  2005/06/09 20:13:47  fisyak
// It looks like that all hits in place (calorimeters have to be check for volumeid)
//
// Revision 1.2  2005/05/03 15:42:14  fisyak
// Adjust for bfc
//
// Revision 1.1  2005/04/25 20:44:28  fisyak
// StarVMCApplication with example in macros/starVMC.C
//

#ifndef StMCSimplePrimaryGenerator_h
#define StMCSimplePrimaryGenerator_h

#include "StMCPrimaryGenerator.h"

class StMCSimplePrimaryGenerator :  public StMCPrimaryGenerator  {
 public:

  StMCSimplePrimaryGenerator(
     int    nprim=1     ,int    Id=6, 
     double pT_min =  0 ,double pT_max  = 10,
     double Eta_min=-10 ,double Eta_max = 10, 
     double Phi_min = 0 ,double Phi_max = 2*3.14159265358, 
     double Z_min   = 0 ,double Z_max   = 0, 
     const char *option = "G");

  virtual ~StMCSimplePrimaryGenerator() {}
  
     void SetGenerator(
     int    nprim=1     ,int    Id=6, 
     double pT_min =  0 ,double pT_max  = 10,
     double Eta_min=-10 ,double Eta_max = 10, 
     double Phi_min = 0 ,double Phi_max = 2*3.14159265358, 
     double Z_min   = 0 ,double Z_max   = 0, 
     const char *option = "G");
     virtual int Fun();

     void SetVertex( Double_t zmin, Double_t zmax )
     {
       fZ_min = zmin; fZ_max = zmax;
     };

 private:
     void GenerateOnePrimary();
 protected:
  double fpT_min;
  double fpT_max;
  double fEta_min;
  double fEta_max;
  double fPhi_min;
  double fPhi_max;
  double fZ_min;
  double fZ_max;
  ClassDef(StMCSimplePrimaryGenerator,0)  //StMCSimplePrimaryGenerator
};
#endif //StMCSimplePrimaryGenerator_h


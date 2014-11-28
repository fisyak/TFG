// main27.cc is a part of the PYTHIA event generator.
// Copyright (C) 2008 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Example how to write a derived class for beam momentum and vertex spread,
// with an instance handed to Pythia for internal generation.
// Warning: the parameters are not realistic. 

#include "Pythia.h"

using namespace Pythia8; 
 
//**************************************************************************

// A derived class to set beam momentum and interaction vertex spread.

class MyBeamShape : public BeamShape {

public:

  // Constructor.
  MyBeamShape() {}

  // Initialize beam parameters.
  // In this particular example we will reuse the existing settings names
  // but with modified meaning, so init() in the base class can be kept. 
  //virtual void init();

  // Set the two beam momentum deviations and the beam vertex.
  virtual void pick();

};

//*********

// Set the two beam momentum deviations and the beam vertex.
// Note that momenta are in units of GeV and vertices in mm,
// always with c = 1, so that e.g. time is in mm/c. 

void MyBeamShape::pick() {

  // Reset all values.
  deltaPxA = deltaPyA = deltaPzA = deltaPxB = deltaPyB = deltaPzB
    = vertexX = vertexY = vertexZ = vertexT = 0.;

  // Set beam A transverse momentum deviation by a two-dimensional Gaussian.
  if (allowMomentumSpread) {
    double totalDev, gauss;
    do {
      totalDev = 0.;
      if (sigmaPxA > 0.) {
        gauss     = Rndm::gauss();
        deltaPxA  = sigmaPxA * gauss;
        totalDev += gauss * gauss; 
      }
      if (sigmaPyA > 0.) {
        gauss     = Rndm::gauss();
        deltaPyA  = sigmaPyA * gauss;
        totalDev += gauss * gauss; 
      }
    } while (totalDev > maxDevA * maxDevA); 

    // Set beam A longitudinal momentum as a triangular shape.
    // Reuse sigmaPzA to represent maximum deviation in this case.
    if (sigmaPzA > 0.) {
      deltaPzA    = sigmaPzA * ( 1. - sqrt(Rndm::flat()) );
      if (Rndm::flat() < 0.5) deltaPzA = -deltaPzA; 
    }

    // Set beam B transverse momentum deviation by a two-dimensional Gaussian.
    do {
      totalDev = 0.;
      if (sigmaPxB > 0.) {
        gauss     = Rndm::gauss();
        deltaPxB  = sigmaPxB * gauss;
        totalDev += gauss * gauss; 
      }
      if (sigmaPyB > 0.) {
        gauss     = Rndm::gauss();
        deltaPyB  = sigmaPyB * gauss;
        totalDev += gauss * gauss; 
      }
    } while (totalDev > maxDevB * maxDevB); 

    // Set beam B longitudinal momentum as a triangular shape.
    // Reuse sigmaPzB to represent maximum deviation in this case.
    if (sigmaPzB > 0.) {
      deltaPzB = sigmaPzB * ( 1. - sqrt(Rndm::flat()) );
      if (Rndm::flat() < 0.5) deltaPzB = -deltaPzB; 
    }
  }

  // Set beam vertex location by a two-dimensional Gaussian.
  if (allowVertexSpread) {
    double totalDev, gauss;
    do {
      totalDev = 0.;
      if (sigmaVertexX > 0.) {
        gauss     = Rndm::gauss();
        vertexX   = sigmaVertexX * gauss;
        totalDev += gauss * gauss; 
      }
      if (sigmaVertexY > 0.) {
        gauss     = Rndm::gauss();
        vertexY   = sigmaVertexY * gauss;
        totalDev += gauss * gauss; 
      }
    } while (totalDev > maxDevVertex * maxDevVertex);

    // Set beam B longitudinal momentum as a triangular shape.
    // This corresponds to two step-function beams colliding.
    // Reuse sigmaVertexZ to represent maximum deviation in this case.
    if (sigmaVertexZ > 0.) {
      vertexZ     = sigmaVertexZ * ( 1. - sqrt(Rndm::flat()) );
      if (Rndm::flat() < 0.5) vertexZ = -vertexZ; 

      // Set beam collision time flat between +-(sigmaVertexZ - |vertexZ|). 
      // This corresponds to two step-function beams colliding (with v = c).
      vertexT = (2. * Rndm::flat() - 1.) * (sigmaVertexZ - abs(vertexZ));  
    }

    // Add offset to beam vertex.
    vertexX      += offsetX;
    vertexY      += offsetY;
    vertexZ      += offsetZ;
    vertexT      += offsetT;
  }  

}
 
//**************************************************************************

int main() {

  // Number of events to generate and to list. Max number of errors.
  int nEvent = 10000;
  int nList  = 1;
  int nAbort = 5;

  // Pythia generator.
  Pythia pythia;

  // Process selection.
  pythia.readString("HardQCD:all = on");    
  pythia.readString("PhaseSpace:pTHatMin = 20."); 

  // LHC with acollinear beams in the x plane.
  // Use that default is pp with pz = +-7000 GeV, so this need not be set.  
  pythia.readString("Beams:frameType = 3");    
  pythia.readString("Beams:pxA = 1.");    
  pythia.readString("Beams:pxB = 1.");  

  // A class to generate beam parameters according to own parametrization.
  BeamShape* myBeamShape = new MyBeamShape();

  // Hand pointer to Pythia. 
  // If you comment this out you get internal Gaussian-style implementation.
  pythia.setBeamShapePtr( myBeamShape);

  // Set up beam spread parameters - reused by MyBeamShape.  
  pythia.readString("Beams:allowMomentumSpread = on");  
  pythia.readString("Beams:sigmapxA = 0.1");  
  pythia.readString("Beams:sigmapyA = 0.1");  
  pythia.readString("Beams:sigmapzA = 5.");  
  pythia.readString("Beams:sigmapxB = 0.1");  
  pythia.readString("Beams:sigmapyB = 0.1");  
  pythia.readString("Beams:sigmapzB = 5."); 

  // Set up beam vertex parameters - reused by MyBeamShape.
  pythia.readString("Beams:allowVertexSpread = on");  
  pythia.readString("Beams:sigmaVertexX = 0.3");  
  pythia.readString("Beams:sigmaVertexY = 0.3");  
  pythia.readString("Beams:sigmaVertexZ = 50.");  
  // In MyBeamShape the time width is not an independent parameter.
  //pythia.readString("Beams:sigmaTime = 50.");  

  // Optionally simplify generation.
  pythia.readString("PartonLevel:all = off");  

  // Initialization.  
  pythia.init();

  // Read out nominal energy.
  double eCMnom = pythia.info.eCM(); 

  // Histograms.
  Hist eCM("center-of-mass energy deviation", 100, -20., 20.);
  Hist pXsum("net pX offset", 100, -1.0, 1.0);
  Hist pYsum("net pY offset", 100, -1.0, 1.0);
  Hist pZsum("net pZ offset", 100, -10., 10.);
  Hist pZind("individual abs(pZ) offset", 100, -10., 10.);
  Hist vtxX("vertex x position", 100, -1.0, 1.0);
  Hist vtxY("vertex y position", 100, -1.0, 1.0);
  Hist vtxZ("vertex z position", 100, -100., 100.);
  Hist vtxT("vertex time", 100, -100., 100.);
  Hist vtxZT("vertex |x| + |t|", 100, 0., 100.);

  // Begin event loop. Generate event. 
  int iAbort = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {
    if (!pythia.next()) {

      // List faulty events and quit if many failures.
      pythia.info.list(); 
      pythia.process.list();
      //pythia.event.list();
      if (++iAbort < nAbort) continue;
      cout << " Event generation aborted prematurely, owing to error!\n"; 
      break;      
    }
 
    // List first few events.
    if (iEvent < nList) { 
      pythia.info.list(); 
      pythia.process.list(); 
      //pythia.event.list();
    } 

    // Fill histograms.
    double eCMnow = pythia.info.eCM();
    eCM.fill( eCMnow - eCMnom);
    pXsum.fill(  pythia.process[0].px() - 2. );
    pYsum.fill(  pythia.process[0].py() );
    pZsum.fill(  pythia.process[0].pz() );
    pZind.fill(  pythia.process[1].pz() - 7000. );
    pZind.fill( -pythia.process[2].pz() - 7000. );
    vtxX.fill(  pythia.process[0].xProd() );
    vtxY.fill(  pythia.process[0].yProd() );
    vtxZ.fill(  pythia.process[0].zProd() );
    vtxT.fill(  pythia.process[0].tProd() );
    double absSum = abs(pythia.process[0].zProd()) 
                  + abs(pythia.process[0].tProd());
    vtxZT.fill( absSum );

  // End of event loop. Statistics. Histograms. Done.
  }
  pythia.statistics();
  cout << eCM << pXsum << pYsum << pZsum << pZind
       << vtxX << vtxY << vtxZ << vtxT << vtxZT; 
  return 0;
}

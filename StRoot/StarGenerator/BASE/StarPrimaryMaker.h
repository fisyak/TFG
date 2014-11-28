#ifndef __StarPrimaryMaker_h__
#define __StarPrimaryMaker_h__

#include "StMaker.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "StarParticleStack.h"

#include "StarGenerator/UTIL/StarParticleData.h"

#include "TEventList.h"

class StarGenerator;
class StarGenEvent;
class StarFilterMaker;

class TTree;
class TFile;

/**
   \class StarPrimaryMaker
   \author Jason C. Webb
   \brief Main steering class for event generation

   StarPrimaryMaker is the main steering maker for event generators in the (new) STAR
   event generator framework.  Users add one or more event generators (derived from
   StarGenerator) to the maker.  The primary maker is responsible for calling the 
   event generators in a well-defined calling sequence:

   1) PreGenerate Phase

   During the PreGenerate phase, the PreGenerate() method will be called on all generators
   in the order in which they were added to the primary maker.  This (optional) method on
   event generators is provided for the developer to perform any event-by-event configuration
   which may be required by the event generator.

   2) Generate Phase

   During the Generate phase, the event generation machinery of the concrete event generator
   is called.  Generate will be called on all event generators, in the order in which they
   were added to the primary maker.  Generate will be called on all generatos before the 
   PostGenerate phase is entered.  At the end of Generate, it is expected that event generators
   have filled their event records.

   3) PostGenerate Phase

   After Generate has been called on all generators, PostGenerate will be called on each 
   generator in the order in which they were added to the primary maker.  Developers may
   at this point access the results from event generators which were found earlier in the
   chain.  This is the last point at which a developer may interact with the event record.

   After PostGenerate, the event is finalized.  The primary maker will loop over all event
   generators in its list, set the vertex, and accumulate the particles from each generator
   at an appropriate vertex.  Two modes are available on generators:  standard and pileup 
   mode.  Standard mode (which is the default) places all events at the same vertex.  
   Pileup mode places pileup events, with some probability, at an independent vertex.

   Event Record

   The event record, which records the particle-wise and event-wise inforamtion from each
   event generator, is saved in a TTree format.  The results from each generator are saved
   in a separate branch, whose name corresponds to the assigned name of the generator.

   Particle-wise information is stored as an array of StarGenParticle s.  The main event
   record lists all particles which were generated, with inices refering to their position
   in their parent event generator, their position in the event record of the main event 
   generator, and their position on the GEANT stack (aka ID truth).


   Event Vertex

   The event-vertex is generated by StarPrimaryMaker.  It is sampled from a gaussian distributions
   in x, y and z.  The possibility exists to correlate the x and y distributions.  See SetSigma()
   for details.


   Beamline Constraint

   The beamline as measured at STAR may be simulated.  For the case where an SDT timestamp is 
   specified, the user may set the attribute "beamline" to true

      primary->SetAttr("beamline", 1)

   This will instruct the StarPrimaryMaker to lookup the beamline constraint as recorded
   in the online database for the requested SDT timestamp.  Otherwise, the beamline may
   be specified by hand by calling

      primary->SetSlope( dxdz, dydz );

   When setting the beamline by hand, you should *not* use SetAttr("beamline",1).  That will
   cause the maker to attempt to load slopes from the database, resulting in no slope.
   
*/

class StarPrimaryMaker : public StMaker
{
 public:
  StarPrimaryMaker();
  ~StarPrimaryMaker();
  
  Int_t InitRun( Int_t runnumber );
  Int_t Init();
  Int_t Make();
  void  Clear( const Option_t *opts="" );
  Int_t Finish();

  /// Set the filename of the output TTree
  void  SetFileName( const Char_t *name ){ mFileName = name; }

  /// Returns a pointer to a particle class containing
  /// PDG information about the particle
  static TParticlePDG *pdg( Int_t pdgid );

  /// Add an event generator to the list of event generators.
  /// @param gener Is a pointer to the user's event generator
  void AddGenerator( StarGenerator *gener );

  /// Attach a filter to the primary generator
  /// @param filt Is a pointer to the filter maker
  void AddFilter( StarFilterMaker *filt );

  /// Set the x, y and z vertex position
  void SetVertex( Double_t x, Double_t y, Double_t z ){ mVx=x; mVy=y; mVz=z; }

  /// Set the smearing in the x, y and z vertex positions.
  /// @param sx is the smearing in x
  /// @param sy is the smearing in y
  /// @param sz is the smearing in z
  /// @param rho is the correlation between x and y
  void SetSigma( Double_t sx, Double_t sy, Double_t sz, Double_t rho=0 ){ mSx=sx; mSy=sy; mSz=sz; mRho=rho; }

  /// Set the slope of the vertex
  /// @param dxdz is the increase in x with increasing z
  /// @param dydz is the increase in y with increasing z
  void SetSlope( Double_t dxdz, Double_t dydz ){ mVdxdz=dxdz; mVdydz=dydz; }

  /// Use the beamline constraint as recorded in the database for the current timestamp.
  /// Values set by the SetVertex and SetSlope will be overwritten by the database.
  /// User must still provide a width using SetSigma.
  /// This should only be used when a valid SDT timestamp has been set in the BFC options.
  /// @param beamline >0 to use the beamline constraint.  =0 to switch it off.
  void SetBeamline( Int_t beamline=1 ){ assert(beamline>=0); SetAttr("beamline",beamline); }

  /// Set particle cuts
  void SetCuts( Double_t ptmin,    Double_t ptmax=-1, 
		Double_t ymin=0,   Double_t ymax=-1,
		Double_t phimin=0, Double_t phimax=-1,
		Double_t zmin=0,   Double_t zmax=-1 );

  /// Set PT range.  Particles falling outside this range will be dropped from simulation.
  void SetPtRange( Double_t ptmin, Double_t ptmax=-1 ){ mPtMin = ptmin; mPtMax = ptmax; }
  /// Set rapidity range.  Particles falling outside this range will be dropped from simulation.
  void SetEtaRange( Double_t etamin, Double_t etamax ){ mRapidityMin = etamin; mRapidityMax = etamax; }
  /// Set phi range.  Particles falling outside this range will be dropped from simulation.
  void SetPhiRange( Double_t phimin, Double_t phimax ){ mPhiMin = phimin; mPhiMax = phimax; }
  /// Set z-vertex range.  Particles falling outside this range will be dropped from simulation.
  void SetZvertexRange( Double_t zmin, Double_t zmax ){ mZMin = zmin; mZMax = zmax; }

  /// Return a pointer to the event
  StarGenEvent *event() { return mPrimaryEvent; }

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StarPrimaryMaker.h,v 1.6 2014/08/06 11:43:57 jeromel Exp $ built " __DATE__ " " __TIME__ ; return cvs;}

 private:
 protected:


  TLorentzVector Vertex();

  Int_t     PreGenerate();
  Int_t        Generate();
  Int_t    PostGenerate();
  Int_t        Finalize();
  void      BuildTables();
  
  /// Total number of particles
  Int_t mNumParticles;

  /// The output tree
  TTree   *mTree;
  TFile   *mFile;
  TString  mTreeName;
  TString  mFileName;
  StarParticleStack *mStack;
  StarGenEvent      *mPrimaryEvent;

  // Vertex position, width and x,y correlation
  Double_t mVx, mVy, mVz, mSx, mSy, mSz, mRho;

  // Vertex slope for beamline constraint
  Double_t mVdxdz, mVdydz;

  // Handle boost onto the beamline
  Bool_t mDoBeamline;

  /// Boosts (rotates) particle from the event generator frame onto the beamline.
  /// @px x-component of the momentum
  /// @py y-component of the momentum
  /// @pz z-component of the momentum
  /// @E  E-component of the momentum
  /// @M  Mass [GeV]
  /// @vx x-component of the production vertex
  /// @vy y-component of the production vertex
  /// @vz z-component of the production vertex
  /// @vt t-component of the production vertex
  void RotateBeamline( Double_t &px, Double_t &py, Double_t &pz, Double_t &E, Double_t &M, Double_t &vx, Double_t &vy, Double_t &vz, Double_t &vt );

  Double_t mPtMin, mPtMax, mRapidityMin, mRapidityMax, mPhiMin, mPhiMax, mZMin, mZMax;

  /// Tests to see whether the particle passes all appropriate cuts to be passed to the simulator
  Bool_t Simulate( StarGenParticle *p );

  Int_t mRunNumber;

  TLorentzVector mPrimaryVertex;

  StarFilterMaker *mFilter;
  TEventList      *mAccepted; //*< event list containing accepted events

  ClassDef(StarPrimaryMaker,1);

};

#endif

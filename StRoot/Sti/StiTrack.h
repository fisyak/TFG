#ifndef StiTrack_H
#define StiTrack_H 1

//std
#include <math.h>

//SCL
#include "StThreeVector.hh"
#include "StThreeVectorF.hh"

//sti
#include "Exception.h"

class StiHit;
class StiTrackFitter;

class StiTrack 
{
public:

  // static methods

  static void setTrackFitter(StiTrackFitter * fitter);
  static StiTrackFitter * getTrackFitter();

  // constructor/destructor/copy/etc
  
  StiTrack();
  virtual ~StiTrack();
  
  // action methods
  
  virtual void reset();
    virtual void fit(); //throw (Exception);


    //This is for full state (3 mom + error matrix) 
    virtual void    getMomentum(double p[3], double e[6]) const =0;

    //Simple gets
    virtual StThreeVector<double> getMomentumAtOrigin() const =0; //3-momentum at first point
    virtual StThreeVector<double> getMomentumNear(double x) =0; //3-momentum at arb. point
    virtual StThreeVector<double> getHitPositionNear(double x) const =0; //3-position at arb. point
    
    virtual double  getPt()             const                   =0;   // transverse momentum
    virtual double  getRapidity()       const                   =0;   // rapidity
    virtual double  getPseudoRapidity() const                   =0;   // pseudo rapidity
    virtual double  getPhi()            const                   =0;   // azimuthal angle
    virtual double  getTanL()           const                   =0;   // tan(lambda)
    virtual double  getDca(StiHit *h=0)    const=0;   // distance of closest approach to given point/hit
    virtual double  getDca2(StiTrack *t)   const=0;   // distance of closest approach to given track - 2D calc
    virtual double  getDca3(StiTrack *t)   const=0;   // distance of closest approach to given track - 3D calc
    
    virtual int    getFitPointCount()  const=0;  // number of points used in fit
    virtual int    getGapCount()  const=0;
    // number of total number of points currently assigned to the track
    virtual int    getPointCount()      const=0;

    /// number of hits used to seed the track
    int getSeedHitCount() const {return mSeedHitCount;}
    
    StiHit * getVertex() const;  // return pointer to vertex associated with this track if any. 
    
  // accessor methods
  
		/// Get mass of the particle that produced this track
		virtual double  getMass() const
			{ 
				return m;
			}
		
		/// Get charge of the particle that produced this track
		virtual int     getCharge() const
			{
				return q;
			}
		
		/// Get chi2 of this track
		virtual double  getChi2() const
			{
				return chi2;
			}
		
		/// Set charge  of the particle that produced this track
    void  setCharge(int v)
			{
				q = v;
			}

		/// Set chi2 of the track
    void  setChi2(double v)
			{
				chi2 = v;
			}

    void  setVertex(StiHit *v);
    void setSeedHitCount(int c) {mSeedHitCount=c;}

    //Flag:
    void setFlag(long v) {mFlag = v;}
    long getFlag() const {return mFlag;}
  
    
protected:
    static StiTrackFitter * trackFitter;

    friend ostream& operator<<(ostream& os, const StiTrack& track);

    int    q;          // charge of the track 
    int    nPts;       // number of points on the track
    int    nFitPts;    // number of points included in the fit of the track
    int mSeedHitCount; //number of points used to seed the track
    long mFlag; //A flag to pack w/ topo info
    StiHit * vertex; // parent vertex of this track
    double  m;          // mass hypothesis
    double  chi2;
};

inline StiHit * StiTrack::getVertex() const
{
  return vertex;
}

inline void  StiTrack::setVertex(StiHit *v)
{
  vertex = v;
}


#endif

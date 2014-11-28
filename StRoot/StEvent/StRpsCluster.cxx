/***************************************************************************
 *
 * $Id: StRpsCluster.cxx,v 2.1 2009/11/23 22:18:25 ullrich Exp $
 *
 * Author: Thomas Ullrich, Nov 2009
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StRpsCluster.cxx,v $
 * Revision 2.1  2009/11/23 22:18:25  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StRpsCluster.h"

static const char rcsid[] = "$Id: StRpsCluster.cxx,v 2.1 2009/11/23 22:18:25 ullrich Exp $";

ClassImp(StRpsCluster)

StRpsCluster::StRpsCluster() 
{
    mPosition = mEnergy = mXY = 0;
    mLength = 0;    
    mQuality = 0;
    mPlaneId = mRomanPotId = 0;
}
    
StRpsCluster::StRpsCluster(double pos, short len, double e, double xy, unsigned char qual)
{
    mPosition = pos;
    mLength = len;    
    mEnergy = e;
    mXY = xy;
    mQuality = qual;
    mPlaneId = mRomanPotId = 0;
    // mPlaneId and mRomanPotId are later set by StRpsPlane::addCluster()
}

StRpsCluster::~StRpsCluster() { /* noop */ };

double 
StRpsCluster::position() const { return mPosition; }

short  
StRpsCluster::length() const { return mLength; }

double 
StRpsCluster::energy() const { return mEnergy; }

double 
StRpsCluster::xy() const { return mXY; }

unsigned char 
StRpsCluster::quality() const { return mQuality; }

unsigned int 
StRpsCluster::romanPotId() const { return mRomanPotId; }

unsigned int 
StRpsCluster::planeId() const { return mPlaneId; }

void 
StRpsCluster::setPosition(double val) { mPosition = val; }

void 
StRpsCluster::setLength(short val) { mLength = val; }

void 
StRpsCluster::setEnergy(double val) { mEnergy = val; }

void 
StRpsCluster::setXY(double val) { mXY = val; }

void 
StRpsCluster::setQuality(unsigned char val) { mQuality = val; }

void 
StRpsCluster::setPlaneId(unsigned char val) { mPlaneId = val; }

void 
StRpsCluster::setRomanPotId(unsigned char val) { mRomanPotId = val; }

// 
// Non class methods
//
std::ostream& operator<<(std::ostream& os, const StRpsCluster& cluster)
{
    os << "position = " << cluster.position() << std::endl;
    os << "length = " << cluster.length() << std::endl;
    os << "energy = " << cluster.energy() << std::endl;
    os << "xy = " << cluster.xy() << std::endl;
    os << "quality = " << static_cast<unsigned int>(cluster.quality()) << std::endl;
    os << "roman pot id = " << cluster.romanPotId() << std::endl;
    os << "plane id = " << cluster.planeId() << std::endl;
    return os;
}

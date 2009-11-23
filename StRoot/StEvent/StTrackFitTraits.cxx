/***************************************************************************
 *
 * $Id: StTrackFitTraits.cxx,v 2.18 2009/11/23 16:34:07 fisyak Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackFitTraits.cxx,v $
 * Revision 2.18  2009/11/23 16:34:07  fisyak
 * Cleanup, remove dependence on dst tables, clean up software monitors
 *
 * Revision 2.17  2008/03/13 16:57:36  ullrich
 * Add include to comply with ROOT.
 *
 * Revision 2.16  2007/10/11 21:52:32  ullrich
 * Added member to handle number of fit points for PXL and IST.
 *
 * Revision 2.15  2004/12/02 23:35:13  ullrich
 * Added misisng setXXX functions.
 *
 * Revision 2.14  2004/08/13 18:15:42  ullrich
 * Added +1 to the number of fit points when bool flag is set.
 *
 * Revision 2.13  2004/08/12 17:22:31  fisyak
 * Switch to automatic streamer for version >4 to account new no. of fit points definition
 *
 * Revision 2.12  2004/08/10 14:20:21  calderon
 * Putting the streamers back in.  They should not be needed, but
 * apparently removing them causes more problems.  Yuri tested that
 * putting them back in allows reading files again.
 *
 * Revision 2.11  2004/08/05 22:24:32  ullrich
 * Changes to the handling of numberOfPoints() to allow ITTF more flexibility.
 *
 * Revision 2.10  2001/05/04 19:49:51  perev
 * Streamer to account old ROOT2
 *
 * Revision 2.9  2001/04/09 22:57:05  perev
 * forget STAR I/O
 *
 * Revision 2.8  2001/04/05 04:00:58  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.7  2001/03/24 03:35:00  perev
 * clone() -> clone() const
 *
 * Revision 2.6  2001/03/16 20:57:44  ullrich
 * Covariant matrix now stored in TArrayF.
 *
 * Revision 2.5  2000/02/22 23:24:08  ullrich
 * Fixed bug in covariantMatrix().
 *
 * Revision 2.4  2000/01/20 14:43:39  ullrich
 * Fixed bug in numberOfFitPoints(). Sum was wrong.
 *
 * Revision 2.3  1999/12/21 15:09:18  ullrich
 * Modified to cope with new compiler version on Sun (CC5.0).
 *
 * Revision 2.2  1999/11/01 12:45:14  ullrich
 * Modified unpacking of point counter
 *
 * Revision 2.1  1999/10/28 22:27:32  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:42:59  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include "TFile.h"
#include <algorithm>
#include "StTrackFitTraits.h"
#include "StParticleTypes.hh"
#include "StParticleTable.hh"
#include "TClass.h"
#if !defined(ST_NO_NAMESPACES)
using std::fill_n;
using std::copy;
#endif

ClassImp(StTrackFitTraits)

static const char rcsid[] = "$Id: StTrackFitTraits.cxx,v 2.18 2009/11/23 16:34:07 fisyak Exp $";

StTrackFitTraits::StTrackFitTraits()
{
    mPidHypothesis = 0;
    mNumberOfFitPoints = 0;
    mNumberOfFitPointsTpc = 0;
    mNumberOfFitPointsFtpcWest = 0;
    mNumberOfFitPointsFtpcEast = 0;
    mNumberOfFitPointsSvt = 0;
    mNumberOfFitPointsSsd = 0;
    mNumberOfFitPointsPxl = 0;
    mNumberOfFitPointsIst = 0;
    mPrimaryVertexUsedInFit = false;
    fill_n(mChi2, 2, 0);
}


StTrackFitTraits::StTrackFitTraits(unsigned short pid, unsigned short nfp,
                 float chi[2], float cov[15])
{
    mPidHypothesis = pid;
    mNumberOfFitPoints = nfp;
    copy(chi, chi+2, mChi2);
    mCovariantMatrix.Set(15, cov);
    mNumberOfFitPointsTpc = 0;
    mNumberOfFitPointsFtpcWest = 0;
    mNumberOfFitPointsFtpcEast = 0;
    mNumberOfFitPointsSvt = 0;
    mNumberOfFitPointsSsd = 0;
    mNumberOfFitPointsPxl = 0;
    mNumberOfFitPointsIst = 0;
    mPrimaryVertexUsedInFit = false;
}

StTrackFitTraits::~StTrackFitTraits() {/* noop */}

unsigned short
StTrackFitTraits::numberOfFitPoints() const
{
    unsigned short result;
    //
    // Old and obsolete
    //
    if (mNumberOfFitPoints) {
        result = numberOfFitPoints(kTpcId) +
	  numberOfFitPoints(kSvtId) +
	  numberOfFitPoints(kSsdId);
    }
    //
    // New version
    //
    else {
        result = numberOfFitPoints(kTpcId) +
	  numberOfFitPoints(kFtpcWestId) +
	  numberOfFitPoints(kFtpcEastId) +
	  numberOfFitPoints(kSvtId) +
	  numberOfFitPoints(kSsdId) +	
	  numberOfFitPoints(kPxlId) +
	  numberOfFitPoints(kIstId);	
    }
    return mPrimaryVertexUsedInFit ? result+1 : result;
}

unsigned short
StTrackFitTraits::numberOfFitPoints(StDetectorId det) const
{
    //
    // Old and obsolete
    //
    if (mNumberOfFitPoints) {    
	// 1*tpc + 1000*svt + 10000*ssd (Helen/Spiros Oct 29, 1999)
	switch (det) {
	case kFtpcWestId:
	case kFtpcEastId:
	case kTpcId:
	    return mNumberOfFitPoints%1000;
	    break;
	case kSvtId:
	    return (mNumberOfFitPoints%10000)/1000;
	    break;
	case kSsdId:
	    return mNumberOfFitPoints/10000;
	    break;
	default:
	    return 0;
	}
    }
    //
    // New version
    //
    else {
	switch (det) {
	case kFtpcWestId:
	    return mNumberOfFitPointsFtpcWest;
	    break;
	case kFtpcEastId:
	    return mNumberOfFitPointsFtpcEast;
	    break;
	case kTpcId:
	    return mNumberOfFitPointsTpc;
	    break;
	case kSvtId:
	    return mNumberOfFitPointsSvt;
	    break;
	case kSsdId:
	    return mNumberOfFitPointsSsd;
	    break;
	case kPxlId:
	    return mNumberOfFitPointsPxl;
	    break;
	case kIstId:
	    return mNumberOfFitPointsIst;
	    break;
	default:
	    return 0;
	}
    }
}

StParticleDefinition*
StTrackFitTraits::pidHypothesis() const
{
    return StParticleTable::instance()->findParticleByGeantId(mPidHypothesis);
}

double
StTrackFitTraits::chi2(unsigned int i) const
{
    if (i < 2)
        return mChi2[i];
    else
        return 0;
}

StMatrixF
StTrackFitTraits::covariantMatrix() const
{
    StMatrixF m(5,5);
    if (mCovariantMatrix.GetSize() == 15) {
#define mCovariantMatrix ((TArrayF&)mCovariantMatrix)         //temporary HACK VP
        m(1,1) = mCovariantMatrix[0];
        m(1,2) = m(2,1) = mCovariantMatrix[1];
        m(1,3) = m(3,1) = mCovariantMatrix[2];
        m(1,4) = m(4,1) = mCovariantMatrix[3];
        m(1,5) = m(5,1) = mCovariantMatrix[4];
        m(2,2) = mCovariantMatrix[5];
        m(2,3) = m(3,2) = mCovariantMatrix[6];
        m(2,4) = m(4,2) = mCovariantMatrix[7];
        m(2,5) = m(5,2) = mCovariantMatrix[8];
        m(3,3) = mCovariantMatrix[9];
        m(3,4) = m(4,3) = mCovariantMatrix[10];
        m(3,5) = m(5,3) = mCovariantMatrix[11];
        m(4,4) = mCovariantMatrix[12];
        m(4,5) = m(5,4) = mCovariantMatrix[13];
        m(5,5) = mCovariantMatrix[14];
#undef mCovariantMatrix                                 //temporary HACK VP
    }
    return m;
}

bool
StTrackFitTraits::primaryVertexUsedInFit() const
{return mPrimaryVertexUsedInFit;}

void
StTrackFitTraits::clearCovariantMatrix() {mCovariantMatrix.Set(0);}

void
StTrackFitTraits::setNumberOfFitPoints(unsigned char val, StDetectorId det)
{
    mNumberOfFitPoints = 0;  // make sure old method is NOT active
    switch (det) {
    case kFtpcWestId:
	mNumberOfFitPointsFtpcWest = val;
	break;
    case kFtpcEastId:
	mNumberOfFitPointsFtpcEast = val;
	break;
    case kTpcId:
	mNumberOfFitPointsTpc = val;
	break;
    case kSvtId:
	mNumberOfFitPointsSvt = val;
	break;
    case kSsdId:
	mNumberOfFitPointsSsd = val;
	break;
    case kPxlId:
	mNumberOfFitPointsPxl = val;
	break;
    case kIstId:
	mNumberOfFitPointsIst = val;
	break;
    default:
	break;
    }
}

void
StTrackFitTraits::setPrimaryVertexUsedInFit(bool val)
{mPrimaryVertexUsedInFit = val;}

void StTrackFitTraits::setPidHypothesis(unsigned short val)
{
    mPidHypothesis = val;
}

void StTrackFitTraits::setChi2(float val, unsigned int i)
{
    if (i<2) mChi2[i] = val;
}

void StTrackFitTraits::setCovariantMatrix(float val[15])
{
    mCovariantMatrix.Set(15, val);
}

void StTrackFitTraits::Streamer(TBuffer &R__b)
{
//        Stream an object of class StTrackFitTraits.

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
    if (R__v > 4) {
       Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
    }
    StObject::Streamer(R__b);

    R__b >> (unsigned short&)mPidHypothesis;
    R__b >> (unsigned short&)mNumberOfFitPoints;

    if (R__v==2 && gFile && gFile->GetVersion()%100000<30000)
       { Int_t dumy; R__b >> dumy;}

    R__b.ReadFastArray(mChi2,2);
    mCovariantMatrix.Streamer(R__b);

  } else {
    Class()->WriteBuffer(R__b,this);
  }
}

/***************************************************************************
 *
 * $Id: StTrackABC.cxx,v 1.2 2014/01/14 14:48:24 fisyak Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackABC.cxx,v $
 * Revision 1.2  2014/01/14 14:48:24  fisyak
 * Freeze
 *
 * Revision 1.1.1.1  2013/07/23 14:13:29  fisyak
 *
 *
 * Revision 2.43  2013/07/16 14:29:04  fisyak
 * Restore mass fit tracks
 *
 * Revision 2.41  2013/04/05 15:11:33  ullrich
 * Changes due to the addition of StTrackMassFit (Yuri)
 *
 * Revision 2.40  2013/01/15 23:21:06  fisyak
 * improve printouts
 *
 * Revision 2.39  2012/05/07 14:42:58  fisyak
 * Add handilings for Track to Fast Detectors Matching
 *
 * Revision 2.38  2011/10/17 00:13:49  fisyak
 * Add handles for IdTruth info
 *
 * Revision 2.37  2011/10/13 21:25:27  perev
 * setting IdTruth from the hits is added
 *
 * Revision 2.36  2011/04/26 21:41:29  fisyak
 * Make mKey Int_t instead of UShort_t (no. of tracks might be more that 64k)
 *
 * Revision 2.35  2011/03/31 19:29:01  fisyak
 * Add IdTruth information for tracks and vertices
 *
 * Revision 2.34  2010/08/31 20:00:09  fisyak
 * Clean up, add mSeedQuality
 *
 * Revision 2.33  2009/11/23 16:34:07  fisyak
 * Cleanup, remove dependence on dst tables, clean up software monitors
 *
 * Revision 2.32  2009/04/29 23:02:36  perev
 * check for big lenght added
 *
 * Revision 2.31  2007/10/11 21:51:40  ullrich
 * Added member to handle number of possible points fpr PXL and IST.
 *
 * Revision 2.30  2006/08/28 17:04:46  fisyak
 * Don't check StPhysicalHelixD quality for Beam Background tracks (flag() == 901)
 *
 * Revision 2.29  2005/07/06 19:00:52  fisyak
 * Add include of StThreeVectorD.hh
 *
 * Revision 2.28  2004/11/08 22:25:38  perev
 * Remove StTrackABC test for wrong length. TPT only
 *
 * Revision 2.27  2004/10/20 18:55:13  ullrich
 * Name of enum changed: StStarMaxR(Z) now StStarMaxTrackRangeR(Z).
 *
 * Revision 2.26  2004/10/17 03:35:10  perev
 * Error check improved
 *
 * Revision 2.25  2004/08/13 18:15:08  ullrich
 * Added +1 to the number of possible points when primary track.
 *
 * Revision 2.24  2004/08/10 14:20:21  calderon
 * Putting the streamers back in.  They should not be needed, but
 * apparently removing them causes more problems.  Yuri tested that
 * putting them back in allows reading files again.
 *
 * Revision 2.23  2004/08/05 22:24:51  ullrich
 * Changes to the handling of numberOfPoints() to allow ITTF more flexibility.
 *
 * Revision 2.22  2004/01/26 22:56:28  perev
 * Add Finite for float
 *
 * Revision 2.21  2003/12/04 03:53:14  perev
 * Set empty, instead of crazy outer geometry
 *
 * Revision 2.20  2003/10/31 16:00:04  ullrich
 * Added setKey() method.
 *
 * Revision 2.19  2003/10/30 20:07:32  perev
 * Check of quality added
 *
 * Revision 2.18  2003/04/25 23:48:18  calderon
 * fittingMethod member function was missing case for kITKalmanFitId.
 *
 * Revision 2.17  2002/03/14 17:42:31  ullrich
 * Added method to set mNumberOfPossiblePoints.
 *
 * Revision 2.16  2002/02/27 19:09:22  ullrich
 * Updated fittingMethod(): L3 added.
 *
 * Revision 2.15  2001/09/28 22:20:49  ullrich
 * Added helix geometry at last point.
 *
 * Revision 2.14  2001/05/30 17:45:54  perev
 * StEvent branching
 *
 * Revision 2.13  2001/04/05 04:00:57  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.12  2001/03/16 20:56:45  ullrich
 * Added non-const version of fitTraits().
 *
 * Revision 2.11  2000/04/20 13:49:07  ullrich
 * Removed redundant line in operator=().
 *
 * Revision 2.10  2000/01/20 14:42:40  ullrich
 * Fixed bug in numberOfPossiblePoints(). Sum was wrong.
 *
 * Revision 2.9  1999/12/01 15:58:08  ullrich
 * New decoding for dst_track::method. New enum added.
 *
 * Revision 2.8  1999/12/01 00:15:27  didenko
 * temporary solution to compile the library
 *
 * Revision 2.7  1999/11/29 17:32:42  ullrich
 * Added non-const method pidTraits().
 *
 * Revision 2.6  1999/11/15 18:48:20  ullrich
 * Adapted new enums for dedx and track reco methods.
 *
 * Revision 2.5  1999/11/09 15:44:14  ullrich
 * Removed method unlink() and all calls to it.
 *
 * Revision 2.4  1999/11/05 15:27:04  ullrich
 * Added non-const versions of several methods
 *
 * Revision 2.3  1999/11/04 13:32:00  ullrich
 * Added non-const versions of some methods
 *
 * Revision 2.2  1999/11/01 12:45:02  ullrich
 * Modified unpacking of point counter
 *
 * Revision 2.1  1999/10/28 22:27:21  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:42:54  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include "StTrackABC.h"
#include "StVertex.h"
#include "StTrackNode.h"
ClassImp(StTrackABC)

static const char rcsid[] = "$Id: StTrackABC.cxx,v 1.2 2014/01/14 14:48:24 fisyak Exp $";
//________________________________________________________________________________
StTrackABC::StTrackABC() : StObject() 
{
    memset(mBeg, 0, mEnd-mBeg+1);
}
//________________________________________________________________________________
void StTrackABC::setVertex(StVertex* val) {
    StVertex *p = dynamic_cast<StVertex*>(val);
    if (p) mVertex = p;
}
//________________________________________________________________________________
StTrackNode*                   StTrackABC::node() {return mNode;}
//________________________________________________________________________________
const StTrackNode*             StTrackABC::node() const {return mNode;}
//________________________________________________________________________________
void                           StTrackABC::setNode(StTrackNode* node) {mNode=node;}
//________________________________________________________________________________
StTrackABC::StTrackABC(const StTrackABC& track) {
    for (Int_t bit = 14; bit < 23; bit++) if (track.TestBit(BIT(bit))) SetBit(BIT(bit));
    memcpy (mBeg, track.mBeg, mEnd-mBeg+1);
    mVertex = track.mVertex;
    mNode = 0;                                 // do not assume any context here
}
//________________________________________________________________________________
StTrackABC& StTrackABC::operator=(const StTrackABC& track) {
    if (this != &track) {
        for (Int_t bit = 14; bit < 23; bit++) if (track.TestBit(BIT(bit))) SetBit(BIT(bit));
        memcpy (mBeg, track.mBeg, mEnd-mBeg+1);
	mVertex = track.mVertex;
	mNode = 0;                                 // do not assume any context here
    }
    return *this;
}
//________________________________________________________________________________
std::ostream&  operator<<(std::ostream& os,  const StTrackABC& track) {
    os << Form("%4i ",track.key());
    if      (track.type() == global )                 os << "global";
    else if (track.type() == primary)                 os << "primary";
    else if (track.type() == massFit)                {os << "massFit"; return os;}
    else if (track.type() == massFitAtVx)            {os << "fitf@Vx"; return os;}
    else if (track.type() == tpt)                     os << "tpt    ";
    else if (track.type() == estGlobal)               os << "estGlob";
    else if (track.type() == estPrimary)              os << "estPrim";
    else                                             {os << "???????"; return os;}
    return os;
}
//________________________________________________________________________________

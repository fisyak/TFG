/***************************************************************************
 *
 * $Id: StTrackNode.cxx,v 1.2 2014/01/14 14:48:24 fisyak Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackNode.cxx,v $
 * Revision 1.2  2014/01/14 14:48:24  fisyak
 * Freeze
 *
 * Revision 1.1.1.1  2013/07/23 14:13:30  fisyak
 *
 *
 * Revision 2.16  2013/07/16 14:29:04  fisyak
 * Restore mass fit tracks
 *
 * Revision 2.14  2013/04/05 15:11:33  ullrich
 * Changes due to the addition of StTrackMassFit (Yuri)
 *
 * Revision 2.13  2003/11/25 04:10:47  perev
 * bug in erase fixed
 *
 * Revision 2.12  2003/09/02 17:58:06  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.11  2003/04/30 20:37:06  perev
 * Warnings cleanup. Modified lines marked VP
 *
 * Revision 2.10  2002/04/24 02:26:57  ullrich
 * Replaced iterator loop by index loop in entries(StTrackType).
 *
 * Revision 2.9  2002/04/18 23:38:21  jeromel
 * Implementation of the SVT 2 tables scheme ...
 *
 * Revision 2.8  2001/04/05 04:00:58  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.7  2000/08/17 00:35:27  ullrich
 * Added hooks for handling tpt tracks.
 *
 * Revision 2.6  2000/03/23 13:49:31  ullrich
 * Not implemented track type 'secondary' now handled
 * in a more clean way; entries(secondary) returns 0.
 *
 * Revision 2.5  1999/12/01 20:04:58  ullrich
 * Fixed bug in track() method.
 *
 * Revision 2.4  1999/11/09 15:44:17  ullrich
 * Removed method unlink() and all calls to it.
 *
 * Revision 2.3  1999/11/05 15:27:10  ullrich
 * Added non-const versions of several methods
 *
 * Revision 2.2  1999/10/28 22:27:44  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.1  1999/10/13 19:45:44  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include <Stiostream.h>
#include "StTrackNode.h"
#include "StGlobalTrack.h"
#include "StPrimaryTrack.h"
#include "StTrackMassFit.h"
ClassImp(StTrackNode)

static const char rcsid[] = "$Id: StTrackNode.cxx,v 1.2 2014/01/14 14:48:24 fisyak Exp $";

StTrackNode::StTrackNode() { /* noop */ }

StTrackNode::~StTrackNode() { /* noop */ }

void
StTrackNode::addTrack(StTrack* track)
{
    if (track) {
        switch (track->type()) {
            case primary:
            case estPrimary:
            case secondary:               
            case massFitAtVx:
                mReferencedTracks.push_back(track);
                break;
            case global:
            case tpt:
            case estGlobal:
            case massFit:               
                mOwnedTracks.push_back(track);
                break;
            default:
                cerr << "StTrackNode::addTrack(): cannot add, unknown track type." << endl;
                return;
                break;
        }
        track->setNode(this);
    }
}

void
StTrackNode::removeTrack(StTrack* track)
{
    StPtrVecTrackIterator  iter;
    StSPtrVecTrackIterator iterS;
    if (track) {
      UInt_t i, N;
        switch (track->type()) {
            case primary:
            case estPrimary:
            case secondary:
            case massFitAtVx:
	      N = mReferencedTracks.size();
	      for (i = 0; i < N; i++) if (mReferencedTracks[i] == track) mReferencedTracks[i] = 0;
	      for (iter = mReferencedTracks.begin(); iter < mReferencedTracks.end(); iter++)
		if (! *iter) mReferencedTracks.erase(iter);
	      break;
            case tpt:
            case global:
            case estGlobal:
            case massFit:
                for (iterS = mOwnedTracks.begin(); iterS < mOwnedTracks.end(); iterS++)
                    if (*iterS == track) mOwnedTracks.erase(iterS);
                break;
            default:
                cerr << "StTrackNode::removeTrack(): cannot remove, unknown track type." << endl;
                break;
        }
        track->setNode(0);
    }
}

unsigned int
StTrackNode::entries() const
{
    return mReferencedTracks.size() + mOwnedTracks.size();
}

const StTrack*
StTrackNode::track(unsigned int i) const
{
    if (i < mOwnedTracks.size())
        return mOwnedTracks[i];
    else {
        i -= mOwnedTracks.size();
        if (i < mReferencedTracks.size())
            return mReferencedTracks[i];
        else
            return 0;
    }
}

StTrack*
StTrackNode::track(unsigned int i)
{
    if (i < mOwnedTracks.size())
        return mOwnedTracks[i];
    else {
        i -= mOwnedTracks.size();
        if (i < mReferencedTracks.size())
            return mReferencedTracks[i];
        else
            return 0;
    }
}

unsigned int
StTrackNode::entries(StTrackType type) const
{
    unsigned int i;
    //VPunused StSPtrVecTrackConstIterator iterS;
    //VPunused StPtrVecTrackConstIterator  iter;
    unsigned int                counter;
    
    switch (type) {
        case primary:
        case estPrimary:
        case secondary:
        case massFitAtVx:
            for (counter=0, i=0; i < mReferencedTracks.size(); i++)
                if (mReferencedTracks[i]->type() == type) counter++;
            return counter;
            break;
        case tpt:
        case global:
        case estGlobal:
        case massFit:
            for (counter=0, i=0; i < mOwnedTracks.size(); i++)
                if (mOwnedTracks[i]->type() == type) counter++;
            return counter;
            break;
        default:
            cerr << "StTrackNode::entries(): unknown track type." << endl;
            return 0;
            break;
    }
}

const StTrack*
StTrackNode::track(StTrackType type, unsigned int i) const
{
    int          j;
    unsigned int k;
    const StTrack* t = 0;
    UInt_t nR = mReferencedTracks.size();
    UInt_t nO = mOwnedTracks.size();
    switch (type) {
        case primary:
        case estPrimary:
        case secondary:
        case massFitAtVx:
            for (j=-1, k=0; k < nR; k++) {
	      t = mReferencedTracks[k];
                if (t->type() == type) j++;
                if (j == static_cast<int>(i)) return t;
            }
            return 0;
            break;
        case tpt:
        case global:
        case estGlobal:
        case massFit:
            for (j=-1, k=0; k < nO; k++) {
	      t = mOwnedTracks[k];
                if (t->type() == type) j++;
                if (j == static_cast<int>(i)) return t;
            }
            return 0;
            break;
        default:
            cerr << "StTrackNode::track(): unknown track type." << endl;
            return 0;
            break;
    }
}

StTrack*
StTrackNode::track(StTrackType type, unsigned int i)
{
    int          j;
    unsigned int k;
    StTrack* t = 0;
    UInt_t nR = mReferencedTracks.size();
    UInt_t nO = mOwnedTracks.size();
    switch (type) {
        case primary:
        case estPrimary:
        case secondary:
        case massFitAtVx:
            for (j=-1, k=0; k < nR; k++) {
	      t = mReferencedTracks[k];
                if (t->type() == type) j++;
                if (j == static_cast<int>(i)) return t;
            }
            return 0;
            break;
        case tpt:
        case global:
        case estGlobal:
        case massFit:
            for (j=-1, k=0; k < nO; k++) {
	      t = mOwnedTracks[k];
                if (t->type() == type) j++;
                if (j == static_cast<int>(i)) return t;
            }
            return 0;
            break;
        default:
            cerr << "StTrackNode::track(): unknown track type." << endl;
            return 0;
            break;
    }
}
//________________________________________________________________________________
ostream&  operator<<(ostream& os,  const StTrackNode& node) {
  const StTrack *track;
  UInt_t nR = node.referencedTracks().size();
  UInt_t nO = node.ownedTracks().size();
#if 0
  os << "Track node : ";
  if (nR > 0) os << "Reference tracks:" << endl;
#endif
  for (UInt_t k = 0; k < nR + nO; k++) {
    if (k) os << endl;
#if 0
    if (k == nR) os << "Owned  tracks:" << endl;
#endif
    if (k < nR) track = node.referencedTracks()[k];
    else        track = node.ownedTracks()[k-nR];
    if (track->type() == primary) {
      os << *((const StPrimaryTrack*) track);
    } else if (track->type() == global) {
      os << *((const StGlobalTrack*) track);
    } else if (track->type() == massFitAtVx || track->type() == massFit) {
      os << *((const StTrackMassFit*) track);
    } else {
      os << *track;
    }
  }
  return os;
}
//________________________________________________________________________________

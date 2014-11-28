/***************************************************************************
 *
 * $Id: StVertex.cxx,v 1.2 2014/01/14 14:48:24 fisyak Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StVertex.cxx,v $
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
 * Revision 2.13  2012/05/07 14:42:58  fisyak
 * Add handilings for Track to Fast Detectors Matching
 *
 * Revision 2.12  2011/10/17 15:35:49  fisyak
 * Comment's fix
 *
 * Revision 2.11  2011/10/17 00:13:49  fisyak
 * Add handles for IdTruth info
 *
 * Revision 2.10  2009/11/23 16:34:08  fisyak
 * Cleanup, remove dependence on dst tables, clean up software monitors
 *
 * Revision 2.9  2003/09/02 17:58:06  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.8  2001/05/30 17:45:55  perev
 * StEvent branching
 *
 * Revision 2.7  2001/04/05 04:00:59  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.6  2000/03/08 14:29:54  ullrich
 * New method probChiSquared() added.
 *
 * Revision 2.5  2000/02/10 16:32:19  ullrich
 * flag changed from unsigned to signed long
 *
 * Revision 2.4  2000/01/11 19:22:12  ullrich
 * Added non-const parent() method.
 *
 * Revision 2.3  1999/12/21 15:09:23  ullrich
 * Modified to cope with new compiler version on Sun (CC5.0).
 *
 * Revision 2.2  1999/11/22 15:04:43  ullrich
 * Forgot to fill mPosition in constructor. Fixed now.
 *
 * Revision 2.1  1999/10/28 22:28:07  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:43:29  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include <algorithm>
#include "Riostream.h"
#include "TClass.h"
#include "TFile.h"
#include "StVertex.h"
#include "StTrack.h"
#include "StG2TrackVertexMap.h"
#include "StPrimaryTrack.h"
#include "TString.h"
#if !defined(ST_NO_NAMESPACES)
using std::fill_n;
using std::copy;
#endif

ClassImp(StVertex)

static const char rcsid[] = "$Id: StVertex.cxx,v 1.2 2014/01/14 14:48:24 fisyak Exp $";

UInt_t StVertex::fgNoFitPointCutForGoodTrack = 15;

StVertex::StVertex()
{
    mType = kUndefinedVtxId;
    memset(mBeg, 0, mEnd-mBeg+1);
    mParent = 0;
}

Int_t
StVertex::operator==(const StVertex& v) const
{
    return mType == v.mType &&
    mFlag == v.mFlag &&
    mPosition == v.mPosition &&
    mChiSquared == v.mChiSquared;
}

Int_t
StVertex::operator!=(const StVertex& v) const
{
    return !(v == *this);
}

StTrack*               StVertex::parent()         {return dynamic_cast<      StTrack*>        (parentABC());}
const StTrack*         StVertex::parent()   const {return dynamic_cast<const StTrack*>        (parentABC());}
StTrackMassFit*        StVertex::parentMF()       {return dynamic_cast<      StTrackMassFit *>(parentABC());}
const StTrackMassFit*  StVertex::parentMF() const {return dynamic_cast<const StTrackMassFit *>(parentABC());}
StMatrixF
StVertex::covariantMatrix() const
{
   const StTrackMassFit *mKFVertex = parentMF();
   StMatrixF m(3,3);
   if (! mKFVertex) {
     m(1,1) = mCovariantMatrix[0];
     m(1,2) = m(2,1) = mCovariantMatrix[1];
     m(2,2) = mCovariantMatrix[2];
     m(1,3) = m(3,1) = mCovariantMatrix[3];
     m(2,3) = m(3,2) = mCovariantMatrix[4];
     m(3,3) = mCovariantMatrix[5];
   } else {
     KFParticle *kf = (KFParticle *) mKFVertex->kfParticle();
     m(1,1) = kf->Covariance(0);
     m(1,2) = m(2,1) = kf->Covariance(1);
     m(2,2) = kf->Covariance(2);
     m(1,3) = m(3,1) = kf->Covariance(3);
     m(2,3) = m(3,2) = kf->Covariance(4);
     m(3,3) = kf->Covariance(5);
   }
   return m;
}

StThreeVectorF
StVertex::positionError() const
{
  const StTrackMassFit *mKFVertex = parentMF();
  if (! mKFVertex) {
    return StThreeVectorF(::sqrt(mCovariantMatrix[0]), ::sqrt(mCovariantMatrix[2]), ::sqrt(mCovariantMatrix[5]));
  } else {
    KFParticle *kf = (KFParticle *) mKFVertex->kfParticle();
    return StThreeVectorF(::sqrt(kf->Covariance(0)), ::sqrt(kf->Covariance(2)), ::sqrt(kf->Covariance(5)));
  }
}

void StVertex::setCovariantMatrix(float val[6]) { copy(val, val+6, mCovariantMatrix); }
void StVertex::setParent(StTrackABC* val) { 
  mParent = val; 
  StTrackMassFit *mKFVertex = parentMF();
  if (mKFVertex) {
    mPosition.set(mKFVertex->kfParticle()->X(),mKFVertex->kfParticle()->Y(),mKFVertex->kfParticle()->Z());
    mCovariantMatrix[0] = mKFVertex->kfParticle()->Covariance(0);
    mCovariantMatrix[1] = mKFVertex->kfParticle()->Covariance(1);
    mCovariantMatrix[2] = mKFVertex->kfParticle()->Covariance(2);
    mCovariantMatrix[3] = mKFVertex->kfParticle()->Covariance(3);
    mCovariantMatrix[4] = mKFVertex->kfParticle()->Covariance(4);
    mCovariantMatrix[5] = mKFVertex->kfParticle()->Covariance(5);
  }
}
//________________________________________________________________________________
void StVertex::setIdTruth() { // match with IdTruth
    typedef std::map< Int_t,Float_t>  myMap_t;
    typedef myMap_t::const_iterator myIter_t;
    myMap_t  idTruths;
    // Loop to store all the mc vertex keys and quality of every reco track on the vertex.
    UInt_t Ntracks = numberOfDaughters();
    Int_t IdVx = 0;
    Int_t qa = 0;
    for (UInt_t l = 0; l < Ntracks; l++) {
      const StTrack *pTrack = dynamic_cast<const StTrack *>(daughter(l));
        if (! pTrack) continue;
        Int_t IdTk = pTrack->idTruth();
        if (IdTk <= 0) continue;
        IdVx = pTrack->idParentVx();
        if (IdVx <= 0) continue;
        qa = pTrack->qaTruth(); if (!qa) qa = 1;
        idTruths[IdVx] += qa;
    }
    if (! idTruths.size()) return;		//no simu info
    Int_t vxBest = 0; 
    Float_t qaBest = 0, qaSum = 0;
    for (myIter_t it=idTruths.begin(); it!=idTruths.end(); ++it) {
        qaSum += (*it).second;
        if ((*it).second < qaBest) continue;
        vxBest = (*it).first; qaBest = (*it).second;
    }
    if (vxBest <= 0 || vxBest > 0xffff) return;
    Int_t avgQua = 100*qaBest/(qaSum+1e-10)+0.5;
    setIdTruth(vxBest,avgQua);
    Int_t IdParentTk = StG2TrackVertexMap::instance()->IdParentTrack(vxBest);
    setIdParent(IdParentTk);
}
//______________________________________________________________________________
void StVertex::NotImplemented(const Char_t *method) const {
    std::cout << "StVertex::" << method << " is no implemented" <<  std::endl;
}
//______________________________________________________________________________
StTrackMassFit* StVertex::massFit(UInt_t i){
    return i < mMassFits.size() ? mMassFits[i] : 0;
}
//______________________________________________________________________________
const StTrackMassFit* StVertex::massFit(UInt_t i) const {
    return i < mMassFits.size() ? mMassFits[i] : 0;
}
//______________________________________________________________________________
void StVertex::addMassFit(StTrackMassFit* t) {
  if (t) {
    mMassFits.push_back(t);
    t->setVertex(this);
  }
}
//______________________________________________________________________________
void StVertex::removeMassFit(StTrackMassFit * t) {
    StTrackMassFit* p = dynamic_cast<StTrackMassFit*>(t);
    if (!p) return;
    StSPtrVecTrackMassFitIterator iter;
    if (p->type() == massFitAtVx) {
        for (iter=mMassFits.begin(); iter != mMassFits.end(); iter++)
            if (*iter == t) {
                mMassFits.erase(iter);
                p->setVertex(0);
            }
    }
}
//________________________________________________________________________________
void StVertex::Print(Option_t *option) const {
  std::cout << *this << std::endl; 
  if (TString(option).Contains("f",TString::kIgnoreCase)) {
    if (parentMF()) std::cout << "Parent:" << *parentMF() << std::endl; 
    UInt_t No = numberOfDaughters();
    for (UInt_t i = 0; i < No; i++) {
      const StPrimaryTrack *pTrack = (const StPrimaryTrack *) daughter(i);
      std::cout << Form("%4i:",i);
      if (pTrack) pTrack->Print();
      else        std::cout << std::endl;
    }
    No = numberOfMassFits();
    for (UInt_t i = 0; i < No; i++) {
      const StTrackMassFit *mf = (const StTrackMassFit *) massFit(i);
      if (mf) {std::cout << Form("%4i:",i) << *mf << std::endl;}
    }
  }
}
//________________________________________________________________________________
std::ostream&  operator<<(std::ostream& os,  const StVertex& v) {
  const StTrackMassFit *mF = dynamic_cast<const StTrackMassFit *>(v.parent());
  if (mF && mF->kfParticle()) {
    os << *mF->kfParticle();
  } else {
    const Float_t *xyz = v.position().xyz();
    const Float_t *dxyz = v.positionError().xyz();
    for (Int_t i = 0; i < 3; i++)     {
      if (dxyz[i] <= 9.999) os << Form("%8.3f+/-%5.3f,",xyz[i],dxyz[i]);
      else 	                os << Form("%8.3f+/-9.999,",xyz[i]);
    }
    UInt_t nGoodTpcTracks = 0, nTpcTracks = 0;
    UInt_t nDaughters = v.numberOfDaughters();
    for (UInt_t i=0; i < nDaughters; i++) {
      StPrimaryTrack* pTrack = (StPrimaryTrack*) v.daughter(i);
      if (! pTrack) continue;
      Int_t good = (pTrack->flag() > 0 && pTrack->fitTraits().numberOfFitPoints() >=  StVertex::NoFitPointCutForGoodTrack()) ? 1 : 0;
      if (pTrack->fitTraits().numberOfFitPoints(kTpcId)) {
	nTpcTracks++; nGoodTpcTracks+=good;
      } 
    }
    os << " Prob/Chi2: " << Form("%5.3f/%7.2f",v.probChiSquared(),v.chiSquared());
#if 0
    os << " Rank: "      << Form("%8.1f",v.ranking());
    os << Form(" U/T/G: %4i,%4i,%4i", v.numTracksUsedInFinder(),nDaughters,v.numberOfGoodTracks());
    if (nTpcTracks != nDaughters || nGoodTpcTracks != v.numberOfGoodTracks()) {
      os << Form(" TPC:%4i,%4i",nTpcTracks,nGoodTpcTracks);
    }
#endif
    if (v.idTruth())  os << Form(" IdT: %5i Q:%3i", v.idTruth(), v.qaTruth());
  }
  return os;
}


/*!
 * \class StTrackFitTraits 
 * \author Thomas Ullrich, Sep 1999
 */
/***************************************************************************
 *
 * $Id: StTrackFitTraits.h,v 1.1.1.1 2013/07/23 14:13:30 fisyak Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 * With the arrival of new detectors when even not known will be this 
 * detector installed or not the following modification was made:
    1) mNumberOfFitPoints == 0xA000 + total number of fit points
    2) numberOfFitPoints() returns total amount of fitted points 
    3) number of hits for XXXX detector is 
       numberOfFitPoints()-numberOfFitPoints(kTpcId)
    4) If detector XXXX will be installed, according member could be added
      
   (VP)  

 * Note the following: with the arrival of ITTF it is now possible to
 * store the numberOfFitPoints for every detector individually. Before
 * that and because of the way the tables were defined TPC and FTPC were
 * one and the same. This caused confusion. However, since we have to
 * stay backwards compatible the "old way" is still working.
 * If
 * a) mNumberOfFitPoints == 0 the new encoding is the one to use, i.e.,
 *    mNumberOfFitPointsTpc
 *    mNumberOfFitPointsFtpcWest 
 *    mNumberOfFitPointsFtpcEast 
 *    mNumberOfFitPointsSvt 
 *    mNumberOfFitPointsSsd 
 *    are the ones that count
 * b) mNumberOfFitPoints != 0 then we still loaded the info from
 *    the tables and all is as it was before, i.e., we do not distinguish
 *    between FTPC and TPC.
 *
 ***************************************************************************
 *
 * $Log: StTrackFitTraits.h,v $
 * Revision 1.1.1.1  2013/07/23 14:13:30  fisyak
 *
 *
 * Revision 2.19  2013/07/16 14:29:04  fisyak
 * Restore mass fit tracks
 *
 * Revision 2.17  2013/04/05 15:11:33  ullrich
 * Changes due to the addition of StTrackMassFit (Yuri)
 *
 * Revision 2.16  2012/04/27 01:45:12  perev
 * Logic for total numbers of fit points changed
 *
 * Revision 2.15  2009/11/23 16:34:07  fisyak
 * Cleanup, remove dependence on dst tables, clean up software monitors
 *
 * Revision 2.14  2008/03/19 14:40:56  fisyak
 * Add access to covariance matrix array
 *
 * Revision 2.13  2007/10/11 21:52:32  ullrich
 * Added member to handle number of fit points for PXL and IST.
 *
 * Revision 2.12  2004/12/02 23:35:13  ullrich
 * Added misisng setXXX functions.
 *
 * Revision 2.11  2004/08/13 18:15:42  ullrich
 * Added +1 to the number of fit points when bool flag is set.
 *
 * Revision 2.10  2004/08/12 17:22:31  fisyak
 * Switch to automatic streamer for version >4 to account new no. of fit points definition
 *
 * Revision 2.9  2004/08/05 22:24:32  ullrich
 * Changes to the handling of numberOfPoints() to allow ITTF more flexibility.
 *
 * Revision 2.8  2002/02/22 22:56:52  jeromel
 * Doxygen basic documentation in all header files. None of this is required
 * for QM production.
 *
 * Revision 2.7  2001/05/04 19:50:52  perev
 * Streamer to account old ROOT2
 *
 * Revision 2.6  2001/04/05 04:00:45  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.5  2001/03/24 03:35:00  perev
 * clone() -> clone() const
 *
 * Revision 2.4  2001/03/16 21:31:42  ullrich
 * Changed version number from 1 to 2.
 *
 * Revision 2.3  2001/03/16 20:57:45  ullrich
 * Covariant matrix now stored in TArrayF.
 *
 * Revision 2.2  1999/11/01 12:45:17  ullrich
 * Modified unpacking of point counter
 *
 * Revision 2.1  1999/10/28 22:27:35  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:43:02  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#ifndef StTrackFitTraits_hh
#define StTrackFitTraits_hh
#include "StObject.h"
#include "StEnumerations.h"
#include "StMatrixF.hh"
#include "StThreeVectorF.hh"
#include "TArrayF.h"

class StParticleDefinition;

class StTrackFitTraits : public StObject {
public:
    StTrackFitTraits();
    StTrackFitTraits(UShort_t, UShort_t, Float_t[2], Float_t[15]);
    StTrackFitTraits(UShort_t, UShort_t, Float_t[2], TArrayF&);
    // StTrackFitTraits(const StTrackFitTraits&);            use default
    // StTrackFitTraits& operator=(const StTrackFitTraits&); use default
    virtual ~StTrackFitTraits();

    UShort_t               numberOfFitPoints() const;
    UShort_t               numberOfFitPoints(StDetectorId) const;
    StParticleDefinition*  pidHypothesis() const;
    StMatrixF              covariantMatrix() const;
    const Float_t*         covariance() const {return mCovariantMatrix.GetArray();}
    const TArrayF&         covarianceMatrixF() const {return *&mCovariantMatrix;}
    Double_t               chi2(UInt_t = 0) const;
    bool                   primaryVertexUsedInFit() const;
    UShort_t               geantId() const {return mPidHypothesis;}
    void                   clearCovariantMatrix();
    void                   setNumberOfFitPoints(unsigned char n, StDetectorId id=kUnknownId);
    void                   setPrimaryVertexUsedInFit(bool);
    
    void                   setPidHypothesis(UShort_t);
    void                   setChi2(Float_t, UInt_t = 0);
    void                   setCovariantMatrix(Float_t[15]);
    void                   setCovariantMatrix(TArrayF &cov);
    StThreeVectorF         momentum();
    StThreeVectorF         momentumErrors();
    StMatrixF              momentumCovariance();
    
protected:
    UShort_t mPidHypothesis;       // GeantId
    UShort_t mNumberOfFitPoints;   // obsolete since ITTF
    UChar_t  mNumberOfFitPointsTpc;
    UChar_t  mNumberOfFitPointsFtpcWest;
    UChar_t  mNumberOfFitPointsFtpcEast;
    UChar_t  mNumberOfFitPointsSvt;
    UChar_t  mNumberOfFitPointsSsd;
    UChar_t  mNumberOfFitPointsPxl;
    UChar_t  mNumberOfFitPointsIst;
    Bool_t   mPrimaryVertexUsedInFit;
    Float_t  mChi2[2];
    TArrayF  mCovariantMatrix;
    
    ClassDef(StTrackFitTraits,8) 
};
#endif

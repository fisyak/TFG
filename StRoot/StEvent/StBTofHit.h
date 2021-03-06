/*!
 * \class StBTofHit
 * \author Xin Dong, Nov 2008
 */
/***************************************************************************
 *
 * $Id: StBTofHit.h,v 2.5 2016/02/25 17:10:19 ullrich Exp $
 *
 * Author: Xin Dong, Nov 2008
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StBTofHit.h,v $
 * Revision 2.5  2016/02/25 17:10:19  ullrich
 * Implemented detector() which is now a pure abstract method in StHit.
 *
 * Revision 2.4  2012/05/07 14:42:57  fisyak
 * Add handilings for Track to Fast Detectors Matching
 *
 * Revision 2.3  2009/02/13 22:29:03  ullrich
 * Fixed typo in ostream<< operator.
 *
 * Revision 2.2  2009/01/15 00:46:26  ullrich
 * tray() now returns int.
 *
 * Revision 2.1  2008/12/22 20:30:58  ullrich
 * Initial Revision.
 *
 *
 **************************************************************************/
#ifndef StBTofHit_hh
#define StBTofHit_hh

#include <Stiostream.h>
#include "StHit.h"
#include "StContainers.h"

class StTrack;
class StBTofHit;
ostream& operator<<(ostream&, const StBTofHit&); // Printing operator

class StBTofHit : public StHit {
public:
    enum {
        kNTray   = 120,  //! 120 TOF trays
        kNModule =  32,  //! 32 modules per tray
        kNCell     = 6   //! 6 cells per module
    };
    StBTofHit();
    ~StBTofHit() {}
    Int_t             tray()             const { return mTray; }
    Int_t             module()           const { return mModule; }
    Int_t             cell()             const { return mCell; }
    Int_t             ID()               const { return kNModule*(tray()-1) + module() - 1;}
    Double_t          leadingEdgeTime()  const { return mLeadingEdgeTime; }
    Double_t          trailingEdgeTime() const { return mTrailingEdgeTime; }
    Double_t          tot()              const { return mTrailingEdgeTime - mLeadingEdgeTime; }
    StTrack*          associatedTrack();
    const StTrack*    associatedTrack() const;
    Float_t           pathLength() const { return mPathLength;}
    Float_t           time()       const { return mTime;}
    void setTray(UChar_t trayId)            { mTray = trayId; }
    void setModule(UChar_t moduleId)        { mModule = moduleId; }
    void setCell(UChar_t cellId)            { mCell = cellId; }
    void setLeadingEdgeTime(Double_t time)  { mLeadingEdgeTime = time; }
    void setTrailingEdgeTime(Double_t time) { mTrailingEdgeTime = time; }
    void setAssociatedTrack(StTrack*);
    void setPathLength(Float_t p = 0)       { mPathLength = p;}
    void setTime(Float_t p = 0)             { mTime = p;}
    const StThreeVectorF& locPosition() const;
    static Float_t    padWidth()            { return mBTofPadWidth;}
    StDetectorId   detector() const;
    Int_t volumeID() const {return 0;}
    virtual void           Print(Option_t *option="") const {cout << *this << endl;}
    static  const Char_t  *GetFMT()                     {return fgFMT.Data();}  
    static  void           SetFMT(const Char_t *path=0) {fgFMT = path;}  
    virtual const Char_t  *GetPath() const;    
protected:
    UChar_t   mTray;
    UChar_t   mModule;
    UChar_t   mCell;
    Double_t  mLeadingEdgeTime;
    Double_t  mTrailingEdgeTime;
    Float_t   mPathLength; // MC
    Float_t   mTime;       // MC
    const static Float_t mBTofPadWidth;
    //    StTrack *mAssociatedTrack;   //$LINK
#if defined(__CINT__) || defined(__CLING__)
    StObjLink        mAssociatedTrack;
#else
    StLink<StTrack>  mAssociatedTrack;
#endif //__CINT__
    static TString fgFMT;        
    ClassDef(StBTofHit,3)
};

inline StDetectorId StBTofHit::detector() const {return kBTofId;}

#endif

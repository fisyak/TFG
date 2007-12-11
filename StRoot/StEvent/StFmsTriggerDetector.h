/*!
 * \class StFmsTriggerDetector 
 * \author Akio Ogawa, Apr 2007
 */
/***************************************************************************
 *
 * $Id: StFmsTriggerDetector.h,v 2.3 2007/12/11 18:11:13 ullrich Exp $
 *
 * Author: Akio Ogawa, Apr 2007
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StFmsTriggerDetector.h,v $
 * Revision 2.3  2007/12/11 18:11:13  ullrich
 * Fix bugs in QT decoding (Akio).
 *
 * Revision 2.2  2007/07/11 23:06:45  perev
 * Cleanup+fix StXXXTriggerDetector
 *
 * Revision 2.1  2007/07/02 20:21:55  ullrich
 * Initial Revision.
 *
 *
 **************************************************************************/
#ifndef StFmsTriggerDetector_hh
#define StFmsTriggerDetector_hh
#include "StObject.h"

class StTriggerData;

class StFmsTriggerDetector : public StObject {
public:
    StFmsTriggerDetector();
    StFmsTriggerDetector(const StTriggerData&);
    virtual ~StFmsTriggerDetector();
    // StFmsTriggerDetector(const StFmsTriggerDetector&);            use default
    // StFmsTriggerDetector& operator=(const StFmsTriggerDetector&); use default
    
    unsigned int   nHit() const;
    unsigned int   hit(int line) const;
    unsigned short adc(int crate,  int addr,  int dcard,  int dch);
    unsigned short tdc(int crate,  int addr,  int dcard,  int dch);
    void  dump();
        
protected:
    void decode();
    unsigned short getNHT(int) const;
    unsigned short getADR(int) const;
    unsigned short getCRT(int) const;
    unsigned short getADC(int) const;
    unsigned short getTDC(int) const;
    unsigned short getQT8(int) const;
    unsigned short getCHA(int) const; 
    
protected:
    enum {
      mMaxLine     = 1600,
      mMaxDSM      = 256,
      mMaxDSM01    = 112,
      mMaxDSM02    = 16,
      mMaxDSM1     = 16,
      mMaxDSM2     = 8,
      mMaxCrate    = 4,
      mMaxAddr     = 16,
      mMaxDCard    = 4,
      mMaxChan     = 8,
      mOffsetCrate = 11,
      mOffsetAddr  = 16
    }; //!
    
    char mBeg[1];//!
    UInt_t   mNumQTdata;
    UInt_t   mQTdata[mMaxLine];
    Char_t   mDSM[mMaxDSM];
    Char_t   mDSM01[mMaxDSM01];
    Char_t   mDSM02[mMaxDSM02];
    UShort_t mDSM1[mMaxDSM1];
    UShort_t mDSM2[mMaxDSM2];

    int  mNumHeader; //!
    unsigned short mADC[mMaxCrate][mMaxAddr][mMaxDCard][mMaxChan]; //!
    unsigned short mTDC[mMaxCrate][mMaxAddr][mMaxDCard][mMaxChan]; //!
    char mEnd[1];//!

    ClassDef(StFmsTriggerDetector,2)      
};

inline unsigned int   StFmsTriggerDetector::nHit() const {return mNumQTdata;} 
inline unsigned short StFmsTriggerDetector::getNHT(int v) const {return (unsigned short)  (v & 0x000000FF); }
inline unsigned short StFmsTriggerDetector::getADR(int v) const {return (unsigned short) ((v & 0x001F0000) >> 16);}
inline unsigned short StFmsTriggerDetector::getCRT(int v) const {return (unsigned short) ((v & 0xFF000000) >> 24);}
inline unsigned short StFmsTriggerDetector::getADC(int v) const {return (unsigned short)  (v & 0x00000FFF);}
inline unsigned short StFmsTriggerDetector::getTDC(int v) const {return (unsigned short) ((v & 0x001F0000) >> 16);}
inline unsigned short StFmsTriggerDetector::getQT8(int v) const {return (unsigned short) ((v & 0x18000000) >> 27);}
inline unsigned short StFmsTriggerDetector::getCHA(int v) const {return (unsigned short) ((v & 0xE0000000) >> 29);} 

#endif

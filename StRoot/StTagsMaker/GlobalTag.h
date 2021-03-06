#ifndef __GlobalTag_st__
#define __GlobalTag_st__
#include "TDataSet.h"
#include "TTable.h"
#include "Ttypes.h"
//
// $Id: GlobalTag.idl,v 1.3 2009/11/23 16:47:59 fisyak Exp $
//
// Global tag
//
// $Log: GlobalTag.idl,v $
// Revision 1.3  2009/11/23 16:47:59  fisyak
// Add Primary vertex position errors
//
// Revision 1.2  2004/07/30 20:15:09  fisyak
// Synchronize Pcoll and Global Tags, remove TpcTags fro chain
//
// Revision 1.1  2004/07/30 14:15:10  fisyak
// Back to idl tag definition
//
//
struct GlobalTag_st {                                           // Global:
  unsigned long   TriggerId[32];                       // Trigger Id's satisfied by an event
  unsigned long   uncorrectedNumberOfPrimaries;        // TPC StuRefMult
  unsigned long   uncorrectedNumberOfFtpcEastPrimaries;// FTPC StuFtpcRefMult
  unsigned long   uncorrectedNumberOfFtpcWestPrimaries;// FTPC StuFtpcRefMult
  double          primaryVertexX;                      // Primary vertex (x)
  double          primaryVertexY;                      //                (y)
  double          primaryVertexZ;                      //                (z)
  double          sigmaPVX;                            // Sigma of Primary vertex (x)
  double          sigmaPVY;                            //                         (y)
  double          sigmaPVZ;                            //                         (z)
  short           primaryVertexFlag;                   // with some flag !=0 if not found
  unsigned short  zdcHardSum;                          // trigData->zdcAtChannel(10)
  float           CTBsum;                              // CTB sum mMips[mMaxTrays][mMaxSlats][0]
 };

class St_GlobalTag : public TTable {
 public:
  ClassDefTable(St_GlobalTag,GlobalTag_st)
  ClassDef(St_GlobalTag,1) //C++ container for chain/makers status 
};

#endif /* __GlobalTag_st__ */

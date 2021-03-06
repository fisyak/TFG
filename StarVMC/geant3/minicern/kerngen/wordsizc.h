/*
* $Id: wordsizc.h,v 1.2 2018/06/28 16:15:59 fisyak Exp $
*
* $Log: wordsizc.h,v $
* Revision 1.2  2018/06/28 16:15:59  fisyak
* Merge with geant3
*
* Revision 1.1.1.1  2002/06/16 15:18:46  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:29  fca
* AliRoot sources
*
* Revision 1.1.1.1  1996/02/15 17:49:19  mclareni
* Kernlib
*
*
*
* wordsizc.h
*/
#if defined(CERNLIB_QMIRTD)
#define NBITPW 64      /* Number of bits  per word */
#define NBYTPW 8       /* Number of bytes per word */
#else
#define NBYTPW 4       /* Number of bytes per word */
#endif

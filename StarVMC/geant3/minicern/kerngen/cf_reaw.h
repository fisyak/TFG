/*
* $Id: cf_reaw.h,v 1.2 2018/06/28 16:15:58 fisyak Exp $
*
* $Log: cf_reaw.h,v $
* Revision 1.2  2018/06/28 16:15:58  fisyak
* Merge with geant3
*
* Revision 1.1.1.1  2002/06/16 15:18:46  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:29  fca
* AliRoot sources
*
* Revision 1.2  1997/02/04 17:35:36  mclareni
* Merge Winnt and 97a versions
*
* Revision 1.1.1.1.2.1  1997/01/21 11:30:26  mclareni
* All mods for Winnt 96a on winnt branch
*
* Revision 1.1.1.1  1996/02/15 17:49:17  mclareni
* Kernlib
*
*
*
* cf#reaw.inc
*/
#if defined(CERNLIB_QMAPO)
#elif defined(CERNLIB_QMOS9)
#include <stdio.h>           /*  Microware OS-9             */
#elif defined(CERNLIB_QMVAX)
#include <file.h>            /*  VAX/VMS                    */
#elif defined(CERNLIB_QMDOS) ||defined(CERNLIB_WINNT)
 #ifdef WIN32
# ifdef __STDC__
#  undef __STDC__
# endif
  #include <io.h>
 #endif
#else
#include <unistd.h>          /*  default Posix              */
#endif

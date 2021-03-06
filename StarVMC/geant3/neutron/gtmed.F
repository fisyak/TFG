*
* $Id: gtmed.F,v 1.2 2018/06/28 19:29:23 fisyak Exp $
*
* $Log: gtmed.F,v $
* Revision 1.2  2018/06/28 19:29:23  fisyak
* Merge with geant3
*
* Revision 1.1.1.3  2009/02/18 20:32:01  fisyak
* *** empty log message ***
*
* Revision 1.1.1.1  2002/06/16 15:18:43  hristov
* Separate distribution  of Geant3
*
* Revision 1.1.1.1  1999/05/18 15:55:21  fca
* AliRoot sources
*
* Revision 1.1.1.1  1995/10/24 10:21:57  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.48  by  S.Giani
*-- Author :
      SUBROUTINE GTMED(MEDGEA,MEDMOR)
#include "geant321/mmicap.inc"
#include "geant321/minput.inc"
C get MICAP material number
      DO 10 I=1,MEDIA
         IF(LD(LGE2MO+I).EQ.MEDGEA) THEN
            MEDMOR = I
            GOTO 20
         ENDIF
   10 CONTINUE
      WRITE(IOUT,'('' MICAP GTMED: GEANT Medium '',I5,
     +             '' not found ==> STOP'')') MEDGEA
      CALL ABEND
   20 RETURN
      END

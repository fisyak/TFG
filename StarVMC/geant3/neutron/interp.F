*
* $Id: interp.F,v 1.2 2018/06/28 19:29:23 fisyak Exp $
*
* $Log: interp.F,v $
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
*CMZ :  3.21/04 23/02/95  14.46.01  by  S.Giani
*-- Author :
      SUBROUTINE INTERP(X,Y,X1,Y1,X2,Y2,INT)
C       THIS ROUTINE PERFORMS THE INTERPOLATION ACCORDING
C       TO THE ENDF/B INTERPOLATION SCHEME INT
#include "geant321/minput.inc"
#if __GNUC__ < 7
      SAVE
#endif
      IF(INT.LT.1.OR.INT.GT.5)GO TO 60
      IF(X2.EQ.X1)GO TO 10
      GO TO (10,20,30,40,50),INT
   10 Y=Y1
      RETURN
   20 Y=Y1+(X-X1)*(Y2-Y1)/(X2-X1)
      RETURN
   30 IF(X1.EQ.0.0.OR.X2.EQ.0.0)GO TO 20
      Y=Y1+ALOG(X/X1)*(Y2-Y1)/ALOG(X2/X1)
      RETURN
   40 IF(Y1.EQ.0.0.OR.Y2.EQ.0.0)GO TO 20
      Y=Y1*EXP((X-X1)*ALOG(Y2/Y1)/(X2-X1))
      RETURN
   50 IF(Y1.EQ.0.0.OR.Y2.EQ.0.0)GO TO 30
      IF(X1.EQ.0.0.OR.X2.EQ.0.0)GO TO 40
      Y=Y1*EXP(ALOG(X/X1)*ALOG(Y2/Y1)/ALOG(X2/X1))
      RETURN
   60 WRITE(IOUT,10000)INT
10000 FORMAT(' MICAP: INTERP-INVALID INTERPOLATION SCHEME',I11)
      WRITE(6,*) ' CALOR: ERROR in INTERP ====> STOP '
      CALL ABEND
      END

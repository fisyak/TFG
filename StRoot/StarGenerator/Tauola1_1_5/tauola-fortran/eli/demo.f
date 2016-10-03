      PROGRAM SPINTAU
C     *****************
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      COMMON/PYSUBS/MSEL,MSELPD,MSUB(500),KFIN(2,-40:40),CKIN(200)
      COMMON/PYPARS/MSTP(200),PARP(200),MSTI(200),PARI(200)
      COMMON/PYDAT1/MSTU(200),PARU(200),MSTJ(200),PARJ(200)
      COMMON/PYDAT3/MDCY(500,3),MDME(8000,2),BRAT(8000),KFDP(8000,5)
      COMMON / INOUT / INUT,NOUT
      EXTERNAL PYDATA !, LUDATA
 
C to prevent tau decays in jetset
       mdcy(15,1)=0
c------------------------------------>>>>>>
C-----output ident for TAUOLA.
         NOUT2 = 6
         NOUT  = 6
c.....JETSET output will be writen on NOUT2
       MSTU(11) = NOUT2
c..... PHOTOS will write on 6
CC-------------------------------------->>>>
c      OPEN(0      ,file='tauola.out')
      OPEN(NOUT   ,file='demo.out')

c------------------------------------>>>>>>
    
c------------------------------------>>>>>>
C------number of requested events
       NEVT=9
         KEYPOL=1
         CALL TAUOLA(-1,KEYPOL)


         msel=0
         msub(30)=1  !fg --> fZ
         msub(15)=1  !ff --> gZ
         DO I=174,189
           MDME(I,1) = 0
         ENDDO
           MDME(186,1) = 1 ! only Z --> tau+ tau- decay
         mstp(128)=1
         call pyinit('cms','p','p',14000.D0)
         call PYlist(12)

C>>>>>>>>>>>>>>
         DO II= 1,NEVT
           IF(MOD(II,50).EQ.1) WRITE(6,*)'event no=',II
           mdcy(15,1)=0
           CALL PYEVNT
!           if(ii.lt.5)CALL PYLIST(1)
           CALL PYHEPC(1)
           n11=ihepdim(dum) ! no of entries in hepevt before tauola
           CALL TAUOLA(0,KEYPOL)
           n12=ihepdim(dum) ! no of entries in hepevt after  tauola


           if (n12.gt.n11) then
             IF(II.LT.101) CALL PYLIST(1)
                           CALL PYHEPC(2)
             IF(II.LT.101) CALL PYLIST(1)
           endif
         ENDDO
CC>>>>>>>>>>>>>>
         CALL TAUOLA(1,KEYPOL)

      CLOSE(NOUT2)
      CLOSE(NOUT)
      END

 

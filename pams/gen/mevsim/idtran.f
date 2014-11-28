        INTEGER FUNCTION IDTRAN(ID)
C Returns the HEP ID of stable particles
C ID is the GEANT3 ID 
	PARAMETER (NID=112)
	INTEGER ID, IDHEP(NID),IDRES(20)
	DATA IDHEP/22,-11,11,14,-13,13,111,211,-211,130,321,-321
     *,2112,2212,-2212,310,221,3122,3222,3212,3112,3322,3312,3334
     *,-2112,-3122,-3112,-3212,-3222,-3322,-3312,-3334,-15,15
     *,411,-411,421,-421,431,-431,4122,24,-24,23,700201,700301
     *,700402,0,700302,333,-4122,443,521,-521,511,-511,531,-531
     *,0,0,700603,700703,700704,700904,701005,701105,701206
     *,701407,701608,701909,702010,702311,702412,702713
     *,702814,703115,703216,703517,703618,703919,704020
     *,704521,704822,705123,705224,705525,705626,705927
     *,705828,706329,706430,707432,708034,708436,708838
     *,709040,709842,710646,711448,712050,713254,713856
     *,714058,715262,716466,717470,718474,719478,719779
     *,720280,720882,723892/
        DATA IDRES/213,-213,113,223,331,333,443,1114,2114,2214,2224
     *,323,-323,313,10221,5*0/
        IF(ID.GT.112) THEN
          IDD=ID-150
          IDTRAN=IDRES(IDD)
        ELSE
	  IDTRAN=IDHEP(ID)
        ENDIF
	RETURN
	END

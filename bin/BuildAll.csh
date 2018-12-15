#! /usr/bin/env tcsh 
set list = "";
set domain = `hostname -d`
switch ($domain) 
  case "*local":
  case "*starp.bnl.gov":
    set list = "gcc gcc521 gcc631" # gcc7";
    breaksw
  case "*bnl.gov":
#    set list = "gcc482 gcc492";
    set list = "gcc"
    breaksw
endsw
foreach gcc (${list})
  foreach opt (debug opt)
    set bits = "64b";
    if ($gcc == "gcc") set bits = "32b 64b";
    foreach bit (${bits})
      if ($opt == "debug") then 
        unsetenv NODEBUG
      else      
        setenv NODEBUG yes
      endif
      setup ${gcc}
      setup ${bit}
      starver ${STAR_LEVEL}
      set log = build.${gcc}.${opt}.${bit}.`date +%m%d%y:%H%M.`${HOST}.log;
      printenv > ${log};
      time cons -k >>& ${log} &
    end
  end
end

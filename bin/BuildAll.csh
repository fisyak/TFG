#! /usr/bin/env tcsh 
foreach gcc (gcc gcc492)
  foreach opt (debug opt)
    foreach bit (32b 64b)
      if ($opt == "debug") then 
        unsetenv NODEBUG
      else      
        setenv NODEBUG yes
      endif
      setup ${gcc}
      setup ${bit}
      starver ${STAR_LEVEL}
      time cons -k >& build.${gcc}.${opt}.${bit}.`date +%Y%m%d%y:%H%M`.log &
    end
  end
end

TString OnDb(Int_t year = 2014, const Char_t *dataset="Conditions_daq") {
  // https://drupal.star.bnl.gov/STAR/comp/db/onlinedb/online-server-port-map/
  TString database;
  if      (year == 2019)     database = "dbbak.starp.bnl.gov:3418";
  else if (year == 2018)     database = "dbbak.starp.bnl.gov:3417";
  else if (year == 2017)     database = "dbbak.starp.bnl.gov:3416";
  else if (year == 2016)     database = "dbbak.starp.bnl.gov:3415";
  else if (year == 2015)     database = "dbbak.starp.bnl.gov:3414";
  else if (year == 2014)     database = "dbbak.starp.bnl.gov:3413";
  else if (year == 2013)     database = "dbbak.starp.bnl.gov:3412";
  else if (year == 2012)     database = "dbbak.starp.bnl.gov:3411";
  else if (year == 2011)     database = "dbbak.starp.bnl.gov:3410";
  else if (year == 2010)     database = "dbbak.starp.bnl.gov:3409";
  else if (year == 2009)     database = "dbbak.starp.bnl.gov:3408";
  else if (year == 2008)     database = "dbbak.starp.bnl.gov:3407";
  else if (year == 2007)     database = "dbbak.starp.bnl.gov:3406";
  else if (year == 2006)     database = "dbbak.starp.bnl.gov:3405";
  else if (year == 2005)     database = "dbbak.starp.bnl.gov:3404";
  else if (year == 2004)     database = "dbbak.starp.bnl.gov:3403";
  else if (year == 2003)     database = "dbbak.starp.bnl.gov:3402";
  else if (year == 2002)     database = "dbbak.starp.bnl.gov:3401";
  else if (year == 2001)     database = "dbbak.starp.bnl.gov:3400";
  else {// current Db
#if 1
    if (dataset[0] == 'R') database = "onldb2.starp.bnl.gov:3501";
    else                   database = "onldb2.starp.bnl.gov:3502";
#else
    if (dataset[0] == 'R') database = "heston.star.bnl.gov:3501";
    else                   database = "heston.star.bnl.gov:3502";
#endif
  }
  return database;
}

#ifndef __CINT__
#include "Riostream.h"
#include "TSQLServer.h"
#include "TSQLResult.h"
#include "TSQLRow.h"
#include "TStopwatch.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
using namespace std;
#endif
   
void sqltpcDriftVelocity()
{
   TSQLServer *db = TSQLServer::Connect("mysql://dbx.star.bnl.gov:3316","", "");

   printf("Server info: %s\n", db->ServerInfo());
   
   TSQLRow *row;
   TSQLResult *res;
   struct tpcDriftVelocity_t {
     Int_t utime, date, time, flavor, deactive;
     Float_t lDvEast,lDvWest,cDvEast,cDvWest;
   };
   static tpcDriftVelocity_t driftVelocity;
   static Int_t   *m = &driftVelocity.utime;
   static Float_t *x = &driftVelocity.lDvEast;
   TFile *fOut = new TFile("tpcDriftVelocity.root","recreate");
   TTree *tree = new TTree("T","driftVelocity Seed");
   tree->Branch("driftVelocity",&driftVelocity,
		"utime/I:date/I:time/I:flavor/I:deactive/I:"
		"lDvEast/F:lDvWest/F:cDvEast/F:cDvWest/F");
   // start timer
   TStopwatch timer;
   timer.Start();
   // query database and print results 30 secs average
   const char *sql = "select UNIX_TIMESTAMP(beginTime),DATE_FORMAT(beginTime,\"%Y%m%d\") AS date,DATE_FORMAT(beginTime,\"%H%i%s\") AS time,"
     "flavor,deactive,laserDriftVelocityEast as lDvEast,laserDriftVelocityWest as lDvWest, "
     "cathodeDriftVelocityEast as cDvEast,cathodeDriftVelocityWest as cDvWest from Calibrations_tpc.tpcDriftVelocity"
     " where beginTime > \"2019\" and  beginTime < \"2019-11-01\"";

     ";";
     //     " where beginTime > \"2013\" limit 100;";
     //     "WHERE flavor='ofl' and deactive=0";
//   const char *sql = "select count(*) from Calibrations_rhic.tpcDriftVelocity "
//                     "WHERE tag&(1<<2)";
   
   res = db->Query(sql);

   int nrows = res->GetRowCount();
   printf("\nGot %d rows in result\n", nrows);
   
   int nfields = res->GetFieldCount();
   for (int i = 0; i < nfields; i++)
      printf("%20s", res->GetFieldName(i));
   printf("\n");
   for (int i = 0; i < nfields*40; i++)
      printf("=");
   printf("\n");
   for (int i = 0; i < nrows; i++) {
      row = res->Next();
      for (int j = 0; j < nfields; j++) {
	 TString Field(row->GetField(j));
         if (! (i%100)) printf("%20s ", Field.Data());
	 if (j == 3) {
	   if (Field == "ofl") m[j] = 0;
	   else                m[j] = 1;
	 } else if (j  < 5) {
	   m[j] = Field.Atoi(); 
	 } else        {
	   x[j-5]        = Field.Atof(); 
	 }
      }
      if (! (i%100)) printf("\n");
      delete row;
      tree->Fill();
   }
   
   delete res;
   delete db;

   // stop timer and print results
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   fOut->Write();
   printf("\nRealTime=%f seconds, CpuTime=%f seconds\n", rtime, ctime);
}
/*
  T->Draw("lDvWest:utime-788936400>>DV(8000,634e6,642e6)","deactive==0&&utime>1420088400","prof")
2019
  T->Draw("lDvWest:utime-788936400>>DV(19000,756e6,775e6)","deactive==0&&utime>1420088400","prof")

 */

#! /opt/star/bin/perl -w
#
# $Id:
#
# $Log: L.Didenko
#
# dbupdateEval.pl
#
# update EvalJobStatus table with test jobs status for 'eval' library different codes options. 
# Author: L.Didenko
#
########################################################################################

use File::Find;
use Net::FTP;
use Class::Struct;
use File::Basename;

use DBI;

$dbhost="duvall.star.bnl.gov";
$dbuser="starreco";
$dbpass="";
$dbname="LibraryJobs";

$JobStatusT = "evalJobStatus";

my @node_dir = ();
my @TOP_DIRD = ();
#my @libTr = ("Sti-CA","Sti","Stv", "Stv_CA");
my $k = 0;

my @libTr = ("Sti-CA","Sti","Stv");

 for ($k =0; $k < scalar(@libTr); $k++) {

 $TOP_DIRD[$k] = "/star/rcf/test/eval_".$libTr[$k]."/" ;

 }

  @node_dir = ("daq_sl53.ittf", "trs_sl53.ittf"); 

my @prt = ();

my @dir_year = ( "year_2000","year_2001", "year_2003", "year_2004", "year_2005","year_2006", "year_2007", "year_2008","year_2009","year_2010","year_2011","year_2012", "year_2013", "year_2014");

my @OUT_DIR0 = ();
my @OUT_DIR1 = ();
my @OUT_DIR2 = ();
my @OUT_DIR3 = ();
my @OUT_DIR4 = ();
my @OUT_DIR5 = ();
my @OUT_DIR6 = ();
my @OUT_DIR7 = ();
my @OUT_DIR8 = ();
my @OUT_DIR9 = ();
my @OUT_DIR10 = ();
my @OUT_DIR11 = ();
my @OUT_DIR12 = ();
my @OUT_DIR13 = ();
my @OUT_DIR14 = ();
my @OUT_DIR15 = ();

my @OUT_DIR = ();
my @OUTD_DIR = ();
my @TDIR = ();
my $min;
my $hour;
my $mday;
my $mon;
my $year;
my $wday;
my $yday;
my $isdst;
my $thisday;
my $thistime;
my $ii = 0;
my $i = 0;
my $nk = 0;

##### setup output directories 

 for ($k =0; $k < scalar(@libTr); $k++) {

 for ($i = 0; $i < scalar(@node_dir); $i++) {

    $TDIR[$nk] = $TOP_DIRD[$k] . $node_dir[$i]."/*/*";    
    $nk++;
   }
}

 $TDIR[6] = "/star/rcf/test/eval_Sti/daq_sl53.ittf_opt/"."*/*";
 $TDIR[7] = "/star/rcf/test/eval_Sti/trs_sl53.ittf_opt/"."*/*";

    @OUT_DIR0 = `ls -d $TDIR[0]`;
    @OUT_DIR1 = `ls -d $TDIR[1]`;
    @OUT_DIR2 = `ls -d $TDIR[2]`;
    @OUT_DIR3 = `ls -d $TDIR[3]`;
    @OUT_DIR4 = `ls -d $TDIR[4]`;
    @OUT_DIR5 = `ls -d $TDIR[5]`;
    @OUT_DIR6 = `ls -d $TDIR[6]`;
    @OUT_DIR7 = `ls -d $TDIR[7]`;

#    @OUT_DIR8 = `ls -d $TDIR[8]`;
#    @OUT_DIR9 = `ls -d $TDIR[9]`;
#    @OUT_DIR10 = `ls -d $TDIR[10]`;
#    @OUT_DIR11 = `ls -d $TDIR[11]`;

#    @OUT_DIR12 = `ls -d $TDIR[12]`;
#    @OUT_DIR13 = `ls -d $TDIR[13]`;
#    @OUT_DIR14 = `ls -d $TDIR[14]`;
#    @OUT_DIR15 = `ls -d $TDIR[15]`;


my $ndir = scalar(@OUT_DIR0);

 $ii = 0;
  
  for ($i = 0; $i <scalar(@OUT_DIR0) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR0[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
      $ii++;
  }

  for ($i = 0; $i <scalar(@OUT_DIR1) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR1[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
      $ii++;
  }

  for ($i = 0; $i <scalar(@OUT_DIR2) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR2[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
      $ii++;
  }

  for ($i = 0; $i <scalar(@OUT_DIR3) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR3[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
      $ii++;
  }

  for ($i = 0; $i <scalar(@OUT_DIR4) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR4[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
      $ii++;
  }

  for ($i = 0; $i <scalar(@OUT_DIR5) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR5[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
      $ii++;
  }
   
 for ($i = 0; $i <scalar(@OUT_DIR6) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR6[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
      $ii++;
  }

 for ($i = 0; $i <scalar(@OUT_DIR7) ; $i++) {
      $OUT_DIR[$ii] = $OUT_DIR7[$i];
      chop $OUT_DIR[$ii];
  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

      $ii++;
  }

# for ($i = 0; $i <scalar(@OUT_DIR8) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR8[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";
#
#      $ii++;
#  }

# for ($i = 0; $i <scalar(@OUT_DIR9) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR9[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

      $ii++;
#  }

# for ($i = 0; $i <scalar(@OUT_DIR10) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR10[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

#      $ii++;
#  }

# for ($i = 0; $i <scalar(@OUT_DIR11) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR11[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

#      $ii++;
#  }
#############

# for ($i = 0; $i <scalar(@OUT_DIR12) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR12[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

#      $ii++;
#  }

# for ($i = 0; $i <scalar(@OUT_DIR13) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR13[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

#      $ii++;
#  }

# for ($i = 0; $i <scalar(@OUT_DIR14) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR14[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

#      $ii++;
#  }

# for ($i = 0; $i <scalar(@OUT_DIR15) ; $i++) {
#      $OUT_DIR[$ii] = $OUT_DIR15[$i];
#      chop $OUT_DIR[$ii];
#  print "Output Dir for EVAL :", $OUT_DIR[$ii],"\n";

#      $ii++;
#  }


struct JFileAttr => {
       oldjbId   => '$',
       oldpath   => '$',
       oldlibtr  => '$',
       oldfile   => '$',
       oldTime   => '$',
       oldcomp   => '$',
       oldvail   => '$',
};

 struct LFileAttr => {
        jbId      => '$',
        pth       => '$',
        pyr       => '$',
        lbT       => '$', 
        lbtr      => '$',
        rtL       => '$',
        lgName    => '$',
        crTm      => '$',
        chOpt     => '$',
        errMs     => '$',
        jbSt      => '$',      
        evDn      => '$',
        evSkp     => '$',
        memF      => '$',
        memL      => '$',
        jCPU      => '$',
        jRT       => '$',
        avTr      => '$',
        avPrVtx   => '$',
        nEvtVtx   => '$',
        avPrTr    => '$',
        avVrt     => '$',
        avTrGd    => '$',
        avPrfit   => '$',
        avPrv1    => '$',
        avPrfitv1 => '$',
        avXi      => '$',
        avKn      => '$',
        prctus    => '$',
        avTrus    => '$',
        avTrfitus => '$',
        avPrv1us  => '$',
        avPrfitv1us => '$',
        avV0us    => '$',
        avXius    => '$',
        avKnus    => '$',
        ndID      => '$', 
};        

 my $fullyear;
 my $mo;
 my $dy;
 my $size;
 my $mTime; 
 my @dirF;
 my $hr;
 my $yr;
 my $timeS;
 my $now;

 my $debugOn = 0;

 my %flagHash = (); 
 my %idHash = ();
 my @old_jobs;
 my $nold_jobs = 0;
 my @testOutNFiles;
 my $nOutNFiles = 0;
 my $eachOutNDir;
 my $fullname;
 my $flname;
 my $libL = "n\/a";
 my $libV = "n\/a";
 my $platf;
 my $EvTp;
 my $lgFile;
 my $EvDone = 0;
 my $EvSkip = 0;
 
 my $newAvail; 
 my $no_event = 0; 
 my @maker_size = ();
 my $jrun = "Run not completed";
 my $nevent_vtx = 0;
 my $numevt_vtx = 0; 
 my $tot_tracks = 0;
 my $tot_vertices = 0;
 my $tot_prtracks = 0;
 my $tot_trck_nfit15 = 0;
 my $tot_prtrck_hfit15 = 0;
 my $tot_prtracks_1vtx = 0;
 my $tot_prtrck_nfit15_1vtx = 0;
 my $tot_knvertices = 0;
 my $tot_xivertices = 0;
 my $avr_tracks = 0;
 my $avr_vertices = 0;
 my $avr_prtracks = 0;
 my $avr_prvertx = 0;
 my $avr_knvertices = 0;
 my $avr_xivertices = 0;
 my $avr_trck_nfit15 = 0; 
 my $avr_prtrck_nfit15 = 0; 
 my $avr_prtracks_1vtx = 0;
 my $avr_prtrck_nfit15_1vtx = 0;
 my $avr_trk_usb = 0;
 my $avr_prtrk_usb= 0;
 my $avr_trkfit15_usb = 0;
 my $avr_prtrkfit15_usb = 0;
 my $avr_v0_usb = 0;
 my $avr_kink_usb = 0;
 my $avr_xi_usb = 0;

 my $node_name = "n/\a";
 my $rootL = "n/\a"; 
 my $Err_messg = "none";
 my $mCPU = 0;
 my $mRealT = 0;
 my $mchain = "n/\a";
 my $fname;
 my $mpath;
 my $memFst = 0;
 my $memLst = 0;
 my $jobTime;
 my $mavail = "n\/a";
 my $lgTest = "test.log";
 my $startId = "Job00000_test";
 my $myID;
 my $mjID;
 my @testJobStFiles;
 my $nJobStFiles = 0;
 my $jpath;
 my $jEvDone;
 my $jlibV;
 my $jfile;
 my $jEvSkip;
 my $logName; 
 
  $now = time;
##### connect to DB TestJobs

  &StDbTJobsConnect(); 

#####  select all files from JobStatusT from testDay direcroties

   for ($k =0; $k < scalar(@TOP_DIRD); $k++) {

 $sql="SELECT jobID, path, LibTracker, logFile, createTime, avail FROM $JobStatusT WHERE path LIKE '$TOP_DIRD[$k]%' AND avail = 'Y'";


   $cursor =$dbh->prepare($sql)
    || die "Cannot prepare statement: $DBI::errstr\n";
   $cursor->execute;
 
    while(@fields = $cursor->fetchrow) {
     my $cols=$cursor->{NUM_OF_FIELDS};
        $fObjAdr = \(JFileAttr->new());
 

    for($i=0;$i<$cols;$i++) {
     my $fvalue=$fields[$i];
       my $fname=$cursor->{NAME}->[$i];
#        print "$fname = $fvalue\n" ;

        ($$fObjAdr)->oldjbId($fvalue)   if( $fname eq 'jobID');
        ($$fObjAdr)->oldpath($fvalue)   if( $fname eq 'path');
        ($$fObjAdr)->oldlibtr($fvalue)  if( $fname eq 'LibTracker');
        ($$fObjAdr)->oldfile($fvalue)   if( $fname eq 'logFile');  
        ($$fObjAdr)->oldTime($fvalue)   if( $fname eq 'createTime');
        ($$fObjAdr)->oldvail($fvalue)   if( $fname eq 'avail'); 
   }

       $old_jobs[$nold_jobs] = $fObjAdr;
       $nold_jobs++; 

    }

 }

##### insert first line to JobStatusT table get last ID 

   $sql="insert into $JobStatusT set ";    
   $sql.="jobID='$startId',"; 
   $sql.="logFile='$lgTest'";
    print "$sql\n" if $debugOn;
   $rv = $dbh->do($sql) || die $dbh->errstr;
   $new_id = $dbh->{'mysql_insertid'};


####### read log files and fill in JobStatus

my $pvfile;
my $pvpath = "n/a";
my $pvTime = 0000;
my $pvjbId;
my $pvavail;
my $pvcomp;
my $pfullName;
my $fflag;
my $Fname;
my @files;
my $pyear = 0;
 @prt = ();
my $libTr;


 foreach  my $eachOutLDir (@OUT_DIR) {

#    next if ( $eachOutLDir =~ /eval_Sti/) ;

          if (-d $eachOutLDir) {
	      next if ($eachOutLDir =~ /temp/) ;

     opendir(DIR, $eachOutLDir) or die "can't open $eachOutLDir\n";
      @files = readdir(DIR);
#    while( defined($fname = readdir(DIR)) ) {
     foreach $fname ( @files) {
      next if !$fname;
      next if $fname =~ /^\.\.?$/;    
      next if $fname =~ /hijing.log/;
      next if $fname =~ /starsim.log/;
     
 $jrun = "Run not completed";
 $EvDone = 0;
 $nevent_vtx = 0;
 $numevt_vtx = 0;
 $perct_usb = 0; 
 $avr_tracks = 0;
 $avr_vertices = 0;
 $avr_prvertx = 0;
 $avr_prtracks = 0;
 $avr_knvertices = 0;
 $avr_xivertices = 0;
 $avr_trck_nfit15 = 0; 
 $avr_prtrck_nfit15 = 0; 
 $avr_prtracks_1vtx = 0;
 $avr_prtrck_nfit15_1vtx = 0;
 $avr_trk_usb = 0;
 $avr_prtrk_usb= 0;
 $avr_trkfit15_usb = 0;
 $avr_prtrkfit15_usb = 0;
 $avr_v0_usb = 0;
 $avr_kink_usb = 0;
 $avr_xi_usb = 0;

 $node_name = "n/a";
 $libL = "n/a";
 $libV = "n/a";
 $libTr = "n/a";   
 $rootL = "n/a"; 
 $Err_messg = "none";
 $mchain = "n/a";
 $no_event = 0;
 $mCPU = 0;
 $mRealT = 0;
 $mavail = 'Y'; 
 $memFst = 0;
 $memLst = 0; 
 $EvSkip = 0;
 $jobTime = 0;
 @prt = ();


       if ($fname =~ /.log/)  {
#    print "File Name:",$fname, "\n";       
       $fullname = $eachOutLDir."/".$fname;
      $mpath = $eachOutLDir;
      @dirF = split(/\//, $eachOutLDir);
       $libL = $dirF[4];
       @prt = ();
       @prt = split("_",$libL);
       $libTr = $prt[1];
       $platf = $dirF[5];
       @prt =split("_", $dirF[6]);
       $pyear = $prt[1]; 

       $logName = $fname; 
#      $Fname =  $mpath . "/" . $fname;
       $flagHash{$fullname} = 1;
      ($size, $mTime) = (stat($fullname))[7, 9];
    
      $ltime = $now - $mTime; 
     ($sec,$min,$hr,$dy,$mo,$yr) = (localtime($mTime))[0,1,2,3,4,5];
      $mo = sprintf("%2.2d", $mo+1);
      $dy = sprintf("%2.2d", $dy);
  
      if( $yr > 97 ) {
        $fullyear = 1900 + $yr;
      } else {
        $fullyear = 2000 + $yr;
      };

       $timeS = sprintf ("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:00",
                       $fullyear,$mo,$dy,$hr,$min);    

#           if( $ltime > 600 && $ltime < 518400 ){         
          if( $ltime > 2400  and $size > 1000 ) { 
#   print "Log time: ", $ltime, "\n";
   print $fullname, "\n";
        &logInfo("$fullname", "$platf");
     $jobTime = $timeS;  

      $fObjAdr = \(LFileAttr->new());
      ($$fObjAdr)->pth($mpath);
      ($$fObjAdr)->pyr($pyear);
      ($$fObjAdr)->lbT($libV);
      ($$fObjAdr)->lbtr($libTr);
      ($$fObjAdr)->rtL($rootL);
      ($$fObjAdr)->crTm($jobTime); 
      ($$fObjAdr)->chOpt($mchain);
      ($$fObjAdr)->jbSt($jrun);
      ($$fObjAdr)->errMs($Err_messg);
      ($$fObjAdr)->lgName($logName);
      ($$fObjAdr)->evDn($EvDone);
      ($$fObjAdr)->evSkp($EvSkip);
      ($$fObjAdr)->memF($memFst);
      ($$fObjAdr)->memL($memLst);
      ($$fObjAdr)->jCPU($mCPU);
      ($$fObjAdr)->jRT($mRealT);
      ($$fObjAdr)->avTr($avr_tracks);
      ($$fObjAdr)->avPrVtx($avr_prvertx);
      ($$fObjAdr)->nEvtVtx($nevent_vtx);
      ($$fObjAdr)->avPrTr($avr_prtracks);
      ($$fObjAdr)->avVrt($avr_vertices);
      ($$fObjAdr)->avXi($avr_xivertices);
      ($$fObjAdr)->avKn($avr_knvertices);
      ($$fObjAdr)->avTrGd($avr_trck_nfit15);
      ($$fObjAdr)->avPrfit($avr_prtrck_nfit15);
      ($$fObjAdr)->avPrv1($avr_prtracks_1vtx);
      ($$fObjAdr)->avPrfitv1($avr_prtrck_nfit15_1vtx);
      ($$fObjAdr)->prctus($perct_usb);
      ($$fObjAdr)->avTrus($avr_trk_usb);
      ($$fObjAdr)->avPrv1us($avr_prtrk_usb);
      ($$fObjAdr)->avTrfitus($avr_trkfit15_usb);
      ($$fObjAdr)->avPrfitv1us($avr_prtrkfit15_usb);
      ($$fObjAdr)->avV0us($avr_v0_usb);
      ($$fObjAdr)->avKnus($avr_kink_usb);
      ($$fObjAdr)->avXius($avr_xi_usb);

      ($$fObjAdr)->ndID($node_name); 
      $testJobStFiles[$nJobStFiles] = $fObjAdr;
      $nJobStFiles++;


      foreach my $eachOldJob (@old_jobs) {
          $pvjbId = ($$eachOldJob)->oldjbId;
          $pvpath = ($$eachOldJob)->oldpath;
          $pvfile = ($$eachOldJob)->oldfile;
          $pvTime = ($$eachOldJob)->oldTime;
          $pvavail = ($$eachOldJob)->oldvail;
          $pfullName = $pvpath . "/" . $pvfile;

        
#       if( ($fullname eq $pfullName) and ($pvavail eq "Y") ) {

        if( $pfullName eq $fullname ) {
        $flagHash{$fullname} = 0;

	 if( $timeS ne $pvTime) {

          $newAvail = "N";
  print  "Changing availability for test files", "\n";
  print  "files to be updated:", $pvjbId, " % ",$pvpath, " % ",$pvTime, " % ",$newAvail, "\n"; 
    &updateJSTable();

      $mavail = 'Y';
      $myID = 100000000 + $new_id;
      $mjID = "Job". $myID ; 
      $idHash{$fullname} = $mjID;

  print  "Filling JobStatus with NEW log files \n";
  print  "files to be inserted:", $mjID, " % ",$mpath, " % ",$timeS , " % ", $memFst," % ",$memLst," % ", $mavail, "\n";  
     &fillJSTable();

   }else{
   }  
   }else{
     next;
   }
    last;
	}
    }
    }else {
      next;
    }
   }
 closedir DIR;
   }
 }
###

             foreach my $newjobFile (@testJobStFiles) {

 $jrun = "Run not completed";
 $EvDone = 0;
 $perct_usb = 0;
 $nevent_vtx = 0;
 $numevt_vtx = 0;
 $avr_tracks = 0;
 $avr_prvertx = 0;
 $avr_vertices = 0;
 $avr_prtracks = 0;
 $avr_knvertices = 0;
 $avr_xivertices = 0;
 $avr_trck_nfit15 = 0;
 $avr_prtrck_nfit15 = 0;
 $avr_prtracks_1vtx = 0;
 $avr_prtrck_nfit15_1vtx = 0;
 $avr_trk_usb = 0;
 $avr_prtrk_usb= 0;
 $avr_trkfit15_usb = 0;
 $avr_prtrkfit15_usb = 0;
 $avr_v0_usb = 0;
 $avr_kink_usb = 0;
 $avr_xi_usb = 0;

 $node_name = "n/a";
 $libL = "n/a";
 $libV = "n/a";  
 $rootL = "n/a"; 
 $Err_messg = "none";
 $mchain = "n/a";
 $no_event = 0;
 $mCPU = 0;
 $mRealT = 0;
 $mavail = 'Y'; 
 $memFst = 0;
 $memLst = 0; 
 $EvSkip = 0;
 $jobTime = 0;

    $mpath =   ($$newjobFile)->pth;
    $pyear =   ($$newjobFile)->pyr;
    $libV=     ($$newjobFile)->lbT;
    $libTr =   ($$newjobFile)->lbtr;
    $rootL =   ($$newjobFile)->rtL;
    $jobTime =  ($$newjobFile)->crTm; 
    $mchain =  ($$newjobFile)->chOpt;
    $jrun =    ($$newjobFile)->jbSt;
    $Err_messg = ($$newjobFile)->errMs;
    $logName = ($$newjobFile)->lgName;
    $EvDone =  ($$newjobFile)->evDn;
    $EvSkip =  ($$newjobFile)->evSkp;
    $memFst=   ($$newjobFile)->memF;
    $memLst=   ($$newjobFile)->memL;
    $mCPU =    ($$newjobFile)->jCPU;
    $mRealT =  ($$newjobFile)->jRT;
    $avr_tracks=  ($$newjobFile)->avTr;
    $nevent_vtx=  ($$newjobFile)->nEvtVtx;
    $avr_prvertx= ($$newjobFile)->avPrVtx;
    $avr_prtracks = ($$newjobFile)->avPrTr;
    $avr_vertices = ($$newjobFile)->avVrt;
    $avr_xivertices = ($$newjobFile)->avXi;
    $avr_knvertices = ($$newjobFile)->avKn;
    $avr_trck_nfit15 =  ($$newjobFile)->avTrGd;
    $avr_prtrck_nfit15 =  ($$newjobFile)->avPrfit;
    $avr_prtracks_1vtx = ($$newjobFile)->avPrv1;
    $avr_prtrck_nfit15_1vtx = ($$newjobFile)->avPrfitv1;
    $perct_usb = ($$newjobFile)->prctus;
    $avr_trk_usb = ($$newjobFile)->avTrus;
    $avr_prtrk_usb= ($$newjobFile)->avPrv1us;
    $avr_trkfit15_usb = ($$newjobFile)->avTrfitus;
    $avr_prtrkfit15_usb = ($$newjobFile)->avPrfitv1us;
    $avr_v0_usb = ($$newjobFile)->avV0us;
    $avr_kink_usb = ($$newjobFile)->avKnus;
    $avr_xi_usb = ($$newjobFile)->avXius;
    $node_name = ($$newjobFile)->ndID; 

    $fullName = $mpath ."/" .$logName;

    if($flagHash{$fullName} == 1) {

      $new_id = $dbh->{'mysql_insertid'};
      $mavail = 'Y';
      $myID = 100000000 + $new_id;
      $mjID = "Job". $myID ; 
 print "Insert new files: ", $mjID, " % ",$fullName, "\n";
      $idHash{$fullName} = $mjID;
    &fillJSTable();

        foreach my $nOldJob (@old_jobs) {
          $pvjbId = ($$nOldJob)->oldjbId;
          $pvpath = ($$nOldJob)->oldpath;
          $pvfile = ($$nOldJob)->oldfile;

	  if($mpath eq  $pvpath) {
            $newAvail = "N";
   print  "Changing avalability for files have been replaced  :", $pvjbId, " % ",$pvpath," % ",$pvfile, "\n";
     &updateJSTable();

        }
      }
    }
   }
##### delete from $JobStatusT inserted JobID


    $sql="delete from $JobStatusT WHERE ";    
    $sql.="jobID='$startId' AND "; 
    $sql.="logFile='$lgTest'";
     print "$sql\n" if $debugOn;
    $rv = $dbh->do($sql) || die $dbh->errstr;


  &StDbTJobsDisconnect();

  exit;

#########################################################################################

sub fillJSTable {

    $sql="insert into $JobStatusT set ";
    $sql.="jobID='$mjID',";
    $sql.="LibTracker='$libTr',";
    $sql.="LibTag='$libV',";
    $sql.="rootLevel='$rootL',";
    $sql.="path='$mpath',";
    $sql.="prodyear='$pyear',";
    $sql.="logFile='$logName',";
    $sql.="createTime='$jobTime',";
    $sql.="chainOpt='$mchain',";
    $sql.="jobStatus='$jrun',";
    $sql.="NoEventDone='$EvDone',";
    $sql.="NoEventSkip='$EvSkip',";
    $sql.="memUsageF='$memFst',";
    $sql.="memUsageL='$memLst',";
    $sql.="CPU_per_evt_sec='$mCPU',";
    $sql.="RealTime_per_evt='$mRealT',";
    $sql.="avg_no_tracks='$avr_tracks',";
    $sql.="NoEventVtx='$nevent_vtx',";
    $sql.="avgNoVtx_evt='$avr_prvertx',";
    $sql.="avg_no_V0Vrt='$avr_vertices',";
    $sql.="avg_no_primaryT='$avr_prtracks',";
    $sql.="avg_no_primaryTnfit15='$avr_prtrck_nfit15',";
    $sql.="avg_no_tracksnfit15='$avr_trck_nfit15',";
    $sql.="avg_no_primaryT_1vtx='$avr_prtracks_1vtx',";
    $sql.="avg_no_primaryTnfit15_1vtx='$avr_prtrck_nfit15_1vtx',";
    $sql.="percent_of_usable_evt='$perct_usb',";
    $sql.="avgNoTrack_usbevt='$avr_trk_usb',";
    $sql.="avgNoTrackNfit15_usbevt='$avr_trkfit15_usb',";
    $sql.="avgNoPrTrack_1vtx_usbevt='$avr_prtrk_usb',";
    $sql.="avgNoPrTrackNfit15_1vtx_usbevt='$avr_prtrkfit15_usb',";
    $sql.="avgNoV0_usbevt='$avr_v0_usb',";
    $sql.="nodeID='$node_name',"; 
    $sql.="avail='$mavail'"; 

    print "$sql\n" if $debugOn;
    $rv = $dbh->do($sql) || die $dbh->errstr;
    $new_id = $dbh->{'mysql_insertid'};  

  }

###########
sub  updateJSTable {

     $sql="update $JobStatusT set ";
     $sql.="avail='$newAvail'";
     $sql.=" WHERE path = '$pvpath' AND logFile = '$pvfile'";   
     print "$sql\n" if $debugOn;
     $rv = $dbh->do($sql) || die $dbh->errstr;

   }

#####=======================================================================
   sub logInfo {

 my ($fl_log,$plt_form) = @_;
 
 my $num_line = 0;
 my @mem_words;
 my $mymaker; 
 my $no_tracks; 
 my $no_vertices;
 my $no_xivertices;
 my $no_knvertices;
 my @no_prtracks = ();
 my $no_trck_nfit15 = 0; 
 my @no_prtrck_nfit15 = ();
 my $no_prtracks_1vtx = 0 ;
 my $no_prtrck_nfit15_1vtx = 0;
 my @word_tr;
 my @nmb = ();
 my @nmbx = ();
 my $i;
 my @part;
 my @size_line;
 my @memSize;
 my @cpu_output;
 my $ij = 0;
 my $end_line; 
 my $npr = 0;
 my $nevt = 0;
 my $max_npr = 0;
 my $max_npr_nfit15 = 0;
 my $no_prvertx = 0;
 my @vrank = ();

    $tot_tracks = 0;
    $tot_vertices = 0;
    $tot_prtracks = 0;
    $tot_trck_nfit15 = 0;
    $tot_prtrck_nfit15 = 0;
    $tot_prtracks_1vtx = 0;
    $tot_prtrck_nfit15_1vtx = 0;
    $tot_knvertices = 0;
    $tot_xivertices = 0;
    $no_event = 0;
    $mCPU = 0;
    $mRealT = 0;
    $memFst = 0;
    $memLst = 0; 
    $EvSkip = 0;
    $EvCom = 0;

@maker_size = ();

#---------------------------------------------------------

# print $fl_log, "\n";

 $nevent_vtx = 0;

# $nevent_vtx = `grep 'primary vertex(0):' $fl_log | wc -l ` ;  

  open (LOGFILE, $fl_log ) or die "cannot open $fl_log: $!\n";

 my @logfile = ();

   @logfile = <LOGFILE>;

my $Anflag = 0;
my $runflag = 0;
my $mCPUbfc = 0;
my $mRealTbfc = 0;
my $embflag = 0;
my $mixline = "/StRoot/macros/embedding";
my $mixer;
my @tmm = ();
my $evtcomp = 0;
$Err_messg = "none";
$jrun = "Run not completed";


  if($fl_log =~ /embed/) {

   $embflag = 1;
 }

 
   foreach my $line (@logfile) {
       chop $line ;
        $num_line++; 
#   get ROOT_LEVEL and node

       if($line =~ /Processing bfc.C/) {
          $runflag++;
	}
   if ($line =~ /StMessageManager message summary/) {
      $Anflag = 1;
    }

       if ($line =~ /QAInfo:You are using STAR_LEVEL/) {
         @part = split (" ", $line);
         $rootL = $part[8];
         $node_name = $part[12];   
       }
#   get library version
      if ( $line =~ /={3} You are in (\w+)/ ) {
        if( $Anflag == 0 or $embflag == 1 ) {
        $libV = $1;
      }else{
       next;
       }
    }
#   get chain option
	  if($runflag == 1 or $embflag == 1) {
	      if ( $line =~ /Processing bfc.C/)   {
         if( $Anflag == 0 ) {
         @part = split /"/, $line ;
         $mchain = $part[1]; 
         $mchain =~ s/ /,/g;  
#   print  $mchain, "\n";
    }else{
       next;
        }

    }elsif ( $line =~ /$mixline/ ) {
     @part = ();
#     @part = split( "/", $line) ;
#     $mchain = $part[4];
      @part = split( "embedding/", $line) ;
      $mixer = $part[1]; 
      @part = ();
      @part = split( ".C", $mixer) ;
      $mchain = $part[0].".C";
 
       }
 
#   get  number of events
    if ( $line =~ /Done with Event/ ) {
        $no_event++;
    }
      if ( $line =~ /Reading Event:/ ) {
        $evtcomp++;
     }

 #  get memory size
      if ($num_line > 500){
	if( $line =~ /EndMaker/ and $line =~ /total/ ) {
        @size_line = split(" ",$line); 

          $mymaker = $size_line[3];

        @memSize = split("=",$size_line[6]);
        if( $mymaker eq "outputStream:"){

### may change back
#          $maker_size[$evtcomp -1] = $memSize[1];
          $maker_size[$evtcomp] = $memSize[1];
       }
      }
    }
# get number of tracks and vertices

      if ($line =~ /QA :INFO  - StAnalysisMaker/ && $Anflag == 0 ) {

  @nmb = ();
  @nmbx = ();
  @word_tr = ();
  $npr = 0;


            my  $string = $logfile[$num_line];
            chop $string;

             if($string =~  /track nodes:/) {
              @word_tr = split /:/,$string;
              @nmb =  split /</,$word_tr[2];
              $no_tracks = $nmb[0];
              $tot_tracks += $no_tracks; 
              if($no_tracks >= 1) {
              $nevt++;
               }
              @nmbx =  split /</,$word_tr[4];
              $no_trck_nfit15 = $nmbx[0];
              $tot_trck_nfit15 += $no_trck_nfit15;  
              @no_prtrck_nfit15 = ();
              @no_prtracks = ();
              $no_prtrck_nfit15[0] = 0;
              $no_prtracks[0] = 0;
              $no_vertices = 0;
              $no_xivertices  = 0;
              $no_knvertices = 0;
              $npr = 0;
              $no_prtracks_1vtx = 0;
              $no_prtrck_nfit15_1vtx = 0;
              @vrank = ();
              $vrank[0] = -0.01 ;
            
             for ($ik = 2; $ik< 120; $ik++)  { 
              $string = $logfile[$num_line + $ik];
              chop $string;

          if(  $string =~ /QA :INFO/ and $string =~ /Rank/ and $string =~ /#V/ ) {

              @word_tr = ();
              @nmbx = ();
              @word_tr = split (":",$string);
              @nmbx = split (" ",$word_tr[4]);
#         print "Check splitting   ",$word_tr[3]," %  ", $word_tr[4]," %  ", $word_tr[5]," % ", $word_tr[6], "\n";
             $vrank[$npr] = $nmbx[0];

             my $string2 = $logfile[$num_line + $ik+1];
             chop $string2;
             my  $string3 = $logfile[$num_line + $ik+2];
             chop $string3;

          if( $string2 =~ /MessageKey/ and $string2 =~ /primary all/ ) {
             $no_prvertx++;
              @word_tr = split /=/,$string2;
              @nmb =  split ("'",$word_tr[3]);
              $no_prtracks[$npr] = $nmb[1];
          }

            if( $string3 =~ /MessageKey/ and $string3 =~ /primary good/ ) {
              @word_tr = ();
              @nmbx = ();
              @word_tr = split /=/,$string3;
              @nmbx =  split ("'",$word_tr[3]);
              $no_prtrck_nfit15[$npr]  = $nmbx[1];

           }
#           print "Vertex rank ", $npr,"   ",$vrank[$npr],"   ", $no_prtracks[$npr], "   ", $no_prtrck_nfit15[$npr],"\n";

              $npr++;
            }
######## 
              if( $string =~ /V0 vertices/) { 
              @word_tr = split /:/,$string;
              @nmb =  split /</,$word_tr[2];
              $no_vertices = $nmb[0];              
              $tot_vertices += $no_vertices;
            } elsif( $string =~ /Xi vertices/) { 
              @word_tr = split /:/,$string;
              @nmb =  split /</,$word_tr[2];
              $no_xivertices = $nmb[0];
              $tot_xivertices += $no_xivertices;
#            } elsif( $string =~ /Kink vertices/) {
#              @word_tr = split /:/,$string;
#              @nmb =  split /</,$word_tr[2];
#              $no_knvertices = $nmb[0];
#              $tot_knvertices += $no_knvertices;

         }
      }

             if ($vrank[0] > 0.00000001) {
              $numevt_vtx++;
              $no_prtracks_1vtx = $no_prtracks[0];
              $no_prtrck_nfit15_1vtx  = $no_prtrck_nfit15[0];

              $tot_prtracks += $no_prtracks[0];
              $tot_prtrck_nfit15 += $no_prtrck_nfit15[0];
              $tot_prtracks_1vtx += $no_prtracks_1vtx;
              $tot_prtrck_nfit15_1vtx += $no_prtrck_nfit15_1vtx;

           }
             if ($npr >= 1 ) {
              $nevent_vtx++;
          }

      }
   }

#  check if job crashed due to break_buss_error
      if($line =~ /bus error/) {
          $Err_messg = "Break bus error";
        }

#  check if job crashed due to segmentation violation
     elsif ($line =~ /segmentation violation/ or $line =~ /Segmentation violation/ ) {
           $Err_messg = "segmentation violation";

    }elsif ($line =~ /segmentation fault/ or $line =~ /Segmentation fault/) {
         $Err_messg = "segmentation fault";

    }elsif ($line =~ /Stale NFS file handle/) {
          $Err_messg = "Stale NFS file handle";
 
   }elsif ( $line =~ /Assertion/ & $line =~ /failed/)  {
        $Err_messg = "Assertion failed";

   }elsif ($line =~ /Tried to find a host for 500 times, will abort now/)  {
             $Err_messg = "DB connection failed";

  }elsif ($line =~ /Killed/)  {
             $Err_messg = "Killed";

   }elsif ($line =~ /Abort/)  {
             $Err_messg = "Abort";

   }elsif ($line =~ /glibc detected/)  {
             $Err_messg = "glibc detected";

    }elsif ($line =~ /FATAL/ and $line =~ /floating point exception/) {

         $Err_messg = "Floating point exception";

    }elsif ($line =~ /Catch exception FATAL/) {

         $Err_messg = "FATAL";

    }elsif ($line =~ /Fatal in <operator delete>/) {
  
        $Err_messg = "Fatal in <operator delete>";   
 
   }elsif ($line =~ /Fatal in <operator new>/) {
  
       $Err_messg = "Fatal in <operator new>";   

   }elsif ($line =~ /std::runtime_error/) {

       $Err_messg = "runtime_error";
  }

       if ( $line =~ /INFO  - QAInfo:Run/ and $line =~ /Total events processed/) {

        @part = split /:/,$line;
        $EvSkip = $part[4];
    }      
# check if job is completed

     if ( $line =~ /Run completed/  and $Err_messg eq "none") {
          
           $jrun = "Done";      

         }elsif($Err_messg ne "none" ){

           $jrun = "$Err_messg";

         }else{

         }
#############
       } 
 }

       $EvDone = $no_event;
      $EvCom = $EvDone - $EvSkip;

##### get CPU and Real Time per event

 if ($EvCom != 0) {

   @cpu_output = ();

    @cpu_output = `tail -2000 $fl_log`;
 
  foreach $end_line (@cpu_output){
          chop $end_line;

   if($embflag == 0 ) {
   if ($end_line =~ /QAInfo:Chain/ and $end_line =~ /StBFChain::bfc/) {
     @part = split (" ", $end_line); 
      $mCPUbfc = $part[8];
      $mRealTbfc = $part[6];

    if($part[6] eq "=" ) {
      $mCPUbfc = $part[10];
      $mRealTbfc = $part[7];
    }else{

      $mCPUbfc = substr($mCPUbfc,1) + 0;
      $mRealTbfc = substr($mRealTbfc,1) + 0;
    }
     $mCPU = $mCPUbfc/$EvCom;
     $mRealT = $mRealTbfc/$EvCom;

   }else{
    next;
      }
 
  }elsif($embflag == 1 ) {

  if ($end_line =~ /QAInfo:Chain/ and $end_line =~ /StChain::Embedding/) {

      @part = split (" ", $end_line);
      $mCPUbfc = $part[8];
      $mRealTbfc = $part[6];
      $mCPUbfc = substr($mCPUbfc,1) + 0;
      $mRealTbfc = substr($mRealTbfc,1) + 0;
#     print "CPU ", $mCPUbfc,"   %   ", $mRealTbfc, "\n";
     $mCPU = $mCPUbfc/$EvCom;
     $mRealT = $mRealTbfc/$EvCom;
#   print "CPU and RealTime  ",$EvCom,"   ",$mCPU, "    ",$mRealT, "\n";

   }else{
    next;
      }
    }
  }

    $perct_usb        = ($nevt/$EvCom)*100;
    $avr_tracks     = $tot_tracks/$EvCom;
    $avr_vertices   = $tot_vertices/$EvCom;
    $avr_trck_nfit15  = $tot_trck_nfit15/$EvCom;   

    $avr_knvertices = $tot_knvertices/$EvCom;
    $avr_xivertices = $tot_xivertices/$EvCom;
    if($nevt >= 1 ) {
    $avr_trk_usb = $tot_tracks/$nevt;
    $avr_prtrk_usb= $tot_prtracks_1vtx/$nevt;
    $avr_trkfit15_usb = $tot_trck_nfit15/$nevt;
    $avr_prtrkfit15_usb = $tot_prtrck_nfit15_1vtx/$nevt;
    $avr_v0_usb = $tot_vertices/$nevt;
    $avr_kink_usb = $tot_knvertices/$nevt;
    $avr_xi_usb =$tot_xivertices/$nevt ; 
   }

       if($numevt_vtx >= 1 ) {
    $avr_prtracks     = $tot_prtracks/$numevt_vtx;
    $avr_prtrck_nfit15  = $tot_prtrck_nfit15/$numevt_vtx;
    $avr_prtracks_1vtx = $tot_prtracks_1vtx/$numevt_vtx;
    $avr_prtrck_nfit15_1vtx = $tot_prtrck_nfit15_1vtx/$numevt_vtx;

    }else{
    $avr_prtracks = 0;
    $avr_prtrck_nfit15  = 0;
    $avr_prtracks_1vtx = 0 ;
    $avr_prtrck_nfit15_1vtx = 0 ;
   }

    if($nevent_vtx >= 1 ) {
    $avr_prvertx      = $no_prvertx/$nevent_vtx;
       }else{
    $avr_prvertx      = 0;
      }

# print "Size of executable:  ", $EvDone, "  ", $no_event,"  ",$maker_size[$evtcomp  -1], "\n"; 

    if ( defined $maker_size[0]) { 
    $memFst = $maker_size[0];
    }else {
    $memFst = 0;
  }
    if ( defined $maker_size[$evtcomp -1]) {
    $memLst = $maker_size[$evtcomp -1];
    } elsif( defined $maker_size[$evtcomp -2]) {
    $memLst = $maker_size[$evtcomp -2];
   }else{
    $memLst = $maker_size[$evtcomp -3];
  }
  }
 
   close (LOGFILE);
 }


######################
sub StDbTJobsConnect {
    $dbh = DBI->connect("dbi:mysql:$dbname:$dbhost", $dbuser, $dbpass)
        || die "Cannot connect to db server $DBI::errstr\n";
}

######################
sub StDbTJobsDisconnect {
    $dbh = $dbh->disconnect() || die "Disconnect failure $DBI::errstr\n";
}

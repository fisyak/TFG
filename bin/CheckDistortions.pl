#! /usr/bin/env perl
use File::Basename;
use Cwd;
#my @DistortionSet = qw(Corr4 Corr3 OBmap2D OTwist OClock Opr13 OIFC OShortR OBmap OSectorAlign);
my @DistortionSet = qw(CorrY OShortR OBmap  OPr40 OIFC OSectorAlign OSpaceZ2 OGridLeakFull);
# my %dates   = (
# 	       'pp500_2012'       => '20120324.042916',
#  	       'pp500_2017'       => '20170423.040951',
# 	       'isobar_2018'      => '20180318.041820',
# 	       '3p85_fixedTarget' => '20180603.040439',
# 	       '19GeV'            => '20190302.054050',
# 	       '14p5GeV'          => '20190406.043336');
#              'pp500_2017'       => '20170423.040951'
#	       '14p5GeV'          => '20190406.043336'
#	       '7p7GeV_2021_RF'  => '20210303.052529',
#	       '7p7GeV_2021_FF'  => '20210304.034026'
#	      );
#	       'OO_200GeV_20217'  => '20210510.134727',
#	       'FF_OO_200GeV_2021' => '20210522.024326',
my %dates   = ( 
	       'FF_OO_200GeV_2021_Ideal' => '20210523.024326'
	      );
foreach $trig  (sort keys %dates) {
  my $datetime = $dates{$trig};  print "$trig => $datetime\n";
  if (! -d $trig) {mkdir $trig;}
  chdir $trig;
  foreach my $corr (@DistortionSet) {
    my $rootfile = $trig . $corr . "_" . $datetime . ".root";
    my $log      = $trig . $corr . "_" . $datetime . ".log";
    my $Corr = $corr;
#    if ($Corr eq 'OPr40' and $datetime < 20181101) {$Corr = "Opr13";}
    if (-r $rootfile) {next;}
    my $cmd = "root.exe -q 'CheckDistortion.C(\"" . $Corr . ",sdt" . $datetime . ",NewTpcAlignment\",\"" . $rootfile . "\",\"" . $trig . "\")' >& " . $log;
    print "cmd $cmd\n";
    my $flag = system($cmd);
    if ($flag) {print "flag = $flag\n"; die;}
  }
  chdir "../";
}

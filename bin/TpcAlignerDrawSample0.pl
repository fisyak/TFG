#! /usr/bin/env perl
use File::Basename;
use Cwd;
my $debug = 0;
my $pwd = cwd(); #print "pwd = $pwd\n";
my @files = glob "./*Aligner.root";
my @runs = ();
my $oldrun = -1;
foreach my $f (@files) {
  my $run = $f;  print "$run\n" if $debug;
  $run =~ s/.\/st_//;# print "$run\n";grep
  $run =~ s/adc_//; #print "$run\n";
  $run =~ s/gmt_//; # print "$run\n";
  $run =~ s/hltcosmic_//;
  $run =~ s/hlt_//;
  $run =~ s/\.\///;
  my ($r) = split '_', $run; print "r = $r\n" if $debug;
#  $run =~ s/hltcosmic_//; print "$run\n";
#  $run =~ s/_raw*.//; print "$run\n";
  if ($r != $oldrun) {
    $oldrun = $r;
    push @runs, $r;
  }
}
#print "runs = @runs\n";
foreach my $r (@runs) {
  my $file = $r . "Aligner_IO.root";
  if (-r $file) {next;}
#  my ($dev,$ino,$mode,$nlink,$uid,$gid,$dev, $size, $atime, $mtim, $ctime, $blksize,$blocks) = stat($file);
#  my $now = time(); #print "now = $now\n";
#  my $dt = $now - $ctime;# print "now = $now, ctime = $ctime, dt = $dt\n";
#  if ($dt < 3600) {next;}
  my $string = "string:" . $r . ":" .  $file;
  print "$string\n";
}

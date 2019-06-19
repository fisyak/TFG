#!/usr/bin/env perl
use File::Basename;
my $line;
my $file = "2019Production.log"; #"bfc_avr_time.log";
my $cmd;
my $flag;
my $debug = 0;
open (In,"$file") or die "Can't open $file";
while ($line = <In>) {
  print $line;
  my @w = split(' ',$line);
  my $log = $w[0];
  if (! -r $log) {next;}
  my $f   = $w[4]; $f =~ s/;//;
  my $l   = $w[7]; $l =~ s/;//;
#  my $b   = File::Basename::basename($log,"B.log");
  my $b   = $log;## print "b = $b\n";# File::Basename::basename($log,"B.log");
  $b   =~ s/B\.log//;# print "b = $b\n";
  my $z   = $b;
  my @z   = split('_',$z);
  my $nz = $#z;
  my $F = $z[$nz-1];
  my $L = $z[$nz];
  my @Z = ();
  for (my $i = 0; $i < $nz-1; $i++) {
    push @Z, $z[$i];
  }
  my $Z = join("_",@Z);
  print "$f $F $l $L  $Z\n";
  if ($f == $F and $l > $L) {next;}
  my $blafile = $b . ".bla.root";
  my $oldfile = $b . ".MuDst.root";
  my $oldpico = $b . ".picoDst.root";
  my $oldeven = $b . ".event.root";
  my $newfile = $Z . "_" . sprintf("%04i",$f) . "_" . sprintf("%04i",$l) . ".MuDst.root";
  my $newpico = $newfile; $newpico =~ s/MuDst/picoDst/;
  my $neweven = $newfile; $neweven =~ s/MuDst/event/;
  my $newlog  = $Z . "_" . sprintf("%04i",$f) . "_" . sprintf("%04i",$l) . ".log"; # print "newlog = $newlog\n";
  if ( -r $log) {
    $cmd = "mv " . $log . " " .$newlog;  print "$cmd\n";
    $flag = system($cmd) if (! $debug); if ($flag) {die "$cmd failed";}
  }
  if (-r $oldfile and $oldfile ne $newfile) {
    $cmd = "mv " . $oldfile . " " . $newfile; print "$cmd\n"; $flag = system($cmd) if (! $debug);; if ($flag) {die "$cmd failed";}
  }
  if (-r $oldpico and $oldpico ne $newpico) {
    $cmd = "mv " . $oldpico . " " . $newpico; print "$cmd\n"; $flag = system($cmd) if (! $debug);; if ($flag) {die "$cmd failed";}
  }
  if (-r $oldeven and $oldeven ne $neweven) {
    $cmd = "mv " . $oldeven . " " . $neweven; print "$cmd\n"; $flag = system($cmd) if (! $debug);; if ($flag) {die "$cmd failed";}
  }
  if (-r $blafile) {
    $cmd = "rm " . $blafile; print "$cmd\n"; print "$cmd\n"; $flag = system($cmd) if (! $debug);; if ($flag) {die "$cmd failed";}
  }
#  die;
}
close(In);

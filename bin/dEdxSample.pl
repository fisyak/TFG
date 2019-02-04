#! /usr/bin/env perl
use File::Basename;
use Cwd;
my $glob = "../*.event.root";
my $pwd = cwd();
if ($#ARGV >= 0) {$glob = $ARGV[0];}
my @Files = glob $glob;
foreach my $file (@Files) {
  my $bf = File::Basename::basename($file,".event.root");# print "bf = $bf\n";
  my $glob = $bf . "*";
  my @list = glob "$glob";# print "list = $#list => @list\n";
  if ($#list > -1) {next;}
#  my ($dev,$ino,$mode,$nlink,$uid,$gid,$dev, $size, $atime, $mtim, $ctime, $blksize,$blocks) = stat($file);
#  my $now = time(); #print "now = $now\n";
#  my $dt = $now - $ctime;# print "now = $now, ctime = $ctime, dt = $dt\n";
#  if ($dt < 3600) {next;}
  my $string = "string:" . $pwd . "/" . $file;
  print "$string\n";
}

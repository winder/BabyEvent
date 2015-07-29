#!/usr/bin/env perl
# Login to baby-connect to download the current baby information, then upload
# the JSON data somewhere.

use warnings;
use strict;
use JSON qw(decode_json);
use Data::Dumper;
use Net::FTP;
use Term::ReadKey;
use Time::Piece;

my $VERBOSE = 1;

sub logDebug {
  my $msg = shift;
  if ($VERBOSE) {
    print $msg . "\n";
  }
}

sub runCommand {
  my $cmd = shift;
  my $msg = shift;
  if ($VERBOSE) {
    my $pr = defined($msg) ? $msg : $cmd;
    print "Running command: '$pr'\n";
  }

  return `$cmd`;
}

# program state
my $cookie = 'cookie.txt';

sub getAuthCommand {
  my ($c, $e, $p, $censor) = (@_);
  my $cmd = "curl -c $c -s https://www.baby-connect.com/Cmd?cmd=UserAuth        -d 'email=$e&pass='"; #$p'";
  $cmd .= (defined($censor) && $censor==1) ? "<password>" : "$p";

  return $cmd;
}

sub getStatusListCommand {
  my ($c, $t) = (@_);
  my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($t);
  my $ymd = sprintf("%02d%02d%02d", ($year%100), $mon+1, $mday);

  #print "sec = $sec\nmin = $min\nhour = $hour\nmday = $mday\nmon = $mon\nyear = $year\nwday = $wday\nyday = $yday\nisdst = $isdst\n";
  #print "YMD = $ymd\n";

  my $bctime = $t * 1000;
  #my $cmd = "curl -b $c -s https://www.baby-connect.com/CmdListW?cmd=StatusList -d 'Kid=0&pdt=$ymd&fmt=long&_ts_=$t'";
  my $cmd = "curl -b $c -s https://www.baby-connect.com/CmdListW?cmd=StatusList -d 'Kid=0&pdt=$ymd&fmt=short&_ts_=$bctime'";
  return $cmd;
}

sub do_login {
  my ($c, $e, $p) = (@_);
  my $cmd = getAuthCommand($c, $e, $p, 0);
  return runCommand($cmd, getAuthCommand($c, $e, $p, 1));
}

sub get_data {
  my ($c, $t) = (@_);
  my $cmd = getStatusListCommand($c, $t);
  return runCommand($cmd);
}

sub is_error_response {
  my $json = shift(@_);

  # Try to decode, return error if can't decode.
  my $decoded_json;
  eval {
   $decoded_json = decode_json($json);
   1;
  } or return 1;

  # Check for 'Error' JSON.
  if (exists $$decoded_json{'Error'}) {
    return 1;
  }

  return 0;

}

sub upload_response {
  #($dataResponse, $ftplogin, $ftppassw, $ftppath, $ftpfile);
  my ($data, $addr, $login, $passw, $path, $file) = (@_);
  my $fh;

  # Create a file with the response.
  `rm $file`;
  open($fh, '>', $file);
  print $fh $data;
  close $fh;

  # FTP stuff.
  my $ftp = Net::FTP->new($addr, Debug => 0)
        or die "Cannot connect to $addr: $@";

  $ftp->login($login,$passw)
        or die "Cannot login ", $ftp->message;

  $ftp->cwd($path)
        or die "Cannot change working directory ", $ftp->message;

  $ftp->delete($file);

  open($fh, '<', $file);
  $ftp->put($fh, $file)
        or die "put failed ", $ftp->message;;
  close $fh;

  $ftp->quit;
}

#==============#
# Main program #
#==============#

# Get user inputs.
if ($#ARGV+1 != 5) {
  print "Arguments:\n";
  print "  <baby-connect-email> <ftp-address> <ftp-login> <ftp-path> <ftp-file>\n";
  die;
}

my $email      = shift (@ARGV);
my $ftpaddress = shift (@ARGV);
my $ftplogin   = shift (@ARGV);
my $ftppath    = shift (@ARGV);
my $ftpfile    = shift (@ARGV);

my $passw;
my $ftppassw;

print "Enter your baby-connect password for $email:\n";
ReadMode('noecho'); # don't echo
chomp($passw = <STDIN>);
ReadMode(0);        # back to normal
print "\n";

print "Enter your ftp password for $ftppath:\n";
ReadMode('noecho'); # don't echo
chomp($ftppassw = <STDIN>);
ReadMode(0);        # back to normal
print "\n";


# Update http accessible file every 60 seconds.
my $amLoggedIn = 0;
while (1) {
  # Login if needed.
  if ($amLoggedIn == 0) {
    my $loginResponse = do_login($cookie, $email, $passw);
    #logDebug("Login response: \n" . $loginResponse);
    $amLoggedIn = 1;
  }

  # Request data, 
  my $timestamp = time;
  my $dataResponse = get_data($cookie, $timestamp);
  if (is_error_response($dataResponse) == 1) {
    logDebug("Error data response!\n".$dataResponse);
    $amLoggedIn = 0;
  } else {
    logDebug("Data received, uploading via ftp.");
    # upload via ftp.
    upload_response($dataResponse, $ftpaddress, $ftplogin, $ftppassw, $ftppath, $ftpfile);
  }

  sleep(60);
}

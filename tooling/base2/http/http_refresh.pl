#! /usr/bin/perl
#
#   Example of password refresh application, this generates a random 
#   new password for any user that is already in the password table.
#
use Digest::MD5;
use Digest::SHA;
my ($mechanism, $user, $realm, $table) = @ARGV;
die "E: syntax: http_refresh.pl Basic|Digest username realm passwdfile\n" unless $table;
$modified = $table.$$;

#   Generate a new random password
my @chars=('a'..'z','A'..'Z','0'..'9','_');
my $password = "";
srand;
foreach (1..10) {
    $password .= $chars [rand @chars];
}

#   Update table with new hash
open ORIGINAL, $table or die $!;
open MODIFIED, ">$modified" or die $!;

while (<ORIGINAL>) {
    if ($mechanism eq "Digest") {
        if (/^$user:$realm:/) {
            print "I: resetting Digest $user:$realm in $table to $password\n";
            $_ = "$user:$realm:".Digest::MD5::md5_hex("$user:$realm:$password")."\n";
        }
    }
    elsif ($mechanism eq "Basic") {
        if (/^$user:/) {
            print "I: resetting Basic $user in $table to $password\n";
            $_ = "$user:$realm:"."{SHA}".Digest::SHA::sha1_base64($password)."=\n";
        }
    }
    else {
        die "E: invalid mechanism '$mechanism'\n";
    }
    print MODIFIED $_;
}
rename $modified, $table;
exit (0);

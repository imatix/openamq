#!/usr/bin/perl
use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;
use Term::ANSIColor qw/:constants/;
use Data::Dumper;
use Net::FTP;
$Term::ANSIColor::AUTORESET = 1;

# TODO: check how much of the `` commands will work on windows.
#       cygwin might be a good option(ie: sh.exe, cp.exe, ...)

# Variables used to map the command line options to, and their defaults.
my $man = 0;
my $help = 0;
my $color = 0;
my $upload = 0;
my $verbose = 0;
my $model = "";
my $ccdef = "";
my $revision = 0;
my $buildroot = `pwd`; chomp $buildroot;

# Parse the options from the command line.
GetOptions('help|?'     => \$help,
           'man'        => \$man,
           'verbose|v!' => \$verbose,
           'color|c!'   => \$color,
	   'revision=n'	=> \$revision,
           'upload!'    => \$upload,
           'model=s'    => \$model,
           'ccdefines=s'=> \$ccdef,
           'buildroot=s'=> \$buildroot,
          ) or pod2usage(2);

pod2usage(1) if $help;
pod2usage(-exitstatus => 0, -verbose => 2) if $man;
pod2usage(1) unless ($model && $revision);

# chdir to the buildroot and setup environment
chdir $buildroot;
$ENV{ANSI_COLORS_DISABLED} = 1 unless $color;
$ENV{BOOM_MODEL} = $model;
$ENV{CCDEFINES} = $ccdef;

my $tmodel;
my $dmodel;
if ($model =~ m/\bmt\b/ || $ccdef =~ m/BASE_THREADSAFE/) { $tmodel = "mt"; } else { $tmodel = "st"; }
if ($model =~ m/\bdebug\b/) { $dmodel = "debug"; } else { $dmodel = "release"; }

# globals and helper functions
my %log;
sub ok   { if ($verbose) { print " [ ",GREEN,"OK",RESET," ]\t"; print $@; print "\n"; }}
sub stop { if ($verbose) { print " [ ",YELLOW,"WARN",RESET," ]\t"; print shift; print "\n";} exit 0; }
sub fail { if ($verbose) { print " [ ",RED,"FAIL",RESET," ]\t"; print $@; print "\n";} showlog(); exit -1; }
sub showlog {
    foreach my $proj (keys %log) {
        foreach my $action (keys %{$log{$proj}}) {
            if ($log{$proj}{"_".$action}) {
                $log{$proj}{$action} =~ s/$buildroot/\$buildroot/g;
                print "Failure in $proj while doing $action.\n";
                print $log{$proj}{$action};
            }
        }
    }
}

# create a builddir and copy an ibase skeleton to it
my $builddir = "build-$tmodel-$dmodel-r$revision-".`date +%s`; chomp $builddir;

print "checking for $tmodel-IBASE skeleton ..." if $verbose;
unless (-d "../bootstrap/ibase_skel_$tmodel") { fail "make sure there is an ibase skeleton in ../bootstrap/ibase_skel_$tmodel\n"; }
ok;
if (-d $builddir) { fail "dir already exist, another build running? use this script sequentially not concurrently"; }
mkdir $builddir; mkdir "$builddir/ibase";
# use File::Copy or cygwin cp.exe?
`cp -ar ../bootstrap/ibase_skel_$tmodel/* $builddir/ibase`; 

# setup the environment for the build
$ENV{IBASE} = "$buildroot/$builddir/ibase";
$ENV{PATH} = "$buildroot/$builddir/ibase/bin:".$ENV{PATH};

# start the process of checking out every component and building it
# if anything goes wrong, the script will exit and print the log
chdir $builddir;
foreach my $proj (qw/foreign base gsl4 base2 openamq/) {
   print "getting $proj ..." if $verbose;
   `svn checkout https://svn.imatix.net/imatix/$proj/trunk $proj`;
   ($? && fail "error checking out $proj\n") || ok;
   chdir "$buildroot/$builddir/$proj";
   
   foreach my $action (qw/configure build test install/) {
       print "trying to $action $proj ..." if $verbose;
       ($log{$proj}{$action} = `boom $action 2>&1`);
       if ($?) { 
           # track which one failed, used in showlog
           $log{$proj}{"_".$action} = 1; 
           fail "error in $action phase of $proj\n";
       } else { 
           ok; 
       }
   }
   chdir "$buildroot/$builddir";
}

# if we got here then everything built fine, make a done file
# so we won't compile this revision again for this $tmodel
`touch $buildroot/$builddir/done`;
print "OpenAMQ rev. $revision (model=$model) built successfully\n";

# TODO: this part can be split into platform dependant scripts
# creating package
mkdir "$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/";
mkdir "$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/bin";
mkdir "$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/etc";
mkdir "$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/lib";
open(README, "> $buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/README");
print README "This package was created on ".localtime()."\n";
print README "\trevision=$revision\n";
print README "\tBOOM_MODEL=$model\n";
close README;
`cp $buildroot/$builddir/ibase/bin/amq_server     	$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/bin`;
`cp $buildroot/$builddir/ibase/bin/amq_client     	$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/bin`;
`cp $buildroot/$builddir/ibase/bin/amq_server_base.cfg  $buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/etc`;
`cp $buildroot/$builddir/ibase/lib/*              	$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/lib`;

chdir "$buildroot/$builddir/OpenAMQ-bin-$tmodel-$dmodel-$revision/lib";
`for i in *.a; do ar x \$i; done`;
`rm -f *.a`;
`ar r libopenamq.a *.*o*`;
`rm -f *.*o*`;

chdir "$buildroot/$builddir";
`tar czf OpenAMQ-bin-$tmodel-$dmodel-$revision.tar.gz OpenAMQ-bin-$tmodel-$dmodel-$revision/`;
`cp OpenAMQ-bin-$tmodel-$dmodel-$revision.tar.gz ../../packages/`;

# uploading package
# TODO: move to other script
#if ($upload) {
#    my $file = "$ENV{HOME}/.bbuilderrc"; 
#    my %config;
#    print "parsing config file \'$file\' ...";
#    unless (my $return = do $file) {
#        fail "couldn't parse $file: $@" if $@;
#        fail "couldn't do $file: $!"    unless defined $return;
#        fail "couldn't run $file"       unless $return;
#    } 
#    ok;
#    
#    my $ftp = new Net::FTP("$config{ftphost}") || fail "$@";
#    $ftp->login($config{ftpuser},$config{ftppass}) || fail "$@";
#    $ftp->cwd($config{ftpdir});
#    $ftp->put("OpenAMQ-bin-$tmodel-$revision.tar.gz");
#    $ftp->quit;
#}

__END__

=head1 NAME

run.pl - A tool to compile and upload OpenAMQ

=head1 AUTHOR

Tim Esselens <tim@imatix.net>

=head1 SYNOPSIS

run.pl [options] --model BOOM_MODEL --revision REV

=head1 OPTIONS

=over 8

=item B<--model>
A comma separated list of BOOM_MODEL options for this build

=item B<--revision>
Revision to download and compile

=item B<--ccdefines>
Will be set as CCDEFINES in the environment.

=item B<--color>
Enable color while building (for manual build)

=item B<--verbose>
Be verbose while building

=back

=cut





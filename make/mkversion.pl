#!/usr/bin/perl -w
#
# This Script generates version.c
# It determines the regfile version based on the diff utility
# Determines SW version based on git tag
#

use Cwd;

$AVENGER_REGFILE_RELEASE_FOLDER = "/projects/cpxsw_dev/avenger_regs_release/";
$AVENGER_REGFILE_NAME = "avenger_fullchip.regs";
$RELEASE_FILE_NAME = $ENV{TOPDIR} . "/RELEASE";

sub get_regfile_relname {
    my $regfile = shift;
    if (! -e $regfile) {
       print "Cannot locate $regfile";
       return "";
    }
    opendir my $dir, $AVENGER_REGFILE_RELEASE_FOLDER || die "Cannot locate folder $AVENGER_REGFILE_RELEASE_FOLDER";

    while (my $item = readdir $dir) {
        next if ($item eq '.' or $item eq '..');
        my $folder = "$AVENGER_REGFILE_RELEASE_FOLDER/$item";
        next if ( -f $folder);
        my $filename = "$folder/$AVENGER_REGFILE_NAME";
        next unless (-e $filename);
        #print $filename,"\n";
        @testresult = `diff -q $filename $regfile`;
        #print (@testresult);
        next if (@testresult);
        #print "Release:",$item;
        @tmp = split(/_/, $item);
        return (join('_', @tmp[3..$#tmp]));
        
    }
}

sub gen_version {
    my $regfile = shift;
    my $outfile = shift;
    my $gittag = `git describe --tags --long --always`;
    my ($major) = (`grep MAJOR $RELEASE_FILE_NAME` =~ m/MAJOR\s*=\s*(\d+)/);
    my ($minor) = (`grep MINOR $RELEASE_FILE_NAME` =~ m/MINOR\s*=\s*(\d+)/);
    my ($subminor) = (`grep SUBMINOR $RELEASE_FILE_NAME` =~ m/SUBMINOR\s*=\s*(\d+)/);
    $subminor = 0 if !(defined $subminor);

    my @tmp = split(/_./, $gittag);
    if (@tmp == 1) {
       #my $br = `git branch | tr -d '* '`; chomp($br);
       my $br = `git rev-parse --abbrev-ref HEAD`; chomp($br);
       my $commits = `git rev-list --count HEAD`;
       $gittag = "$br-$major\_$minor\_$subminor\-$commits";
       print "$gittag" .  "$br-$major\_$minor\_$subminor\-$commits";
    }
    my $date =  localtime();
    my $regsversion = get_regfile_relname($regfile);

    $date =~  s/\s+/ /g;
    $gittag =~  s/\s+/ /g;
    print("Writing file $outfile\n");
    open $fd, ">$outfile" || die "Cannot open $outfile for writing";
    print $fd <<EOT;
/*
 * \$Copyright: (c) 2017 Broadcom Corp.
 * \All Rights Reserved$
 * File:
 *    version.h
 * Description:
 *    Robo2 SW Release Version Info
 */

#ifndef __ROBO2_VERSION__
#define __ROBO2_VERSION__

#define REGS_VERSION \"$regsversion\"
#define SW_VERSION   \"$gittag\"
#define SW_BUILD_DATE  \"$date\"

#endif /* __ROBO2_VERSION__ */
EOT

    close $fd;
}
 
if (@ARGV < 2) {
    print("Usage: <regfile> < outfile version.h>\n");
    exit(-1);
}
gen_version($ARGV[0], $ARGV[1]);

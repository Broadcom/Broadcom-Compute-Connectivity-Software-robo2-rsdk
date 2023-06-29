#!/usr/bin/perl -w
#
# This Script generates version.c
#

use Cwd;

$RELEASE_FILE_NAME = $ENV{TOPDIR} . "/RELEASE";

sub gen_version {
    my $outfile = shift;
    my $swtag = "ROBO_OS_REL";
    my ($major) = (`grep MAJOR $RELEASE_FILE_NAME` =~ m/MAJOR\s*=\s*(\d+)/);
    my ($minor) = (`grep MINOR $RELEASE_FILE_NAME` =~ m/MINOR\s*=\s*(\d+)/);
    my ($subminor) = (`grep SUBMINOR $RELEASE_FILE_NAME` =~ m/SUBMINOR\s*=\s*(\d+)/);
    $subminor = 0 if !(defined $subminor);
    $swtag .= "_$major\_$minor\_$subminor";
    my $date =  localtime();
    my $regsversion = "";

    $date =~  s/\s+/ /g;
    $swtag =~  s/\s+/ /g;
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
#define SW_VERSION   \"$swtag\"
#define SW_BUILD_DATE  \"$date\"

#endif /* __ROBO2_VERSION__ */
EOT

    close $fd;
}
 
if (@ARGV < 1) {
    print("Usage: < outfile version.h>\n");
    exit(-1);
}
gen_version($ARGV[0]);

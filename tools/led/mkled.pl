#!/usr/bin/perl
# Shell script, front end for regsfile generation
# Now calls mreg2c directly.
#

#GDIFF=/tools/bin/diff	# Gnu diff

##
## Accessor functions for register information
##
## $Id$
##

##
## The scope of this file is to provide the routines to update
## and access the register information.
##

use FileHandle;
use Time::localtime;	# localtime->year
use strict;
use warnings;

################################################################
# Open two files and merge into a third file 
################################################################
sub merge_files {
    my @srcs = @{$_[0]};
    my $dest = $_[1];


    die "Usage: $0 in in ... in  out\n" if not @srcs;
    open my $outfile, '>>', $dest or die "Failed to open '$dest' for appending\n"; 
     foreach my $file (@srcs) {
         if (open my $infile, '<', $file) {
             while (my $curline = <$infile>) {
                 if ($curline =~ /unsigned char/) {
                     $curline =~ s/unsigned char/uint8/;
                 }
                 if ($curline =~ /ledproc_led/) {
                     $curline =~ s/ledproc_led\s*\[\s*\]/led_firmware[256]/;
                 }
                 next if ($curline =~ /autogenerated/);
                 print $outfile $curline;
              }
              close $infile;
          } else {
              warn "Failed to open '$file' for reading\n";
          }
       }
    close $outfile;
    print "done\n";
}

################################################################
# Open file and generate header info
################################################################
sub gen_header {
    my $filename = shift;
    my $namebase = shift;
    my $message = shift;
    my $isheader = shift;
    my $hname;
    my $year = localtime->year() + 1900;
    my $fhandle = new FileHandle;
    my $dol = "\$";

    print STDERR "Writing $namebase\n";
    open ($fhandle, ">$filename") ||
        die "$filename open failed: $!\n";

    print $fhandle <<"EOT";
/*
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated from the led.asm file.
 * Edits to this file will be lost when it is regenerated.
 *
 * ${dol}Id: ${dol}
 * ${dol}Copyright: (c) $year Broadcom Corp.
 * All Rights Reserved.${dol}
 *
 * File:	$namebase
 * Purpose:	$message
EOT

    print $fhandle " */\n\n";

    if ($isheader) {
        # Put in conditional include defines
        ($hname = $namebase) =~ s/\./_/g;
        $hname = uc($hname);
        print $fhandle "#ifndef _LED_H \n";;
        print $fhandle "#define _LED_H \n\n";
    }
    return $fhandle;
}

    my $fhdl;
    my $ledhdr = "led.h";
    my $ledc = "led.c";
    my $ledtmp = "led.tmp";
    my $mergefile = "ledmerge.h";
    my @infiles = ($ledhdr, $ledc);

    # Generate .h file.
    $fhdl = gen_header($ledhdr, $ledhdr, 
        "This file contains definitions for LED ucode", 1);
    close ($fhdl);

    merge_files ( \@infiles, $mergefile);

    open ($fhdl, '>>', $mergefile) ||
        die "$ledhdr open failed: $!\n";

    say $fhdl "#endif\t /* _LED_H */\n";
    close ($fhdl);
    rename $mergefile, $ledtmp;


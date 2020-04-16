#!/usr/bin/perl
#
#   Synopsis: The purpose of the scrip is to translate intel-hex file
#   format into format of $readmemh in Verilog.
#
#   Usage:  ihex2mem.pl <hex filename> [<output filename>]
#
#


if ( @ARGV  == 2 ) {
  open (HEXFILE, "<$ARGV[0]") or die ("Unable to open file: $ARGV[0]\n");
  open (MEMFILE, ">$ARGV[1]") or die ("Unable to open file: $ARGV[1]\n");
} elsif (@ARGV == 1 ){
  open (HEXFILE, "<$ARGV[0]") or die "Unable to open file: $ARGV[0]\n";
  $ARGV[0] =~ s/\..*//; 
  open (MEMFILE, ">$ARGV[0].mem") or 
    die "Unable to open file: $ARGV[0].mem\n";
} else {
    die ( "Syntax:  ihex2mem.pl <hex filename> [<output filename>]\n" );
}

print "Processing $ARGV[0] ....\n";
while(<HEXFILE>){
    @line = split("", $_);

    #-------------------------------------
    # caculate bytecount
    #-------------------------------------

    $bytenct_char = "$line[1]$line[2]";
    $bytecount = hex ($bytenct_char);

    #-------------------------------------
    # Process data
    #-------------------------------------

    if($bytecount != 0){
      printf MEMFILE  "\@$line[3]$line[4]$line[5]$line[6]\t";
      for($i = 0;$i<$bytecount;$i++){
            printf MEMFILE "$line[9+$i*2]$line[10+$i*2] ";
      }
      printf MEMFILE  "\n";
    }
}

close(MEMFILE);
close(HEXFILE);
print "done\n";



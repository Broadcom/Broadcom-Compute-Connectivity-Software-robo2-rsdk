#! /usr/bin/perl -w

   no warnings 'uninitialized';
   open(IFD, "< $ARGV[0]") || die "Cannot open input file $ARGV[0]";
   open(OFD, "> $ARGV[1]") || die "Cannot open output file $ARGV[0]";
   binmode(IFD);
   printf OFD ("\$IFDSTANCE qt_serial_flash.mem\n");
   printf OFD ("\$RADIX   HEX\n");
   printf OFD ("\$ADDRESS   0    ffffff\n");
   $A = hex($ARGV[2]) || 0x0;
   #$is_little_endian = unpack( 'c', pack( 's', 1 ) );
   while (read(IFD,$b,4)) {
      my ($b0, $b1, $b2, $b3) = unpack("C4", $b);
      printf OFD ("%x %02x\n", $A++, $b0);
      printf OFD ("%x %02x\n", $A++, $b1);
      printf OFD ("%x %02x\n", $A++, $b2);
      printf OFD ("%x %02x\n", $A++, $b3);
   }
   close(IFD);
   close(OFD);


#!/usr/bin/perl


   open(IFD, "< ../src/phymod/eagle_phy_1_ucode.c") || die "Cannot open input file eagle_phy_1_ucode.c";
   open(OFD, "> eagle1_ucode.bin") || die "Cannot open output file eagle1_ucode.bin";
   open(OFD_HEX, "> eagle1_ucode.hex") || die "Cannot open output file eagle1_ucode.hex";
   binmode(OFD);
   #$is_little_endian = unpack( 'c', pack( 's', 1 ) );
   $count = 0;

   $addr = 98304;

    printf OFD_HEX ("\$INSTANCE qt_qspi_serial_flash.mem\n");
    printf OFD_HEX ("\$RADIX   HEX\n");
    printf OFD_HEX ("\$ADDRESS   0    ffffff\n");
 
    $skip_count = 1;

    while (<IFD>){
        # the ucode data starts at line no. 10 
        if ($skip_count < 10) {
            $skip_count = $skip_count + 1;
            next;
        }
        @line = split(",",$_);
        print "----------\n";
        $count = 0;
        foreach my $val (@line) {
            $count = $count + 1;
            $val =~ s/^\s+//;
            $int_val = hex($val);
            if ($count <= 12) { 
                print "$count $val $int_val \n";
                print OFD pack('c', $int_val);
                printf OFD_HEX ("%x %02x\n", $addr, $int_val);
                $addr = $addr + 1;
               # print pack('c', $val); 
            }
        }
        print "----------\n";
    }
    close(IFD);
    close(OFD);
    close(OFD_HEX);

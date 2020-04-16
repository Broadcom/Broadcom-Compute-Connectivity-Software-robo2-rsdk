;Updated code to handle port ordering for QSGMII/XFI ports -- 02/08/2017
;Updated code to support active high LED in serial mode. -- 01/05/2017
;--------------Start of Program------------------
begin:
            ld      A,0
            ld      (ledbit),A
            ld      A,0x5           ;Start with QSGMII
            ld      (portnum),A
loop2:  
            port    (portnum)
            pushst  SPEED1
            tinv
            pushst  RX_EV
            pushst  TX_EV
            tor
            tinv
            tor
            call    pack_cmd
            port    (portnum)
            pushst  SPEED1
            pushst  RX_EV
            pushst  TX_EV
            tor
            tinv
            tor
            call    pack_cmd
            ld      A,(portnum)
            cmp     A,0x4
            jz      setport2
            dec     (portnum)
            jmp     loop2

setport2:
            ld      A,0x1           ;Start with QSGMII port10
            ld      (portnum),A
loop4:  
            port    (portnum)
            pushst  SPEED1
            tinv
            pushst  RX_EV
            pushst  TX_EV
            tor
            tinv
            tor
            call    pack_cmd
            port    (portnum)
            pushst  SPEED1
            pushst  RX_EV
            pushst  TX_EV
            tor
            tinv
            tor
            call    pack_cmd
            ld      A,(portnum)
            cmp     A,0x0
            jz      setport0
            dec     (portnum)
            jmp     loop4

setport0:
            ld      A,0xD           ;Start with GPHY P0
            ld      (portnum),A

loop1:    
            port    (portnum)
            pushst  SPD0
            pushst  SPD1
            tor     
            pushst  ACT
            tor
            call skip1
            port   (portnum) 
            pushst  SPD0
            pushst  SPD1
            txor
            tinv
            pushst  ACT
            tor
            call skip1
            ld      A,(portnum)
            cmp     A,0x6
            jz      setport1
            dec     (portnum)
            jmp     loop1

pack_cmd:
            ld      A,(0xFE)        ;strap related change of polarity only required in parallel mode
            TST     A,7
            jnc     skip1
            ld      A,(0xFE)
            ld      B,(ledbit)
            TST     A,B
            push    CY
            txor
            tinv
            jmp     skip2
skip1:
            ld      A,(0xFE)
	        TST     A,7     	;change polarity only if in serial mode
            push    CY
            txor
            tinv
skip2:      inc    (ledbit)
            pack
            ret

setport1:
            ld      A,0x3           ;Start with PM
            ld      (portnum),A
loop3:
            port    (portnum)
            pushst  SPEED1
            pushst  SPEED0
            tand
            tinv 
            pushst  RX_EV
            pushst  TX_EV
            tor
            tinv
            tor
            call skip1
            port    (portnum)
            pushst  SPEED1
            pushst  SPEED0
            tand
            pushst  RX_EV
            pushst  TX_EV
            tor
            tinv
            tor
            call skip1
            ld      A,(portnum)
            cmp     A,0x2
            jz      exit
            dec     (portnum)
            jmp     loop3

exit:       send    28


;data storage
ledbit      equ 0x7E
portnum     equ 0x7F                ;temp to hold current port number address
;symbolic labels
SPD0        equ 0x0                 ;led_o[1]
SPD1        equ 0x1                 ;led_o[2]
SPEED0      equ 0x3                 ;SPD[0]
SPEED1      equ 0x4                 ;SPD[1]
ACT         equ 0x2                 ;led_o[3] Activity status
RX_EV       equ 0x0
TX_EV       equ 0x1
strap       equ 0x7F                ;strap status on led[7:0] pins (Actual address is 0xfe)


;--------------End of Program------------------


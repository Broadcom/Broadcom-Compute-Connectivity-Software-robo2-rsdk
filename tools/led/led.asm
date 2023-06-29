;Updated code to support RM310. -- 07/22/2023
;-------------------------- start of program --------------------------
begin:
	ld A,12
	Port A
	Pushst LNK
	pack
	ld A,13
	Port A
	Pushst LNK
	pack
	;update refresh phase within 1Hz
	ld A, (phase)
	inc A ;next phase
	cmp A, 10
	jc chkblink
	sub A, A ;A = 0
chkblink:
	ld (phase), A
	sub B, B ; B =0
	cmp A, 5
	jnc noblink
	inc B ; B = 1
noblink:
	ld (blink), B
	ld A, 0
setxfiport:
	ld A, 0xB ;port 13 status[0:6]
	ld (portnum), A
	ld A, 0xB ; port 13 status[7]
	ld (portlnk), A
	call sgmiixfiloop
	dec(portnum) ; port 12 status[0:6]
	dec (portlnk) ; port 12 status[7]
	call sgmiixfiloop
	jmp exit
sgmiixfiloop:
	port (portnum)
	pushst TX_EV;
	pushst RX_EV;
	tor;
	pushst LNK;
	tand;
	push (blink);
	tand;
	pushst TX_EV;
	pushst RX_EV;
	tor;
	pushst LNK;
	txor;
	tor;
	tinv;
	call pack_cmd2;

	port (portnum)
	pushst TX_EV;
	pushst RX_EV;
	tor;
	pushst LNK;
	tand;
	push (blink);
	tand;
	pushst TX_EV;
	pushst RX_EV;
	tor;
	pushst LNK;
	txor;
	tor;
	tinv;
	call pack_cmd2;

	port (portnum)
	pushst TX_EV;
	pushst RX_EV;
	tor;
	pushst LNK;
	tand;
	push (blink);
	tand;
	pushst TX_EV;
	pushst RX_EV;
	tor;
	pushst LNK;
	txor;
	tor;
	tinv;
	call pack_cmd2;
	ret
pack_cmd2:
	pack
	ret
exit:
	send 8

;data storage
phase equ 0xF3;phase within 30 Hz
blink   equ 0xF2;1 if blink on, 0 if blink off
portlnk equ 0xF1
portnum equ 0xF0;temp to hold whichport we are working on
RX_EV equ 0x0
TX_EV equ 0x1
LNK equ 0x7
;-------------end---------------




















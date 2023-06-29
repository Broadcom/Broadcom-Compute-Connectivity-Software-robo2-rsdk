
/projects/cpxsw_dev/home/spalanis/robo2sdk/hybrid_m7/build/hybrid_m7.img:     file format elf32-littlearm


Disassembly of section .text:

20000140 <_sbrk>:
20000140:	b480      	push	{r7}
20000142:	b083      	sub	sp, #12
20000144:	af00      	add	r7, sp, #0
20000146:	6078      	str	r0, [r7, #4]
20000148:	2300      	movs	r3, #0
2000014a:	4618      	mov	r0, r3
2000014c:	370c      	adds	r7, #12
2000014e:	46bd      	mov	sp, r7
20000150:	f85d 7b04 	ldr.w	r7, [sp], #4
20000154:	4770      	bx	lr
20000156:	bf00      	nop

20000158 <dump_frame>:
20000158:	b590      	push	{r4, r7, lr}
2000015a:	b08d      	sub	sp, #52	; 0x34
2000015c:	af02      	add	r7, sp, #8
2000015e:	6078      	str	r0, [r7, #4]
20000160:	687b      	ldr	r3, [r7, #4]
20000162:	681b      	ldr	r3, [r3, #0]
20000164:	627b      	str	r3, [r7, #36]	; 0x24
20000166:	687b      	ldr	r3, [r7, #4]
20000168:	685b      	ldr	r3, [r3, #4]
2000016a:	623b      	str	r3, [r7, #32]
2000016c:	687b      	ldr	r3, [r7, #4]
2000016e:	689b      	ldr	r3, [r3, #8]
20000170:	61fb      	str	r3, [r7, #28]
20000172:	687b      	ldr	r3, [r7, #4]
20000174:	68db      	ldr	r3, [r3, #12]
20000176:	61bb      	str	r3, [r7, #24]
20000178:	687b      	ldr	r3, [r7, #4]
2000017a:	691b      	ldr	r3, [r3, #16]
2000017c:	617b      	str	r3, [r7, #20]
2000017e:	687b      	ldr	r3, [r7, #4]
20000180:	695b      	ldr	r3, [r3, #20]
20000182:	613b      	str	r3, [r7, #16]
20000184:	687b      	ldr	r3, [r7, #4]
20000186:	699b      	ldr	r3, [r3, #24]
20000188:	60fb      	str	r3, [r7, #12]
2000018a:	687b      	ldr	r3, [r7, #4]
2000018c:	69db      	ldr	r3, [r3, #28]
2000018e:	60bb      	str	r3, [r7, #8]
20000190:	68f9      	ldr	r1, [r7, #12]
20000192:	693a      	ldr	r2, [r7, #16]
20000194:	68bc      	ldr	r4, [r7, #8]
20000196:	697b      	ldr	r3, [r7, #20]
20000198:	9300      	str	r3, [sp, #0]
2000019a:	480a      	ldr	r0, [pc, #40]	; (200001c4 <dump_frame+0x6c>)
2000019c:	4623      	mov	r3, r4
2000019e:	f000 fa87 	bl	200006b0 <sysprintf>
200001a2:	6a79      	ldr	r1, [r7, #36]	; 0x24
200001a4:	6a3a      	ldr	r2, [r7, #32]
200001a6:	69fc      	ldr	r4, [r7, #28]
200001a8:	69bb      	ldr	r3, [r7, #24]
200001aa:	9300      	str	r3, [sp, #0]
200001ac:	4806      	ldr	r0, [pc, #24]	; (200001c8 <dump_frame+0x70>)
200001ae:	4623      	mov	r3, r4
200001b0:	f000 fa7e 	bl	200006b0 <sysprintf>
200001b4:	f3bf 8f4f 	dsb	sy
200001b8:	f3bf 8f6f 	isb	sy
200001bc:	4803      	ldr	r0, [pc, #12]	; (200001cc <dump_frame+0x74>)
200001be:	f000 fa45 	bl	2000064c <sysprint>
200001c2:	e7fe      	b.n	200001c2 <dump_frame+0x6a>
200001c4:	20008a58 	.word	0x20008a58
200001c8:	20008a8c 	.word	0x20008a8c
200001cc:	20008ac0 	.word	0x20008ac0

200001d0 <hardfault_handler>:
200001d0:	b672      	cpsid	i
200001d2:	f01e 0f04 	tst.w	lr, #4
200001d6:	bf0c      	ite	eq
200001d8:	f3ef 8008 	mrseq	r0, MSP
200001dc:	f3ef 8009 	mrsne	r0, PSP
200001e0:	4a00      	ldr	r2, [pc, #0]	; (200001e4 <dump_frame_const>)
200001e2:	4710      	bx	r2

200001e4 <dump_frame_const>:
200001e4:	20000159 	.word	0x20000159

200001e8 <common_handler>:
200001e8:	b672      	cpsid	i
200001ea:	f01e 0f04 	tst.w	lr, #4
200001ee:	bf0c      	ite	eq
200001f0:	f3ef 8008 	mrseq	r0, MSP
200001f4:	f3ef 8009 	mrsne	r0, PSP
200001f8:	6981      	ldr	r1, [r0, #24]
200001fa:	f8df 2004 	ldr.w	r2, [pc, #4]	; 20000200 <cb_const>
200001fe:	4710      	bx	r2

20000200 <cb_const>:
20000200:	20000205 	.word	0x20000205

20000204 <common_handler_callback>:
20000204:	b590      	push	{r4, r7, lr}
20000206:	b08f      	sub	sp, #60	; 0x3c
20000208:	af02      	add	r7, sp, #8
2000020a:	6078      	str	r0, [r7, #4]
2000020c:	4b76      	ldr	r3, [pc, #472]	; (200003e8 <common_handler_callback+0x1e4>)
2000020e:	62fb      	str	r3, [r7, #44]	; 0x2c
20000210:	4b76      	ldr	r3, [pc, #472]	; (200003ec <common_handler_callback+0x1e8>)
20000212:	62bb      	str	r3, [r7, #40]	; 0x28
20000214:	4b76      	ldr	r3, [pc, #472]	; (200003f0 <common_handler_callback+0x1ec>)
20000216:	627b      	str	r3, [r7, #36]	; 0x24
20000218:	4b76      	ldr	r3, [pc, #472]	; (200003f4 <common_handler_callback+0x1f0>)
2000021a:	623b      	str	r3, [r7, #32]
2000021c:	f3ef 8305 	mrs	r3, IPSR
20000220:	61fb      	str	r3, [r7, #28]
20000222:	4875      	ldr	r0, [pc, #468]	; (200003f8 <common_handler_callback+0x1f4>)
20000224:	f000 fa12 	bl	2000064c <sysprint>
20000228:	6a3b      	ldr	r3, [r7, #32]
2000022a:	681a      	ldr	r2, [r3, #0]
2000022c:	6afb      	ldr	r3, [r7, #44]	; 0x2c
2000022e:	681b      	ldr	r3, [r3, #0]
20000230:	4872      	ldr	r0, [pc, #456]	; (200003fc <common_handler_callback+0x1f8>)
20000232:	69f9      	ldr	r1, [r7, #28]
20000234:	f000 fa3c 	bl	200006b0 <sysprintf>
20000238:	6afb      	ldr	r3, [r7, #44]	; 0x2c
2000023a:	681b      	ldr	r3, [r3, #0]
2000023c:	61bb      	str	r3, [r7, #24]
2000023e:	69bb      	ldr	r3, [r7, #24]
20000240:	0a1b      	lsrs	r3, r3, #8
20000242:	b2db      	uxtb	r3, r3
20000244:	617b      	str	r3, [r7, #20]
20000246:	69bb      	ldr	r3, [r7, #24]
20000248:	b2db      	uxtb	r3, r3
2000024a:	613b      	str	r3, [r7, #16]
2000024c:	69bb      	ldr	r3, [r7, #24]
2000024e:	0c1b      	lsrs	r3, r3, #16
20000250:	60fb      	str	r3, [r7, #12]
20000252:	697b      	ldr	r3, [r7, #20]
20000254:	f003 0380 	and.w	r3, r3, #128	; 0x80
20000258:	2b00      	cmp	r3, #0
2000025a:	d005      	beq.n	20000268 <common_handler_callback+0x64>
2000025c:	6abb      	ldr	r3, [r7, #40]	; 0x28
2000025e:	681b      	ldr	r3, [r3, #0]
20000260:	4867      	ldr	r0, [pc, #412]	; (20000400 <common_handler_callback+0x1fc>)
20000262:	4619      	mov	r1, r3
20000264:	f000 fa24 	bl	200006b0 <sysprintf>
20000268:	697b      	ldr	r3, [r7, #20]
2000026a:	f003 0301 	and.w	r3, r3, #1
2000026e:	2b00      	cmp	r3, #0
20000270:	d002      	beq.n	20000278 <common_handler_callback+0x74>
20000272:	4864      	ldr	r0, [pc, #400]	; (20000404 <common_handler_callback+0x200>)
20000274:	f000 f9ea 	bl	2000064c <sysprint>
20000278:	697b      	ldr	r3, [r7, #20]
2000027a:	f003 0302 	and.w	r3, r3, #2
2000027e:	2b00      	cmp	r3, #0
20000280:	d002      	beq.n	20000288 <common_handler_callback+0x84>
20000282:	4861      	ldr	r0, [pc, #388]	; (20000408 <common_handler_callback+0x204>)
20000284:	f000 f9e2 	bl	2000064c <sysprint>
20000288:	697b      	ldr	r3, [r7, #20]
2000028a:	f003 0304 	and.w	r3, r3, #4
2000028e:	2b00      	cmp	r3, #0
20000290:	d002      	beq.n	20000298 <common_handler_callback+0x94>
20000292:	485e      	ldr	r0, [pc, #376]	; (2000040c <common_handler_callback+0x208>)
20000294:	f000 f9da 	bl	2000064c <sysprint>
20000298:	697b      	ldr	r3, [r7, #20]
2000029a:	f003 0308 	and.w	r3, r3, #8
2000029e:	2b00      	cmp	r3, #0
200002a0:	d002      	beq.n	200002a8 <common_handler_callback+0xa4>
200002a2:	485b      	ldr	r0, [pc, #364]	; (20000410 <common_handler_callback+0x20c>)
200002a4:	f000 f9d2 	bl	2000064c <sysprint>
200002a8:	697b      	ldr	r3, [r7, #20]
200002aa:	f003 0310 	and.w	r3, r3, #16
200002ae:	2b00      	cmp	r3, #0
200002b0:	d002      	beq.n	200002b8 <common_handler_callback+0xb4>
200002b2:	4858      	ldr	r0, [pc, #352]	; (20000414 <common_handler_callback+0x210>)
200002b4:	f000 f9ca 	bl	2000064c <sysprint>
200002b8:	693b      	ldr	r3, [r7, #16]
200002ba:	f003 0380 	and.w	r3, r3, #128	; 0x80
200002be:	2b00      	cmp	r3, #0
200002c0:	d005      	beq.n	200002ce <common_handler_callback+0xca>
200002c2:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200002c4:	681b      	ldr	r3, [r3, #0]
200002c6:	4854      	ldr	r0, [pc, #336]	; (20000418 <common_handler_callback+0x214>)
200002c8:	4619      	mov	r1, r3
200002ca:	f000 f9f1 	bl	200006b0 <sysprintf>
200002ce:	693b      	ldr	r3, [r7, #16]
200002d0:	f003 0301 	and.w	r3, r3, #1
200002d4:	2b00      	cmp	r3, #0
200002d6:	d002      	beq.n	200002de <common_handler_callback+0xda>
200002d8:	4850      	ldr	r0, [pc, #320]	; (2000041c <common_handler_callback+0x218>)
200002da:	f000 f9b7 	bl	2000064c <sysprint>
200002de:	693b      	ldr	r3, [r7, #16]
200002e0:	f003 0302 	and.w	r3, r3, #2
200002e4:	2b00      	cmp	r3, #0
200002e6:	d002      	beq.n	200002ee <common_handler_callback+0xea>
200002e8:	484d      	ldr	r0, [pc, #308]	; (20000420 <common_handler_callback+0x21c>)
200002ea:	f000 f9af 	bl	2000064c <sysprint>
200002ee:	693b      	ldr	r3, [r7, #16]
200002f0:	f003 0308 	and.w	r3, r3, #8
200002f4:	2b00      	cmp	r3, #0
200002f6:	d002      	beq.n	200002fe <common_handler_callback+0xfa>
200002f8:	484a      	ldr	r0, [pc, #296]	; (20000424 <common_handler_callback+0x220>)
200002fa:	f000 f9a7 	bl	2000064c <sysprint>
200002fe:	693b      	ldr	r3, [r7, #16]
20000300:	f003 0310 	and.w	r3, r3, #16
20000304:	2b00      	cmp	r3, #0
20000306:	d002      	beq.n	2000030e <common_handler_callback+0x10a>
20000308:	4847      	ldr	r0, [pc, #284]	; (20000428 <common_handler_callback+0x224>)
2000030a:	f000 f99f 	bl	2000064c <sysprint>
2000030e:	68fb      	ldr	r3, [r7, #12]
20000310:	f003 0301 	and.w	r3, r3, #1
20000314:	2b00      	cmp	r3, #0
20000316:	d006      	beq.n	20000326 <common_handler_callback+0x122>
20000318:	687b      	ldr	r3, [r7, #4]
2000031a:	3318      	adds	r3, #24
2000031c:	681b      	ldr	r3, [r3, #0]
2000031e:	4843      	ldr	r0, [pc, #268]	; (2000042c <common_handler_callback+0x228>)
20000320:	4619      	mov	r1, r3
20000322:	f000 f9c5 	bl	200006b0 <sysprintf>
20000326:	68fb      	ldr	r3, [r7, #12]
20000328:	f003 0302 	and.w	r3, r3, #2
2000032c:	2b00      	cmp	r3, #0
2000032e:	d006      	beq.n	2000033e <common_handler_callback+0x13a>
20000330:	687b      	ldr	r3, [r7, #4]
20000332:	3318      	adds	r3, #24
20000334:	681b      	ldr	r3, [r3, #0]
20000336:	483e      	ldr	r0, [pc, #248]	; (20000430 <common_handler_callback+0x22c>)
20000338:	4619      	mov	r1, r3
2000033a:	f000 f9b9 	bl	200006b0 <sysprintf>
2000033e:	68fb      	ldr	r3, [r7, #12]
20000340:	f003 0304 	and.w	r3, r3, #4
20000344:	2b00      	cmp	r3, #0
20000346:	d006      	beq.n	20000356 <common_handler_callback+0x152>
20000348:	687b      	ldr	r3, [r7, #4]
2000034a:	3318      	adds	r3, #24
2000034c:	681b      	ldr	r3, [r3, #0]
2000034e:	4839      	ldr	r0, [pc, #228]	; (20000434 <common_handler_callback+0x230>)
20000350:	4619      	mov	r1, r3
20000352:	f000 f9ad 	bl	200006b0 <sysprintf>
20000356:	68fb      	ldr	r3, [r7, #12]
20000358:	f003 0308 	and.w	r3, r3, #8
2000035c:	2b00      	cmp	r3, #0
2000035e:	d006      	beq.n	2000036e <common_handler_callback+0x16a>
20000360:	687b      	ldr	r3, [r7, #4]
20000362:	3318      	adds	r3, #24
20000364:	681b      	ldr	r3, [r3, #0]
20000366:	4834      	ldr	r0, [pc, #208]	; (20000438 <common_handler_callback+0x234>)
20000368:	4619      	mov	r1, r3
2000036a:	f000 f9a1 	bl	200006b0 <sysprintf>
2000036e:	68fb      	ldr	r3, [r7, #12]
20000370:	f403 7380 	and.w	r3, r3, #256	; 0x100
20000374:	2b00      	cmp	r3, #0
20000376:	d006      	beq.n	20000386 <common_handler_callback+0x182>
20000378:	687b      	ldr	r3, [r7, #4]
2000037a:	3318      	adds	r3, #24
2000037c:	681b      	ldr	r3, [r3, #0]
2000037e:	482f      	ldr	r0, [pc, #188]	; (2000043c <common_handler_callback+0x238>)
20000380:	4619      	mov	r1, r3
20000382:	f000 f995 	bl	200006b0 <sysprintf>
20000386:	68fb      	ldr	r3, [r7, #12]
20000388:	f403 7300 	and.w	r3, r3, #512	; 0x200
2000038c:	2b00      	cmp	r3, #0
2000038e:	d006      	beq.n	2000039e <common_handler_callback+0x19a>
20000390:	687b      	ldr	r3, [r7, #4]
20000392:	3318      	adds	r3, #24
20000394:	681b      	ldr	r3, [r3, #0]
20000396:	482a      	ldr	r0, [pc, #168]	; (20000440 <common_handler_callback+0x23c>)
20000398:	4619      	mov	r1, r3
2000039a:	f000 f989 	bl	200006b0 <sysprintf>
2000039e:	687b      	ldr	r3, [r7, #4]
200003a0:	3318      	adds	r3, #24
200003a2:	6819      	ldr	r1, [r3, #0]
200003a4:	687b      	ldr	r3, [r7, #4]
200003a6:	3314      	adds	r3, #20
200003a8:	681a      	ldr	r2, [r3, #0]
200003aa:	687b      	ldr	r3, [r7, #4]
200003ac:	331c      	adds	r3, #28
200003ae:	681c      	ldr	r4, [r3, #0]
200003b0:	687b      	ldr	r3, [r7, #4]
200003b2:	3310      	adds	r3, #16
200003b4:	681b      	ldr	r3, [r3, #0]
200003b6:	9300      	str	r3, [sp, #0]
200003b8:	4822      	ldr	r0, [pc, #136]	; (20000444 <common_handler_callback+0x240>)
200003ba:	4623      	mov	r3, r4
200003bc:	f000 f978 	bl	200006b0 <sysprintf>
200003c0:	687b      	ldr	r3, [r7, #4]
200003c2:	6819      	ldr	r1, [r3, #0]
200003c4:	687b      	ldr	r3, [r7, #4]
200003c6:	3304      	adds	r3, #4
200003c8:	681a      	ldr	r2, [r3, #0]
200003ca:	687b      	ldr	r3, [r7, #4]
200003cc:	3308      	adds	r3, #8
200003ce:	681c      	ldr	r4, [r3, #0]
200003d0:	687b      	ldr	r3, [r7, #4]
200003d2:	330c      	adds	r3, #12
200003d4:	681b      	ldr	r3, [r3, #0]
200003d6:	9300      	str	r3, [sp, #0]
200003d8:	481b      	ldr	r0, [pc, #108]	; (20000448 <common_handler_callback+0x244>)
200003da:	4623      	mov	r3, r4
200003dc:	f000 f968 	bl	200006b0 <sysprintf>
200003e0:	481a      	ldr	r0, [pc, #104]	; (2000044c <common_handler_callback+0x248>)
200003e2:	f000 f933 	bl	2000064c <sysprint>
200003e6:	e7fe      	b.n	200003e6 <common_handler_callback+0x1e2>
200003e8:	e000ed28 	.word	0xe000ed28
200003ec:	e000ed38 	.word	0xe000ed38
200003f0:	e000ed34 	.word	0xe000ed34
200003f4:	e000ed04 	.word	0xe000ed04
200003f8:	20008ac4 	.word	0x20008ac4
200003fc:	20008ae8 	.word	0x20008ae8
20000400:	20008b08 	.word	0x20008b08
20000404:	20008b34 	.word	0x20008b34
20000408:	20008b4c 	.word	0x20008b4c
2000040c:	20008b64 	.word	0x20008b64
20000410:	20008b80 	.word	0x20008b80
20000414:	20008b98 	.word	0x20008b98
20000418:	20008bb4 	.word	0x20008bb4
2000041c:	20008be8 	.word	0x20008be8
20000420:	20008c00 	.word	0x20008c00
20000424:	20008c18 	.word	0x20008c18
20000428:	20008c30 	.word	0x20008c30
2000042c:	20008c44 	.word	0x20008c44
20000430:	20008c6c 	.word	0x20008c6c
20000434:	20008c90 	.word	0x20008c90
20000438:	20008cb0 	.word	0x20008cb0
2000043c:	20008cd4 	.word	0x20008cd4
20000440:	20008cfc 	.word	0x20008cfc
20000444:	20008d20 	.word	0x20008d20
20000448:	20008d48 	.word	0x20008d48
2000044c:	20008ac0 	.word	0x20008ac0

20000450 <reset_handler>:
20000450:	f000 f852 	bl	200004f8 <SystemEarlyInit>
20000454:	4a0e      	ldr	r2, [pc, #56]	; (20000490 <reset_handler+0x40>)
20000456:	4b0f      	ldr	r3, [pc, #60]	; (20000494 <reset_handler+0x44>)
20000458:	429a      	cmp	r2, r3
2000045a:	d00b      	beq.n	20000474 <reset_handler+0x24>
2000045c:	4c0d      	ldr	r4, [pc, #52]	; (20000494 <reset_handler+0x44>)
2000045e:	4d0c      	ldr	r5, [pc, #48]	; (20000490 <reset_handler+0x40>)
20000460:	e005      	b.n	2000046e <reset_handler+0x1e>
20000462:	4623      	mov	r3, r4
20000464:	1d1c      	adds	r4, r3, #4
20000466:	462a      	mov	r2, r5
20000468:	1d15      	adds	r5, r2, #4
2000046a:	6812      	ldr	r2, [r2, #0]
2000046c:	601a      	str	r2, [r3, #0]
2000046e:	4b0a      	ldr	r3, [pc, #40]	; (20000498 <reset_handler+0x48>)
20000470:	429c      	cmp	r4, r3
20000472:	d3f6      	bcc.n	20000462 <reset_handler+0x12>
20000474:	4c09      	ldr	r4, [pc, #36]	; (2000049c <reset_handler+0x4c>)
20000476:	e002      	b.n	2000047e <reset_handler+0x2e>
20000478:	2300      	movs	r3, #0
2000047a:	6023      	str	r3, [r4, #0]
2000047c:	3404      	adds	r4, #4
2000047e:	4b08      	ldr	r3, [pc, #32]	; (200004a0 <reset_handler+0x50>)
20000480:	429c      	cmp	r4, r3
20000482:	d3f9      	bcc.n	20000478 <reset_handler+0x28>
20000484:	f000 f83e 	bl	20000504 <SystemInit>
20000488:	f001 fc4a 	bl	20001d20 <main>
2000048c:	e7fe      	b.n	2000048c <reset_handler+0x3c>
2000048e:	bf00      	nop
20000490:	200092a8 	.word	0x200092a8
20000494:	200092a8 	.word	0x200092a8
20000498:	20009b80 	.word	0x20009b80
2000049c:	20009b80 	.word	0x20009b80
200004a0:	20009c64 	.word	0x20009c64

200004a4 <sysdelay>:
200004a4:	b480      	push	{r7}
200004a6:	b085      	sub	sp, #20
200004a8:	af00      	add	r7, sp, #0
200004aa:	6078      	str	r0, [r7, #4]
200004ac:	687b      	ldr	r3, [r7, #4]
200004ae:	60fb      	str	r3, [r7, #12]
200004b0:	e002      	b.n	200004b8 <sysdelay+0x14>
200004b2:	68fb      	ldr	r3, [r7, #12]
200004b4:	3b01      	subs	r3, #1
200004b6:	60fb      	str	r3, [r7, #12]
200004b8:	68fb      	ldr	r3, [r7, #12]
200004ba:	2b00      	cmp	r3, #0
200004bc:	dcf9      	bgt.n	200004b2 <sysdelay+0xe>
200004be:	687b      	ldr	r3, [r7, #4]
200004c0:	60fb      	str	r3, [r7, #12]
200004c2:	e002      	b.n	200004ca <sysdelay+0x26>
200004c4:	68fb      	ldr	r3, [r7, #12]
200004c6:	3b01      	subs	r3, #1
200004c8:	60fb      	str	r3, [r7, #12]
200004ca:	68fb      	ldr	r3, [r7, #12]
200004cc:	2b00      	cmp	r3, #0
200004ce:	dcf9      	bgt.n	200004c4 <sysdelay+0x20>
200004d0:	687b      	ldr	r3, [r7, #4]
200004d2:	60fb      	str	r3, [r7, #12]
200004d4:	e002      	b.n	200004dc <sysdelay+0x38>
200004d6:	68fb      	ldr	r3, [r7, #12]
200004d8:	3b01      	subs	r3, #1
200004da:	60fb      	str	r3, [r7, #12]
200004dc:	68fb      	ldr	r3, [r7, #12]
200004de:	2b00      	cmp	r3, #0
200004e0:	dcf9      	bgt.n	200004d6 <sysdelay+0x32>
200004e2:	3714      	adds	r7, #20
200004e4:	46bd      	mov	sp, r7
200004e6:	f85d 7b04 	ldr.w	r7, [sp], #4
200004ea:	4770      	bx	lr

200004ec <sysdbgwait>:
200004ec:	b580      	push	{r7, lr}
200004ee:	af00      	add	r7, sp, #0
200004f0:	f000 fb24 	bl	20000b3c <wait_for_debugger>
200004f4:	bd80      	pop	{r7, pc}
200004f6:	bf00      	nop

200004f8 <SystemEarlyInit>:
200004f8:	b580      	push	{r7, lr}
200004fa:	af00      	add	r7, sp, #0
200004fc:	f000 f88e 	bl	2000061c <bcm53158_early_init>
20000500:	bd80      	pop	{r7, pc}
20000502:	bf00      	nop

20000504 <SystemInit>:
20000504:	b580      	push	{r7, lr}
20000506:	af00      	add	r7, sp, #0
20000508:	f000 f892 	bl	20000630 <bcm53158_core_init>
2000050c:	bd80      	pop	{r7, pc}
2000050e:	bf00      	nop

20000510 <init_cpu>:
20000510:	b480      	push	{r7}
20000512:	af00      	add	r7, sp, #0
20000514:	4b05      	ldr	r3, [pc, #20]	; (2000052c <init_cpu+0x1c>)
20000516:	4a06      	ldr	r2, [pc, #24]	; (20000530 <init_cpu+0x20>)
20000518:	615a      	str	r2, [r3, #20]
2000051a:	4b04      	ldr	r3, [pc, #16]	; (2000052c <init_cpu+0x1c>)
2000051c:	f44f 22e0 	mov.w	r2, #458752	; 0x70000
20000520:	625a      	str	r2, [r3, #36]	; 0x24
20000522:	46bd      	mov	sp, r7
20000524:	f85d 7b04 	ldr.w	r7, [sp], #4
20000528:	4770      	bx	lr
2000052a:	bf00      	nop
2000052c:	e000ed00 	.word	0xe000ed00
20000530:	00040201 	.word	0x00040201

20000534 <init_mpu>:
20000534:	b580      	push	{r7, lr}
20000536:	b082      	sub	sp, #8
20000538:	af00      	add	r7, sp, #0
2000053a:	2300      	movs	r3, #0
2000053c:	607b      	str	r3, [r7, #4]
2000053e:	2300      	movs	r3, #0
20000540:	603b      	str	r3, [r7, #0]
20000542:	683b      	ldr	r3, [r7, #0]
20000544:	1c5a      	adds	r2, r3, #1
20000546:	603a      	str	r2, [r7, #0]
20000548:	4618      	mov	r0, r3
2000054a:	2100      	movs	r1, #0
2000054c:	f44f 4200 	mov.w	r2, #32768	; 0x8000
20000550:	f04f 7342 	mov.w	r3, #50855936	; 0x3080000
20000554:	f000 fdac 	bl	200010b0 <m7_mpu_set_region>
20000558:	4602      	mov	r2, r0
2000055a:	687b      	ldr	r3, [r7, #4]
2000055c:	4413      	add	r3, r2
2000055e:	607b      	str	r3, [r7, #4]
20000560:	683b      	ldr	r3, [r7, #0]
20000562:	1c5a      	adds	r2, r3, #1
20000564:	603a      	str	r2, [r7, #0]
20000566:	4618      	mov	r0, r3
20000568:	f04f 5180 	mov.w	r1, #268435456	; 0x10000000
2000056c:	f04f 5280 	mov.w	r2, #268435456	; 0x10000000
20000570:	4b11      	ldr	r3, [pc, #68]	; (200005b8 <init_mpu+0x84>)
20000572:	f000 fd9d 	bl	200010b0 <m7_mpu_set_region>
20000576:	4602      	mov	r2, r0
20000578:	687b      	ldr	r3, [r7, #4]
2000057a:	4413      	add	r3, r2
2000057c:	607b      	str	r3, [r7, #4]
2000057e:	683b      	ldr	r3, [r7, #0]
20000580:	1c5a      	adds	r2, r3, #1
20000582:	603a      	str	r2, [r7, #0]
20000584:	4618      	mov	r0, r3
20000586:	f04f 5100 	mov.w	r1, #536870912	; 0x20000000
2000058a:	f44f 3280 	mov.w	r2, #65536	; 0x10000
2000058e:	f04f 7342 	mov.w	r3, #50855936	; 0x3080000
20000592:	f000 fd8d 	bl	200010b0 <m7_mpu_set_region>
20000596:	4602      	mov	r2, r0
20000598:	687b      	ldr	r3, [r7, #4]
2000059a:	4413      	add	r3, r2
2000059c:	607b      	str	r3, [r7, #4]
2000059e:	687b      	ldr	r3, [r7, #4]
200005a0:	2b00      	cmp	r3, #0
200005a2:	d104      	bne.n	200005ae <init_mpu+0x7a>
200005a4:	f000 fdea 	bl	2000117c <m7_mpu_enable>
200005a8:	4804      	ldr	r0, [pc, #16]	; (200005bc <init_mpu+0x88>)
200005aa:	f000 f84f 	bl	2000064c <sysprint>
200005ae:	687b      	ldr	r3, [r7, #4]
200005b0:	4618      	mov	r0, r3
200005b2:	3708      	adds	r7, #8
200005b4:	46bd      	mov	sp, r7
200005b6:	bd80      	pop	{r7, pc}
200005b8:	03020000 	.word	0x03020000
200005bc:	20008d70 	.word	0x20008d70

200005c0 <init_tcm>:
200005c0:	b580      	push	{r7, lr}
200005c2:	af00      	add	r7, sp, #0
200005c4:	f000 fcce 	bl	20000f64 <m7_tcm_enable>
200005c8:	bd80      	pop	{r7, pc}
200005ca:	bf00      	nop

200005cc <init_irq>:
200005cc:	b580      	push	{r7, lr}
200005ce:	b082      	sub	sp, #8
200005d0:	af00      	add	r7, sp, #0
200005d2:	2300      	movs	r3, #0
200005d4:	71fb      	strb	r3, [r7, #7]
200005d6:	e00a      	b.n	200005ee <init_irq+0x22>
200005d8:	f997 3007 	ldrsb.w	r3, [r7, #7]
200005dc:	4618      	mov	r0, r3
200005de:	2104      	movs	r1, #4
200005e0:	f000 fd42 	bl	20001068 <m7_set_irq_priority>
200005e4:	79fb      	ldrb	r3, [r7, #7]
200005e6:	b2db      	uxtb	r3, r3
200005e8:	3301      	adds	r3, #1
200005ea:	b2db      	uxtb	r3, r3
200005ec:	71fb      	strb	r3, [r7, #7]
200005ee:	f997 3007 	ldrsb.w	r3, [r7, #7]
200005f2:	2b3d      	cmp	r3, #61	; 0x3d
200005f4:	ddf0      	ble.n	200005d8 <init_irq+0xc>
200005f6:	2000      	movs	r0, #0
200005f8:	f000 fd28 	bl	2000104c <m7_set_irq_grouping>
200005fc:	3708      	adds	r7, #8
200005fe:	46bd      	mov	sp, r7
20000600:	bd80      	pop	{r7, pc}
20000602:	bf00      	nop

20000604 <init_caches>:
20000604:	b580      	push	{r7, lr}
20000606:	af00      	add	r7, sp, #0
20000608:	f000 fe6c 	bl	200012e4 <m7_dcache_invalidate_all>
2000060c:	f000 fe7e 	bl	2000130c <m7_dcache_enable>
20000610:	f000 fe72 	bl	200012f8 <m7_icache_invalidate_all>
20000614:	f000 fe8e 	bl	20001334 <m7_icache_enable>
20000618:	bd80      	pop	{r7, pc}
2000061a:	bf00      	nop

2000061c <bcm53158_early_init>:
2000061c:	b580      	push	{r7, lr}
2000061e:	af00      	add	r7, sp, #0
20000620:	f7ff ff76 	bl	20000510 <init_cpu>
20000624:	f7ff ffcc 	bl	200005c0 <init_tcm>
20000628:	2300      	movs	r3, #0
2000062a:	4618      	mov	r0, r3
2000062c:	bd80      	pop	{r7, pc}
2000062e:	bf00      	nop

20000630 <bcm53158_core_init>:
20000630:	b580      	push	{r7, lr}
20000632:	b082      	sub	sp, #8
20000634:	af00      	add	r7, sp, #0
20000636:	2300      	movs	r3, #0
20000638:	607b      	str	r3, [r7, #4]
2000063a:	f7ff ffe3 	bl	20000604 <init_caches>
2000063e:	f7ff ffc5 	bl	200005cc <init_irq>
20000642:	2300      	movs	r3, #0
20000644:	4618      	mov	r0, r3
20000646:	3708      	adds	r7, #8
20000648:	46bd      	mov	sp, r7
2000064a:	bd80      	pop	{r7, pc}

2000064c <sysprint>:
2000064c:	b480      	push	{r7}
2000064e:	b085      	sub	sp, #20
20000650:	af00      	add	r7, sp, #0
20000652:	6078      	str	r0, [r7, #4]
20000654:	4b14      	ldr	r3, [pc, #80]	; (200006a8 <sysprint+0x5c>)
20000656:	60fb      	str	r3, [r7, #12]
20000658:	4b14      	ldr	r3, [pc, #80]	; (200006ac <sysprint+0x60>)
2000065a:	60bb      	str	r3, [r7, #8]
2000065c:	e01b      	b.n	20000696 <sysprint+0x4a>
2000065e:	bf00      	nop
20000660:	68fb      	ldr	r3, [r7, #12]
20000662:	681b      	ldr	r3, [r3, #0]
20000664:	f003 0320 	and.w	r3, r3, #32
20000668:	2b00      	cmp	r3, #0
2000066a:	d0f9      	beq.n	20000660 <sysprint+0x14>
2000066c:	687b      	ldr	r3, [r7, #4]
2000066e:	781b      	ldrb	r3, [r3, #0]
20000670:	2b0a      	cmp	r3, #10
20000672:	d109      	bne.n	20000688 <sysprint+0x3c>
20000674:	68bb      	ldr	r3, [r7, #8]
20000676:	220d      	movs	r2, #13
20000678:	601a      	str	r2, [r3, #0]
2000067a:	bf00      	nop
2000067c:	68fb      	ldr	r3, [r7, #12]
2000067e:	681b      	ldr	r3, [r3, #0]
20000680:	f003 0320 	and.w	r3, r3, #32
20000684:	2b00      	cmp	r3, #0
20000686:	d0f9      	beq.n	2000067c <sysprint+0x30>
20000688:	687b      	ldr	r3, [r7, #4]
2000068a:	1c5a      	adds	r2, r3, #1
2000068c:	607a      	str	r2, [r7, #4]
2000068e:	781b      	ldrb	r3, [r3, #0]
20000690:	461a      	mov	r2, r3
20000692:	68bb      	ldr	r3, [r7, #8]
20000694:	601a      	str	r2, [r3, #0]
20000696:	687b      	ldr	r3, [r7, #4]
20000698:	781b      	ldrb	r3, [r3, #0]
2000069a:	2b00      	cmp	r3, #0
2000069c:	d1df      	bne.n	2000065e <sysprint+0x12>
2000069e:	3714      	adds	r7, #20
200006a0:	46bd      	mov	sp, r7
200006a2:	f85d 7b04 	ldr.w	r7, [sp], #4
200006a6:	4770      	bx	lr
200006a8:	41027014 	.word	0x41027014
200006ac:	41027000 	.word	0x41027000

200006b0 <sysprintf>:
200006b0:	b40f      	push	{r0, r1, r2, r3}
200006b2:	b580      	push	{r7, lr}
200006b4:	b0c8      	sub	sp, #288	; 0x120
200006b6:	af02      	add	r7, sp, #8
200006b8:	2300      	movs	r3, #0
200006ba:	f8c7 310c 	str.w	r3, [r7, #268]	; 0x10c
200006be:	2320      	movs	r3, #32
200006c0:	f887 310b 	strb.w	r3, [r7, #267]	; 0x10b
200006c4:	2300      	movs	r3, #0
200006c6:	f8c7 3104 	str.w	r3, [r7, #260]	; 0x104
200006ca:	e00a      	b.n	200006e2 <sysprintf+0x32>
200006cc:	463a      	mov	r2, r7
200006ce:	f8d7 3104 	ldr.w	r3, [r7, #260]	; 0x104
200006d2:	4413      	add	r3, r2
200006d4:	2200      	movs	r2, #0
200006d6:	701a      	strb	r2, [r3, #0]
200006d8:	f8d7 3104 	ldr.w	r3, [r7, #260]	; 0x104
200006dc:	3301      	adds	r3, #1
200006de:	f8c7 3104 	str.w	r3, [r7, #260]	; 0x104
200006e2:	f8d7 3104 	ldr.w	r3, [r7, #260]	; 0x104
200006e6:	2bff      	cmp	r3, #255	; 0xff
200006e8:	ddf0      	ble.n	200006cc <sysprintf+0x1c>
200006ea:	f507 7392 	add.w	r3, r7, #292	; 0x124
200006ee:	f8c7 3100 	str.w	r3, [r7, #256]	; 0x100
200006f2:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200006f6:	1c5a      	adds	r2, r3, #1
200006f8:	f8c7 2120 	str.w	r2, [r7, #288]	; 0x120
200006fc:	781b      	ldrb	r3, [r3, #0]
200006fe:	f887 3117 	strb.w	r3, [r7, #279]	; 0x117
20000702:	e0f5      	b.n	200008f0 <sysprintf+0x240>
20000704:	f897 3117 	ldrb.w	r3, [r7, #279]	; 0x117
20000708:	2b25      	cmp	r3, #37	; 0x25
2000070a:	d009      	beq.n	20000720 <sysprintf+0x70>
2000070c:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
20000710:	1c5a      	adds	r2, r3, #1
20000712:	f8c7 210c 	str.w	r2, [r7, #268]	; 0x10c
20000716:	463a      	mov	r2, r7
20000718:	f897 1117 	ldrb.w	r1, [r7, #279]	; 0x117
2000071c:	54d1      	strb	r1, [r2, r3]
2000071e:	e0d6      	b.n	200008ce <sysprintf+0x21e>
20000720:	2300      	movs	r3, #0
20000722:	f8c7 3110 	str.w	r3, [r7, #272]	; 0x110
20000726:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
2000072a:	1c5a      	adds	r2, r3, #1
2000072c:	f8c7 2120 	str.w	r2, [r7, #288]	; 0x120
20000730:	781b      	ldrb	r3, [r3, #0]
20000732:	f887 3117 	strb.w	r3, [r7, #279]	; 0x117
20000736:	e024      	b.n	20000782 <sysprintf+0xd2>
20000738:	f897 3117 	ldrb.w	r3, [r7, #279]	; 0x117
2000073c:	2b30      	cmp	r3, #48	; 0x30
2000073e:	d103      	bne.n	20000748 <sysprintf+0x98>
20000740:	2330      	movs	r3, #48	; 0x30
20000742:	f887 310b 	strb.w	r3, [r7, #267]	; 0x10b
20000746:	e014      	b.n	20000772 <sysprintf+0xc2>
20000748:	f8d7 2110 	ldr.w	r2, [r7, #272]	; 0x110
2000074c:	4613      	mov	r3, r2
2000074e:	009b      	lsls	r3, r3, #2
20000750:	4413      	add	r3, r2
20000752:	005b      	lsls	r3, r3, #1
20000754:	461a      	mov	r2, r3
20000756:	f897 3117 	ldrb.w	r3, [r7, #279]	; 0x117
2000075a:	3b30      	subs	r3, #48	; 0x30
2000075c:	4413      	add	r3, r2
2000075e:	f8c7 3110 	str.w	r3, [r7, #272]	; 0x110
20000762:	f8d7 3110 	ldr.w	r3, [r7, #272]	; 0x110
20000766:	2b4f      	cmp	r3, #79	; 0x4f
20000768:	dd03      	ble.n	20000772 <sysprintf+0xc2>
2000076a:	486c      	ldr	r0, [pc, #432]	; (2000091c <sysprintf+0x26c>)
2000076c:	f7ff ff6e 	bl	2000064c <sysprint>
20000770:	e0cd      	b.n	2000090e <sysprintf+0x25e>
20000772:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
20000776:	1c5a      	adds	r2, r3, #1
20000778:	f8c7 2120 	str.w	r2, [r7, #288]	; 0x120
2000077c:	781b      	ldrb	r3, [r3, #0]
2000077e:	f887 3117 	strb.w	r3, [r7, #279]	; 0x117
20000782:	f897 3117 	ldrb.w	r3, [r7, #279]	; 0x117
20000786:	2b2f      	cmp	r3, #47	; 0x2f
20000788:	d903      	bls.n	20000792 <sysprintf+0xe2>
2000078a:	f897 3117 	ldrb.w	r3, [r7, #279]	; 0x117
2000078e:	2b38      	cmp	r3, #56	; 0x38
20000790:	d9d2      	bls.n	20000738 <sysprintf+0x88>
20000792:	f897 3117 	ldrb.w	r3, [r7, #279]	; 0x117
20000796:	3b58      	subs	r3, #88	; 0x58
20000798:	2b20      	cmp	r3, #32
2000079a:	f200 8094 	bhi.w	200008c6 <sysprintf+0x216>
2000079e:	a201      	add	r2, pc, #4	; (adr r2, 200007a4 <sysprintf+0xf4>)
200007a0:	f852 f023 	ldr.w	pc, [r2, r3, lsl #2]
200007a4:	20000829 	.word	0x20000829
200007a8:	200008c7 	.word	0x200008c7
200007ac:	200008c7 	.word	0x200008c7
200007b0:	200008c7 	.word	0x200008c7
200007b4:	200008c7 	.word	0x200008c7
200007b8:	200008c7 	.word	0x200008c7
200007bc:	200008c7 	.word	0x200008c7
200007c0:	200008c7 	.word	0x200008c7
200007c4:	200008c7 	.word	0x200008c7
200007c8:	200008c7 	.word	0x200008c7
200007cc:	200008c7 	.word	0x200008c7
200007d0:	200008c7 	.word	0x200008c7
200007d4:	20000891 	.word	0x20000891
200007d8:	200008c7 	.word	0x200008c7
200007dc:	200008c7 	.word	0x200008c7
200007e0:	200008c7 	.word	0x200008c7
200007e4:	200008c7 	.word	0x200008c7
200007e8:	200008c7 	.word	0x200008c7
200007ec:	200008c7 	.word	0x200008c7
200007f0:	200008c7 	.word	0x200008c7
200007f4:	200008c7 	.word	0x200008c7
200007f8:	200008c7 	.word	0x200008c7
200007fc:	200008c7 	.word	0x200008c7
20000800:	200008c7 	.word	0x200008c7
20000804:	200008c7 	.word	0x200008c7
20000808:	200008c7 	.word	0x200008c7
2000080c:	200008c7 	.word	0x200008c7
20000810:	200008c7 	.word	0x200008c7
20000814:	200008c7 	.word	0x200008c7
20000818:	2000085d 	.word	0x2000085d
2000081c:	200008c7 	.word	0x200008c7
20000820:	200008c7 	.word	0x200008c7
20000824:	20000829 	.word	0x20000829
20000828:	463a      	mov	r2, r7
2000082a:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
2000082e:	18d1      	adds	r1, r2, r3
20000830:	f8d7 3100 	ldr.w	r3, [r7, #256]	; 0x100
20000834:	1d1a      	adds	r2, r3, #4
20000836:	f8c7 2100 	str.w	r2, [r7, #256]	; 0x100
2000083a:	681a      	ldr	r2, [r3, #0]
2000083c:	f897 310b 	ldrb.w	r3, [r7, #267]	; 0x10b
20000840:	9300      	str	r3, [sp, #0]
20000842:	4608      	mov	r0, r1
20000844:	2100      	movs	r1, #0
20000846:	f8d7 3110 	ldr.w	r3, [r7, #272]	; 0x110
2000084a:	f000 f8d3 	bl	200009f4 <int2str>
2000084e:	4602      	mov	r2, r0
20000850:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
20000854:	4413      	add	r3, r2
20000856:	f8c7 310c 	str.w	r3, [r7, #268]	; 0x10c
2000085a:	e038      	b.n	200008ce <sysprintf+0x21e>
2000085c:	463a      	mov	r2, r7
2000085e:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
20000862:	18d1      	adds	r1, r2, r3
20000864:	f8d7 3100 	ldr.w	r3, [r7, #256]	; 0x100
20000868:	1d1a      	adds	r2, r3, #4
2000086a:	f8c7 2100 	str.w	r2, [r7, #256]	; 0x100
2000086e:	681a      	ldr	r2, [r3, #0]
20000870:	f897 310b 	ldrb.w	r3, [r7, #267]	; 0x10b
20000874:	9300      	str	r3, [sp, #0]
20000876:	4608      	mov	r0, r1
20000878:	2101      	movs	r1, #1
2000087a:	f8d7 3110 	ldr.w	r3, [r7, #272]	; 0x110
2000087e:	f000 f8b9 	bl	200009f4 <int2str>
20000882:	4602      	mov	r2, r0
20000884:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
20000888:	4413      	add	r3, r2
2000088a:	f8c7 310c 	str.w	r3, [r7, #268]	; 0x10c
2000088e:	e01e      	b.n	200008ce <sysprintf+0x21e>
20000890:	463a      	mov	r2, r7
20000892:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
20000896:	18d1      	adds	r1, r2, r3
20000898:	f8d7 3100 	ldr.w	r3, [r7, #256]	; 0x100
2000089c:	1d1a      	adds	r2, r3, #4
2000089e:	f8c7 2100 	str.w	r2, [r7, #256]	; 0x100
200008a2:	681b      	ldr	r3, [r3, #0]
200008a4:	461a      	mov	r2, r3
200008a6:	f897 310b 	ldrb.w	r3, [r7, #267]	; 0x10b
200008aa:	9300      	str	r3, [sp, #0]
200008ac:	4608      	mov	r0, r1
200008ae:	2102      	movs	r1, #2
200008b0:	f8d7 3110 	ldr.w	r3, [r7, #272]	; 0x110
200008b4:	f000 f89e 	bl	200009f4 <int2str>
200008b8:	4602      	mov	r2, r0
200008ba:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
200008be:	4413      	add	r3, r2
200008c0:	f8c7 310c 	str.w	r3, [r7, #268]	; 0x10c
200008c4:	e003      	b.n	200008ce <sysprintf+0x21e>
200008c6:	4816      	ldr	r0, [pc, #88]	; (20000920 <sysprintf+0x270>)
200008c8:	f7ff fec0 	bl	2000064c <sysprint>
200008cc:	e01f      	b.n	2000090e <sysprintf+0x25e>
200008ce:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
200008d2:	2bfe      	cmp	r3, #254	; 0xfe
200008d4:	dd04      	ble.n	200008e0 <sysprintf+0x230>
200008d6:	463b      	mov	r3, r7
200008d8:	2200      	movs	r2, #0
200008da:	f883 20ff 	strb.w	r2, [r3, #255]	; 0xff
200008de:	e00c      	b.n	200008fa <sysprintf+0x24a>
200008e0:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200008e4:	1c5a      	adds	r2, r3, #1
200008e6:	f8c7 2120 	str.w	r2, [r7, #288]	; 0x120
200008ea:	781b      	ldrb	r3, [r3, #0]
200008ec:	f887 3117 	strb.w	r3, [r7, #279]	; 0x117
200008f0:	f897 3117 	ldrb.w	r3, [r7, #279]	; 0x117
200008f4:	2b00      	cmp	r3, #0
200008f6:	f47f af05 	bne.w	20000704 <sysprintf+0x54>
200008fa:	463a      	mov	r2, r7
200008fc:	f8d7 310c 	ldr.w	r3, [r7, #268]	; 0x10c
20000900:	4413      	add	r3, r2
20000902:	2200      	movs	r2, #0
20000904:	701a      	strb	r2, [r3, #0]
20000906:	463b      	mov	r3, r7
20000908:	4618      	mov	r0, r3
2000090a:	f7ff fe9f 	bl	2000064c <sysprint>
2000090e:	f507 778c 	add.w	r7, r7, #280	; 0x118
20000912:	46bd      	mov	sp, r7
20000914:	e8bd 4080 	ldmia.w	sp!, {r7, lr}
20000918:	b004      	add	sp, #16
2000091a:	4770      	bx	lr
2000091c:	20008d80 	.word	0x20008d80
20000920:	20008d9c 	.word	0x20008d9c

20000924 <sysputchar>:
20000924:	b480      	push	{r7}
20000926:	b085      	sub	sp, #20
20000928:	af00      	add	r7, sp, #0
2000092a:	4603      	mov	r3, r0
2000092c:	71fb      	strb	r3, [r7, #7]
2000092e:	4b09      	ldr	r3, [pc, #36]	; (20000954 <sysputchar+0x30>)
20000930:	60fb      	str	r3, [r7, #12]
20000932:	4b09      	ldr	r3, [pc, #36]	; (20000958 <sysputchar+0x34>)
20000934:	60bb      	str	r3, [r7, #8]
20000936:	bf00      	nop
20000938:	68fb      	ldr	r3, [r7, #12]
2000093a:	681b      	ldr	r3, [r3, #0]
2000093c:	f003 0320 	and.w	r3, r3, #32
20000940:	2b00      	cmp	r3, #0
20000942:	d0f9      	beq.n	20000938 <sysputchar+0x14>
20000944:	79fa      	ldrb	r2, [r7, #7]
20000946:	68bb      	ldr	r3, [r7, #8]
20000948:	601a      	str	r2, [r3, #0]
2000094a:	3714      	adds	r7, #20
2000094c:	46bd      	mov	sp, r7
2000094e:	f85d 7b04 	ldr.w	r7, [sp], #4
20000952:	4770      	bx	lr
20000954:	41027014 	.word	0x41027014
20000958:	41027000 	.word	0x41027000

2000095c <sysgetchar>:
2000095c:	b480      	push	{r7}
2000095e:	b083      	sub	sp, #12
20000960:	af00      	add	r7, sp, #0
20000962:	4b0a      	ldr	r3, [pc, #40]	; (2000098c <sysgetchar+0x30>)
20000964:	607b      	str	r3, [r7, #4]
20000966:	4b0a      	ldr	r3, [pc, #40]	; (20000990 <sysgetchar+0x34>)
20000968:	603b      	str	r3, [r7, #0]
2000096a:	bf00      	nop
2000096c:	687b      	ldr	r3, [r7, #4]
2000096e:	681b      	ldr	r3, [r3, #0]
20000970:	f003 0301 	and.w	r3, r3, #1
20000974:	2b00      	cmp	r3, #0
20000976:	d0f9      	beq.n	2000096c <sysgetchar+0x10>
20000978:	683b      	ldr	r3, [r7, #0]
2000097a:	681b      	ldr	r3, [r3, #0]
2000097c:	b2db      	uxtb	r3, r3
2000097e:	4618      	mov	r0, r3
20000980:	370c      	adds	r7, #12
20000982:	46bd      	mov	sp, r7
20000984:	f85d 7b04 	ldr.w	r7, [sp], #4
20000988:	4770      	bx	lr
2000098a:	bf00      	nop
2000098c:	41027014 	.word	0x41027014
20000990:	41027000 	.word	0x41027000

20000994 <syskbhit>:
20000994:	b480      	push	{r7}
20000996:	b083      	sub	sp, #12
20000998:	af00      	add	r7, sp, #0
2000099a:	4b08      	ldr	r3, [pc, #32]	; (200009bc <syskbhit+0x28>)
2000099c:	607b      	str	r3, [r7, #4]
2000099e:	687b      	ldr	r3, [r7, #4]
200009a0:	681b      	ldr	r3, [r3, #0]
200009a2:	f003 0301 	and.w	r3, r3, #1
200009a6:	2b00      	cmp	r3, #0
200009a8:	bf14      	ite	ne
200009aa:	2301      	movne	r3, #1
200009ac:	2300      	moveq	r3, #0
200009ae:	b2db      	uxtb	r3, r3
200009b0:	4618      	mov	r0, r3
200009b2:	370c      	adds	r7, #12
200009b4:	46bd      	mov	sp, r7
200009b6:	f85d 7b04 	ldr.w	r7, [sp], #4
200009ba:	4770      	bx	lr
200009bc:	41027014 	.word	0x41027014

200009c0 <sysassert>:
200009c0:	b580      	push	{r7, lr}
200009c2:	b0a6      	sub	sp, #152	; 0x98
200009c4:	af02      	add	r7, sp, #8
200009c6:	60f8      	str	r0, [r7, #12]
200009c8:	60b9      	str	r1, [r7, #8]
200009ca:	607a      	str	r2, [r7, #4]
200009cc:	f107 0210 	add.w	r2, r7, #16
200009d0:	687b      	ldr	r3, [r7, #4]
200009d2:	9300      	str	r3, [sp, #0]
200009d4:	4610      	mov	r0, r2
200009d6:	4906      	ldr	r1, [pc, #24]	; (200009f0 <sysassert+0x30>)
200009d8:	68fa      	ldr	r2, [r7, #12]
200009da:	68bb      	ldr	r3, [r7, #8]
200009dc:	f002 f9a6 	bl	20002d2c <sprintf>
200009e0:	f107 0310 	add.w	r3, r7, #16
200009e4:	4618      	mov	r0, r3
200009e6:	f7ff fe31 	bl	2000064c <sysprint>
200009ea:	3790      	adds	r7, #144	; 0x90
200009ec:	46bd      	mov	sp, r7
200009ee:	bd80      	pop	{r7, pc}
200009f0:	20008db4 	.word	0x20008db4

200009f4 <int2str>:
200009f4:	b490      	push	{r4, r7}
200009f6:	b092      	sub	sp, #72	; 0x48
200009f8:	af00      	add	r7, sp, #0
200009fa:	60f8      	str	r0, [r7, #12]
200009fc:	60b9      	str	r1, [r7, #8]
200009fe:	607a      	str	r2, [r7, #4]
20000a00:	603b      	str	r3, [r7, #0]
20000a02:	4a4c      	ldr	r2, [pc, #304]	; (20000b34 <int2str+0x140>)
20000a04:	f107 0320 	add.w	r3, r7, #32
20000a08:	6810      	ldr	r0, [r2, #0]
20000a0a:	6851      	ldr	r1, [r2, #4]
20000a0c:	c303      	stmia	r3!, {r0, r1}
20000a0e:	8912      	ldrh	r2, [r2, #8]
20000a10:	801a      	strh	r2, [r3, #0]
20000a12:	4b49      	ldr	r3, [pc, #292]	; (20000b38 <int2str+0x144>)
20000a14:	f107 0410 	add.w	r4, r7, #16
20000a18:	6818      	ldr	r0, [r3, #0]
20000a1a:	6859      	ldr	r1, [r3, #4]
20000a1c:	689a      	ldr	r2, [r3, #8]
20000a1e:	68db      	ldr	r3, [r3, #12]
20000a20:	c40f      	stmia	r4!, {r0, r1, r2, r3}
20000a22:	68fb      	ldr	r3, [r7, #12]
20000a24:	647b      	str	r3, [r7, #68]	; 0x44
20000a26:	68bb      	ldr	r3, [r7, #8]
20000a28:	2b00      	cmp	r3, #0
20000a2a:	d101      	bne.n	20000a30 <int2str+0x3c>
20000a2c:	2310      	movs	r3, #16
20000a2e:	e000      	b.n	20000a32 <int2str+0x3e>
20000a30:	230a      	movs	r3, #10
20000a32:	633b      	str	r3, [r7, #48]	; 0x30
20000a34:	2300      	movs	r3, #0
20000a36:	63fb      	str	r3, [r7, #60]	; 0x3c
20000a38:	2300      	movs	r3, #0
20000a3a:	63bb      	str	r3, [r7, #56]	; 0x38
20000a3c:	687b      	ldr	r3, [r7, #4]
20000a3e:	637b      	str	r3, [r7, #52]	; 0x34
20000a40:	687b      	ldr	r3, [r7, #4]
20000a42:	62fb      	str	r3, [r7, #44]	; 0x2c
20000a44:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20000a46:	2b00      	cmp	r3, #0
20000a48:	d06e      	beq.n	20000b28 <int2str+0x134>
20000a4a:	68bb      	ldr	r3, [r7, #8]
20000a4c:	2b02      	cmp	r3, #2
20000a4e:	d10a      	bne.n	20000a66 <int2str+0x72>
20000a50:	6afb      	ldr	r3, [r7, #44]	; 0x2c
20000a52:	2b00      	cmp	r3, #0
20000a54:	da07      	bge.n	20000a66 <int2str+0x72>
20000a56:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20000a58:	1c5a      	adds	r2, r3, #1
20000a5a:	647a      	str	r2, [r7, #68]	; 0x44
20000a5c:	222d      	movs	r2, #45	; 0x2d
20000a5e:	701a      	strb	r2, [r3, #0]
20000a60:	6afb      	ldr	r3, [r7, #44]	; 0x2c
20000a62:	425b      	negs	r3, r3
20000a64:	637b      	str	r3, [r7, #52]	; 0x34
20000a66:	e007      	b.n	20000a78 <int2str+0x84>
20000a68:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
20000a6a:	3301      	adds	r3, #1
20000a6c:	63fb      	str	r3, [r7, #60]	; 0x3c
20000a6e:	6b3b      	ldr	r3, [r7, #48]	; 0x30
20000a70:	6b7a      	ldr	r2, [r7, #52]	; 0x34
20000a72:	fbb2 f3f3 	udiv	r3, r2, r3
20000a76:	637b      	str	r3, [r7, #52]	; 0x34
20000a78:	6b7b      	ldr	r3, [r7, #52]	; 0x34
20000a7a:	2b00      	cmp	r3, #0
20000a7c:	d1f4      	bne.n	20000a68 <int2str+0x74>
20000a7e:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
20000a80:	2b00      	cmp	r3, #0
20000a82:	d102      	bne.n	20000a8a <int2str+0x96>
20000a84:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
20000a86:	3301      	adds	r3, #1
20000a88:	63fb      	str	r3, [r7, #60]	; 0x3c
20000a8a:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
20000a8c:	683b      	ldr	r3, [r7, #0]
20000a8e:	429a      	cmp	r2, r3
20000a90:	da0f      	bge.n	20000ab2 <int2str+0xbe>
20000a92:	683a      	ldr	r2, [r7, #0]
20000a94:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
20000a96:	1ad3      	subs	r3, r2, r3
20000a98:	63bb      	str	r3, [r7, #56]	; 0x38
20000a9a:	e005      	b.n	20000aa8 <int2str+0xb4>
20000a9c:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20000a9e:	1c5a      	adds	r2, r3, #1
20000aa0:	647a      	str	r2, [r7, #68]	; 0x44
20000aa2:	f897 2050 	ldrb.w	r2, [r7, #80]	; 0x50
20000aa6:	701a      	strb	r2, [r3, #0]
20000aa8:	6bbb      	ldr	r3, [r7, #56]	; 0x38
20000aaa:	1e5a      	subs	r2, r3, #1
20000aac:	63ba      	str	r2, [r7, #56]	; 0x38
20000aae:	2b00      	cmp	r3, #0
20000ab0:	d1f4      	bne.n	20000a9c <int2str+0xa8>
20000ab2:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
20000ab4:	3b01      	subs	r3, #1
20000ab6:	643b      	str	r3, [r7, #64]	; 0x40
20000ab8:	e02b      	b.n	20000b12 <int2str+0x11e>
20000aba:	68bb      	ldr	r3, [r7, #8]
20000abc:	2b00      	cmp	r3, #0
20000abe:	d010      	beq.n	20000ae2 <int2str+0xee>
20000ac0:	6c3b      	ldr	r3, [r7, #64]	; 0x40
20000ac2:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20000ac4:	18d1      	adds	r1, r2, r3
20000ac6:	6b3a      	ldr	r2, [r7, #48]	; 0x30
20000ac8:	687b      	ldr	r3, [r7, #4]
20000aca:	fbb3 f0f2 	udiv	r0, r3, r2
20000ace:	fb02 f200 	mul.w	r2, r2, r0
20000ad2:	1a9b      	subs	r3, r3, r2
20000ad4:	f107 0248 	add.w	r2, r7, #72	; 0x48
20000ad8:	4413      	add	r3, r2
20000ada:	f813 3c28 	ldrb.w	r3, [r3, #-40]
20000ade:	700b      	strb	r3, [r1, #0]
20000ae0:	e00f      	b.n	20000b02 <int2str+0x10e>
20000ae2:	6c3b      	ldr	r3, [r7, #64]	; 0x40
20000ae4:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20000ae6:	18d1      	adds	r1, r2, r3
20000ae8:	6b3a      	ldr	r2, [r7, #48]	; 0x30
20000aea:	687b      	ldr	r3, [r7, #4]
20000aec:	fbb3 f0f2 	udiv	r0, r3, r2
20000af0:	fb02 f200 	mul.w	r2, r2, r0
20000af4:	1a9b      	subs	r3, r3, r2
20000af6:	f107 0248 	add.w	r2, r7, #72	; 0x48
20000afa:	4413      	add	r3, r2
20000afc:	f813 3c38 	ldrb.w	r3, [r3, #-56]
20000b00:	700b      	strb	r3, [r1, #0]
20000b02:	6b3b      	ldr	r3, [r7, #48]	; 0x30
20000b04:	687a      	ldr	r2, [r7, #4]
20000b06:	fbb2 f3f3 	udiv	r3, r2, r3
20000b0a:	607b      	str	r3, [r7, #4]
20000b0c:	6c3b      	ldr	r3, [r7, #64]	; 0x40
20000b0e:	3b01      	subs	r3, #1
20000b10:	643b      	str	r3, [r7, #64]	; 0x40
20000b12:	6c3b      	ldr	r3, [r7, #64]	; 0x40
20000b14:	2b00      	cmp	r3, #0
20000b16:	dad0      	bge.n	20000aba <int2str+0xc6>
20000b18:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
20000b1a:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20000b1c:	4413      	add	r3, r2
20000b1e:	647b      	str	r3, [r7, #68]	; 0x44
20000b20:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20000b22:	68fb      	ldr	r3, [r7, #12]
20000b24:	1ad3      	subs	r3, r2, r3
20000b26:	e000      	b.n	20000b2a <int2str+0x136>
20000b28:	2300      	movs	r3, #0
20000b2a:	4618      	mov	r0, r3
20000b2c:	3748      	adds	r7, #72	; 0x48
20000b2e:	46bd      	mov	sp, r7
20000b30:	bc90      	pop	{r4, r7}
20000b32:	4770      	bx	lr
20000b34:	20008de4 	.word	0x20008de4
20000b38:	20008df0 	.word	0x20008df0

20000b3c <wait_for_debugger>:
20000b3c:	b580      	push	{r7, lr}
20000b3e:	b082      	sub	sp, #8
20000b40:	af00      	add	r7, sp, #0
20000b42:	4b08      	ldr	r3, [pc, #32]	; (20000b64 <wait_for_debugger+0x28>)
20000b44:	607b      	str	r3, [r7, #4]
20000b46:	4808      	ldr	r0, [pc, #32]	; (20000b68 <wait_for_debugger+0x2c>)
20000b48:	f7ff fd80 	bl	2000064c <sysprint>
20000b4c:	687b      	ldr	r3, [r7, #4]
20000b4e:	2200      	movs	r2, #0
20000b50:	601a      	str	r2, [r3, #0]
20000b52:	bf00      	nop
20000b54:	687b      	ldr	r3, [r7, #4]
20000b56:	681b      	ldr	r3, [r3, #0]
20000b58:	2b00      	cmp	r3, #0
20000b5a:	d0fb      	beq.n	20000b54 <wait_for_debugger+0x18>
20000b5c:	3708      	adds	r7, #8
20000b5e:	46bd      	mov	sp, r7
20000b60:	bd80      	pop	{r7, pc}
20000b62:	bf00      	nop
20000b64:	4102701c 	.word	0x4102701c
20000b68:	20008e00 	.word	0x20008e00

20000b6c <NVIC_SetPriorityGrouping>:
20000b6c:	b480      	push	{r7}
20000b6e:	b085      	sub	sp, #20
20000b70:	af00      	add	r7, sp, #0
20000b72:	6078      	str	r0, [r7, #4]
20000b74:	687b      	ldr	r3, [r7, #4]
20000b76:	f003 0307 	and.w	r3, r3, #7
20000b7a:	60fb      	str	r3, [r7, #12]
20000b7c:	4b0b      	ldr	r3, [pc, #44]	; (20000bac <NVIC_SetPriorityGrouping+0x40>)
20000b7e:	68db      	ldr	r3, [r3, #12]
20000b80:	60bb      	str	r3, [r7, #8]
20000b82:	68ba      	ldr	r2, [r7, #8]
20000b84:	f64f 03ff 	movw	r3, #63743	; 0xf8ff
20000b88:	4013      	ands	r3, r2
20000b8a:	60bb      	str	r3, [r7, #8]
20000b8c:	68fb      	ldr	r3, [r7, #12]
20000b8e:	021a      	lsls	r2, r3, #8
20000b90:	68bb      	ldr	r3, [r7, #8]
20000b92:	431a      	orrs	r2, r3
20000b94:	4b06      	ldr	r3, [pc, #24]	; (20000bb0 <NVIC_SetPriorityGrouping+0x44>)
20000b96:	4313      	orrs	r3, r2
20000b98:	60bb      	str	r3, [r7, #8]
20000b9a:	4a04      	ldr	r2, [pc, #16]	; (20000bac <NVIC_SetPriorityGrouping+0x40>)
20000b9c:	68bb      	ldr	r3, [r7, #8]
20000b9e:	60d3      	str	r3, [r2, #12]
20000ba0:	3714      	adds	r7, #20
20000ba2:	46bd      	mov	sp, r7
20000ba4:	f85d 7b04 	ldr.w	r7, [sp], #4
20000ba8:	4770      	bx	lr
20000baa:	bf00      	nop
20000bac:	e000ed00 	.word	0xe000ed00
20000bb0:	05fa0000 	.word	0x05fa0000

20000bb4 <NVIC_EnableIRQ>:
20000bb4:	b480      	push	{r7}
20000bb6:	b083      	sub	sp, #12
20000bb8:	af00      	add	r7, sp, #0
20000bba:	4603      	mov	r3, r0
20000bbc:	71fb      	strb	r3, [r7, #7]
20000bbe:	4908      	ldr	r1, [pc, #32]	; (20000be0 <NVIC_EnableIRQ+0x2c>)
20000bc0:	f997 3007 	ldrsb.w	r3, [r7, #7]
20000bc4:	095b      	lsrs	r3, r3, #5
20000bc6:	79fa      	ldrb	r2, [r7, #7]
20000bc8:	f002 021f 	and.w	r2, r2, #31
20000bcc:	2001      	movs	r0, #1
20000bce:	fa00 f202 	lsl.w	r2, r0, r2
20000bd2:	f841 2023 	str.w	r2, [r1, r3, lsl #2]
20000bd6:	370c      	adds	r7, #12
20000bd8:	46bd      	mov	sp, r7
20000bda:	f85d 7b04 	ldr.w	r7, [sp], #4
20000bde:	4770      	bx	lr
20000be0:	e000e100 	.word	0xe000e100

20000be4 <NVIC_DisableIRQ>:
20000be4:	b480      	push	{r7}
20000be6:	b083      	sub	sp, #12
20000be8:	af00      	add	r7, sp, #0
20000bea:	4603      	mov	r3, r0
20000bec:	71fb      	strb	r3, [r7, #7]
20000bee:	4909      	ldr	r1, [pc, #36]	; (20000c14 <NVIC_DisableIRQ+0x30>)
20000bf0:	f997 3007 	ldrsb.w	r3, [r7, #7]
20000bf4:	095b      	lsrs	r3, r3, #5
20000bf6:	79fa      	ldrb	r2, [r7, #7]
20000bf8:	f002 021f 	and.w	r2, r2, #31
20000bfc:	2001      	movs	r0, #1
20000bfe:	fa00 f202 	lsl.w	r2, r0, r2
20000c02:	3320      	adds	r3, #32
20000c04:	f841 2023 	str.w	r2, [r1, r3, lsl #2]
20000c08:	370c      	adds	r7, #12
20000c0a:	46bd      	mov	sp, r7
20000c0c:	f85d 7b04 	ldr.w	r7, [sp], #4
20000c10:	4770      	bx	lr
20000c12:	bf00      	nop
20000c14:	e000e100 	.word	0xe000e100

20000c18 <NVIC_SetPriority>:
20000c18:	b480      	push	{r7}
20000c1a:	b083      	sub	sp, #12
20000c1c:	af00      	add	r7, sp, #0
20000c1e:	4603      	mov	r3, r0
20000c20:	6039      	str	r1, [r7, #0]
20000c22:	71fb      	strb	r3, [r7, #7]
20000c24:	f997 3007 	ldrsb.w	r3, [r7, #7]
20000c28:	2b00      	cmp	r3, #0
20000c2a:	da0b      	bge.n	20000c44 <NVIC_SetPriority+0x2c>
20000c2c:	490d      	ldr	r1, [pc, #52]	; (20000c64 <NVIC_SetPriority+0x4c>)
20000c2e:	79fb      	ldrb	r3, [r7, #7]
20000c30:	f003 030f 	and.w	r3, r3, #15
20000c34:	3b04      	subs	r3, #4
20000c36:	683a      	ldr	r2, [r7, #0]
20000c38:	b2d2      	uxtb	r2, r2
20000c3a:	0152      	lsls	r2, r2, #5
20000c3c:	b2d2      	uxtb	r2, r2
20000c3e:	440b      	add	r3, r1
20000c40:	761a      	strb	r2, [r3, #24]
20000c42:	e009      	b.n	20000c58 <NVIC_SetPriority+0x40>
20000c44:	4908      	ldr	r1, [pc, #32]	; (20000c68 <NVIC_SetPriority+0x50>)
20000c46:	f997 3007 	ldrsb.w	r3, [r7, #7]
20000c4a:	683a      	ldr	r2, [r7, #0]
20000c4c:	b2d2      	uxtb	r2, r2
20000c4e:	0152      	lsls	r2, r2, #5
20000c50:	b2d2      	uxtb	r2, r2
20000c52:	440b      	add	r3, r1
20000c54:	f883 2300 	strb.w	r2, [r3, #768]	; 0x300
20000c58:	370c      	adds	r7, #12
20000c5a:	46bd      	mov	sp, r7
20000c5c:	f85d 7b04 	ldr.w	r7, [sp], #4
20000c60:	4770      	bx	lr
20000c62:	bf00      	nop
20000c64:	e000ed00 	.word	0xe000ed00
20000c68:	e000e100 	.word	0xe000e100

20000c6c <NVIC_GetPriority>:
20000c6c:	b480      	push	{r7}
20000c6e:	b083      	sub	sp, #12
20000c70:	af00      	add	r7, sp, #0
20000c72:	4603      	mov	r3, r0
20000c74:	71fb      	strb	r3, [r7, #7]
20000c76:	f997 3007 	ldrsb.w	r3, [r7, #7]
20000c7a:	2b00      	cmp	r3, #0
20000c7c:	da0a      	bge.n	20000c94 <NVIC_GetPriority+0x28>
20000c7e:	4a0d      	ldr	r2, [pc, #52]	; (20000cb4 <NVIC_GetPriority+0x48>)
20000c80:	79fb      	ldrb	r3, [r7, #7]
20000c82:	f003 030f 	and.w	r3, r3, #15
20000c86:	3b04      	subs	r3, #4
20000c88:	4413      	add	r3, r2
20000c8a:	7e1b      	ldrb	r3, [r3, #24]
20000c8c:	b2db      	uxtb	r3, r3
20000c8e:	095b      	lsrs	r3, r3, #5
20000c90:	b2db      	uxtb	r3, r3
20000c92:	e008      	b.n	20000ca6 <NVIC_GetPriority+0x3a>
20000c94:	4a08      	ldr	r2, [pc, #32]	; (20000cb8 <NVIC_GetPriority+0x4c>)
20000c96:	f997 3007 	ldrsb.w	r3, [r7, #7]
20000c9a:	4413      	add	r3, r2
20000c9c:	f893 3300 	ldrb.w	r3, [r3, #768]	; 0x300
20000ca0:	b2db      	uxtb	r3, r3
20000ca2:	095b      	lsrs	r3, r3, #5
20000ca4:	b2db      	uxtb	r3, r3
20000ca6:	4618      	mov	r0, r3
20000ca8:	370c      	adds	r7, #12
20000caa:	46bd      	mov	sp, r7
20000cac:	f85d 7b04 	ldr.w	r7, [sp], #4
20000cb0:	4770      	bx	lr
20000cb2:	bf00      	nop
20000cb4:	e000ed00 	.word	0xe000ed00
20000cb8:	e000e100 	.word	0xe000e100

20000cbc <SCB_EnableICache>:
20000cbc:	b480      	push	{r7}
20000cbe:	af00      	add	r7, sp, #0
20000cc0:	f3bf 8f4f 	dsb	sy
20000cc4:	f3bf 8f6f 	isb	sy
20000cc8:	4b08      	ldr	r3, [pc, #32]	; (20000cec <SCB_EnableICache+0x30>)
20000cca:	2200      	movs	r2, #0
20000ccc:	f8c3 2250 	str.w	r2, [r3, #592]	; 0x250
20000cd0:	4a06      	ldr	r2, [pc, #24]	; (20000cec <SCB_EnableICache+0x30>)
20000cd2:	4b06      	ldr	r3, [pc, #24]	; (20000cec <SCB_EnableICache+0x30>)
20000cd4:	695b      	ldr	r3, [r3, #20]
20000cd6:	f443 3300 	orr.w	r3, r3, #131072	; 0x20000
20000cda:	6153      	str	r3, [r2, #20]
20000cdc:	f3bf 8f4f 	dsb	sy
20000ce0:	f3bf 8f6f 	isb	sy
20000ce4:	46bd      	mov	sp, r7
20000ce6:	f85d 7b04 	ldr.w	r7, [sp], #4
20000cea:	4770      	bx	lr
20000cec:	e000ed00 	.word	0xe000ed00

20000cf0 <SCB_DisableICache>:
20000cf0:	b480      	push	{r7}
20000cf2:	af00      	add	r7, sp, #0
20000cf4:	f3bf 8f4f 	dsb	sy
20000cf8:	f3bf 8f6f 	isb	sy
20000cfc:	4a08      	ldr	r2, [pc, #32]	; (20000d20 <SCB_DisableICache+0x30>)
20000cfe:	4b08      	ldr	r3, [pc, #32]	; (20000d20 <SCB_DisableICache+0x30>)
20000d00:	695b      	ldr	r3, [r3, #20]
20000d02:	f423 3300 	bic.w	r3, r3, #131072	; 0x20000
20000d06:	6153      	str	r3, [r2, #20]
20000d08:	4b05      	ldr	r3, [pc, #20]	; (20000d20 <SCB_DisableICache+0x30>)
20000d0a:	2200      	movs	r2, #0
20000d0c:	f8c3 2250 	str.w	r2, [r3, #592]	; 0x250
20000d10:	f3bf 8f4f 	dsb	sy
20000d14:	f3bf 8f6f 	isb	sy
20000d18:	46bd      	mov	sp, r7
20000d1a:	f85d 7b04 	ldr.w	r7, [sp], #4
20000d1e:	4770      	bx	lr
20000d20:	e000ed00 	.word	0xe000ed00

20000d24 <SCB_InvalidateICache>:
20000d24:	b480      	push	{r7}
20000d26:	af00      	add	r7, sp, #0
20000d28:	f3bf 8f4f 	dsb	sy
20000d2c:	f3bf 8f6f 	isb	sy
20000d30:	4b05      	ldr	r3, [pc, #20]	; (20000d48 <SCB_InvalidateICache+0x24>)
20000d32:	2200      	movs	r2, #0
20000d34:	f8c3 2250 	str.w	r2, [r3, #592]	; 0x250
20000d38:	f3bf 8f4f 	dsb	sy
20000d3c:	f3bf 8f6f 	isb	sy
20000d40:	46bd      	mov	sp, r7
20000d42:	f85d 7b04 	ldr.w	r7, [sp], #4
20000d46:	4770      	bx	lr
20000d48:	e000ed00 	.word	0xe000ed00

20000d4c <SCB_EnableDCache>:
20000d4c:	b480      	push	{r7}
20000d4e:	b085      	sub	sp, #20
20000d50:	af00      	add	r7, sp, #0
20000d52:	4b1d      	ldr	r3, [pc, #116]	; (20000dc8 <SCB_EnableDCache+0x7c>)
20000d54:	2200      	movs	r2, #0
20000d56:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
20000d5a:	f3bf 8f4f 	dsb	sy
20000d5e:	4b1a      	ldr	r3, [pc, #104]	; (20000dc8 <SCB_EnableDCache+0x7c>)
20000d60:	f8d3 3080 	ldr.w	r3, [r3, #128]	; 0x80
20000d64:	607b      	str	r3, [r7, #4]
20000d66:	687a      	ldr	r2, [r7, #4]
20000d68:	4b18      	ldr	r3, [pc, #96]	; (20000dcc <SCB_EnableDCache+0x80>)
20000d6a:	4013      	ands	r3, r2
20000d6c:	0b5b      	lsrs	r3, r3, #13
20000d6e:	60fb      	str	r3, [r7, #12]
20000d70:	687a      	ldr	r2, [r7, #4]
20000d72:	f641 73f8 	movw	r3, #8184	; 0x1ff8
20000d76:	4013      	ands	r3, r2
20000d78:	08db      	lsrs	r3, r3, #3
20000d7a:	60bb      	str	r3, [r7, #8]
20000d7c:	4912      	ldr	r1, [pc, #72]	; (20000dc8 <SCB_EnableDCache+0x7c>)
20000d7e:	68fb      	ldr	r3, [r7, #12]
20000d80:	015a      	lsls	r2, r3, #5
20000d82:	f643 73e0 	movw	r3, #16352	; 0x3fe0
20000d86:	4013      	ands	r3, r2
20000d88:	68ba      	ldr	r2, [r7, #8]
20000d8a:	0792      	lsls	r2, r2, #30
20000d8c:	4313      	orrs	r3, r2
20000d8e:	f8c1 3260 	str.w	r3, [r1, #608]	; 0x260
20000d92:	68bb      	ldr	r3, [r7, #8]
20000d94:	1e5a      	subs	r2, r3, #1
20000d96:	60ba      	str	r2, [r7, #8]
20000d98:	2b00      	cmp	r3, #0
20000d9a:	d1ef      	bne.n	20000d7c <SCB_EnableDCache+0x30>
20000d9c:	68fb      	ldr	r3, [r7, #12]
20000d9e:	1e5a      	subs	r2, r3, #1
20000da0:	60fa      	str	r2, [r7, #12]
20000da2:	2b00      	cmp	r3, #0
20000da4:	d1e4      	bne.n	20000d70 <SCB_EnableDCache+0x24>
20000da6:	f3bf 8f4f 	dsb	sy
20000daa:	4a07      	ldr	r2, [pc, #28]	; (20000dc8 <SCB_EnableDCache+0x7c>)
20000dac:	4b06      	ldr	r3, [pc, #24]	; (20000dc8 <SCB_EnableDCache+0x7c>)
20000dae:	695b      	ldr	r3, [r3, #20]
20000db0:	f443 3380 	orr.w	r3, r3, #65536	; 0x10000
20000db4:	6153      	str	r3, [r2, #20]
20000db6:	f3bf 8f4f 	dsb	sy
20000dba:	f3bf 8f6f 	isb	sy
20000dbe:	3714      	adds	r7, #20
20000dc0:	46bd      	mov	sp, r7
20000dc2:	f85d 7b04 	ldr.w	r7, [sp], #4
20000dc6:	4770      	bx	lr
20000dc8:	e000ed00 	.word	0xe000ed00
20000dcc:	0fffe000 	.word	0x0fffe000

20000dd0 <SCB_DisableDCache>:
20000dd0:	b480      	push	{r7}
20000dd2:	b085      	sub	sp, #20
20000dd4:	af00      	add	r7, sp, #0
20000dd6:	4b1c      	ldr	r3, [pc, #112]	; (20000e48 <SCB_DisableDCache+0x78>)
20000dd8:	2200      	movs	r2, #0
20000dda:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
20000dde:	f3bf 8f4f 	dsb	sy
20000de2:	4b19      	ldr	r3, [pc, #100]	; (20000e48 <SCB_DisableDCache+0x78>)
20000de4:	f8d3 3080 	ldr.w	r3, [r3, #128]	; 0x80
20000de8:	607b      	str	r3, [r7, #4]
20000dea:	4a17      	ldr	r2, [pc, #92]	; (20000e48 <SCB_DisableDCache+0x78>)
20000dec:	4b16      	ldr	r3, [pc, #88]	; (20000e48 <SCB_DisableDCache+0x78>)
20000dee:	695b      	ldr	r3, [r3, #20]
20000df0:	f423 3380 	bic.w	r3, r3, #65536	; 0x10000
20000df4:	6153      	str	r3, [r2, #20]
20000df6:	687a      	ldr	r2, [r7, #4]
20000df8:	4b14      	ldr	r3, [pc, #80]	; (20000e4c <SCB_DisableDCache+0x7c>)
20000dfa:	4013      	ands	r3, r2
20000dfc:	0b5b      	lsrs	r3, r3, #13
20000dfe:	60fb      	str	r3, [r7, #12]
20000e00:	687a      	ldr	r2, [r7, #4]
20000e02:	f641 73f8 	movw	r3, #8184	; 0x1ff8
20000e06:	4013      	ands	r3, r2
20000e08:	08db      	lsrs	r3, r3, #3
20000e0a:	60bb      	str	r3, [r7, #8]
20000e0c:	490e      	ldr	r1, [pc, #56]	; (20000e48 <SCB_DisableDCache+0x78>)
20000e0e:	68fb      	ldr	r3, [r7, #12]
20000e10:	015a      	lsls	r2, r3, #5
20000e12:	f643 73e0 	movw	r3, #16352	; 0x3fe0
20000e16:	4013      	ands	r3, r2
20000e18:	68ba      	ldr	r2, [r7, #8]
20000e1a:	0792      	lsls	r2, r2, #30
20000e1c:	4313      	orrs	r3, r2
20000e1e:	f8c1 3274 	str.w	r3, [r1, #628]	; 0x274
20000e22:	68bb      	ldr	r3, [r7, #8]
20000e24:	1e5a      	subs	r2, r3, #1
20000e26:	60ba      	str	r2, [r7, #8]
20000e28:	2b00      	cmp	r3, #0
20000e2a:	d1ef      	bne.n	20000e0c <SCB_DisableDCache+0x3c>
20000e2c:	68fb      	ldr	r3, [r7, #12]
20000e2e:	1e5a      	subs	r2, r3, #1
20000e30:	60fa      	str	r2, [r7, #12]
20000e32:	2b00      	cmp	r3, #0
20000e34:	d1e4      	bne.n	20000e00 <SCB_DisableDCache+0x30>
20000e36:	f3bf 8f4f 	dsb	sy
20000e3a:	f3bf 8f6f 	isb	sy
20000e3e:	3714      	adds	r7, #20
20000e40:	46bd      	mov	sp, r7
20000e42:	f85d 7b04 	ldr.w	r7, [sp], #4
20000e46:	4770      	bx	lr
20000e48:	e000ed00 	.word	0xe000ed00
20000e4c:	0fffe000 	.word	0x0fffe000

20000e50 <SCB_InvalidateDCache>:
20000e50:	b480      	push	{r7}
20000e52:	b085      	sub	sp, #20
20000e54:	af00      	add	r7, sp, #0
20000e56:	4b19      	ldr	r3, [pc, #100]	; (20000ebc <SCB_InvalidateDCache+0x6c>)
20000e58:	2200      	movs	r2, #0
20000e5a:	f8c3 2084 	str.w	r2, [r3, #132]	; 0x84
20000e5e:	f3bf 8f4f 	dsb	sy
20000e62:	4b16      	ldr	r3, [pc, #88]	; (20000ebc <SCB_InvalidateDCache+0x6c>)
20000e64:	f8d3 3080 	ldr.w	r3, [r3, #128]	; 0x80
20000e68:	607b      	str	r3, [r7, #4]
20000e6a:	687a      	ldr	r2, [r7, #4]
20000e6c:	4b14      	ldr	r3, [pc, #80]	; (20000ec0 <SCB_InvalidateDCache+0x70>)
20000e6e:	4013      	ands	r3, r2
20000e70:	0b5b      	lsrs	r3, r3, #13
20000e72:	60fb      	str	r3, [r7, #12]
20000e74:	687a      	ldr	r2, [r7, #4]
20000e76:	f641 73f8 	movw	r3, #8184	; 0x1ff8
20000e7a:	4013      	ands	r3, r2
20000e7c:	08db      	lsrs	r3, r3, #3
20000e7e:	60bb      	str	r3, [r7, #8]
20000e80:	490e      	ldr	r1, [pc, #56]	; (20000ebc <SCB_InvalidateDCache+0x6c>)
20000e82:	68fb      	ldr	r3, [r7, #12]
20000e84:	015a      	lsls	r2, r3, #5
20000e86:	f643 73e0 	movw	r3, #16352	; 0x3fe0
20000e8a:	4013      	ands	r3, r2
20000e8c:	68ba      	ldr	r2, [r7, #8]
20000e8e:	0792      	lsls	r2, r2, #30
20000e90:	4313      	orrs	r3, r2
20000e92:	f8c1 3260 	str.w	r3, [r1, #608]	; 0x260
20000e96:	68bb      	ldr	r3, [r7, #8]
20000e98:	1e5a      	subs	r2, r3, #1
20000e9a:	60ba      	str	r2, [r7, #8]
20000e9c:	2b00      	cmp	r3, #0
20000e9e:	d1ef      	bne.n	20000e80 <SCB_InvalidateDCache+0x30>
20000ea0:	68fb      	ldr	r3, [r7, #12]
20000ea2:	1e5a      	subs	r2, r3, #1
20000ea4:	60fa      	str	r2, [r7, #12]
20000ea6:	2b00      	cmp	r3, #0
20000ea8:	d1e4      	bne.n	20000e74 <SCB_InvalidateDCache+0x24>
20000eaa:	f3bf 8f4f 	dsb	sy
20000eae:	f3bf 8f6f 	isb	sy
20000eb2:	3714      	adds	r7, #20
20000eb4:	46bd      	mov	sp, r7
20000eb6:	f85d 7b04 	ldr.w	r7, [sp], #4
20000eba:	4770      	bx	lr
20000ebc:	e000ed00 	.word	0xe000ed00
20000ec0:	0fffe000 	.word	0x0fffe000

20000ec4 <SCB_InvalidateDCache_by_Addr>:
20000ec4:	b480      	push	{r7}
20000ec6:	b087      	sub	sp, #28
20000ec8:	af00      	add	r7, sp, #0
20000eca:	6078      	str	r0, [r7, #4]
20000ecc:	6039      	str	r1, [r7, #0]
20000ece:	683b      	ldr	r3, [r7, #0]
20000ed0:	617b      	str	r3, [r7, #20]
20000ed2:	687b      	ldr	r3, [r7, #4]
20000ed4:	613b      	str	r3, [r7, #16]
20000ed6:	2320      	movs	r3, #32
20000ed8:	60fb      	str	r3, [r7, #12]
20000eda:	f3bf 8f4f 	dsb	sy
20000ede:	e00b      	b.n	20000ef8 <SCB_InvalidateDCache_by_Addr+0x34>
20000ee0:	4a0b      	ldr	r2, [pc, #44]	; (20000f10 <SCB_InvalidateDCache_by_Addr+0x4c>)
20000ee2:	693b      	ldr	r3, [r7, #16]
20000ee4:	f8c2 325c 	str.w	r3, [r2, #604]	; 0x25c
20000ee8:	68fb      	ldr	r3, [r7, #12]
20000eea:	693a      	ldr	r2, [r7, #16]
20000eec:	4413      	add	r3, r2
20000eee:	613b      	str	r3, [r7, #16]
20000ef0:	697a      	ldr	r2, [r7, #20]
20000ef2:	68fb      	ldr	r3, [r7, #12]
20000ef4:	1ad3      	subs	r3, r2, r3
20000ef6:	617b      	str	r3, [r7, #20]
20000ef8:	697b      	ldr	r3, [r7, #20]
20000efa:	2b00      	cmp	r3, #0
20000efc:	dcf0      	bgt.n	20000ee0 <SCB_InvalidateDCache_by_Addr+0x1c>
20000efe:	f3bf 8f4f 	dsb	sy
20000f02:	f3bf 8f6f 	isb	sy
20000f06:	371c      	adds	r7, #28
20000f08:	46bd      	mov	sp, r7
20000f0a:	f85d 7b04 	ldr.w	r7, [sp], #4
20000f0e:	4770      	bx	lr
20000f10:	e000ed00 	.word	0xe000ed00

20000f14 <SCB_CleanDCache_by_Addr>:
20000f14:	b480      	push	{r7}
20000f16:	b087      	sub	sp, #28
20000f18:	af00      	add	r7, sp, #0
20000f1a:	6078      	str	r0, [r7, #4]
20000f1c:	6039      	str	r1, [r7, #0]
20000f1e:	683b      	ldr	r3, [r7, #0]
20000f20:	617b      	str	r3, [r7, #20]
20000f22:	687b      	ldr	r3, [r7, #4]
20000f24:	613b      	str	r3, [r7, #16]
20000f26:	2320      	movs	r3, #32
20000f28:	60fb      	str	r3, [r7, #12]
20000f2a:	f3bf 8f4f 	dsb	sy
20000f2e:	e00b      	b.n	20000f48 <SCB_CleanDCache_by_Addr+0x34>
20000f30:	4a0b      	ldr	r2, [pc, #44]	; (20000f60 <SCB_CleanDCache_by_Addr+0x4c>)
20000f32:	693b      	ldr	r3, [r7, #16]
20000f34:	f8c2 3268 	str.w	r3, [r2, #616]	; 0x268
20000f38:	68fb      	ldr	r3, [r7, #12]
20000f3a:	693a      	ldr	r2, [r7, #16]
20000f3c:	4413      	add	r3, r2
20000f3e:	613b      	str	r3, [r7, #16]
20000f40:	697a      	ldr	r2, [r7, #20]
20000f42:	68fb      	ldr	r3, [r7, #12]
20000f44:	1ad3      	subs	r3, r2, r3
20000f46:	617b      	str	r3, [r7, #20]
20000f48:	697b      	ldr	r3, [r7, #20]
20000f4a:	2b00      	cmp	r3, #0
20000f4c:	dcf0      	bgt.n	20000f30 <SCB_CleanDCache_by_Addr+0x1c>
20000f4e:	f3bf 8f4f 	dsb	sy
20000f52:	f3bf 8f6f 	isb	sy
20000f56:	371c      	adds	r7, #28
20000f58:	46bd      	mov	sp, r7
20000f5a:	f85d 7b04 	ldr.w	r7, [sp], #4
20000f5e:	4770      	bx	lr
20000f60:	e000ed00 	.word	0xe000ed00

20000f64 <m7_tcm_enable>:
20000f64:	b480      	push	{r7}
20000f66:	af00      	add	r7, sp, #0
20000f68:	f3bf 8f4f 	dsb	sy
20000f6c:	f3bf 8f6f 	isb	sy
20000f70:	4b07      	ldr	r3, [pc, #28]	; (20000f90 <m7_tcm_enable+0x2c>)
20000f72:	2207      	movs	r2, #7
20000f74:	f8c3 2290 	str.w	r2, [r3, #656]	; 0x290
20000f78:	4b05      	ldr	r3, [pc, #20]	; (20000f90 <m7_tcm_enable+0x2c>)
20000f7a:	2207      	movs	r2, #7
20000f7c:	f8c3 2294 	str.w	r2, [r3, #660]	; 0x294
20000f80:	f3bf 8f4f 	dsb	sy
20000f84:	f3bf 8f6f 	isb	sy
20000f88:	46bd      	mov	sp, r7
20000f8a:	f85d 7b04 	ldr.w	r7, [sp], #4
20000f8e:	4770      	bx	lr
20000f90:	e000ed00 	.word	0xe000ed00

20000f94 <m7_tcm_disable>:
20000f94:	b480      	push	{r7}
20000f96:	af00      	add	r7, sp, #0
20000f98:	f3bf 8f4f 	dsb	sy
20000f9c:	f3bf 8f6f 	isb	sy
20000fa0:	4a0b      	ldr	r2, [pc, #44]	; (20000fd0 <m7_tcm_disable+0x3c>)
20000fa2:	4b0b      	ldr	r3, [pc, #44]	; (20000fd0 <m7_tcm_disable+0x3c>)
20000fa4:	f8d3 3290 	ldr.w	r3, [r3, #656]	; 0x290
20000fa8:	f023 0301 	bic.w	r3, r3, #1
20000fac:	f8c2 3290 	str.w	r3, [r2, #656]	; 0x290
20000fb0:	4a07      	ldr	r2, [pc, #28]	; (20000fd0 <m7_tcm_disable+0x3c>)
20000fb2:	4b07      	ldr	r3, [pc, #28]	; (20000fd0 <m7_tcm_disable+0x3c>)
20000fb4:	f8d3 3294 	ldr.w	r3, [r3, #660]	; 0x294
20000fb8:	f023 0301 	bic.w	r3, r3, #1
20000fbc:	f8c2 3294 	str.w	r3, [r2, #660]	; 0x294
20000fc0:	f3bf 8f4f 	dsb	sy
20000fc4:	f3bf 8f6f 	isb	sy
20000fc8:	46bd      	mov	sp, r7
20000fca:	f85d 7b04 	ldr.w	r7, [sp], #4
20000fce:	4770      	bx	lr
20000fd0:	e000ed00 	.word	0xe000ed00

20000fd4 <m7_vector_table_set>:
20000fd4:	b480      	push	{r7}
20000fd6:	b083      	sub	sp, #12
20000fd8:	af00      	add	r7, sp, #0
20000fda:	6078      	str	r0, [r7, #4]
20000fdc:	4a04      	ldr	r2, [pc, #16]	; (20000ff0 <m7_vector_table_set+0x1c>)
20000fde:	687b      	ldr	r3, [r7, #4]
20000fe0:	f023 037f 	bic.w	r3, r3, #127	; 0x7f
20000fe4:	6093      	str	r3, [r2, #8]
20000fe6:	370c      	adds	r7, #12
20000fe8:	46bd      	mov	sp, r7
20000fea:	f85d 7b04 	ldr.w	r7, [sp], #4
20000fee:	4770      	bx	lr
20000ff0:	e000ed00 	.word	0xe000ed00

20000ff4 <m7_enable_irq>:
20000ff4:	b580      	push	{r7, lr}
20000ff6:	b082      	sub	sp, #8
20000ff8:	af00      	add	r7, sp, #0
20000ffa:	4603      	mov	r3, r0
20000ffc:	71fb      	strb	r3, [r7, #7]
20000ffe:	f997 3007 	ldrsb.w	r3, [r7, #7]
20001002:	4618      	mov	r0, r3
20001004:	f7ff fdd6 	bl	20000bb4 <NVIC_EnableIRQ>
20001008:	3708      	adds	r7, #8
2000100a:	46bd      	mov	sp, r7
2000100c:	bd80      	pop	{r7, pc}
2000100e:	bf00      	nop

20001010 <m7_disable_irq>:
20001010:	b580      	push	{r7, lr}
20001012:	b082      	sub	sp, #8
20001014:	af00      	add	r7, sp, #0
20001016:	4603      	mov	r3, r0
20001018:	71fb      	strb	r3, [r7, #7]
2000101a:	f997 3007 	ldrsb.w	r3, [r7, #7]
2000101e:	4618      	mov	r0, r3
20001020:	f7ff fde0 	bl	20000be4 <NVIC_DisableIRQ>
20001024:	3708      	adds	r7, #8
20001026:	46bd      	mov	sp, r7
20001028:	bd80      	pop	{r7, pc}
2000102a:	bf00      	nop

2000102c <m7_enable_intr>:
2000102c:	b480      	push	{r7}
2000102e:	af00      	add	r7, sp, #0
20001030:	b662      	cpsie	i
20001032:	46bd      	mov	sp, r7
20001034:	f85d 7b04 	ldr.w	r7, [sp], #4
20001038:	4770      	bx	lr
2000103a:	bf00      	nop

2000103c <m7_disable_intr>:
2000103c:	b480      	push	{r7}
2000103e:	af00      	add	r7, sp, #0
20001040:	b672      	cpsid	i
20001042:	46bd      	mov	sp, r7
20001044:	f85d 7b04 	ldr.w	r7, [sp], #4
20001048:	4770      	bx	lr
2000104a:	bf00      	nop

2000104c <m7_set_irq_grouping>:
2000104c:	b580      	push	{r7, lr}
2000104e:	b082      	sub	sp, #8
20001050:	af00      	add	r7, sp, #0
20001052:	6078      	str	r0, [r7, #4]
20001054:	687b      	ldr	r3, [r7, #4]
20001056:	2b07      	cmp	r3, #7
20001058:	d900      	bls.n	2000105c <m7_set_irq_grouping+0x10>
2000105a:	e002      	b.n	20001062 <m7_set_irq_grouping+0x16>
2000105c:	6878      	ldr	r0, [r7, #4]
2000105e:	f7ff fd85 	bl	20000b6c <NVIC_SetPriorityGrouping>
20001062:	3708      	adds	r7, #8
20001064:	46bd      	mov	sp, r7
20001066:	bd80      	pop	{r7, pc}

20001068 <m7_set_irq_priority>:
20001068:	b580      	push	{r7, lr}
2000106a:	b082      	sub	sp, #8
2000106c:	af00      	add	r7, sp, #0
2000106e:	4603      	mov	r3, r0
20001070:	6039      	str	r1, [r7, #0]
20001072:	71fb      	strb	r3, [r7, #7]
20001074:	683b      	ldr	r3, [r7, #0]
20001076:	2b07      	cmp	r3, #7
20001078:	d900      	bls.n	2000107c <m7_set_irq_priority+0x14>
2000107a:	e005      	b.n	20001088 <m7_set_irq_priority+0x20>
2000107c:	f997 3007 	ldrsb.w	r3, [r7, #7]
20001080:	4618      	mov	r0, r3
20001082:	6839      	ldr	r1, [r7, #0]
20001084:	f7ff fdc8 	bl	20000c18 <NVIC_SetPriority>
20001088:	3708      	adds	r7, #8
2000108a:	46bd      	mov	sp, r7
2000108c:	bd80      	pop	{r7, pc}
2000108e:	bf00      	nop

20001090 <m7_get_irq_priority>:
20001090:	b580      	push	{r7, lr}
20001092:	b082      	sub	sp, #8
20001094:	af00      	add	r7, sp, #0
20001096:	4603      	mov	r3, r0
20001098:	71fb      	strb	r3, [r7, #7]
2000109a:	f997 3007 	ldrsb.w	r3, [r7, #7]
2000109e:	4618      	mov	r0, r3
200010a0:	f7ff fde4 	bl	20000c6c <NVIC_GetPriority>
200010a4:	4603      	mov	r3, r0
200010a6:	4618      	mov	r0, r3
200010a8:	3708      	adds	r7, #8
200010aa:	46bd      	mov	sp, r7
200010ac:	bd80      	pop	{r7, pc}
200010ae:	bf00      	nop

200010b0 <m7_mpu_set_region>:
200010b0:	b580      	push	{r7, lr}
200010b2:	b086      	sub	sp, #24
200010b4:	af00      	add	r7, sp, #0
200010b6:	60f8      	str	r0, [r7, #12]
200010b8:	60b9      	str	r1, [r7, #8]
200010ba:	607a      	str	r2, [r7, #4]
200010bc:	603b      	str	r3, [r7, #0]
200010be:	2300      	movs	r3, #0
200010c0:	613b      	str	r3, [r7, #16]
200010c2:	4b2c      	ldr	r3, [pc, #176]	; (20001174 <m7_mpu_set_region+0xc4>)
200010c4:	681b      	ldr	r3, [r3, #0]
200010c6:	2b00      	cmp	r3, #0
200010c8:	d102      	bne.n	200010d0 <m7_mpu_set_region+0x20>
200010ca:	f06f 0310 	mvn.w	r3, #16
200010ce:	e04c      	b.n	2000116a <m7_mpu_set_region+0xba>
200010d0:	687b      	ldr	r3, [r7, #4]
200010d2:	2b1f      	cmp	r3, #31
200010d4:	d802      	bhi.n	200010dc <m7_mpu_set_region+0x2c>
200010d6:	f06f 0303 	mvn.w	r3, #3
200010da:	e046      	b.n	2000116a <m7_mpu_set_region+0xba>
200010dc:	683a      	ldr	r2, [r7, #0]
200010de:	4b26      	ldr	r3, [pc, #152]	; (20001178 <m7_mpu_set_region+0xc8>)
200010e0:	4013      	ands	r3, r2
200010e2:	2b00      	cmp	r3, #0
200010e4:	d002      	beq.n	200010ec <m7_mpu_set_region+0x3c>
200010e6:	f06f 0303 	mvn.w	r3, #3
200010ea:	e03e      	b.n	2000116a <m7_mpu_set_region+0xba>
200010ec:	2301      	movs	r3, #1
200010ee:	613b      	str	r3, [r7, #16]
200010f0:	2300      	movs	r3, #0
200010f2:	617b      	str	r3, [r7, #20]
200010f4:	687b      	ldr	r3, [r7, #4]
200010f6:	f1b3 3fff 	cmp.w	r3, #4294967295
200010fa:	d102      	bne.n	20001102 <m7_mpu_set_region+0x52>
200010fc:	2320      	movs	r3, #32
200010fe:	617b      	str	r3, [r7, #20]
20001100:	e00a      	b.n	20001118 <m7_mpu_set_region+0x68>
20001102:	e005      	b.n	20001110 <m7_mpu_set_region+0x60>
20001104:	697b      	ldr	r3, [r7, #20]
20001106:	3301      	adds	r3, #1
20001108:	617b      	str	r3, [r7, #20]
2000110a:	693b      	ldr	r3, [r7, #16]
2000110c:	005b      	lsls	r3, r3, #1
2000110e:	613b      	str	r3, [r7, #16]
20001110:	693a      	ldr	r2, [r7, #16]
20001112:	687b      	ldr	r3, [r7, #4]
20001114:	429a      	cmp	r2, r3
20001116:	d9f5      	bls.n	20001104 <m7_mpu_set_region+0x54>
20001118:	f7ff ff90 	bl	2000103c <m7_disable_intr>
2000111c:	f3bf 8f4f 	dsb	sy
20001120:	f3bf 8f6f 	isb	sy
20001124:	4a13      	ldr	r2, [pc, #76]	; (20001174 <m7_mpu_set_region+0xc4>)
20001126:	68fb      	ldr	r3, [r7, #12]
20001128:	6093      	str	r3, [r2, #8]
2000112a:	4a12      	ldr	r2, [pc, #72]	; (20001174 <m7_mpu_set_region+0xc4>)
2000112c:	4b11      	ldr	r3, [pc, #68]	; (20001174 <m7_mpu_set_region+0xc4>)
2000112e:	691b      	ldr	r3, [r3, #16]
20001130:	f023 0301 	bic.w	r3, r3, #1
20001134:	6113      	str	r3, [r2, #16]
20001136:	4a0f      	ldr	r2, [pc, #60]	; (20001174 <m7_mpu_set_region+0xc4>)
20001138:	697b      	ldr	r3, [r7, #20]
2000113a:	2101      	movs	r1, #1
2000113c:	fa01 f303 	lsl.w	r3, r1, r3
20001140:	425b      	negs	r3, r3
20001142:	4619      	mov	r1, r3
20001144:	68bb      	ldr	r3, [r7, #8]
20001146:	400b      	ands	r3, r1
20001148:	60d3      	str	r3, [r2, #12]
2000114a:	490a      	ldr	r1, [pc, #40]	; (20001174 <m7_mpu_set_region+0xc4>)
2000114c:	697b      	ldr	r3, [r7, #20]
2000114e:	3b01      	subs	r3, #1
20001150:	005a      	lsls	r2, r3, #1
20001152:	683b      	ldr	r3, [r7, #0]
20001154:	4313      	orrs	r3, r2
20001156:	f043 0301 	orr.w	r3, r3, #1
2000115a:	610b      	str	r3, [r1, #16]
2000115c:	f3bf 8f4f 	dsb	sy
20001160:	f3bf 8f6f 	isb	sy
20001164:	f7ff ff62 	bl	2000102c <m7_enable_intr>
20001168:	2300      	movs	r3, #0
2000116a:	4618      	mov	r0, r3
2000116c:	3718      	adds	r7, #24
2000116e:	46bd      	mov	sp, r7
20001170:	bd80      	pop	{r7, pc}
20001172:	bf00      	nop
20001174:	e000ed90 	.word	0xe000ed90
20001178:	e8c000c0 	.word	0xe8c000c0

2000117c <m7_mpu_enable>:
2000117c:	b480      	push	{r7}
2000117e:	af00      	add	r7, sp, #0
20001180:	4b07      	ldr	r3, [pc, #28]	; (200011a0 <m7_mpu_enable+0x24>)
20001182:	681b      	ldr	r3, [r3, #0]
20001184:	2b00      	cmp	r3, #0
20001186:	d102      	bne.n	2000118e <m7_mpu_enable+0x12>
20001188:	f06f 0310 	mvn.w	r3, #16
2000118c:	e003      	b.n	20001196 <m7_mpu_enable+0x1a>
2000118e:	4b04      	ldr	r3, [pc, #16]	; (200011a0 <m7_mpu_enable+0x24>)
20001190:	2205      	movs	r2, #5
20001192:	605a      	str	r2, [r3, #4]
20001194:	2300      	movs	r3, #0
20001196:	4618      	mov	r0, r3
20001198:	46bd      	mov	sp, r7
2000119a:	f85d 7b04 	ldr.w	r7, [sp], #4
2000119e:	4770      	bx	lr
200011a0:	e000ed90 	.word	0xe000ed90

200011a4 <m7_mpu_disable>:
200011a4:	b480      	push	{r7}
200011a6:	af00      	add	r7, sp, #0
200011a8:	4b09      	ldr	r3, [pc, #36]	; (200011d0 <m7_mpu_disable+0x2c>)
200011aa:	681b      	ldr	r3, [r3, #0]
200011ac:	2b00      	cmp	r3, #0
200011ae:	d102      	bne.n	200011b6 <m7_mpu_disable+0x12>
200011b0:	f06f 0310 	mvn.w	r3, #16
200011b4:	e006      	b.n	200011c4 <m7_mpu_disable+0x20>
200011b6:	4a06      	ldr	r2, [pc, #24]	; (200011d0 <m7_mpu_disable+0x2c>)
200011b8:	4b05      	ldr	r3, [pc, #20]	; (200011d0 <m7_mpu_disable+0x2c>)
200011ba:	685b      	ldr	r3, [r3, #4]
200011bc:	f023 0301 	bic.w	r3, r3, #1
200011c0:	6053      	str	r3, [r2, #4]
200011c2:	2300      	movs	r3, #0
200011c4:	4618      	mov	r0, r3
200011c6:	46bd      	mov	sp, r7
200011c8:	f85d 7b04 	ldr.w	r7, [sp], #4
200011cc:	4770      	bx	lr
200011ce:	bf00      	nop
200011d0:	e000ed90 	.word	0xe000ed90

200011d4 <m7_mpu_enable_region>:
200011d4:	b580      	push	{r7, lr}
200011d6:	b082      	sub	sp, #8
200011d8:	af00      	add	r7, sp, #0
200011da:	6078      	str	r0, [r7, #4]
200011dc:	f7ff ff2e 	bl	2000103c <m7_disable_intr>
200011e0:	f3bf 8f4f 	dsb	sy
200011e4:	f3bf 8f6f 	isb	sy
200011e8:	4a08      	ldr	r2, [pc, #32]	; (2000120c <m7_mpu_enable_region+0x38>)
200011ea:	687b      	ldr	r3, [r7, #4]
200011ec:	6093      	str	r3, [r2, #8]
200011ee:	4a07      	ldr	r2, [pc, #28]	; (2000120c <m7_mpu_enable_region+0x38>)
200011f0:	4b06      	ldr	r3, [pc, #24]	; (2000120c <m7_mpu_enable_region+0x38>)
200011f2:	691b      	ldr	r3, [r3, #16]
200011f4:	f043 0301 	orr.w	r3, r3, #1
200011f8:	6113      	str	r3, [r2, #16]
200011fa:	f3bf 8f4f 	dsb	sy
200011fe:	f3bf 8f6f 	isb	sy
20001202:	f7ff ff13 	bl	2000102c <m7_enable_intr>
20001206:	3708      	adds	r7, #8
20001208:	46bd      	mov	sp, r7
2000120a:	bd80      	pop	{r7, pc}
2000120c:	e000ed90 	.word	0xe000ed90

20001210 <m7_mpu_disable_region>:
20001210:	b580      	push	{r7, lr}
20001212:	b082      	sub	sp, #8
20001214:	af00      	add	r7, sp, #0
20001216:	6078      	str	r0, [r7, #4]
20001218:	f7ff ff10 	bl	2000103c <m7_disable_intr>
2000121c:	f3bf 8f4f 	dsb	sy
20001220:	f3bf 8f6f 	isb	sy
20001224:	4a08      	ldr	r2, [pc, #32]	; (20001248 <m7_mpu_disable_region+0x38>)
20001226:	687b      	ldr	r3, [r7, #4]
20001228:	6093      	str	r3, [r2, #8]
2000122a:	4a07      	ldr	r2, [pc, #28]	; (20001248 <m7_mpu_disable_region+0x38>)
2000122c:	4b06      	ldr	r3, [pc, #24]	; (20001248 <m7_mpu_disable_region+0x38>)
2000122e:	691b      	ldr	r3, [r3, #16]
20001230:	f023 0301 	bic.w	r3, r3, #1
20001234:	6113      	str	r3, [r2, #16]
20001236:	f3bf 8f4f 	dsb	sy
2000123a:	f3bf 8f6f 	isb	sy
2000123e:	f7ff fef5 	bl	2000102c <m7_enable_intr>
20001242:	3708      	adds	r7, #8
20001244:	46bd      	mov	sp, r7
20001246:	bd80      	pop	{r7, pc}
20001248:	e000ed90 	.word	0xe000ed90

2000124c <m7_dcache_invalidate>:
2000124c:	b580      	push	{r7, lr}
2000124e:	b084      	sub	sp, #16
20001250:	af00      	add	r7, sp, #0
20001252:	6078      	str	r0, [r7, #4]
20001254:	6039      	str	r1, [r7, #0]
20001256:	687b      	ldr	r3, [r7, #4]
20001258:	f003 031f 	and.w	r3, r3, #31
2000125c:	2b00      	cmp	r3, #0
2000125e:	d00c      	beq.n	2000127a <m7_dcache_invalidate+0x2e>
20001260:	687a      	ldr	r2, [r7, #4]
20001262:	687b      	ldr	r3, [r7, #4]
20001264:	f003 031f 	and.w	r3, r3, #31
20001268:	1ad3      	subs	r3, r2, r3
2000126a:	60fb      	str	r3, [r7, #12]
2000126c:	687a      	ldr	r2, [r7, #4]
2000126e:	68fb      	ldr	r3, [r7, #12]
20001270:	1ad2      	subs	r2, r2, r3
20001272:	683b      	ldr	r3, [r7, #0]
20001274:	4413      	add	r3, r2
20001276:	60bb      	str	r3, [r7, #8]
20001278:	e003      	b.n	20001282 <m7_dcache_invalidate+0x36>
2000127a:	687b      	ldr	r3, [r7, #4]
2000127c:	60fb      	str	r3, [r7, #12]
2000127e:	683b      	ldr	r3, [r7, #0]
20001280:	60bb      	str	r3, [r7, #8]
20001282:	68f8      	ldr	r0, [r7, #12]
20001284:	68b9      	ldr	r1, [r7, #8]
20001286:	f7ff fe1d 	bl	20000ec4 <SCB_InvalidateDCache_by_Addr>
2000128a:	f3bf 8f4f 	dsb	sy
2000128e:	f3bf 8f6f 	isb	sy
20001292:	3710      	adds	r7, #16
20001294:	46bd      	mov	sp, r7
20001296:	bd80      	pop	{r7, pc}

20001298 <m7_dcache_flush>:
20001298:	b580      	push	{r7, lr}
2000129a:	b084      	sub	sp, #16
2000129c:	af00      	add	r7, sp, #0
2000129e:	6078      	str	r0, [r7, #4]
200012a0:	6039      	str	r1, [r7, #0]
200012a2:	687b      	ldr	r3, [r7, #4]
200012a4:	f003 031f 	and.w	r3, r3, #31
200012a8:	2b00      	cmp	r3, #0
200012aa:	d00c      	beq.n	200012c6 <m7_dcache_flush+0x2e>
200012ac:	687a      	ldr	r2, [r7, #4]
200012ae:	687b      	ldr	r3, [r7, #4]
200012b0:	f003 031f 	and.w	r3, r3, #31
200012b4:	1ad3      	subs	r3, r2, r3
200012b6:	60fb      	str	r3, [r7, #12]
200012b8:	687a      	ldr	r2, [r7, #4]
200012ba:	68fb      	ldr	r3, [r7, #12]
200012bc:	1ad2      	subs	r2, r2, r3
200012be:	683b      	ldr	r3, [r7, #0]
200012c0:	4413      	add	r3, r2
200012c2:	60bb      	str	r3, [r7, #8]
200012c4:	e003      	b.n	200012ce <m7_dcache_flush+0x36>
200012c6:	687b      	ldr	r3, [r7, #4]
200012c8:	60fb      	str	r3, [r7, #12]
200012ca:	683b      	ldr	r3, [r7, #0]
200012cc:	60bb      	str	r3, [r7, #8]
200012ce:	68f8      	ldr	r0, [r7, #12]
200012d0:	68b9      	ldr	r1, [r7, #8]
200012d2:	f7ff fe1f 	bl	20000f14 <SCB_CleanDCache_by_Addr>
200012d6:	f3bf 8f4f 	dsb	sy
200012da:	f3bf 8f6f 	isb	sy
200012de:	3710      	adds	r7, #16
200012e0:	46bd      	mov	sp, r7
200012e2:	bd80      	pop	{r7, pc}

200012e4 <m7_dcache_invalidate_all>:
200012e4:	b580      	push	{r7, lr}
200012e6:	af00      	add	r7, sp, #0
200012e8:	f7ff fdb2 	bl	20000e50 <SCB_InvalidateDCache>
200012ec:	f3bf 8f4f 	dsb	sy
200012f0:	f3bf 8f6f 	isb	sy
200012f4:	bd80      	pop	{r7, pc}
200012f6:	bf00      	nop

200012f8 <m7_icache_invalidate_all>:
200012f8:	b580      	push	{r7, lr}
200012fa:	af00      	add	r7, sp, #0
200012fc:	f7ff fd12 	bl	20000d24 <SCB_InvalidateICache>
20001300:	f3bf 8f4f 	dsb	sy
20001304:	f3bf 8f6f 	isb	sy
20001308:	bd80      	pop	{r7, pc}
2000130a:	bf00      	nop

2000130c <m7_dcache_enable>:
2000130c:	b580      	push	{r7, lr}
2000130e:	af00      	add	r7, sp, #0
20001310:	f7ff fd1c 	bl	20000d4c <SCB_EnableDCache>
20001314:	f3bf 8f4f 	dsb	sy
20001318:	f3bf 8f6f 	isb	sy
2000131c:	bd80      	pop	{r7, pc}
2000131e:	bf00      	nop

20001320 <m7_dcache_disable>:
20001320:	b580      	push	{r7, lr}
20001322:	af00      	add	r7, sp, #0
20001324:	f7ff fd54 	bl	20000dd0 <SCB_DisableDCache>
20001328:	f3bf 8f4f 	dsb	sy
2000132c:	f3bf 8f6f 	isb	sy
20001330:	bd80      	pop	{r7, pc}
20001332:	bf00      	nop

20001334 <m7_icache_enable>:
20001334:	b580      	push	{r7, lr}
20001336:	af00      	add	r7, sp, #0
20001338:	f7ff fcc0 	bl	20000cbc <SCB_EnableICache>
2000133c:	f3bf 8f4f 	dsb	sy
20001340:	f3bf 8f6f 	isb	sy
20001344:	bd80      	pop	{r7, pc}
20001346:	bf00      	nop

20001348 <m7_icache_disable>:
20001348:	b580      	push	{r7, lr}
2000134a:	af00      	add	r7, sp, #0
2000134c:	f7ff fcd0 	bl	20000cf0 <SCB_DisableICache>
20001350:	f3bf 8f4f 	dsb	sy
20001354:	f3bf 8f6f 	isb	sy
20001358:	bd80      	pop	{r7, pc}
2000135a:	bf00      	nop

2000135c <bcm53158_reg_get>:
2000135c:	b480      	push	{r7}
2000135e:	b087      	sub	sp, #28
20001360:	af00      	add	r7, sp, #0
20001362:	4603      	mov	r3, r0
20001364:	60b9      	str	r1, [r7, #8]
20001366:	607a      	str	r2, [r7, #4]
20001368:	73fb      	strb	r3, [r7, #15]
2000136a:	7bfb      	ldrb	r3, [r7, #15]
2000136c:	2b01      	cmp	r3, #1
2000136e:	d104      	bne.n	2000137a <bcm53158_reg_get+0x1e>
20001370:	68bb      	ldr	r3, [r7, #8]
20001372:	f043 6380 	orr.w	r3, r3, #67108864	; 0x4000000
20001376:	617b      	str	r3, [r7, #20]
20001378:	e001      	b.n	2000137e <bcm53158_reg_get+0x22>
2000137a:	68bb      	ldr	r3, [r7, #8]
2000137c:	617b      	str	r3, [r7, #20]
2000137e:	697b      	ldr	r3, [r7, #20]
20001380:	681a      	ldr	r2, [r3, #0]
20001382:	687b      	ldr	r3, [r7, #4]
20001384:	601a      	str	r2, [r3, #0]
20001386:	2300      	movs	r3, #0
20001388:	b25b      	sxtb	r3, r3
2000138a:	4618      	mov	r0, r3
2000138c:	371c      	adds	r7, #28
2000138e:	46bd      	mov	sp, r7
20001390:	f85d 7b04 	ldr.w	r7, [sp], #4
20001394:	4770      	bx	lr
20001396:	bf00      	nop

20001398 <bcm53158_reg_set_by_addr>:
20001398:	b480      	push	{r7}
2000139a:	b083      	sub	sp, #12
2000139c:	af00      	add	r7, sp, #0
2000139e:	6078      	str	r0, [r7, #4]
200013a0:	6039      	str	r1, [r7, #0]
200013a2:	687b      	ldr	r3, [r7, #4]
200013a4:	683a      	ldr	r2, [r7, #0]
200013a6:	601a      	str	r2, [r3, #0]
200013a8:	2300      	movs	r3, #0
200013aa:	b25b      	sxtb	r3, r3
200013ac:	4618      	mov	r0, r3
200013ae:	370c      	adds	r7, #12
200013b0:	46bd      	mov	sp, r7
200013b2:	f85d 7b04 	ldr.w	r7, [sp], #4
200013b6:	4770      	bx	lr

200013b8 <slave_1pps_start>:
200013b8:	b5b0      	push	{r4, r5, r7, lr}
200013ba:	b086      	sub	sp, #24
200013bc:	af00      	add	r7, sp, #0
200013be:	e9c7 0100 	strd	r0, r1, [r7]
200013c2:	e9d7 2300 	ldrd	r2, r3, [r7]
200013c6:	4610      	mov	r0, r2
200013c8:	4619      	mov	r1, r3
200013ca:	a32c      	add	r3, pc, #176	; (adr r3, 2000147c <slave_1pps_start+0xc4>)
200013cc:	e9d3 2300 	ldrd	r2, r3, [r3]
200013d0:	f007 f892 	bl	200084f8 <__aeabi_uldivmod>
200013d4:	4602      	mov	r2, r0
200013d6:	460b      	mov	r3, r1
200013d8:	3205      	adds	r2, #5
200013da:	f143 0300 	adc.w	r3, r3, #0
200013de:	e9c7 2304 	strd	r2, r3, [r7, #16]
200013e2:	2000      	movs	r0, #0
200013e4:	491c      	ldr	r1, [pc, #112]	; (20001458 <slave_1pps_start+0xa0>)
200013e6:	2200      	movs	r2, #0
200013e8:	f000 f84c 	bl	20001484 <bcm53158_reg_set>
200013ec:	2000      	movs	r0, #0
200013ee:	491b      	ldr	r1, [pc, #108]	; (2000145c <slave_1pps_start+0xa4>)
200013f0:	2200      	movs	r2, #0
200013f2:	f000 f847 	bl	20001484 <bcm53158_reg_set>
200013f6:	2000      	movs	r0, #0
200013f8:	4917      	ldr	r1, [pc, #92]	; (20001458 <slave_1pps_start+0xa0>)
200013fa:	2201      	movs	r2, #1
200013fc:	f000 f842 	bl	20001484 <bcm53158_reg_set>
20001400:	2000      	movs	r0, #0
20001402:	4917      	ldr	r1, [pc, #92]	; (20001460 <slave_1pps_start+0xa8>)
20001404:	4a17      	ldr	r2, [pc, #92]	; (20001464 <slave_1pps_start+0xac>)
20001406:	f000 f83d 	bl	20001484 <bcm53158_reg_set>
2000140a:	2000      	movs	r0, #0
2000140c:	4916      	ldr	r1, [pc, #88]	; (20001468 <slave_1pps_start+0xb0>)
2000140e:	4a15      	ldr	r2, [pc, #84]	; (20001464 <slave_1pps_start+0xac>)
20001410:	f000 f838 	bl	20001484 <bcm53158_reg_set>
20001414:	e9d7 2304 	ldrd	r2, r3, [r7, #16]
20001418:	001c      	movs	r4, r3
2000141a:	2500      	movs	r5, #0
2000141c:	4623      	mov	r3, r4
2000141e:	60fb      	str	r3, [r7, #12]
20001420:	2000      	movs	r0, #0
20001422:	4912      	ldr	r1, [pc, #72]	; (2000146c <slave_1pps_start+0xb4>)
20001424:	68fa      	ldr	r2, [r7, #12]
20001426:	f000 f82d 	bl	20001484 <bcm53158_reg_set>
2000142a:	693b      	ldr	r3, [r7, #16]
2000142c:	60fb      	str	r3, [r7, #12]
2000142e:	2000      	movs	r0, #0
20001430:	490f      	ldr	r1, [pc, #60]	; (20001470 <slave_1pps_start+0xb8>)
20001432:	68fa      	ldr	r2, [r7, #12]
20001434:	f000 f826 	bl	20001484 <bcm53158_reg_set>
20001438:	2000      	movs	r0, #0
2000143a:	4908      	ldr	r1, [pc, #32]	; (2000145c <slave_1pps_start+0xa4>)
2000143c:	2201      	movs	r2, #1
2000143e:	f000 f821 	bl	20001484 <bcm53158_reg_set>
20001442:	480c      	ldr	r0, [pc, #48]	; (20001474 <slave_1pps_start+0xbc>)
20001444:	e9d7 2304 	ldrd	r2, r3, [r7, #16]
20001448:	f000 fdaa 	bl	20001fa0 <sal_printf>
2000144c:	4b0a      	ldr	r3, [pc, #40]	; (20001478 <slave_1pps_start+0xc0>)
2000144e:	2201      	movs	r2, #1
20001450:	701a      	strb	r2, [r3, #0]
20001452:	3718      	adds	r7, #24
20001454:	46bd      	mov	sp, r7
20001456:	bdb0      	pop	{r4, r5, r7, pc}
20001458:	40300190 	.word	0x40300190
2000145c:	4030018c 	.word	0x4030018c
20001460:	40300194 	.word	0x40300194
20001464:	1dcd6500 	.word	0x1dcd6500
20001468:	40300198 	.word	0x40300198
2000146c:	403001a0 	.word	0x403001a0
20001470:	4030019c 	.word	0x4030019c
20001474:	20008e18 	.word	0x20008e18
20001478:	20009bc9 	.word	0x20009bc9
2000147c:	3b9aca00 	.word	0x3b9aca00
20001480:	00000000 	.word	0x00000000

20001484 <bcm53158_reg_set>:
20001484:	b580      	push	{r7, lr}
20001486:	b086      	sub	sp, #24
20001488:	af00      	add	r7, sp, #0
2000148a:	4603      	mov	r3, r0
2000148c:	60b9      	str	r1, [r7, #8]
2000148e:	607a      	str	r2, [r7, #4]
20001490:	73fb      	strb	r3, [r7, #15]
20001492:	2300      	movs	r3, #0
20001494:	75fb      	strb	r3, [r7, #23]
20001496:	7bfb      	ldrb	r3, [r7, #15]
20001498:	2b00      	cmp	r3, #0
2000149a:	d106      	bne.n	200014aa <bcm53158_reg_set+0x26>
2000149c:	68b8      	ldr	r0, [r7, #8]
2000149e:	6879      	ldr	r1, [r7, #4]
200014a0:	f7ff ff7a 	bl	20001398 <bcm53158_reg_set_by_addr>
200014a4:	4603      	mov	r3, r0
200014a6:	75fb      	strb	r3, [r7, #23]
200014a8:	e00f      	b.n	200014ca <bcm53158_reg_set+0x46>
200014aa:	4b0b      	ldr	r3, [pc, #44]	; (200014d8 <bcm53158_reg_set+0x54>)
200014ac:	781b      	ldrb	r3, [r3, #0]
200014ae:	2b00      	cmp	r3, #0
200014b0:	d00b      	beq.n	200014ca <bcm53158_reg_set+0x46>
200014b2:	7bfb      	ldrb	r3, [r7, #15]
200014b4:	2b01      	cmp	r3, #1
200014b6:	d108      	bne.n	200014ca <bcm53158_reg_set+0x46>
200014b8:	68bb      	ldr	r3, [r7, #8]
200014ba:	f043 6380 	orr.w	r3, r3, #67108864	; 0x4000000
200014be:	4618      	mov	r0, r3
200014c0:	6879      	ldr	r1, [r7, #4]
200014c2:	f7ff ff69 	bl	20001398 <bcm53158_reg_set_by_addr>
200014c6:	4603      	mov	r3, r0
200014c8:	75fb      	strb	r3, [r7, #23]
200014ca:	7dfb      	ldrb	r3, [r7, #23]
200014cc:	b25b      	sxtb	r3, r3
200014ce:	4618      	mov	r0, r3
200014d0:	3718      	adds	r7, #24
200014d2:	46bd      	mov	sp, r7
200014d4:	bd80      	pop	{r7, pc}
200014d6:	bf00      	nop
200014d8:	20009bca 	.word	0x20009bca

200014dc <tse_tod_to_ts>:
200014dc:	b4b0      	push	{r4, r5, r7}
200014de:	b087      	sub	sp, #28
200014e0:	af00      	add	r7, sp, #0
200014e2:	463c      	mov	r4, r7
200014e4:	e884 000f 	stmia.w	r4, {r0, r1, r2, r3}
200014e8:	68bb      	ldr	r3, [r7, #8]
200014ea:	4618      	mov	r0, r3
200014ec:	f04f 0100 	mov.w	r1, #0
200014f0:	e9d7 2300 	ldrd	r2, r3, [r7]
200014f4:	4c10      	ldr	r4, [pc, #64]	; (20001538 <tse_tod_to_ts+0x5c>)
200014f6:	fb04 f503 	mul.w	r5, r4, r3
200014fa:	2400      	movs	r4, #0
200014fc:	fb04 f402 	mul.w	r4, r4, r2
20001500:	442c      	add	r4, r5
20001502:	4d0d      	ldr	r5, [pc, #52]	; (20001538 <tse_tod_to_ts+0x5c>)
20001504:	fba2 2305 	umull	r2, r3, r2, r5
20001508:	441c      	add	r4, r3
2000150a:	4623      	mov	r3, r4
2000150c:	1812      	adds	r2, r2, r0
2000150e:	eb43 0301 	adc.w	r3, r3, r1
20001512:	f04f 30ff 	mov.w	r0, #4294967295
20001516:	f64f 71ff 	movw	r1, #65535	; 0xffff
2000151a:	ea02 0200 	and.w	r2, r2, r0
2000151e:	ea03 0301 	and.w	r3, r3, r1
20001522:	e9c7 2304 	strd	r2, r3, [r7, #16]
20001526:	e9d7 2304 	ldrd	r2, r3, [r7, #16]
2000152a:	4610      	mov	r0, r2
2000152c:	4619      	mov	r1, r3
2000152e:	371c      	adds	r7, #28
20001530:	46bd      	mov	sp, r7
20001532:	bcb0      	pop	{r4, r5, r7}
20001534:	4770      	bx	lr
20001536:	bf00      	nop
20001538:	3b9aca00 	.word	0x3b9aca00

2000153c <tse_phase_adjust>:
2000153c:	b5b0      	push	{r4, r5, r7, lr}
2000153e:	b08e      	sub	sp, #56	; 0x38
20001540:	af06      	add	r7, sp, #24
20001542:	e9c7 0102 	strd	r0, r1, [r7, #8]
20001546:	4613      	mov	r3, r2
20001548:	71fb      	strb	r3, [r7, #7]
2000154a:	e9d7 2302 	ldrd	r2, r3, [r7, #8]
2000154e:	e9c7 2306 	strd	r2, r3, [r7, #24]
20001552:	79fb      	ldrb	r3, [r7, #7]
20001554:	2b00      	cmp	r3, #0
20001556:	d00a      	beq.n	2000156e <tse_phase_adjust+0x32>
20001558:	f04f 0000 	mov.w	r0, #0
2000155c:	f44f 3180 	mov.w	r1, #65536	; 0x10000
20001560:	e9d7 2302 	ldrd	r2, r3, [r7, #8]
20001564:	1a82      	subs	r2, r0, r2
20001566:	eb61 0303 	sbc.w	r3, r1, r3
2000156a:	e9c7 2302 	strd	r2, r3, [r7, #8]
2000156e:	e9d7 2302 	ldrd	r2, r3, [r7, #8]
20001572:	001c      	movs	r4, r3
20001574:	2500      	movs	r5, #0
20001576:	4623      	mov	r3, r4
20001578:	617b      	str	r3, [r7, #20]
2000157a:	68bb      	ldr	r3, [r7, #8]
2000157c:	613b      	str	r3, [r7, #16]
2000157e:	2000      	movs	r0, #0
20001580:	4913      	ldr	r1, [pc, #76]	; (200015d0 <tse_phase_adjust+0x94>)
20001582:	697a      	ldr	r2, [r7, #20]
20001584:	f7ff ff7e 	bl	20001484 <bcm53158_reg_set>
20001588:	2000      	movs	r0, #0
2000158a:	4912      	ldr	r1, [pc, #72]	; (200015d4 <tse_phase_adjust+0x98>)
2000158c:	693a      	ldr	r2, [r7, #16]
2000158e:	f7ff ff79 	bl	20001484 <bcm53158_reg_set>
20001592:	79fb      	ldrb	r3, [r7, #7]
20001594:	2b00      	cmp	r3, #0
20001596:	d001      	beq.n	2000159c <tse_phase_adjust+0x60>
20001598:	490f      	ldr	r1, [pc, #60]	; (200015d8 <tse_phase_adjust+0x9c>)
2000159a:	e000      	b.n	2000159e <tse_phase_adjust+0x62>
2000159c:	490f      	ldr	r1, [pc, #60]	; (200015dc <tse_phase_adjust+0xa0>)
2000159e:	79fb      	ldrb	r3, [r7, #7]
200015a0:	2b00      	cmp	r3, #0
200015a2:	d001      	beq.n	200015a8 <tse_phase_adjust+0x6c>
200015a4:	4b0c      	ldr	r3, [pc, #48]	; (200015d8 <tse_phase_adjust+0x9c>)
200015a6:	e000      	b.n	200015aa <tse_phase_adjust+0x6e>
200015a8:	4b0c      	ldr	r3, [pc, #48]	; (200015dc <tse_phase_adjust+0xa0>)
200015aa:	9300      	str	r3, [sp, #0]
200015ac:	e9d7 2306 	ldrd	r2, r3, [r7, #24]
200015b0:	e9cd 2302 	strd	r2, r3, [sp, #8]
200015b4:	697b      	ldr	r3, [r7, #20]
200015b6:	9304      	str	r3, [sp, #16]
200015b8:	693b      	ldr	r3, [r7, #16]
200015ba:	9305      	str	r3, [sp, #20]
200015bc:	4808      	ldr	r0, [pc, #32]	; (200015e0 <tse_phase_adjust+0xa4>)
200015be:	e9d7 2306 	ldrd	r2, r3, [r7, #24]
200015c2:	f000 fced 	bl	20001fa0 <sal_printf>
200015c6:	2300      	movs	r3, #0
200015c8:	4618      	mov	r0, r3
200015ca:	3720      	adds	r7, #32
200015cc:	46bd      	mov	sp, r7
200015ce:	bdb0      	pop	{r4, r5, r7, pc}
200015d0:	40300234 	.word	0x40300234
200015d4:	40300230 	.word	0x40300230
200015d8:	20008e48 	.word	0x20008e48
200015dc:	20008e4c 	.word	0x20008e4c
200015e0:	20008e50 	.word	0x20008e50
200015e4:	00000000 	.word	0x00000000

200015e8 <tse_tod_set>:
200015e8:	b5f0      	push	{r4, r5, r6, r7, lr}
200015ea:	b097      	sub	sp, #92	; 0x5c
200015ec:	af04      	add	r7, sp, #16
200015ee:	463e      	mov	r6, r7
200015f0:	e886 000f 	stmia.w	r6, {r0, r1, r2, r3}
200015f4:	2300      	movs	r3, #0
200015f6:	f887 3047 	strb.w	r3, [r7, #71]	; 0x47
200015fa:	f7ff fd1f 	bl	2000103c <m7_disable_intr>
200015fe:	463b      	mov	r3, r7
20001600:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
20001602:	f7ff ff6b 	bl	200014dc <tse_tod_to_ts>
20001606:	e9c7 010a 	strd	r0, r1, [r7, #40]	; 0x28
2000160a:	e9d7 2300 	ldrd	r2, r3, [r7]
2000160e:	4984      	ldr	r1, [pc, #528]	; (20001820 <tse_tod_set+0x238>)
20001610:	e9c1 2300 	strd	r2, r3, [r1]
20001614:	68bb      	ldr	r3, [r7, #8]
20001616:	4a82      	ldr	r2, [pc, #520]	; (20001820 <tse_tod_set+0x238>)
20001618:	6093      	str	r3, [r2, #8]
2000161a:	4b82      	ldr	r3, [pc, #520]	; (20001824 <tse_tod_set+0x23c>)
2000161c:	e9d3 0100 	ldrd	r0, r1, [r3]
20001620:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
20001624:	4299      	cmp	r1, r3
20001626:	bf08      	it	eq
20001628:	4290      	cmpeq	r0, r2
2000162a:	d30a      	bcc.n	20001642 <tse_tod_set+0x5a>
2000162c:	4b7d      	ldr	r3, [pc, #500]	; (20001824 <tse_tod_set+0x23c>)
2000162e:	e9d3 0100 	ldrd	r0, r1, [r3]
20001632:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
20001636:	1a82      	subs	r2, r0, r2
20001638:	eb61 0303 	sbc.w	r3, r1, r3
2000163c:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
20001640:	e009      	b.n	20001656 <tse_tod_set+0x6e>
20001642:	4b78      	ldr	r3, [pc, #480]	; (20001824 <tse_tod_set+0x23c>)
20001644:	e9d3 2300 	ldrd	r2, r3, [r3]
20001648:	e9d7 010a 	ldrd	r0, r1, [r7, #40]	; 0x28
2000164c:	1a82      	subs	r2, r0, r2
2000164e:	eb61 0303 	sbc.w	r3, r1, r3
20001652:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
20001656:	4b72      	ldr	r3, [pc, #456]	; (20001820 <tse_tod_set+0x238>)
20001658:	689b      	ldr	r3, [r3, #8]
2000165a:	4618      	mov	r0, r3
2000165c:	f04f 0100 	mov.w	r1, #0
20001660:	e9d7 230c 	ldrd	r2, r3, [r7, #48]	; 0x30
20001664:	1812      	adds	r2, r2, r0
20001666:	eb43 0301 	adc.w	r3, r3, r1
2000166a:	e9c7 2308 	strd	r2, r3, [r7, #32]
2000166e:	e9d7 2308 	ldrd	r2, r3, [r7, #32]
20001672:	4610      	mov	r0, r2
20001674:	4619      	mov	r1, r3
20001676:	a366      	add	r3, pc, #408	; (adr r3, 20001810 <tse_tod_set+0x228>)
20001678:	e9d3 2300 	ldrd	r2, r3, [r3]
2000167c:	f006 ff3c 	bl	200084f8 <__aeabi_uldivmod>
20001680:	4b67      	ldr	r3, [pc, #412]	; (20001820 <tse_tod_set+0x238>)
20001682:	609a      	str	r2, [r3, #8]
20001684:	4b66      	ldr	r3, [pc, #408]	; (20001820 <tse_tod_set+0x238>)
20001686:	689b      	ldr	r3, [r3, #8]
20001688:	0a5b      	lsrs	r3, r3, #9
2000168a:	4a67      	ldr	r2, [pc, #412]	; (20001828 <tse_tod_set+0x240>)
2000168c:	fba2 2303 	umull	r2, r3, r2, r3
20001690:	09db      	lsrs	r3, r3, #7
20001692:	461a      	mov	r2, r3
20001694:	f04f 0300 	mov.w	r3, #0
20001698:	4961      	ldr	r1, [pc, #388]	; (20001820 <tse_tod_set+0x238>)
2000169a:	e9d1 0100 	ldrd	r0, r1, [r1]
2000169e:	1812      	adds	r2, r2, r0
200016a0:	eb43 0301 	adc.w	r3, r3, r1
200016a4:	495e      	ldr	r1, [pc, #376]	; (20001820 <tse_tod_set+0x238>)
200016a6:	e9c1 2300 	strd	r2, r3, [r1]
200016aa:	4b5d      	ldr	r3, [pc, #372]	; (20001820 <tse_tod_set+0x238>)
200016ac:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
200016ae:	f7ff ff15 	bl	200014dc <tse_tod_to_ts>
200016b2:	4602      	mov	r2, r0
200016b4:	460b      	mov	r3, r1
200016b6:	495b      	ldr	r1, [pc, #364]	; (20001824 <tse_tod_set+0x23c>)
200016b8:	e9c1 2300 	strd	r2, r3, [r1]
200016bc:	e9d7 010a 	ldrd	r0, r1, [r7, #40]	; 0x28
200016c0:	e9d7 2318 	ldrd	r2, r3, [r7, #96]	; 0x60
200016c4:	4299      	cmp	r1, r3
200016c6:	bf08      	it	eq
200016c8:	4290      	cmpeq	r0, r2
200016ca:	d309      	bcc.n	200016e0 <tse_tod_set+0xf8>
200016cc:	e9d7 010a 	ldrd	r0, r1, [r7, #40]	; 0x28
200016d0:	e9d7 2318 	ldrd	r2, r3, [r7, #96]	; 0x60
200016d4:	1a82      	subs	r2, r0, r2
200016d6:	eb61 0303 	sbc.w	r3, r1, r3
200016da:	e9c7 230e 	strd	r2, r3, [r7, #56]	; 0x38
200016de:	e00b      	b.n	200016f8 <tse_tod_set+0x110>
200016e0:	e9d7 0118 	ldrd	r0, r1, [r7, #96]	; 0x60
200016e4:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
200016e8:	1a82      	subs	r2, r0, r2
200016ea:	eb61 0303 	sbc.w	r3, r1, r3
200016ee:	e9c7 230e 	strd	r2, r3, [r7, #56]	; 0x38
200016f2:	2301      	movs	r3, #1
200016f4:	f887 3047 	strb.w	r3, [r7, #71]	; 0x47
200016f8:	e9d7 230e 	ldrd	r2, r3, [r7, #56]	; 0x38
200016fc:	f06f 407f 	mvn.w	r0, #4278190080	; 0xff000000
20001700:	f04f 0100 	mov.w	r1, #0
20001704:	4299      	cmp	r1, r3
20001706:	bf08      	it	eq
20001708:	4290      	cmpeq	r0, r2
2000170a:	d315      	bcc.n	20001738 <tse_tod_set+0x150>
2000170c:	4b47      	ldr	r3, [pc, #284]	; (2000182c <tse_tod_set+0x244>)
2000170e:	781b      	ldrb	r3, [r3, #0]
20001710:	2b00      	cmp	r3, #0
20001712:	d007      	beq.n	20001724 <tse_tod_set+0x13c>
20001714:	4846      	ldr	r0, [pc, #280]	; (20001830 <tse_tod_set+0x248>)
20001716:	e9d7 230e 	ldrd	r2, r3, [r7, #56]	; 0x38
2000171a:	f000 fc41 	bl	20001fa0 <sal_printf>
2000171e:	4b43      	ldr	r3, [pc, #268]	; (2000182c <tse_tod_set+0x244>)
20001720:	2200      	movs	r2, #0
20001722:	701a      	strb	r2, [r3, #0]
20001724:	4b43      	ldr	r3, [pc, #268]	; (20001834 <tse_tod_set+0x24c>)
20001726:	781b      	ldrb	r3, [r3, #0]
20001728:	2b00      	cmp	r3, #0
2000172a:	d104      	bne.n	20001736 <tse_tod_set+0x14e>
2000172c:	e9d7 0118 	ldrd	r0, r1, [r7, #96]	; 0x60
20001730:	f7ff fe42 	bl	200013b8 <slave_1pps_start>
20001734:	e063      	b.n	200017fe <tse_tod_set+0x216>
20001736:	e062      	b.n	200017fe <tse_tod_set+0x216>
20001738:	e9d7 230e 	ldrd	r2, r3, [r7, #56]	; 0x38
2000173c:	f06f 407f 	mvn.w	r0, #4278190080	; 0xff000000
20001740:	f04f 0100 	mov.w	r1, #0
20001744:	ea02 0200 	and.w	r2, r2, r0
20001748:	ea03 0301 	and.w	r3, r3, r1
2000174c:	e9c7 2306 	strd	r2, r3, [r7, #24]
20001750:	e9d7 2306 	ldrd	r2, r3, [r7, #24]
20001754:	4838      	ldr	r0, [pc, #224]	; (20001838 <tse_tod_set+0x250>)
20001756:	f04f 0100 	mov.w	r1, #0
2000175a:	4299      	cmp	r1, r3
2000175c:	bf08      	it	eq
2000175e:	4290      	cmpeq	r0, r2
20001760:	d308      	bcc.n	20001774 <tse_tod_set+0x18c>
20001762:	e9d7 010e 	ldrd	r0, r1, [r7, #56]	; 0x38
20001766:	e9d7 2306 	ldrd	r2, r3, [r7, #24]
2000176a:	1a82      	subs	r2, r0, r2
2000176c:	eb61 0303 	sbc.w	r3, r1, r3
20001770:	e9c7 230e 	strd	r2, r3, [r7, #56]	; 0x38
20001774:	4b2d      	ldr	r3, [pc, #180]	; (2000182c <tse_tod_set+0x244>)
20001776:	2201      	movs	r2, #1
20001778:	701a      	strb	r2, [r3, #0]
2000177a:	e9d7 230e 	ldrd	r2, r3, [r7, #56]	; 0x38
2000177e:	4313      	orrs	r3, r2
20001780:	d03d      	beq.n	200017fe <tse_tod_set+0x216>
20001782:	e9d7 010e 	ldrd	r0, r1, [r7, #56]	; 0x38
20001786:	f006 fbbf 	bl	20007f08 <__aeabi_ul2d>
2000178a:	4602      	mov	r2, r0
2000178c:	460b      	mov	r3, r1
2000178e:	4610      	mov	r0, r2
20001790:	4619      	mov	r1, r3
20001792:	a321      	add	r3, pc, #132	; (adr r3, 20001818 <tse_tod_set+0x230>)
20001794:	e9d3 2300 	ldrd	r2, r3, [r3]
20001798:	f006 fe72 	bl	20008480 <__aeabi_dcmpge>
2000179c:	4603      	mov	r3, r0
2000179e:	2b00      	cmp	r3, #0
200017a0:	d026      	beq.n	200017f0 <tse_tod_set+0x208>
200017a2:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
200017a6:	001c      	movs	r4, r3
200017a8:	2500      	movs	r5, #0
200017aa:	4623      	mov	r3, r4
200017ac:	617b      	str	r3, [r7, #20]
200017ae:	6abb      	ldr	r3, [r7, #40]	; 0x28
200017b0:	613b      	str	r3, [r7, #16]
200017b2:	f897 3047 	ldrb.w	r3, [r7, #71]	; 0x47
200017b6:	2b00      	cmp	r3, #0
200017b8:	d001      	beq.n	200017be <tse_tod_set+0x1d6>
200017ba:	4920      	ldr	r1, [pc, #128]	; (2000183c <tse_tod_set+0x254>)
200017bc:	e000      	b.n	200017c0 <tse_tod_set+0x1d8>
200017be:	4920      	ldr	r1, [pc, #128]	; (20001840 <tse_tod_set+0x258>)
200017c0:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
200017c4:	e9cd 2300 	strd	r2, r3, [sp]
200017c8:	697b      	ldr	r3, [r7, #20]
200017ca:	9302      	str	r3, [sp, #8]
200017cc:	693b      	ldr	r3, [r7, #16]
200017ce:	9303      	str	r3, [sp, #12]
200017d0:	481c      	ldr	r0, [pc, #112]	; (20001844 <tse_tod_set+0x25c>)
200017d2:	e9d7 230e 	ldrd	r2, r3, [r7, #56]	; 0x38
200017d6:	f000 fbe3 	bl	20001fa0 <sal_printf>
200017da:	2000      	movs	r0, #0
200017dc:	491a      	ldr	r1, [pc, #104]	; (20001848 <tse_tod_set+0x260>)
200017de:	697a      	ldr	r2, [r7, #20]
200017e0:	f7ff fe50 	bl	20001484 <bcm53158_reg_set>
200017e4:	2000      	movs	r0, #0
200017e6:	4919      	ldr	r1, [pc, #100]	; (2000184c <tse_tod_set+0x264>)
200017e8:	693a      	ldr	r2, [r7, #16]
200017ea:	f7ff fe4b 	bl	20001484 <bcm53158_reg_set>
200017ee:	e006      	b.n	200017fe <tse_tod_set+0x216>
200017f0:	f897 3047 	ldrb.w	r3, [r7, #71]	; 0x47
200017f4:	e9d7 010e 	ldrd	r0, r1, [r7, #56]	; 0x38
200017f8:	461a      	mov	r2, r3
200017fa:	f7ff fe9f 	bl	2000153c <tse_phase_adjust>
200017fe:	f7ff fc15 	bl	2000102c <m7_enable_intr>
20001802:	2300      	movs	r3, #0
20001804:	4618      	mov	r0, r3
20001806:	374c      	adds	r7, #76	; 0x4c
20001808:	46bd      	mov	sp, r7
2000180a:	bdf0      	pop	{r4, r5, r6, r7, pc}
2000180c:	f3af 8000 	nop.w
20001810:	3b9aca00 	.word	0x3b9aca00
20001814:	00000000 	.word	0x00000000
20001818:	b0000000 	.word	0xb0000000
2000181c:	422bf08e 	.word	0x422bf08e
20001820:	20009ba8 	.word	0x20009ba8
20001824:	20009bb8 	.word	0x20009bb8
20001828:	00044b83 	.word	0x00044b83
2000182c:	200092ac 	.word	0x200092ac
20001830:	20008e7c 	.word	0x20008e7c
20001834:	20009bc9 	.word	0x20009bc9
20001838:	007fffff 	.word	0x007fffff
2000183c:	20008e48 	.word	0x20008e48
20001840:	20008e4c 	.word	0x20008e4c
20001844:	20008eac 	.word	0x20008eac
20001848:	40300010 	.word	0x40300010
2000184c:	4030000c 	.word	0x4030000c

20001850 <tse_tod_update>:
20001850:	b5b0      	push	{r4, r5, r7, lr}
20001852:	b08c      	sub	sp, #48	; 0x30
20001854:	af00      	add	r7, sp, #0
20001856:	60f8      	str	r0, [r7, #12]
20001858:	e9c7 2300 	strd	r2, r3, [r7]
2000185c:	4b5a      	ldr	r3, [pc, #360]	; (200019c8 <tse_tod_update+0x178>)
2000185e:	e9d3 2300 	ldrd	r2, r3, [r3]
20001862:	e9d7 0100 	ldrd	r0, r1, [r7]
20001866:	4299      	cmp	r1, r3
20001868:	bf08      	it	eq
2000186a:	4290      	cmpeq	r0, r2
2000186c:	d335      	bcc.n	200018da <tse_tod_update+0x8a>
2000186e:	4b56      	ldr	r3, [pc, #344]	; (200019c8 <tse_tod_update+0x178>)
20001870:	e9d3 2300 	ldrd	r2, r3, [r3]
20001874:	e9d7 0100 	ldrd	r0, r1, [r7]
20001878:	1a82      	subs	r2, r0, r2
2000187a:	eb61 0303 	sbc.w	r3, r1, r3
2000187e:	e9c7 230a 	strd	r2, r3, [r7, #40]	; 0x28
20001882:	4b52      	ldr	r3, [pc, #328]	; (200019cc <tse_tod_update+0x17c>)
20001884:	689b      	ldr	r3, [r3, #8]
20001886:	4618      	mov	r0, r3
20001888:	f04f 0100 	mov.w	r1, #0
2000188c:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
20001890:	1812      	adds	r2, r2, r0
20001892:	eb43 0301 	adc.w	r3, r3, r1
20001896:	e9c7 2308 	strd	r2, r3, [r7, #32]
2000189a:	e9d7 2308 	ldrd	r2, r3, [r7, #32]
2000189e:	4610      	mov	r0, r2
200018a0:	4619      	mov	r1, r3
200018a2:	a347      	add	r3, pc, #284	; (adr r3, 200019c0 <tse_tod_update+0x170>)
200018a4:	e9d3 2300 	ldrd	r2, r3, [r3]
200018a8:	f006 fe26 	bl	200084f8 <__aeabi_uldivmod>
200018ac:	4b47      	ldr	r3, [pc, #284]	; (200019cc <tse_tod_update+0x17c>)
200018ae:	609a      	str	r2, [r3, #8]
200018b0:	4b46      	ldr	r3, [pc, #280]	; (200019cc <tse_tod_update+0x17c>)
200018b2:	e9d3 4500 	ldrd	r4, r5, [r3]
200018b6:	e9d7 2308 	ldrd	r2, r3, [r7, #32]
200018ba:	4610      	mov	r0, r2
200018bc:	4619      	mov	r1, r3
200018be:	a340      	add	r3, pc, #256	; (adr r3, 200019c0 <tse_tod_update+0x170>)
200018c0:	e9d3 2300 	ldrd	r2, r3, [r3]
200018c4:	f006 fe18 	bl	200084f8 <__aeabi_uldivmod>
200018c8:	4602      	mov	r2, r0
200018ca:	460b      	mov	r3, r1
200018cc:	1912      	adds	r2, r2, r4
200018ce:	eb43 0305 	adc.w	r3, r3, r5
200018d2:	493e      	ldr	r1, [pc, #248]	; (200019cc <tse_tod_update+0x17c>)
200018d4:	e9c1 2300 	strd	r2, r3, [r1]
200018d8:	e05d      	b.n	20001996 <tse_tod_update+0x146>
200018da:	4b3b      	ldr	r3, [pc, #236]	; (200019c8 <tse_tod_update+0x178>)
200018dc:	e9d3 0100 	ldrd	r0, r1, [r3]
200018e0:	e9d7 2300 	ldrd	r2, r3, [r7]
200018e4:	1a82      	subs	r2, r0, r2
200018e6:	eb61 0303 	sbc.w	r3, r1, r3
200018ea:	e9c7 230a 	strd	r2, r3, [r7, #40]	; 0x28
200018ee:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
200018f2:	4610      	mov	r0, r2
200018f4:	4619      	mov	r1, r3
200018f6:	a332      	add	r3, pc, #200	; (adr r3, 200019c0 <tse_tod_update+0x170>)
200018f8:	e9d3 2300 	ldrd	r2, r3, [r3]
200018fc:	f006 fdfc 	bl	200084f8 <__aeabi_uldivmod>
20001900:	e9c7 2306 	strd	r2, r3, [r7, #24]
20001904:	e9d7 230a 	ldrd	r2, r3, [r7, #40]	; 0x28
20001908:	4610      	mov	r0, r2
2000190a:	4619      	mov	r1, r3
2000190c:	a32c      	add	r3, pc, #176	; (adr r3, 200019c0 <tse_tod_update+0x170>)
2000190e:	e9d3 2300 	ldrd	r2, r3, [r3]
20001912:	f006 fdf1 	bl	200084f8 <__aeabi_uldivmod>
20001916:	4602      	mov	r2, r0
20001918:	460b      	mov	r3, r1
2000191a:	e9c7 2304 	strd	r2, r3, [r7, #16]
2000191e:	4b2b      	ldr	r3, [pc, #172]	; (200019cc <tse_tod_update+0x17c>)
20001920:	689b      	ldr	r3, [r3, #8]
20001922:	4618      	mov	r0, r3
20001924:	f04f 0100 	mov.w	r1, #0
20001928:	e9d7 2306 	ldrd	r2, r3, [r7, #24]
2000192c:	4299      	cmp	r1, r3
2000192e:	bf08      	it	eq
20001930:	4290      	cmpeq	r0, r2
20001932:	d21f      	bcs.n	20001974 <tse_tod_update+0x124>
20001934:	4b25      	ldr	r3, [pc, #148]	; (200019cc <tse_tod_update+0x17c>)
20001936:	e9d3 2300 	ldrd	r2, r3, [r3]
2000193a:	f112 32ff 	adds.w	r2, r2, #4294967295
2000193e:	f143 33ff 	adc.w	r3, r3, #4294967295
20001942:	4922      	ldr	r1, [pc, #136]	; (200019cc <tse_tod_update+0x17c>)
20001944:	e9c1 2300 	strd	r2, r3, [r1]
20001948:	4b20      	ldr	r3, [pc, #128]	; (200019cc <tse_tod_update+0x17c>)
2000194a:	689b      	ldr	r3, [r3, #8]
2000194c:	4618      	mov	r0, r3
2000194e:	f04f 0100 	mov.w	r1, #0
20001952:	e9d7 2306 	ldrd	r2, r3, [r7, #24]
20001956:	1a82      	subs	r2, r0, r2
20001958:	eb61 0303 	sbc.w	r3, r1, r3
2000195c:	a118      	add	r1, pc, #96	; (adr r1, 200019c0 <tse_tod_update+0x170>)
2000195e:	e9d1 0100 	ldrd	r0, r1, [r1]
20001962:	1812      	adds	r2, r2, r0
20001964:	eb43 0301 	adc.w	r3, r3, r1
20001968:	e9c7 2308 	strd	r2, r3, [r7, #32]
2000196c:	6a3b      	ldr	r3, [r7, #32]
2000196e:	4a17      	ldr	r2, [pc, #92]	; (200019cc <tse_tod_update+0x17c>)
20001970:	6093      	str	r3, [r2, #8]
20001972:	e005      	b.n	20001980 <tse_tod_update+0x130>
20001974:	4b15      	ldr	r3, [pc, #84]	; (200019cc <tse_tod_update+0x17c>)
20001976:	689a      	ldr	r2, [r3, #8]
20001978:	69bb      	ldr	r3, [r7, #24]
2000197a:	1ad3      	subs	r3, r2, r3
2000197c:	4a13      	ldr	r2, [pc, #76]	; (200019cc <tse_tod_update+0x17c>)
2000197e:	6093      	str	r3, [r2, #8]
20001980:	4b12      	ldr	r3, [pc, #72]	; (200019cc <tse_tod_update+0x17c>)
20001982:	e9d3 0100 	ldrd	r0, r1, [r3]
20001986:	e9d7 2304 	ldrd	r2, r3, [r7, #16]
2000198a:	1a82      	subs	r2, r0, r2
2000198c:	eb61 0303 	sbc.w	r3, r1, r3
20001990:	490e      	ldr	r1, [pc, #56]	; (200019cc <tse_tod_update+0x17c>)
20001992:	e9c1 2300 	strd	r2, r3, [r1]
20001996:	490c      	ldr	r1, [pc, #48]	; (200019c8 <tse_tod_update+0x178>)
20001998:	e9d7 2300 	ldrd	r2, r3, [r7]
2000199c:	e9c1 2300 	strd	r2, r3, [r1]
200019a0:	68fb      	ldr	r3, [r7, #12]
200019a2:	2b09      	cmp	r3, #9
200019a4:	d107      	bne.n	200019b6 <tse_tod_update+0x166>
200019a6:	4b0a      	ldr	r3, [pc, #40]	; (200019d0 <tse_tod_update+0x180>)
200019a8:	2201      	movs	r2, #1
200019aa:	701a      	strb	r2, [r3, #0]
200019ac:	4909      	ldr	r1, [pc, #36]	; (200019d4 <tse_tod_update+0x184>)
200019ae:	e9d7 2300 	ldrd	r2, r3, [r7]
200019b2:	e9c1 2300 	strd	r2, r3, [r1]
200019b6:	3730      	adds	r7, #48	; 0x30
200019b8:	46bd      	mov	sp, r7
200019ba:	bdb0      	pop	{r4, r5, r7, pc}
200019bc:	f3af 8000 	nop.w
200019c0:	3b9aca00 	.word	0x3b9aca00
200019c4:	00000000 	.word	0x00000000
200019c8:	20009bb8 	.word	0x20009bb8
200019cc:	20009ba8 	.word	0x20009ba8
200019d0:	20009bc8 	.word	0x20009bc8
200019d4:	20009bc0 	.word	0x20009bc0

200019d8 <rimesync_handler>:
200019d8:	e92d 43b0 	stmdb	sp!, {r4, r5, r7, r8, r9, lr}
200019dc:	b090      	sub	sp, #64	; 0x40
200019de:	af06      	add	r7, sp, #24
200019e0:	2300      	movs	r3, #0
200019e2:	61fb      	str	r3, [r7, #28]
200019e4:	4b4f      	ldr	r3, [pc, #316]	; (20001b24 <rimesync_handler+0x14c>)
200019e6:	681b      	ldr	r3, [r3, #0]
200019e8:	3301      	adds	r3, #1
200019ea:	4a4e      	ldr	r2, [pc, #312]	; (20001b24 <rimesync_handler+0x14c>)
200019ec:	6013      	str	r3, [r2, #0]
200019ee:	f7ff fb25 	bl	2000103c <m7_disable_intr>
200019f2:	f107 030c 	add.w	r3, r7, #12
200019f6:	2000      	movs	r0, #0
200019f8:	494b      	ldr	r1, [pc, #300]	; (20001b28 <rimesync_handler+0x150>)
200019fa:	461a      	mov	r2, r3
200019fc:	f7ff fcae 	bl	2000135c <bcm53158_reg_get>
20001a00:	4b4a      	ldr	r3, [pc, #296]	; (20001b2c <rimesync_handler+0x154>)
20001a02:	681b      	ldr	r3, [r3, #0]
20001a04:	3301      	adds	r3, #1
20001a06:	4a49      	ldr	r2, [pc, #292]	; (20001b2c <rimesync_handler+0x154>)
20001a08:	6013      	str	r3, [r2, #0]
20001a0a:	68fb      	ldr	r3, [r7, #12]
20001a0c:	2b00      	cmp	r3, #0
20001a0e:	d008      	beq.n	20001a22 <rimesync_handler+0x4a>
20001a10:	4b47      	ldr	r3, [pc, #284]	; (20001b30 <rimesync_handler+0x158>)
20001a12:	681b      	ldr	r3, [r3, #0]
20001a14:	3301      	adds	r3, #1
20001a16:	4a46      	ldr	r2, [pc, #280]	; (20001b30 <rimesync_handler+0x158>)
20001a18:	6013      	str	r3, [r2, #0]
20001a1a:	68fb      	ldr	r3, [r7, #12]
20001a1c:	461a      	mov	r2, r3
20001a1e:	4b45      	ldr	r3, [pc, #276]	; (20001b34 <rimesync_handler+0x15c>)
20001a20:	601a      	str	r2, [r3, #0]
20001a22:	f04f 33ff 	mov.w	r3, #4294967295
20001a26:	60bb      	str	r3, [r7, #8]
20001a28:	68bb      	ldr	r3, [r7, #8]
20001a2a:	2000      	movs	r0, #0
20001a2c:	4942      	ldr	r1, [pc, #264]	; (20001b38 <rimesync_handler+0x160>)
20001a2e:	461a      	mov	r2, r3
20001a30:	f7ff fd28 	bl	20001484 <bcm53158_reg_set>
20001a34:	f107 0308 	add.w	r3, r7, #8
20001a38:	2000      	movs	r0, #0
20001a3a:	4940      	ldr	r1, [pc, #256]	; (20001b3c <rimesync_handler+0x164>)
20001a3c:	461a      	mov	r2, r3
20001a3e:	f7ff fc8d 	bl	2000135c <bcm53158_reg_get>
20001a42:	68bb      	ldr	r3, [r7, #8]
20001a44:	f403 53fe 	and.w	r3, r3, #8128	; 0x1fc0
20001a48:	099b      	lsrs	r3, r3, #6
20001a4a:	627b      	str	r3, [r7, #36]	; 0x24
20001a4c:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20001a4e:	61fb      	str	r3, [r7, #28]
20001a50:	68bb      	ldr	r3, [r7, #8]
20001a52:	461a      	mov	r2, r3
20001a54:	4b3a      	ldr	r3, [pc, #232]	; (20001b40 <rimesync_handler+0x168>)
20001a56:	601a      	str	r2, [r3, #0]
20001a58:	69fb      	ldr	r3, [r7, #28]
20001a5a:	4a3a      	ldr	r2, [pc, #232]	; (20001b44 <rimesync_handler+0x16c>)
20001a5c:	6013      	str	r3, [r2, #0]
20001a5e:	2300      	movs	r3, #0
20001a60:	623b      	str	r3, [r7, #32]
20001a62:	e050      	b.n	20001b06 <rimesync_handler+0x12e>
20001a64:	f107 0308 	add.w	r3, r7, #8
20001a68:	2000      	movs	r0, #0
20001a6a:	4937      	ldr	r1, [pc, #220]	; (20001b48 <rimesync_handler+0x170>)
20001a6c:	461a      	mov	r2, r3
20001a6e:	f7ff fc75 	bl	2000135c <bcm53158_reg_get>
20001a72:	1d3b      	adds	r3, r7, #4
20001a74:	2000      	movs	r0, #0
20001a76:	4935      	ldr	r1, [pc, #212]	; (20001b4c <rimesync_handler+0x174>)
20001a78:	461a      	mov	r2, r3
20001a7a:	f7ff fc6f 	bl	2000135c <bcm53158_reg_get>
20001a7e:	687b      	ldr	r3, [r7, #4]
20001a80:	461a      	mov	r2, r3
20001a82:	4b33      	ldr	r3, [pc, #204]	; (20001b50 <rimesync_handler+0x178>)
20001a84:	601a      	str	r2, [r3, #0]
20001a86:	68bb      	ldr	r3, [r7, #8]
20001a88:	461a      	mov	r2, r3
20001a8a:	4b32      	ldr	r3, [pc, #200]	; (20001b54 <rimesync_handler+0x17c>)
20001a8c:	601a      	str	r2, [r3, #0]
20001a8e:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20001a90:	61bb      	str	r3, [r7, #24]
20001a92:	69bb      	ldr	r3, [r7, #24]
20001a94:	2b00      	cmp	r3, #0
20001a96:	d033      	beq.n	20001b00 <rimesync_handler+0x128>
20001a98:	687b      	ldr	r3, [r7, #4]
20001a9a:	f403 03f8 	and.w	r3, r3, #8126464	; 0x7c0000
20001a9e:	0c9b      	lsrs	r3, r3, #18
20001aa0:	61bb      	str	r3, [r7, #24]
20001aa2:	687b      	ldr	r3, [r7, #4]
20001aa4:	b29b      	uxth	r3, r3
20001aa6:	627b      	str	r3, [r7, #36]	; 0x24
20001aa8:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20001aaa:	607b      	str	r3, [r7, #4]
20001aac:	687b      	ldr	r3, [r7, #4]
20001aae:	461a      	mov	r2, r3
20001ab0:	f04f 0300 	mov.w	r3, #0
20001ab4:	0015      	movs	r5, r2
20001ab6:	2400      	movs	r4, #0
20001ab8:	68bb      	ldr	r3, [r7, #8]
20001aba:	461a      	mov	r2, r3
20001abc:	f04f 0300 	mov.w	r3, #0
20001ac0:	ea42 0204 	orr.w	r2, r2, r4
20001ac4:	ea43 0305 	orr.w	r3, r3, r5
20001ac8:	e9c7 2304 	strd	r2, r3, [r7, #16]
20001acc:	69b8      	ldr	r0, [r7, #24]
20001ace:	e9d7 2304 	ldrd	r2, r3, [r7, #16]
20001ad2:	f7ff febd 	bl	20001850 <tse_tod_update>
20001ad6:	4b20      	ldr	r3, [pc, #128]	; (20001b58 <rimesync_handler+0x180>)
20001ad8:	e9d3 8900 	ldrd	r8, r9, [r3]
20001adc:	4b1e      	ldr	r3, [pc, #120]	; (20001b58 <rimesync_handler+0x180>)
20001ade:	6899      	ldr	r1, [r3, #8]
20001ae0:	4b1e      	ldr	r3, [pc, #120]	; (20001b5c <rimesync_handler+0x184>)
20001ae2:	e9d3 2300 	ldrd	r2, r3, [r3]
20001ae6:	9100      	str	r1, [sp, #0]
20001ae8:	e9cd 2302 	strd	r2, r3, [sp, #8]
20001aec:	e9d7 2304 	ldrd	r2, r3, [r7, #16]
20001af0:	e9cd 2304 	strd	r2, r3, [sp, #16]
20001af4:	481a      	ldr	r0, [pc, #104]	; (20001b60 <rimesync_handler+0x188>)
20001af6:	69b9      	ldr	r1, [r7, #24]
20001af8:	4642      	mov	r2, r8
20001afa:	464b      	mov	r3, r9
20001afc:	f000 fa50 	bl	20001fa0 <sal_printf>
20001b00:	6a3b      	ldr	r3, [r7, #32]
20001b02:	3301      	adds	r3, #1
20001b04:	623b      	str	r3, [r7, #32]
20001b06:	6a3a      	ldr	r2, [r7, #32]
20001b08:	69fb      	ldr	r3, [r7, #28]
20001b0a:	429a      	cmp	r2, r3
20001b0c:	d3aa      	bcc.n	20001a64 <rimesync_handler+0x8c>
20001b0e:	68fb      	ldr	r3, [r7, #12]
20001b10:	2b00      	cmp	r3, #0
20001b12:	d100      	bne.n	20001b16 <rimesync_handler+0x13e>
20001b14:	e000      	b.n	20001b18 <rimesync_handler+0x140>
20001b16:	e76c      	b.n	200019f2 <rimesync_handler+0x1a>
20001b18:	f7ff fa88 	bl	2000102c <m7_enable_intr>
20001b1c:	3728      	adds	r7, #40	; 0x28
20001b1e:	46bd      	mov	sp, r7
20001b20:	e8bd 83b0 	ldmia.w	sp!, {r4, r5, r7, r8, r9, pc}
20001b24:	20009b80 	.word	0x20009b80
20001b28:	40300228 	.word	0x40300228
20001b2c:	20009b94 	.word	0x20009b94
20001b30:	20009b98 	.word	0x20009b98
20001b34:	20009b90 	.word	0x20009b90
20001b38:	4030022c 	.word	0x4030022c
20001b3c:	4030021c 	.word	0x4030021c
20001b40:	20009b8c 	.word	0x20009b8c
20001b44:	20009b88 	.word	0x20009b88
20001b48:	4030002c 	.word	0x4030002c
20001b4c:	40300030 	.word	0x40300030
20001b50:	20009ba0 	.word	0x20009ba0
20001b54:	20009b9c 	.word	0x20009b9c
20001b58:	20009ba8 	.word	0x20009ba8
20001b5c:	20009bb8 	.word	0x20009bb8
20001b60:	20008ef0 	.word	0x20008ef0

20001b64 <read_serial>:
20001b64:	b580      	push	{r7, lr}
20001b66:	b084      	sub	sp, #16
20001b68:	af00      	add	r7, sp, #0
20001b6a:	6078      	str	r0, [r7, #4]
20001b6c:	687b      	ldr	r3, [r7, #4]
20001b6e:	60fb      	str	r3, [r7, #12]
20001b70:	f000 fa3e 	bl	20001ff0 <sal_getchar>
20001b74:	4603      	mov	r3, r0
20001b76:	72fb      	strb	r3, [r7, #11]
20001b78:	7afb      	ldrb	r3, [r7, #11]
20001b7a:	2b0a      	cmp	r3, #10
20001b7c:	d007      	beq.n	20001b8e <read_serial+0x2a>
20001b7e:	2b0d      	cmp	r3, #13
20001b80:	d005      	beq.n	20001b8e <read_serial+0x2a>
20001b82:	2b00      	cmp	r3, #0
20001b84:	d10a      	bne.n	20001b9c <read_serial+0x38>
20001b86:	68fa      	ldr	r2, [r7, #12]
20001b88:	687b      	ldr	r3, [r7, #4]
20001b8a:	1ad3      	subs	r3, r2, r3
20001b8c:	e00c      	b.n	20001ba8 <read_serial+0x44>
20001b8e:	68fb      	ldr	r3, [r7, #12]
20001b90:	2200      	movs	r2, #0
20001b92:	701a      	strb	r2, [r3, #0]
20001b94:	68fa      	ldr	r2, [r7, #12]
20001b96:	687b      	ldr	r3, [r7, #4]
20001b98:	1ad3      	subs	r3, r2, r3
20001b9a:	e005      	b.n	20001ba8 <read_serial+0x44>
20001b9c:	68fb      	ldr	r3, [r7, #12]
20001b9e:	1c5a      	adds	r2, r3, #1
20001ba0:	60fa      	str	r2, [r7, #12]
20001ba2:	7afa      	ldrb	r2, [r7, #11]
20001ba4:	701a      	strb	r2, [r3, #0]
20001ba6:	e7e3      	b.n	20001b70 <read_serial+0xc>
20001ba8:	4618      	mov	r0, r3
20001baa:	3710      	adds	r7, #16
20001bac:	46bd      	mov	sp, r7
20001bae:	bd80      	pop	{r7, pc}

20001bb0 <ts_tod_string_parse>:
20001bb0:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20001bb4:	b09f      	sub	sp, #124	; 0x7c
20001bb6:	af08      	add	r7, sp, #32
20001bb8:	62f8      	str	r0, [r7, #44]	; 0x2c
20001bba:	62b9      	str	r1, [r7, #40]	; 0x28
20001bbc:	f107 0254 	add.w	r2, r7, #84	; 0x54
20001bc0:	f107 0650 	add.w	r6, r7, #80	; 0x50
20001bc4:	f107 034c 	add.w	r3, r7, #76	; 0x4c
20001bc8:	9300      	str	r3, [sp, #0]
20001bca:	f107 0348 	add.w	r3, r7, #72	; 0x48
20001bce:	9301      	str	r3, [sp, #4]
20001bd0:	f107 0344 	add.w	r3, r7, #68	; 0x44
20001bd4:	9302      	str	r3, [sp, #8]
20001bd6:	f107 0340 	add.w	r3, r7, #64	; 0x40
20001bda:	9303      	str	r3, [sp, #12]
20001bdc:	f107 0330 	add.w	r3, r7, #48	; 0x30
20001be0:	9304      	str	r3, [sp, #16]
20001be2:	f107 033c 	add.w	r3, r7, #60	; 0x3c
20001be6:	9305      	str	r3, [sp, #20]
20001be8:	f107 0338 	add.w	r3, r7, #56	; 0x38
20001bec:	9306      	str	r3, [sp, #24]
20001bee:	f107 0334 	add.w	r3, r7, #52	; 0x34
20001bf2:	9307      	str	r3, [sp, #28]
20001bf4:	6af8      	ldr	r0, [r7, #44]	; 0x2c
20001bf6:	4948      	ldr	r1, [pc, #288]	; (20001d18 <ts_tod_string_parse+0x168>)
20001bf8:	4633      	mov	r3, r6
20001bfa:	f000 fd03 	bl	20002604 <sal_sscanf>
20001bfe:	4603      	mov	r3, r0
20001c00:	2b0a      	cmp	r3, #10
20001c02:	d005      	beq.n	20001c10 <ts_tod_string_parse+0x60>
20001c04:	4845      	ldr	r0, [pc, #276]	; (20001d1c <ts_tod_string_parse+0x16c>)
20001c06:	f000 f9cb 	bl	20001fa0 <sal_printf>
20001c0a:	f04f 33ff 	mov.w	r3, #4294967295
20001c0e:	e07e      	b.n	20001d0e <ts_tod_string_parse+0x15e>
20001c10:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20001c12:	3b01      	subs	r3, #1
20001c14:	4618      	mov	r0, r3
20001c16:	f04f 0100 	mov.w	r1, #0
20001c1a:	4602      	mov	r2, r0
20001c1c:	460b      	mov	r3, r1
20001c1e:	1892      	adds	r2, r2, r2
20001c20:	eb43 0303 	adc.w	r3, r3, r3
20001c24:	1812      	adds	r2, r2, r0
20001c26:	eb43 0301 	adc.w	r3, r3, r1
20001c2a:	ea4f 1b03 	mov.w	fp, r3, lsl #4
20001c2e:	ea4b 7b12 	orr.w	fp, fp, r2, lsr #28
20001c32:	ea4f 1a02 	mov.w	sl, r2, lsl #4
20001c36:	ebba 0a02 	subs.w	sl, sl, r2
20001c3a:	eb6b 0b03 	sbc.w	fp, fp, r3
20001c3e:	ea4f 190b 	mov.w	r9, fp, lsl #4
20001c42:	ea49 791a 	orr.w	r9, r9, sl, lsr #28
20001c46:	ea4f 180a 	mov.w	r8, sl, lsl #4
20001c4a:	ebb8 080a 	subs.w	r8, r8, sl
20001c4e:	eb69 090b 	sbc.w	r9, r9, fp
20001c52:	ea4f 13c9 	mov.w	r3, r9, lsl #7
20001c56:	627b      	str	r3, [r7, #36]	; 0x24
20001c58:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20001c5a:	ea43 6358 	orr.w	r3, r3, r8, lsr #25
20001c5e:	627b      	str	r3, [r7, #36]	; 0x24
20001c60:	ea4f 13c8 	mov.w	r3, r8, lsl #7
20001c64:	623b      	str	r3, [r7, #32]
20001c66:	e9d7 8908 	ldrd	r8, r9, [r7, #32]
20001c6a:	4640      	mov	r0, r8
20001c6c:	4649      	mov	r1, r9
20001c6e:	6d7b      	ldr	r3, [r7, #84]	; 0x54
20001c70:	461a      	mov	r2, r3
20001c72:	f04f 0300 	mov.w	r3, #0
20001c76:	011e      	lsls	r6, r3, #4
20001c78:	61fe      	str	r6, [r7, #28]
20001c7a:	69fe      	ldr	r6, [r7, #28]
20001c7c:	ea46 7612 	orr.w	r6, r6, r2, lsr #28
20001c80:	61fe      	str	r6, [r7, #28]
20001c82:	0113      	lsls	r3, r2, #4
20001c84:	61bb      	str	r3, [r7, #24]
20001c86:	e9d7 2306 	ldrd	r2, r3, [r7, #24]
20001c8a:	011d      	lsls	r5, r3, #4
20001c8c:	ea45 7512 	orr.w	r5, r5, r2, lsr #28
20001c90:	0114      	lsls	r4, r2, #4
20001c92:	1aa4      	subs	r4, r4, r2
20001c94:	eb65 0503 	sbc.w	r5, r5, r3
20001c98:	012b      	lsls	r3, r5, #4
20001c9a:	617b      	str	r3, [r7, #20]
20001c9c:	697b      	ldr	r3, [r7, #20]
20001c9e:	ea43 7314 	orr.w	r3, r3, r4, lsr #28
20001ca2:	617b      	str	r3, [r7, #20]
20001ca4:	0123      	lsls	r3, r4, #4
20001ca6:	613b      	str	r3, [r7, #16]
20001ca8:	e9d7 2304 	ldrd	r2, r3, [r7, #16]
20001cac:	1b12      	subs	r2, r2, r4
20001cae:	eb63 0305 	sbc.w	r3, r3, r5
20001cb2:	1880      	adds	r0, r0, r2
20001cb4:	eb41 0103 	adc.w	r1, r1, r3
20001cb8:	6d3b      	ldr	r3, [r7, #80]	; 0x50
20001cba:	461a      	mov	r2, r3
20001cbc:	f04f 0300 	mov.w	r3, #0
20001cc0:	009c      	lsls	r4, r3, #2
20001cc2:	60fc      	str	r4, [r7, #12]
20001cc4:	68fc      	ldr	r4, [r7, #12]
20001cc6:	ea44 7492 	orr.w	r4, r4, r2, lsr #30
20001cca:	60fc      	str	r4, [r7, #12]
20001ccc:	0093      	lsls	r3, r2, #2
20001cce:	60bb      	str	r3, [r7, #8]
20001cd0:	e9d7 2302 	ldrd	r2, r3, [r7, #8]
20001cd4:	011c      	lsls	r4, r3, #4
20001cd6:	607c      	str	r4, [r7, #4]
20001cd8:	687c      	ldr	r4, [r7, #4]
20001cda:	ea44 7412 	orr.w	r4, r4, r2, lsr #28
20001cde:	607c      	str	r4, [r7, #4]
20001ce0:	0114      	lsls	r4, r2, #4
20001ce2:	603c      	str	r4, [r7, #0]
20001ce4:	e9d7 4500 	ldrd	r4, r5, [r7]
20001ce8:	1aa4      	subs	r4, r4, r2
20001cea:	eb65 0503 	sbc.w	r5, r5, r3
20001cee:	4622      	mov	r2, r4
20001cf0:	462b      	mov	r3, r5
20001cf2:	1880      	adds	r0, r0, r2
20001cf4:	eb41 0103 	adc.w	r1, r1, r3
20001cf8:	6cfb      	ldr	r3, [r7, #76]	; 0x4c
20001cfa:	461a      	mov	r2, r3
20001cfc:	f04f 0300 	mov.w	r3, #0
20001d00:	1812      	adds	r2, r2, r0
20001d02:	eb43 0301 	adc.w	r3, r3, r1
20001d06:	6ab9      	ldr	r1, [r7, #40]	; 0x28
20001d08:	e9c1 2300 	strd	r2, r3, [r1]
20001d0c:	2300      	movs	r3, #0
20001d0e:	4618      	mov	r0, r3
20001d10:	375c      	adds	r7, #92	; 0x5c
20001d12:	46bd      	mov	sp, r7
20001d14:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20001d18:	20008f24 	.word	0x20008f24
20001d1c:	20008f5c 	.word	0x20008f5c

20001d20 <main>:
20001d20:	b580      	push	{r7, lr}
20001d22:	b09c      	sub	sp, #112	; 0x70
20001d24:	af02      	add	r7, sp, #8
20001d26:	482d      	ldr	r0, [pc, #180]	; (20001ddc <main+0xbc>)
20001d28:	f000 f86a 	bl	20001e00 <sal_strlen>
20001d2c:	4603      	mov	r3, r0
20001d2e:	667b      	str	r3, [r7, #100]	; 0x64
20001d30:	f7ff f984 	bl	2000103c <m7_disable_intr>
20001d34:	2037      	movs	r0, #55	; 0x37
20001d36:	f7ff f95d 	bl	20000ff4 <m7_enable_irq>
20001d3a:	f7ff f977 	bl	2000102c <m7_enable_intr>
20001d3e:	2300      	movs	r3, #0
20001d40:	663b      	str	r3, [r7, #96]	; 0x60
20001d42:	2000      	movs	r0, #0
20001d44:	4926      	ldr	r1, [pc, #152]	; (20001de0 <main+0xc0>)
20001d46:	6e3a      	ldr	r2, [r7, #96]	; 0x60
20001d48:	f7ff fb9c 	bl	20001484 <bcm53158_reg_set>
20001d4c:	2300      	movs	r3, #0
20001d4e:	663b      	str	r3, [r7, #96]	; 0x60
20001d50:	2000      	movs	r0, #0
20001d52:	4924      	ldr	r1, [pc, #144]	; (20001de4 <main+0xc4>)
20001d54:	6e3a      	ldr	r2, [r7, #96]	; 0x60
20001d56:	f7ff fb95 	bl	20001484 <bcm53158_reg_set>
20001d5a:	2300      	movs	r3, #0
20001d5c:	663b      	str	r3, [r7, #96]	; 0x60
20001d5e:	2000      	movs	r0, #0
20001d60:	4921      	ldr	r1, [pc, #132]	; (20001de8 <main+0xc8>)
20001d62:	6e3a      	ldr	r2, [r7, #96]	; 0x60
20001d64:	f7ff fb8e 	bl	20001484 <bcm53158_reg_set>
20001d68:	2300      	movs	r3, #0
20001d6a:	663b      	str	r3, [r7, #96]	; 0x60
20001d6c:	2000      	movs	r0, #0
20001d6e:	491f      	ldr	r1, [pc, #124]	; (20001dec <main+0xcc>)
20001d70:	6e3a      	ldr	r2, [r7, #96]	; 0x60
20001d72:	f7ff fb87 	bl	20001484 <bcm53158_reg_set>
20001d76:	481e      	ldr	r0, [pc, #120]	; (20001df0 <main+0xd0>)
20001d78:	f7fe fc9a 	bl	200006b0 <sysprintf>
20001d7c:	481d      	ldr	r0, [pc, #116]	; (20001df4 <main+0xd4>)
20001d7e:	f7fe fc97 	bl	200006b0 <sysprintf>
20001d82:	f107 031c 	add.w	r3, r7, #28
20001d86:	4618      	mov	r0, r3
20001d88:	f7ff feec 	bl	20001b64 <read_serial>
20001d8c:	65f8      	str	r0, [r7, #92]	; 0x5c
20001d8e:	6dfa      	ldr	r2, [r7, #92]	; 0x5c
20001d90:	6e7b      	ldr	r3, [r7, #100]	; 0x64
20001d92:	429a      	cmp	r2, r3
20001d94:	d121      	bne.n	20001dda <main+0xba>
20001d96:	f107 021c 	add.w	r2, r7, #28
20001d9a:	463b      	mov	r3, r7
20001d9c:	4610      	mov	r0, r2
20001d9e:	4619      	mov	r1, r3
20001da0:	f7ff ff06 	bl	20001bb0 <ts_tod_string_parse>
20001da4:	4603      	mov	r3, r0
20001da6:	2b00      	cmp	r3, #0
20001da8:	d117      	bne.n	20001dda <main+0xba>
20001daa:	e9d7 2300 	ldrd	r2, r3, [r7]
20001dae:	e9c7 2302 	strd	r2, r3, [r7, #8]
20001db2:	2300      	movs	r3, #0
20001db4:	613b      	str	r3, [r7, #16]
20001db6:	4b10      	ldr	r3, [pc, #64]	; (20001df8 <main+0xd8>)
20001db8:	781b      	ldrb	r3, [r3, #0]
20001dba:	b2db      	uxtb	r3, r3
20001dbc:	2b01      	cmp	r3, #1
20001dbe:	d10c      	bne.n	20001dda <main+0xba>
20001dc0:	4b0e      	ldr	r3, [pc, #56]	; (20001dfc <main+0xdc>)
20001dc2:	e9d3 2300 	ldrd	r2, r3, [r3]
20001dc6:	e9cd 2300 	strd	r2, r3, [sp]
20001dca:	f107 0308 	add.w	r3, r7, #8
20001dce:	cb0f      	ldmia	r3, {r0, r1, r2, r3}
20001dd0:	f7ff fc0a 	bl	200015e8 <tse_tod_set>
20001dd4:	4b08      	ldr	r3, [pc, #32]	; (20001df8 <main+0xd8>)
20001dd6:	2200      	movs	r2, #0
20001dd8:	701a      	strb	r2, [r3, #0]
20001dda:	e7d2      	b.n	20001d82 <main+0x62>
20001ddc:	20008f88 	.word	0x20008f88
20001de0:	40300104 	.word	0x40300104
20001de4:	40300100 	.word	0x40300100
20001de8:	40300120 	.word	0x40300120
20001dec:	4030011c 	.word	0x4030011c
20001df0:	20008fb0 	.word	0x20008fb0
20001df4:	20008fe8 	.word	0x20008fe8
20001df8:	20009bc8 	.word	0x20009bc8
20001dfc:	20009bc0 	.word	0x20009bc0

20001e00 <sal_strlen>:
20001e00:	b580      	push	{r7, lr}
20001e02:	b082      	sub	sp, #8
20001e04:	af00      	add	r7, sp, #0
20001e06:	6078      	str	r0, [r7, #4]
20001e08:	6878      	ldr	r0, [r7, #4]
20001e0a:	f001 f82b 	bl	20002e64 <strlen>
20001e0e:	4603      	mov	r3, r0
20001e10:	4618      	mov	r0, r3
20001e12:	3708      	adds	r7, #8
20001e14:	46bd      	mov	sp, r7
20001e16:	bd80      	pop	{r7, pc}

20001e18 <sal_strchr>:
20001e18:	b580      	push	{r7, lr}
20001e1a:	b082      	sub	sp, #8
20001e1c:	af00      	add	r7, sp, #0
20001e1e:	6078      	str	r0, [r7, #4]
20001e20:	6039      	str	r1, [r7, #0]
20001e22:	6878      	ldr	r0, [r7, #4]
20001e24:	6839      	ldr	r1, [r7, #0]
20001e26:	f000 ffa7 	bl	20002d78 <strchr>
20001e2a:	4603      	mov	r3, r0
20001e2c:	4618      	mov	r0, r3
20001e2e:	3708      	adds	r7, #8
20001e30:	46bd      	mov	sp, r7
20001e32:	bd80      	pop	{r7, pc}

20001e34 <put_char>:
20001e34:	b580      	push	{r7, lr}
20001e36:	b084      	sub	sp, #16
20001e38:	af00      	add	r7, sp, #0
20001e3a:	4603      	mov	r3, r0
20001e3c:	71fb      	strb	r3, [r7, #7]
20001e3e:	2300      	movs	r3, #0
20001e40:	73fb      	strb	r3, [r7, #15]
20001e42:	7bfa      	ldrb	r2, [r7, #15]
20001e44:	f107 0308 	add.w	r3, r7, #8
20001e48:	4610      	mov	r0, r2
20001e4a:	490f      	ldr	r1, [pc, #60]	; (20001e88 <put_char+0x54>)
20001e4c:	461a      	mov	r2, r3
20001e4e:	f7ff fa85 	bl	2000135c <bcm53158_reg_get>
20001e52:	e007      	b.n	20001e64 <put_char+0x30>
20001e54:	7bfa      	ldrb	r2, [r7, #15]
20001e56:	f107 0308 	add.w	r3, r7, #8
20001e5a:	4610      	mov	r0, r2
20001e5c:	490a      	ldr	r1, [pc, #40]	; (20001e88 <put_char+0x54>)
20001e5e:	461a      	mov	r2, r3
20001e60:	f7ff fa7c 	bl	2000135c <bcm53158_reg_get>
20001e64:	68bb      	ldr	r3, [r7, #8]
20001e66:	f003 0340 	and.w	r3, r3, #64	; 0x40
20001e6a:	2b00      	cmp	r3, #0
20001e6c:	d0f2      	beq.n	20001e54 <put_char+0x20>
20001e6e:	79fb      	ldrb	r3, [r7, #7]
20001e70:	7bfa      	ldrb	r2, [r7, #15]
20001e72:	4610      	mov	r0, r2
20001e74:	4905      	ldr	r1, [pc, #20]	; (20001e8c <put_char+0x58>)
20001e76:	461a      	mov	r2, r3
20001e78:	f7ff fb04 	bl	20001484 <bcm53158_reg_set>
20001e7c:	79fb      	ldrb	r3, [r7, #7]
20001e7e:	4618      	mov	r0, r3
20001e80:	3710      	adds	r7, #16
20001e82:	46bd      	mov	sp, r7
20001e84:	bd80      	pop	{r7, pc}
20001e86:	bf00      	nop
20001e88:	41027014 	.word	0x41027014
20001e8c:	41027000 	.word	0x41027000

20001e90 <get_char>:
20001e90:	b580      	push	{r7, lr}
20001e92:	b082      	sub	sp, #8
20001e94:	af00      	add	r7, sp, #0
20001e96:	2300      	movs	r3, #0
20001e98:	71fb      	strb	r3, [r7, #7]
20001e9a:	79fa      	ldrb	r2, [r7, #7]
20001e9c:	463b      	mov	r3, r7
20001e9e:	4610      	mov	r0, r2
20001ea0:	490f      	ldr	r1, [pc, #60]	; (20001ee0 <get_char+0x50>)
20001ea2:	461a      	mov	r2, r3
20001ea4:	f7ff fa5a 	bl	2000135c <bcm53158_reg_get>
20001ea8:	e006      	b.n	20001eb8 <get_char+0x28>
20001eaa:	79fa      	ldrb	r2, [r7, #7]
20001eac:	463b      	mov	r3, r7
20001eae:	4610      	mov	r0, r2
20001eb0:	490b      	ldr	r1, [pc, #44]	; (20001ee0 <get_char+0x50>)
20001eb2:	461a      	mov	r2, r3
20001eb4:	f7ff fa52 	bl	2000135c <bcm53158_reg_get>
20001eb8:	683b      	ldr	r3, [r7, #0]
20001eba:	f003 0301 	and.w	r3, r3, #1
20001ebe:	2b00      	cmp	r3, #0
20001ec0:	d0f3      	beq.n	20001eaa <get_char+0x1a>
20001ec2:	79fa      	ldrb	r2, [r7, #7]
20001ec4:	463b      	mov	r3, r7
20001ec6:	4610      	mov	r0, r2
20001ec8:	4906      	ldr	r1, [pc, #24]	; (20001ee4 <get_char+0x54>)
20001eca:	461a      	mov	r2, r3
20001ecc:	f7ff fa46 	bl	2000135c <bcm53158_reg_get>
20001ed0:	683b      	ldr	r3, [r7, #0]
20001ed2:	71bb      	strb	r3, [r7, #6]
20001ed4:	79bb      	ldrb	r3, [r7, #6]
20001ed6:	4618      	mov	r0, r3
20001ed8:	3708      	adds	r7, #8
20001eda:	46bd      	mov	sp, r7
20001edc:	bd80      	pop	{r7, pc}
20001ede:	bf00      	nop
20001ee0:	41027014 	.word	0x41027014
20001ee4:	41027000 	.word	0x41027000

20001ee8 <um_console_write>:
20001ee8:	b580      	push	{r7, lr}
20001eea:	b084      	sub	sp, #16
20001eec:	af00      	add	r7, sp, #0
20001eee:	6078      	str	r0, [r7, #4]
20001ef0:	6039      	str	r1, [r7, #0]
20001ef2:	683b      	ldr	r3, [r7, #0]
20001ef4:	60fb      	str	r3, [r7, #12]
20001ef6:	687b      	ldr	r3, [r7, #4]
20001ef8:	60bb      	str	r3, [r7, #8]
20001efa:	e00a      	b.n	20001f12 <um_console_write+0x2a>
20001efc:	68bb      	ldr	r3, [r7, #8]
20001efe:	781b      	ldrb	r3, [r3, #0]
20001f00:	4618      	mov	r0, r3
20001f02:	f7ff ff97 	bl	20001e34 <put_char>
20001f06:	68bb      	ldr	r3, [r7, #8]
20001f08:	3301      	adds	r3, #1
20001f0a:	60bb      	str	r3, [r7, #8]
20001f0c:	68fb      	ldr	r3, [r7, #12]
20001f0e:	3b01      	subs	r3, #1
20001f10:	60fb      	str	r3, [r7, #12]
20001f12:	68fb      	ldr	r3, [r7, #12]
20001f14:	2b00      	cmp	r3, #0
20001f16:	dcf1      	bgt.n	20001efc <um_console_write+0x14>
20001f18:	2300      	movs	r3, #0
20001f1a:	4618      	mov	r0, r3
20001f1c:	3710      	adds	r7, #16
20001f1e:	46bd      	mov	sp, r7
20001f20:	bd80      	pop	{r7, pc}
20001f22:	bf00      	nop

20001f24 <um_console_print>:
20001f24:	b580      	push	{r7, lr}
20001f26:	b086      	sub	sp, #24
20001f28:	af00      	add	r7, sp, #0
20001f2a:	6078      	str	r0, [r7, #4]
20001f2c:	2300      	movs	r3, #0
20001f2e:	617b      	str	r3, [r7, #20]
20001f30:	e013      	b.n	20001f5a <um_console_print+0x36>
20001f32:	693a      	ldr	r2, [r7, #16]
20001f34:	687b      	ldr	r3, [r7, #4]
20001f36:	1ad3      	subs	r3, r2, r3
20001f38:	6878      	ldr	r0, [r7, #4]
20001f3a:	4619      	mov	r1, r3
20001f3c:	f7ff ffd4 	bl	20001ee8 <um_console_write>
20001f40:	4813      	ldr	r0, [pc, #76]	; (20001f90 <um_console_print+0x6c>)
20001f42:	2102      	movs	r1, #2
20001f44:	f7ff ffd0 	bl	20001ee8 <um_console_write>
20001f48:	693a      	ldr	r2, [r7, #16]
20001f4a:	687b      	ldr	r3, [r7, #4]
20001f4c:	1ad3      	subs	r3, r2, r3
20001f4e:	697a      	ldr	r2, [r7, #20]
20001f50:	4413      	add	r3, r2
20001f52:	617b      	str	r3, [r7, #20]
20001f54:	693b      	ldr	r3, [r7, #16]
20001f56:	3301      	adds	r3, #1
20001f58:	607b      	str	r3, [r7, #4]
20001f5a:	6878      	ldr	r0, [r7, #4]
20001f5c:	210a      	movs	r1, #10
20001f5e:	f7ff ff5b 	bl	20001e18 <sal_strchr>
20001f62:	6138      	str	r0, [r7, #16]
20001f64:	693b      	ldr	r3, [r7, #16]
20001f66:	2b00      	cmp	r3, #0
20001f68:	d1e3      	bne.n	20001f32 <um_console_print+0xe>
20001f6a:	6878      	ldr	r0, [r7, #4]
20001f6c:	f7ff ff48 	bl	20001e00 <sal_strlen>
20001f70:	4603      	mov	r3, r0
20001f72:	60fb      	str	r3, [r7, #12]
20001f74:	6878      	ldr	r0, [r7, #4]
20001f76:	68f9      	ldr	r1, [r7, #12]
20001f78:	f7ff ffb6 	bl	20001ee8 <um_console_write>
20001f7c:	697a      	ldr	r2, [r7, #20]
20001f7e:	68fb      	ldr	r3, [r7, #12]
20001f80:	4413      	add	r3, r2
20001f82:	617b      	str	r3, [r7, #20]
20001f84:	697b      	ldr	r3, [r7, #20]
20001f86:	4618      	mov	r0, r3
20001f88:	3718      	adds	r7, #24
20001f8a:	46bd      	mov	sp, r7
20001f8c:	bd80      	pop	{r7, pc}
20001f8e:	bf00      	nop
20001f90:	20009014 	.word	0x20009014

20001f94 <sal_console_init>:
20001f94:	b480      	push	{r7}
20001f96:	af00      	add	r7, sp, #0
20001f98:	46bd      	mov	sp, r7
20001f9a:	f85d 7b04 	ldr.w	r7, [sp], #4
20001f9e:	4770      	bx	lr

20001fa0 <sal_printf>:
20001fa0:	b40f      	push	{r0, r1, r2, r3}
20001fa2:	b580      	push	{r7, lr}
20001fa4:	b082      	sub	sp, #8
20001fa6:	af00      	add	r7, sp, #0
20001fa8:	f107 0314 	add.w	r3, r7, #20
20001fac:	607b      	str	r3, [r7, #4]
20001fae:	4807      	ldr	r0, [pc, #28]	; (20001fcc <sal_printf+0x2c>)
20001fb0:	6939      	ldr	r1, [r7, #16]
20001fb2:	687a      	ldr	r2, [r7, #4]
20001fb4:	f002 fa26 	bl	20004404 <vsprintf>
20001fb8:	4804      	ldr	r0, [pc, #16]	; (20001fcc <sal_printf+0x2c>)
20001fba:	f7ff ffb3 	bl	20001f24 <um_console_print>
20001fbe:	3708      	adds	r7, #8
20001fc0:	46bd      	mov	sp, r7
20001fc2:	e8bd 4080 	ldmia.w	sp!, {r7, lr}
20001fc6:	b004      	add	sp, #16
20001fc8:	4770      	bx	lr
20001fca:	bf00      	nop
20001fcc:	20009bd0 	.word	0x20009bd0

20001fd0 <sal_assert>:
20001fd0:	b580      	push	{r7, lr}
20001fd2:	b084      	sub	sp, #16
20001fd4:	af00      	add	r7, sp, #0
20001fd6:	60f8      	str	r0, [r7, #12]
20001fd8:	60b9      	str	r1, [r7, #8]
20001fda:	4613      	mov	r3, r2
20001fdc:	80fb      	strh	r3, [r7, #6]
20001fde:	88fb      	ldrh	r3, [r7, #6]
20001fe0:	4802      	ldr	r0, [pc, #8]	; (20001fec <sal_assert+0x1c>)
20001fe2:	68f9      	ldr	r1, [r7, #12]
20001fe4:	68ba      	ldr	r2, [r7, #8]
20001fe6:	f7ff ffdb 	bl	20001fa0 <sal_printf>
20001fea:	e7fe      	b.n	20001fea <sal_assert+0x1a>
20001fec:	20009018 	.word	0x20009018

20001ff0 <sal_getchar>:
20001ff0:	b580      	push	{r7, lr}
20001ff2:	af00      	add	r7, sp, #0
20001ff4:	f7ff ff4c 	bl	20001e90 <get_char>
20001ff8:	4603      	mov	r3, r0
20001ffa:	461a      	mov	r2, r3
20001ffc:	4b11      	ldr	r3, [pc, #68]	; (20002044 <sal_getchar+0x54>)
20001ffe:	701a      	strb	r2, [r3, #0]
20002000:	4b10      	ldr	r3, [pc, #64]	; (20002044 <sal_getchar+0x54>)
20002002:	781b      	ldrb	r3, [r3, #0]
20002004:	2b0a      	cmp	r3, #10
20002006:	d009      	beq.n	2000201c <sal_getchar+0x2c>
20002008:	2b0a      	cmp	r3, #10
2000200a:	dc02      	bgt.n	20002012 <sal_getchar+0x22>
2000200c:	2b08      	cmp	r3, #8
2000200e:	d014      	beq.n	2000203a <sal_getchar+0x4a>
20002010:	e009      	b.n	20002026 <sal_getchar+0x36>
20002012:	2b0d      	cmp	r3, #13
20002014:	d002      	beq.n	2000201c <sal_getchar+0x2c>
20002016:	2b7f      	cmp	r3, #127	; 0x7f
20002018:	d00f      	beq.n	2000203a <sal_getchar+0x4a>
2000201a:	e004      	b.n	20002026 <sal_getchar+0x36>
2000201c:	480a      	ldr	r0, [pc, #40]	; (20002048 <sal_getchar+0x58>)
2000201e:	2102      	movs	r1, #2
20002020:	f7ff ff62 	bl	20001ee8 <um_console_write>
20002024:	e00a      	b.n	2000203c <sal_getchar+0x4c>
20002026:	4b07      	ldr	r3, [pc, #28]	; (20002044 <sal_getchar+0x54>)
20002028:	781b      	ldrb	r3, [r3, #0]
2000202a:	2b1f      	cmp	r3, #31
2000202c:	d904      	bls.n	20002038 <sal_getchar+0x48>
2000202e:	4805      	ldr	r0, [pc, #20]	; (20002044 <sal_getchar+0x54>)
20002030:	2101      	movs	r1, #1
20002032:	f7ff ff59 	bl	20001ee8 <um_console_write>
20002036:	e001      	b.n	2000203c <sal_getchar+0x4c>
20002038:	e000      	b.n	2000203c <sal_getchar+0x4c>
2000203a:	bf00      	nop
2000203c:	4b01      	ldr	r3, [pc, #4]	; (20002044 <sal_getchar+0x54>)
2000203e:	781b      	ldrb	r3, [r3, #0]
20002040:	4618      	mov	r0, r3
20002042:	bd80      	pop	{r7, pc}
20002044:	20009bcc 	.word	0x20009bcc
20002048:	20009014 	.word	0x20009014

2000204c <sal_get_last_char>:
2000204c:	b480      	push	{r7}
2000204e:	af00      	add	r7, sp, #0
20002050:	4b03      	ldr	r3, [pc, #12]	; (20002060 <sal_get_last_char+0x14>)
20002052:	781b      	ldrb	r3, [r3, #0]
20002054:	4618      	mov	r0, r3
20002056:	46bd      	mov	sp, r7
20002058:	f85d 7b04 	ldr.w	r7, [sp], #4
2000205c:	4770      	bx	lr
2000205e:	bf00      	nop
20002060:	20009bcc 	.word	0x20009bcc

20002064 <sal_putchar>:
20002064:	b580      	push	{r7, lr}
20002066:	b082      	sub	sp, #8
20002068:	af00      	add	r7, sp, #0
2000206a:	4603      	mov	r3, r0
2000206c:	71fb      	strb	r3, [r7, #7]
2000206e:	79fb      	ldrb	r3, [r7, #7]
20002070:	2b0a      	cmp	r3, #10
20002072:	d008      	beq.n	20002086 <sal_putchar+0x22>
20002074:	2b0d      	cmp	r3, #13
20002076:	d006      	beq.n	20002086 <sal_putchar+0x22>
20002078:	2b08      	cmp	r3, #8
2000207a:	d109      	bne.n	20002090 <sal_putchar+0x2c>
2000207c:	480a      	ldr	r0, [pc, #40]	; (200020a8 <sal_putchar+0x44>)
2000207e:	2103      	movs	r1, #3
20002080:	f7ff ff32 	bl	20001ee8 <um_console_write>
20002084:	e00a      	b.n	2000209c <sal_putchar+0x38>
20002086:	4809      	ldr	r0, [pc, #36]	; (200020ac <sal_putchar+0x48>)
20002088:	2102      	movs	r1, #2
2000208a:	f7ff ff2d 	bl	20001ee8 <um_console_write>
2000208e:	e005      	b.n	2000209c <sal_putchar+0x38>
20002090:	1dfb      	adds	r3, r7, #7
20002092:	4618      	mov	r0, r3
20002094:	2101      	movs	r1, #1
20002096:	f7ff ff27 	bl	20001ee8 <um_console_write>
2000209a:	bf00      	nop
2000209c:	79fb      	ldrb	r3, [r7, #7]
2000209e:	4618      	mov	r0, r3
200020a0:	3708      	adds	r7, #8
200020a2:	46bd      	mov	sp, r7
200020a4:	bd80      	pop	{r7, pc}
200020a6:	bf00      	nop
200020a8:	20009040 	.word	0x20009040
200020ac:	20009014 	.word	0x20009014

200020b0 <get_field_len>:
200020b0:	b480      	push	{r7}
200020b2:	b085      	sub	sp, #20
200020b4:	af00      	add	r7, sp, #0
200020b6:	6078      	str	r0, [r7, #4]
200020b8:	2300      	movs	r3, #0
200020ba:	60fb      	str	r3, [r7, #12]
200020bc:	687b      	ldr	r3, [r7, #4]
200020be:	f403 5300 	and.w	r3, r3, #8192	; 0x2000
200020c2:	2b00      	cmp	r3, #0
200020c4:	d002      	beq.n	200020cc <get_field_len+0x1c>
200020c6:	2302      	movs	r3, #2
200020c8:	60fb      	str	r3, [r7, #12]
200020ca:	e011      	b.n	200020f0 <get_field_len+0x40>
200020cc:	687b      	ldr	r3, [r7, #4]
200020ce:	f403 4380 	and.w	r3, r3, #16384	; 0x4000
200020d2:	2b00      	cmp	r3, #0
200020d4:	d002      	beq.n	200020dc <get_field_len+0x2c>
200020d6:	2304      	movs	r3, #4
200020d8:	60fb      	str	r3, [r7, #12]
200020da:	e009      	b.n	200020f0 <get_field_len+0x40>
200020dc:	687b      	ldr	r3, [r7, #4]
200020de:	f403 4300 	and.w	r3, r3, #32768	; 0x8000
200020e2:	2b00      	cmp	r3, #0
200020e4:	d002      	beq.n	200020ec <get_field_len+0x3c>
200020e6:	2308      	movs	r3, #8
200020e8:	60fb      	str	r3, [r7, #12]
200020ea:	e001      	b.n	200020f0 <get_field_len+0x40>
200020ec:	2301      	movs	r3, #1
200020ee:	60fb      	str	r3, [r7, #12]
200020f0:	68fb      	ldr	r3, [r7, #12]
200020f2:	4618      	mov	r0, r3
200020f4:	3714      	adds	r7, #20
200020f6:	46bd      	mov	sp, r7
200020f8:	f85d 7b04 	ldr.w	r7, [sp], #4
200020fc:	4770      	bx	lr
200020fe:	bf00      	nop

20002100 <extract_bytes>:
20002100:	b5f0      	push	{r4, r5, r6, r7, lr}
20002102:	b093      	sub	sp, #76	; 0x4c
20002104:	af00      	add	r7, sp, #0
20002106:	60f8      	str	r0, [r7, #12]
20002108:	60b9      	str	r1, [r7, #8]
2000210a:	607a      	str	r2, [r7, #4]
2000210c:	603b      	str	r3, [r7, #0]
2000210e:	f04f 0200 	mov.w	r2, #0
20002112:	f04f 0300 	mov.w	r3, #0
20002116:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
2000211a:	2300      	movs	r3, #0
2000211c:	62fb      	str	r3, [r7, #44]	; 0x2c
2000211e:	2300      	movs	r3, #0
20002120:	62bb      	str	r3, [r7, #40]	; 0x28
20002122:	2300      	movs	r3, #0
20002124:	61bb      	str	r3, [r7, #24]
20002126:	2300      	movs	r3, #0
20002128:	61fb      	str	r3, [r7, #28]
2000212a:	6e7b      	ldr	r3, [r7, #100]	; 0x64
2000212c:	63fb      	str	r3, [r7, #60]	; 0x3c
2000212e:	68fb      	ldr	r3, [r7, #12]
20002130:	647b      	str	r3, [r7, #68]	; 0x44
20002132:	e25a      	b.n	200025ea <extract_bytes+0x4ea>
20002134:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002136:	781b      	ldrb	r3, [r3, #0]
20002138:	2b20      	cmp	r3, #32
2000213a:	d00f      	beq.n	2000215c <extract_bytes+0x5c>
2000213c:	6c7b      	ldr	r3, [r7, #68]	; 0x44
2000213e:	781b      	ldrb	r3, [r3, #0]
20002140:	2b09      	cmp	r3, #9
20002142:	d00b      	beq.n	2000215c <extract_bytes+0x5c>
20002144:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002146:	781b      	ldrb	r3, [r3, #0]
20002148:	2b0a      	cmp	r3, #10
2000214a:	d007      	beq.n	2000215c <extract_bytes+0x5c>
2000214c:	6c7b      	ldr	r3, [r7, #68]	; 0x44
2000214e:	781b      	ldrb	r3, [r3, #0]
20002150:	2b0c      	cmp	r3, #12
20002152:	d003      	beq.n	2000215c <extract_bytes+0x5c>
20002154:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002156:	781b      	ldrb	r3, [r3, #0]
20002158:	2b0d      	cmp	r3, #13
2000215a:	d100      	bne.n	2000215e <extract_bytes+0x5e>
2000215c:	e23f      	b.n	200025de <extract_bytes+0x4de>
2000215e:	68bb      	ldr	r3, [r7, #8]
20002160:	f003 0301 	and.w	r3, r3, #1
20002164:	2b00      	cmp	r3, #0
20002166:	d02a      	beq.n	200021be <extract_bytes+0xbe>
20002168:	6c7b      	ldr	r3, [r7, #68]	; 0x44
2000216a:	781b      	ldrb	r3, [r3, #0]
2000216c:	2b2d      	cmp	r3, #45	; 0x2d
2000216e:	d109      	bne.n	20002184 <extract_bytes+0x84>
20002170:	68bb      	ldr	r3, [r7, #8]
20002172:	f043 0380 	orr.w	r3, r3, #128	; 0x80
20002176:	60bb      	str	r3, [r7, #8]
20002178:	6c7b      	ldr	r3, [r7, #68]	; 0x44
2000217a:	3301      	adds	r3, #1
2000217c:	647b      	str	r3, [r7, #68]	; 0x44
2000217e:	69fb      	ldr	r3, [r7, #28]
20002180:	3301      	adds	r3, #1
20002182:	61fb      	str	r3, [r7, #28]
20002184:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002186:	781b      	ldrb	r3, [r3, #0]
20002188:	2b30      	cmp	r3, #48	; 0x30
2000218a:	d111      	bne.n	200021b0 <extract_bytes+0xb0>
2000218c:	6c7b      	ldr	r3, [r7, #68]	; 0x44
2000218e:	3301      	adds	r3, #1
20002190:	781b      	ldrb	r3, [r3, #0]
20002192:	2b78      	cmp	r3, #120	; 0x78
20002194:	d004      	beq.n	200021a0 <extract_bytes+0xa0>
20002196:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002198:	3301      	adds	r3, #1
2000219a:	781b      	ldrb	r3, [r3, #0]
2000219c:	2b58      	cmp	r3, #88	; 0x58
2000219e:	d10e      	bne.n	200021be <extract_bytes+0xbe>
200021a0:	68bb      	ldr	r3, [r7, #8]
200021a2:	f043 0308 	orr.w	r3, r3, #8
200021a6:	60bb      	str	r3, [r7, #8]
200021a8:	4ba2      	ldr	r3, [pc, #648]	; (20002434 <extract_bytes+0x334>)
200021aa:	681b      	ldr	r3, [r3, #0]
200021ac:	603b      	str	r3, [r7, #0]
200021ae:	e006      	b.n	200021be <extract_bytes+0xbe>
200021b0:	68bb      	ldr	r3, [r7, #8]
200021b2:	f043 0304 	orr.w	r3, r3, #4
200021b6:	60bb      	str	r3, [r7, #8]
200021b8:	4b9f      	ldr	r3, [pc, #636]	; (20002438 <extract_bytes+0x338>)
200021ba:	681b      	ldr	r3, [r3, #0]
200021bc:	603b      	str	r3, [r7, #0]
200021be:	68bb      	ldr	r3, [r7, #8]
200021c0:	f003 0302 	and.w	r3, r3, #2
200021c4:	2b00      	cmp	r3, #0
200021c6:	d004      	beq.n	200021d2 <extract_bytes+0xd2>
200021c8:	2301      	movs	r3, #1
200021ca:	62fb      	str	r3, [r7, #44]	; 0x2c
200021cc:	230a      	movs	r3, #10
200021ce:	62bb      	str	r3, [r7, #40]	; 0x28
200021d0:	e03a      	b.n	20002248 <extract_bytes+0x148>
200021d2:	68bb      	ldr	r3, [r7, #8]
200021d4:	f003 0304 	and.w	r3, r3, #4
200021d8:	2b00      	cmp	r3, #0
200021da:	d004      	beq.n	200021e6 <extract_bytes+0xe6>
200021dc:	2301      	movs	r3, #1
200021de:	62fb      	str	r3, [r7, #44]	; 0x2c
200021e0:	230a      	movs	r3, #10
200021e2:	62bb      	str	r3, [r7, #40]	; 0x28
200021e4:	e030      	b.n	20002248 <extract_bytes+0x148>
200021e6:	68bb      	ldr	r3, [r7, #8]
200021e8:	f003 0310 	and.w	r3, r3, #16
200021ec:	2b00      	cmp	r3, #0
200021ee:	d00e      	beq.n	2000220e <extract_bytes+0x10e>
200021f0:	2301      	movs	r3, #1
200021f2:	62fb      	str	r3, [r7, #44]	; 0x2c
200021f4:	6c7b      	ldr	r3, [r7, #68]	; 0x44
200021f6:	781b      	ldrb	r3, [r3, #0]
200021f8:	2b30      	cmp	r3, #48	; 0x30
200021fa:	d105      	bne.n	20002208 <extract_bytes+0x108>
200021fc:	6c7b      	ldr	r3, [r7, #68]	; 0x44
200021fe:	3301      	adds	r3, #1
20002200:	647b      	str	r3, [r7, #68]	; 0x44
20002202:	69fb      	ldr	r3, [r7, #28]
20002204:	3301      	adds	r3, #1
20002206:	61fb      	str	r3, [r7, #28]
20002208:	2308      	movs	r3, #8
2000220a:	62bb      	str	r3, [r7, #40]	; 0x28
2000220c:	e01c      	b.n	20002248 <extract_bytes+0x148>
2000220e:	68bb      	ldr	r3, [r7, #8]
20002210:	f003 0308 	and.w	r3, r3, #8
20002214:	2b00      	cmp	r3, #0
20002216:	d017      	beq.n	20002248 <extract_bytes+0x148>
20002218:	2301      	movs	r3, #1
2000221a:	62fb      	str	r3, [r7, #44]	; 0x2c
2000221c:	6c7b      	ldr	r3, [r7, #68]	; 0x44
2000221e:	781b      	ldrb	r3, [r3, #0]
20002220:	2b30      	cmp	r3, #48	; 0x30
20002222:	d10f      	bne.n	20002244 <extract_bytes+0x144>
20002224:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002226:	3301      	adds	r3, #1
20002228:	781b      	ldrb	r3, [r3, #0]
2000222a:	2b78      	cmp	r3, #120	; 0x78
2000222c:	d004      	beq.n	20002238 <extract_bytes+0x138>
2000222e:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002230:	3301      	adds	r3, #1
20002232:	781b      	ldrb	r3, [r3, #0]
20002234:	2b58      	cmp	r3, #88	; 0x58
20002236:	d105      	bne.n	20002244 <extract_bytes+0x144>
20002238:	6c7b      	ldr	r3, [r7, #68]	; 0x44
2000223a:	3302      	adds	r3, #2
2000223c:	647b      	str	r3, [r7, #68]	; 0x44
2000223e:	69fb      	ldr	r3, [r7, #28]
20002240:	3302      	adds	r3, #2
20002242:	61fb      	str	r3, [r7, #28]
20002244:	2310      	movs	r3, #16
20002246:	62bb      	str	r3, [r7, #40]	; 0x28
20002248:	6afb      	ldr	r3, [r7, #44]	; 0x2c
2000224a:	2b00      	cmp	r3, #0
2000224c:	f000 80f6 	beq.w	2000243c <extract_bytes+0x33c>
20002250:	68b8      	ldr	r0, [r7, #8]
20002252:	f7ff ff2d 	bl	200020b0 <get_field_len>
20002256:	61b8      	str	r0, [r7, #24]
20002258:	6e3b      	ldr	r3, [r7, #96]	; 0x60
2000225a:	2bff      	cmp	r3, #255	; 0xff
2000225c:	d101      	bne.n	20002262 <extract_bytes+0x162>
2000225e:	2310      	movs	r3, #16
20002260:	663b      	str	r3, [r7, #96]	; 0x60
20002262:	2300      	movs	r3, #0
20002264:	627b      	str	r3, [r7, #36]	; 0x24
20002266:	e0a0      	b.n	200023aa <extract_bytes+0x2aa>
20002268:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000226a:	6c7a      	ldr	r2, [r7, #68]	; 0x44
2000226c:	4413      	add	r3, r2
2000226e:	781b      	ldrb	r3, [r3, #0]
20002270:	2b20      	cmp	r3, #32
20002272:	f000 809f 	beq.w	200023b4 <extract_bytes+0x2b4>
20002276:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002278:	6c7a      	ldr	r2, [r7, #68]	; 0x44
2000227a:	4413      	add	r3, r2
2000227c:	781b      	ldrb	r3, [r3, #0]
2000227e:	2b09      	cmp	r3, #9
20002280:	f000 8098 	beq.w	200023b4 <extract_bytes+0x2b4>
20002284:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002286:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002288:	4413      	add	r3, r2
2000228a:	781b      	ldrb	r3, [r3, #0]
2000228c:	2b0a      	cmp	r3, #10
2000228e:	f000 8091 	beq.w	200023b4 <extract_bytes+0x2b4>
20002292:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002294:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002296:	4413      	add	r3, r2
20002298:	781b      	ldrb	r3, [r3, #0]
2000229a:	2b0c      	cmp	r3, #12
2000229c:	f000 808a 	beq.w	200023b4 <extract_bytes+0x2b4>
200022a0:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200022a2:	6c7a      	ldr	r2, [r7, #68]	; 0x44
200022a4:	4413      	add	r3, r2
200022a6:	781b      	ldrb	r3, [r3, #0]
200022a8:	2b0d      	cmp	r3, #13
200022aa:	f000 8083 	beq.w	200023b4 <extract_bytes+0x2b4>
200022ae:	687b      	ldr	r3, [r7, #4]
200022b0:	643b      	str	r3, [r7, #64]	; 0x40
200022b2:	683b      	ldr	r3, [r7, #0]
200022b4:	6c3a      	ldr	r2, [r7, #64]	; 0x40
200022b6:	4413      	add	r3, r2
200022b8:	617b      	str	r3, [r7, #20]
200022ba:	e002      	b.n	200022c2 <extract_bytes+0x1c2>
200022bc:	6c3b      	ldr	r3, [r7, #64]	; 0x40
200022be:	3301      	adds	r3, #1
200022c0:	643b      	str	r3, [r7, #64]	; 0x40
200022c2:	6c3a      	ldr	r2, [r7, #64]	; 0x40
200022c4:	697b      	ldr	r3, [r7, #20]
200022c6:	429a      	cmp	r2, r3
200022c8:	d207      	bcs.n	200022da <extract_bytes+0x1da>
200022ca:	6c3b      	ldr	r3, [r7, #64]	; 0x40
200022cc:	781a      	ldrb	r2, [r3, #0]
200022ce:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200022d0:	6c79      	ldr	r1, [r7, #68]	; 0x44
200022d2:	440b      	add	r3, r1
200022d4:	781b      	ldrb	r3, [r3, #0]
200022d6:	429a      	cmp	r2, r3
200022d8:	d1f0      	bne.n	200022bc <extract_bytes+0x1bc>
200022da:	6c3a      	ldr	r2, [r7, #64]	; 0x40
200022dc:	697b      	ldr	r3, [r7, #20]
200022de:	429a      	cmp	r2, r3
200022e0:	d25c      	bcs.n	2000239c <extract_bytes+0x29c>
200022e2:	6abb      	ldr	r3, [r7, #40]	; 0x28
200022e4:	461a      	mov	r2, r3
200022e6:	ea4f 73e2 	mov.w	r3, r2, asr #31
200022ea:	6b79      	ldr	r1, [r7, #52]	; 0x34
200022ec:	fb02 f001 	mul.w	r0, r2, r1
200022f0:	6b39      	ldr	r1, [r7, #48]	; 0x30
200022f2:	fb03 f101 	mul.w	r1, r3, r1
200022f6:	4401      	add	r1, r0
200022f8:	6b38      	ldr	r0, [r7, #48]	; 0x30
200022fa:	fba0 2302 	umull	r2, r3, r0, r2
200022fe:	4419      	add	r1, r3
20002300:	460b      	mov	r3, r1
20002302:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
20002306:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
2000230a:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000230c:	6c7a      	ldr	r2, [r7, #68]	; 0x44
2000230e:	4413      	add	r3, r2
20002310:	781b      	ldrb	r3, [r3, #0]
20002312:	2b40      	cmp	r3, #64	; 0x40
20002314:	d915      	bls.n	20002342 <extract_bytes+0x242>
20002316:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002318:	6c7a      	ldr	r2, [r7, #68]	; 0x44
2000231a:	4413      	add	r3, r2
2000231c:	781b      	ldrb	r3, [r3, #0]
2000231e:	2b46      	cmp	r3, #70	; 0x46
20002320:	d80f      	bhi.n	20002342 <extract_bytes+0x242>
20002322:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002324:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002326:	4413      	add	r3, r2
20002328:	781b      	ldrb	r3, [r3, #0]
2000232a:	3b37      	subs	r3, #55	; 0x37
2000232c:	461a      	mov	r2, r3
2000232e:	ea4f 73e2 	mov.w	r3, r2, asr #31
20002332:	e9d7 010c 	ldrd	r0, r1, [r7, #48]	; 0x30
20002336:	1812      	adds	r2, r2, r0
20002338:	eb43 0301 	adc.w	r3, r3, r1
2000233c:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
20002340:	e02b      	b.n	2000239a <extract_bytes+0x29a>
20002342:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002344:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002346:	4413      	add	r3, r2
20002348:	781b      	ldrb	r3, [r3, #0]
2000234a:	2b60      	cmp	r3, #96	; 0x60
2000234c:	d915      	bls.n	2000237a <extract_bytes+0x27a>
2000234e:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002350:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002352:	4413      	add	r3, r2
20002354:	781b      	ldrb	r3, [r3, #0]
20002356:	2b66      	cmp	r3, #102	; 0x66
20002358:	d80f      	bhi.n	2000237a <extract_bytes+0x27a>
2000235a:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000235c:	6c7a      	ldr	r2, [r7, #68]	; 0x44
2000235e:	4413      	add	r3, r2
20002360:	781b      	ldrb	r3, [r3, #0]
20002362:	3b57      	subs	r3, #87	; 0x57
20002364:	461a      	mov	r2, r3
20002366:	ea4f 73e2 	mov.w	r3, r2, asr #31
2000236a:	e9d7 010c 	ldrd	r0, r1, [r7, #48]	; 0x30
2000236e:	1812      	adds	r2, r2, r0
20002370:	eb43 0301 	adc.w	r3, r3, r1
20002374:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
20002378:	e00f      	b.n	2000239a <extract_bytes+0x29a>
2000237a:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000237c:	6c7a      	ldr	r2, [r7, #68]	; 0x44
2000237e:	4413      	add	r3, r2
20002380:	781b      	ldrb	r3, [r3, #0]
20002382:	3b30      	subs	r3, #48	; 0x30
20002384:	461a      	mov	r2, r3
20002386:	ea4f 73e2 	mov.w	r3, r2, asr #31
2000238a:	e9d7 010c 	ldrd	r0, r1, [r7, #48]	; 0x30
2000238e:	1812      	adds	r2, r2, r0
20002390:	eb43 0301 	adc.w	r3, r3, r1
20002394:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
20002398:	e001      	b.n	2000239e <extract_bytes+0x29e>
2000239a:	e000      	b.n	2000239e <extract_bytes+0x29e>
2000239c:	e00a      	b.n	200023b4 <extract_bytes+0x2b4>
2000239e:	69fb      	ldr	r3, [r7, #28]
200023a0:	3301      	adds	r3, #1
200023a2:	61fb      	str	r3, [r7, #28]
200023a4:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200023a6:	3301      	adds	r3, #1
200023a8:	627b      	str	r3, [r7, #36]	; 0x24
200023aa:	6a7a      	ldr	r2, [r7, #36]	; 0x24
200023ac:	6e3b      	ldr	r3, [r7, #96]	; 0x60
200023ae:	429a      	cmp	r2, r3
200023b0:	f6ff af5a 	blt.w	20002268 <extract_bytes+0x168>
200023b4:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200023b6:	2b0f      	cmp	r3, #15
200023b8:	dd05      	ble.n	200023c6 <extract_bytes+0x2c6>
200023ba:	f04f 32ff 	mov.w	r2, #4294967295
200023be:	f04f 33ff 	mov.w	r3, #4294967295
200023c2:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
200023c6:	68bb      	ldr	r3, [r7, #8]
200023c8:	f403 7380 	and.w	r3, r3, #256	; 0x100
200023cc:	2b00      	cmp	r3, #0
200023ce:	f040 8106 	bne.w	200025de <extract_bytes+0x4de>
200023d2:	68bb      	ldr	r3, [r7, #8]
200023d4:	f003 0380 	and.w	r3, r3, #128	; 0x80
200023d8:	2b00      	cmp	r3, #0
200023da:	d006      	beq.n	200023ea <extract_bytes+0x2ea>
200023dc:	e9d7 230c 	ldrd	r2, r3, [r7, #48]	; 0x30
200023e0:	4252      	negs	r2, r2
200023e2:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
200023e6:	e9c7 230c 	strd	r2, r3, [r7, #48]	; 0x30
200023ea:	2300      	movs	r3, #0
200023ec:	623b      	str	r3, [r7, #32]
200023ee:	e01b      	b.n	20002428 <extract_bytes+0x328>
200023f0:	6a3b      	ldr	r3, [r7, #32]
200023f2:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200023f4:	eb02 0e03 	add.w	lr, r2, r3
200023f8:	6a3b      	ldr	r3, [r7, #32]
200023fa:	00d9      	lsls	r1, r3, #3
200023fc:	e9d7 230c 	ldrd	r2, r3, [r7, #48]	; 0x30
20002400:	f1c1 0620 	rsb	r6, r1, #32
20002404:	f1a1 0020 	sub.w	r0, r1, #32
20002408:	fa22 f401 	lsr.w	r4, r2, r1
2000240c:	fa03 f606 	lsl.w	r6, r3, r6
20002410:	4334      	orrs	r4, r6
20002412:	fa23 f000 	lsr.w	r0, r3, r0
20002416:	4304      	orrs	r4, r0
20002418:	fa23 f501 	lsr.w	r5, r3, r1
2000241c:	b2e3      	uxtb	r3, r4
2000241e:	f88e 3000 	strb.w	r3, [lr]
20002422:	6a3b      	ldr	r3, [r7, #32]
20002424:	3301      	adds	r3, #1
20002426:	623b      	str	r3, [r7, #32]
20002428:	6a3a      	ldr	r2, [r7, #32]
2000242a:	69bb      	ldr	r3, [r7, #24]
2000242c:	429a      	cmp	r2, r3
2000242e:	dbdf      	blt.n	200023f0 <extract_bytes+0x2f0>
20002430:	69fb      	ldr	r3, [r7, #28]
20002432:	e0e2      	b.n	200025fa <extract_bytes+0x4fa>
20002434:	200092b4 	.word	0x200092b4
20002438:	200092b8 	.word	0x200092b8
2000243c:	68bb      	ldr	r3, [r7, #8]
2000243e:	f003 0340 	and.w	r3, r3, #64	; 0x40
20002442:	2b00      	cmp	r3, #0
20002444:	d00a      	beq.n	2000245c <extract_bytes+0x35c>
20002446:	68bb      	ldr	r3, [r7, #8]
20002448:	f403 7380 	and.w	r3, r3, #256	; 0x100
2000244c:	2b00      	cmp	r3, #0
2000244e:	f040 80c6 	bne.w	200025de <extract_bytes+0x4de>
20002452:	6c7b      	ldr	r3, [r7, #68]	; 0x44
20002454:	781a      	ldrb	r2, [r3, #0]
20002456:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
20002458:	701a      	strb	r2, [r3, #0]
2000245a:	e0c0      	b.n	200025de <extract_bytes+0x4de>
2000245c:	68bb      	ldr	r3, [r7, #8]
2000245e:	f003 0320 	and.w	r3, r3, #32
20002462:	2b00      	cmp	r3, #0
20002464:	d03d      	beq.n	200024e2 <extract_bytes+0x3e2>
20002466:	2300      	movs	r3, #0
20002468:	627b      	str	r3, [r7, #36]	; 0x24
2000246a:	e02f      	b.n	200024cc <extract_bytes+0x3cc>
2000246c:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000246e:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002470:	4413      	add	r3, r2
20002472:	781b      	ldrb	r3, [r3, #0]
20002474:	2b20      	cmp	r3, #32
20002476:	d017      	beq.n	200024a8 <extract_bytes+0x3a8>
20002478:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000247a:	6c7a      	ldr	r2, [r7, #68]	; 0x44
2000247c:	4413      	add	r3, r2
2000247e:	781b      	ldrb	r3, [r3, #0]
20002480:	2b09      	cmp	r3, #9
20002482:	d011      	beq.n	200024a8 <extract_bytes+0x3a8>
20002484:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002486:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002488:	4413      	add	r3, r2
2000248a:	781b      	ldrb	r3, [r3, #0]
2000248c:	2b0a      	cmp	r3, #10
2000248e:	d00b      	beq.n	200024a8 <extract_bytes+0x3a8>
20002490:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002492:	6c7a      	ldr	r2, [r7, #68]	; 0x44
20002494:	4413      	add	r3, r2
20002496:	781b      	ldrb	r3, [r3, #0]
20002498:	2b0c      	cmp	r3, #12
2000249a:	d005      	beq.n	200024a8 <extract_bytes+0x3a8>
2000249c:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000249e:	6c7a      	ldr	r2, [r7, #68]	; 0x44
200024a0:	4413      	add	r3, r2
200024a2:	781b      	ldrb	r3, [r3, #0]
200024a4:	2b0d      	cmp	r3, #13
200024a6:	d106      	bne.n	200024b6 <extract_bytes+0x3b6>
200024a8:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200024aa:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200024ac:	4413      	add	r3, r2
200024ae:	2200      	movs	r2, #0
200024b0:	701a      	strb	r2, [r3, #0]
200024b2:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200024b4:	e0a1      	b.n	200025fa <extract_bytes+0x4fa>
200024b6:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200024b8:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200024ba:	4413      	add	r3, r2
200024bc:	6a7a      	ldr	r2, [r7, #36]	; 0x24
200024be:	6c79      	ldr	r1, [r7, #68]	; 0x44
200024c0:	440a      	add	r2, r1
200024c2:	7812      	ldrb	r2, [r2, #0]
200024c4:	701a      	strb	r2, [r3, #0]
200024c6:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200024c8:	3301      	adds	r3, #1
200024ca:	627b      	str	r3, [r7, #36]	; 0x24
200024cc:	6a7a      	ldr	r2, [r7, #36]	; 0x24
200024ce:	6e3b      	ldr	r3, [r7, #96]	; 0x60
200024d0:	429a      	cmp	r2, r3
200024d2:	dbcb      	blt.n	2000246c <extract_bytes+0x36c>
200024d4:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200024d6:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200024d8:	4413      	add	r3, r2
200024da:	2200      	movs	r2, #0
200024dc:	701a      	strb	r2, [r3, #0]
200024de:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200024e0:	e08b      	b.n	200025fa <extract_bytes+0x4fa>
200024e2:	68bb      	ldr	r3, [r7, #8]
200024e4:	f403 7300 	and.w	r3, r3, #512	; 0x200
200024e8:	2b00      	cmp	r3, #0
200024ea:	d039      	beq.n	20002560 <extract_bytes+0x460>
200024ec:	2300      	movs	r3, #0
200024ee:	627b      	str	r3, [r7, #36]	; 0x24
200024f0:	e02b      	b.n	2000254a <extract_bytes+0x44a>
200024f2:	687b      	ldr	r3, [r7, #4]
200024f4:	643b      	str	r3, [r7, #64]	; 0x40
200024f6:	683b      	ldr	r3, [r7, #0]
200024f8:	6c3a      	ldr	r2, [r7, #64]	; 0x40
200024fa:	4413      	add	r3, r2
200024fc:	617b      	str	r3, [r7, #20]
200024fe:	e002      	b.n	20002506 <extract_bytes+0x406>
20002500:	6c3b      	ldr	r3, [r7, #64]	; 0x40
20002502:	3301      	adds	r3, #1
20002504:	643b      	str	r3, [r7, #64]	; 0x40
20002506:	6c3a      	ldr	r2, [r7, #64]	; 0x40
20002508:	697b      	ldr	r3, [r7, #20]
2000250a:	429a      	cmp	r2, r3
2000250c:	d207      	bcs.n	2000251e <extract_bytes+0x41e>
2000250e:	6c3b      	ldr	r3, [r7, #64]	; 0x40
20002510:	781a      	ldrb	r2, [r3, #0]
20002512:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002514:	6c79      	ldr	r1, [r7, #68]	; 0x44
20002516:	440b      	add	r3, r1
20002518:	781b      	ldrb	r3, [r3, #0]
2000251a:	429a      	cmp	r2, r3
2000251c:	d1f0      	bne.n	20002500 <extract_bytes+0x400>
2000251e:	6c3a      	ldr	r2, [r7, #64]	; 0x40
20002520:	697b      	ldr	r3, [r7, #20]
20002522:	429a      	cmp	r2, r3
20002524:	d206      	bcs.n	20002534 <extract_bytes+0x434>
20002526:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002528:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
2000252a:	4413      	add	r3, r2
2000252c:	2200      	movs	r2, #0
2000252e:	701a      	strb	r2, [r3, #0]
20002530:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002532:	e062      	b.n	200025fa <extract_bytes+0x4fa>
20002534:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002536:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
20002538:	4413      	add	r3, r2
2000253a:	6a7a      	ldr	r2, [r7, #36]	; 0x24
2000253c:	6c79      	ldr	r1, [r7, #68]	; 0x44
2000253e:	440a      	add	r2, r1
20002540:	7812      	ldrb	r2, [r2, #0]
20002542:	701a      	strb	r2, [r3, #0]
20002544:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002546:	3301      	adds	r3, #1
20002548:	627b      	str	r3, [r7, #36]	; 0x24
2000254a:	6a7a      	ldr	r2, [r7, #36]	; 0x24
2000254c:	6e3b      	ldr	r3, [r7, #96]	; 0x60
2000254e:	429a      	cmp	r2, r3
20002550:	dbcf      	blt.n	200024f2 <extract_bytes+0x3f2>
20002552:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002554:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
20002556:	4413      	add	r3, r2
20002558:	2200      	movs	r2, #0
2000255a:	701a      	strb	r2, [r3, #0]
2000255c:	6a7b      	ldr	r3, [r7, #36]	; 0x24
2000255e:	e04c      	b.n	200025fa <extract_bytes+0x4fa>
20002560:	68bb      	ldr	r3, [r7, #8]
20002562:	f403 6380 	and.w	r3, r3, #1024	; 0x400
20002566:	2b00      	cmp	r3, #0
20002568:	d039      	beq.n	200025de <extract_bytes+0x4de>
2000256a:	2300      	movs	r3, #0
2000256c:	627b      	str	r3, [r7, #36]	; 0x24
2000256e:	e02b      	b.n	200025c8 <extract_bytes+0x4c8>
20002570:	687b      	ldr	r3, [r7, #4]
20002572:	643b      	str	r3, [r7, #64]	; 0x40
20002574:	683b      	ldr	r3, [r7, #0]
20002576:	6c3a      	ldr	r2, [r7, #64]	; 0x40
20002578:	4413      	add	r3, r2
2000257a:	617b      	str	r3, [r7, #20]
2000257c:	e002      	b.n	20002584 <extract_bytes+0x484>
2000257e:	6c3b      	ldr	r3, [r7, #64]	; 0x40
20002580:	3301      	adds	r3, #1
20002582:	643b      	str	r3, [r7, #64]	; 0x40
20002584:	6c3a      	ldr	r2, [r7, #64]	; 0x40
20002586:	697b      	ldr	r3, [r7, #20]
20002588:	429a      	cmp	r2, r3
2000258a:	d207      	bcs.n	2000259c <extract_bytes+0x49c>
2000258c:	6c3b      	ldr	r3, [r7, #64]	; 0x40
2000258e:	781a      	ldrb	r2, [r3, #0]
20002590:	6a7b      	ldr	r3, [r7, #36]	; 0x24
20002592:	6c79      	ldr	r1, [r7, #68]	; 0x44
20002594:	440b      	add	r3, r1
20002596:	781b      	ldrb	r3, [r3, #0]
20002598:	429a      	cmp	r2, r3
2000259a:	d1f0      	bne.n	2000257e <extract_bytes+0x47e>
2000259c:	6c3a      	ldr	r2, [r7, #64]	; 0x40
2000259e:	697b      	ldr	r3, [r7, #20]
200025a0:	429a      	cmp	r2, r3
200025a2:	d306      	bcc.n	200025b2 <extract_bytes+0x4b2>
200025a4:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200025a6:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200025a8:	4413      	add	r3, r2
200025aa:	2200      	movs	r2, #0
200025ac:	701a      	strb	r2, [r3, #0]
200025ae:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200025b0:	e023      	b.n	200025fa <extract_bytes+0x4fa>
200025b2:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200025b4:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200025b6:	4413      	add	r3, r2
200025b8:	6a7a      	ldr	r2, [r7, #36]	; 0x24
200025ba:	6c79      	ldr	r1, [r7, #68]	; 0x44
200025bc:	440a      	add	r2, r1
200025be:	7812      	ldrb	r2, [r2, #0]
200025c0:	701a      	strb	r2, [r3, #0]
200025c2:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200025c4:	3301      	adds	r3, #1
200025c6:	627b      	str	r3, [r7, #36]	; 0x24
200025c8:	6a7a      	ldr	r2, [r7, #36]	; 0x24
200025ca:	6e3b      	ldr	r3, [r7, #96]	; 0x60
200025cc:	429a      	cmp	r2, r3
200025ce:	dbcf      	blt.n	20002570 <extract_bytes+0x470>
200025d0:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200025d2:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200025d4:	4413      	add	r3, r2
200025d6:	2200      	movs	r2, #0
200025d8:	701a      	strb	r2, [r3, #0]
200025da:	6a7b      	ldr	r3, [r7, #36]	; 0x24
200025dc:	e00d      	b.n	200025fa <extract_bytes+0x4fa>
200025de:	6bfb      	ldr	r3, [r7, #60]	; 0x3c
200025e0:	3301      	adds	r3, #1
200025e2:	63fb      	str	r3, [r7, #60]	; 0x3c
200025e4:	6c7b      	ldr	r3, [r7, #68]	; 0x44
200025e6:	3301      	adds	r3, #1
200025e8:	647b      	str	r3, [r7, #68]	; 0x44
200025ea:	6c7b      	ldr	r3, [r7, #68]	; 0x44
200025ec:	781b      	ldrb	r3, [r3, #0]
200025ee:	2b00      	cmp	r3, #0
200025f0:	f47f ada0 	bne.w	20002134 <extract_bytes+0x34>
200025f4:	6bfa      	ldr	r2, [r7, #60]	; 0x3c
200025f6:	6e7b      	ldr	r3, [r7, #100]	; 0x64
200025f8:	1ad3      	subs	r3, r2, r3
200025fa:	4618      	mov	r0, r3
200025fc:	374c      	adds	r7, #76	; 0x4c
200025fe:	46bd      	mov	sp, r7
20002600:	bdf0      	pop	{r4, r5, r6, r7, pc}
20002602:	bf00      	nop

20002604 <sal_sscanf>:
20002604:	b40e      	push	{r1, r2, r3}
20002606:	b580      	push	{r7, lr}
20002608:	b0d3      	sub	sp, #332	; 0x14c
2000260a:	af02      	add	r7, sp, #8
2000260c:	1d3b      	adds	r3, r7, #4
2000260e:	6018      	str	r0, [r3, #0]
20002610:	2300      	movs	r3, #0
20002612:	f8c7 312c 	str.w	r3, [r7, #300]	; 0x12c
20002616:	2300      	movs	r3, #0
20002618:	f8c7 3128 	str.w	r3, [r7, #296]	; 0x128
2000261c:	23ff      	movs	r3, #255	; 0xff
2000261e:	f8c7 3124 	str.w	r3, [r7, #292]	; 0x124
20002622:	2300      	movs	r3, #0
20002624:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
20002628:	2300      	movs	r3, #0
2000262a:	f8c7 311c 	str.w	r3, [r7, #284]	; 0x11c
2000262e:	2300      	movs	r3, #0
20002630:	f8c7 3114 	str.w	r3, [r7, #276]	; 0x114
20002634:	f8d7 314c 	ldr.w	r3, [r7, #332]	; 0x14c
20002638:	2b00      	cmp	r3, #0
2000263a:	d003      	beq.n	20002644 <sal_sscanf+0x40>
2000263c:	1d3b      	adds	r3, r7, #4
2000263e:	681b      	ldr	r3, [r3, #0]
20002640:	2b00      	cmp	r3, #0
20002642:	d101      	bne.n	20002648 <sal_sscanf+0x44>
20002644:	2300      	movs	r3, #0
20002646:	e219      	b.n	20002a7c <sal_sscanf+0x478>
20002648:	1d3b      	adds	r3, r7, #4
2000264a:	681b      	ldr	r3, [r3, #0]
2000264c:	f8c7 3134 	str.w	r3, [r7, #308]	; 0x134
20002650:	f507 72a8 	add.w	r2, r7, #336	; 0x150
20002654:	f107 030c 	add.w	r3, r7, #12
20002658:	601a      	str	r2, [r3, #0]
2000265a:	2301      	movs	r3, #1
2000265c:	f8c7 3118 	str.w	r3, [r7, #280]	; 0x118
20002660:	f8d7 314c 	ldr.w	r3, [r7, #332]	; 0x14c
20002664:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
20002668:	e200      	b.n	20002a6c <sal_sscanf+0x468>
2000266a:	f44f 4380 	mov.w	r3, #16384	; 0x4000
2000266e:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
20002672:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002676:	781b      	ldrb	r3, [r3, #0]
20002678:	2b20      	cmp	r3, #32
2000267a:	d013      	beq.n	200026a4 <sal_sscanf+0xa0>
2000267c:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002680:	781b      	ldrb	r3, [r3, #0]
20002682:	2b09      	cmp	r3, #9
20002684:	d00e      	beq.n	200026a4 <sal_sscanf+0xa0>
20002686:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000268a:	781b      	ldrb	r3, [r3, #0]
2000268c:	2b0a      	cmp	r3, #10
2000268e:	d009      	beq.n	200026a4 <sal_sscanf+0xa0>
20002690:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002694:	781b      	ldrb	r3, [r3, #0]
20002696:	2b0c      	cmp	r3, #12
20002698:	d004      	beq.n	200026a4 <sal_sscanf+0xa0>
2000269a:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000269e:	781b      	ldrb	r3, [r3, #0]
200026a0:	2b0d      	cmp	r3, #13
200026a2:	d109      	bne.n	200026b8 <sal_sscanf+0xb4>
200026a4:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200026a8:	3301      	adds	r3, #1
200026aa:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
200026ae:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200026b2:	781b      	ldrb	r3, [r3, #0]
200026b4:	2b00      	cmp	r3, #0
200026b6:	d1dc      	bne.n	20002672 <sal_sscanf+0x6e>
200026b8:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200026bc:	781b      	ldrb	r3, [r3, #0]
200026be:	2b25      	cmp	r3, #37	; 0x25
200026c0:	f040 81bf 	bne.w	20002a42 <sal_sscanf+0x43e>
200026c4:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200026c8:	3301      	adds	r3, #1
200026ca:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
200026ce:	f107 030c 	add.w	r3, r7, #12
200026d2:	681b      	ldr	r3, [r3, #0]
200026d4:	1d19      	adds	r1, r3, #4
200026d6:	f107 020c 	add.w	r2, r7, #12
200026da:	6011      	str	r1, [r2, #0]
200026dc:	681b      	ldr	r3, [r3, #0]
200026de:	f8c7 3110 	str.w	r3, [r7, #272]	; 0x110
200026e2:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200026e6:	781b      	ldrb	r3, [r3, #0]
200026e8:	2b2a      	cmp	r3, #42	; 0x2a
200026ea:	d10a      	bne.n	20002702 <sal_sscanf+0xfe>
200026ec:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200026f0:	f443 7380 	orr.w	r3, r3, #256	; 0x100
200026f4:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200026f8:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200026fc:	3301      	adds	r3, #1
200026fe:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
20002702:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002706:	781b      	ldrb	r3, [r3, #0]
20002708:	2b2f      	cmp	r3, #47	; 0x2f
2000270a:	d92a      	bls.n	20002762 <sal_sscanf+0x15e>
2000270c:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002710:	781b      	ldrb	r3, [r3, #0]
20002712:	2b39      	cmp	r3, #57	; 0x39
20002714:	d825      	bhi.n	20002762 <sal_sscanf+0x15e>
20002716:	2300      	movs	r3, #0
20002718:	f8c7 3124 	str.w	r3, [r7, #292]	; 0x124
2000271c:	e01c      	b.n	20002758 <sal_sscanf+0x154>
2000271e:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002722:	781b      	ldrb	r3, [r3, #0]
20002724:	2b2f      	cmp	r3, #47	; 0x2f
20002726:	d91c      	bls.n	20002762 <sal_sscanf+0x15e>
20002728:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000272c:	781b      	ldrb	r3, [r3, #0]
2000272e:	2b39      	cmp	r3, #57	; 0x39
20002730:	d817      	bhi.n	20002762 <sal_sscanf+0x15e>
20002732:	f8d7 2124 	ldr.w	r2, [r7, #292]	; 0x124
20002736:	4613      	mov	r3, r2
20002738:	009b      	lsls	r3, r3, #2
2000273a:	4413      	add	r3, r2
2000273c:	005b      	lsls	r3, r3, #1
2000273e:	461a      	mov	r2, r3
20002740:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002744:	781b      	ldrb	r3, [r3, #0]
20002746:	4413      	add	r3, r2
20002748:	3b30      	subs	r3, #48	; 0x30
2000274a:	f8c7 3124 	str.w	r3, [r7, #292]	; 0x124
2000274e:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002752:	3301      	adds	r3, #1
20002754:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
20002758:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000275c:	781b      	ldrb	r3, [r3, #0]
2000275e:	2b00      	cmp	r3, #0
20002760:	d1dd      	bne.n	2000271e <sal_sscanf+0x11a>
20002762:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002766:	781b      	ldrb	r3, [r3, #0]
20002768:	2b5b      	cmp	r3, #91	; 0x5b
2000276a:	d16f      	bne.n	2000284c <sal_sscanf+0x248>
2000276c:	f107 0310 	add.w	r3, r7, #16
20002770:	4618      	mov	r0, r3
20002772:	2100      	movs	r1, #0
20002774:	f44f 7280 	mov.w	r2, #256	; 0x100
20002778:	f000 f9f8 	bl	20002b6c <sal_memset>
2000277c:	f107 0310 	add.w	r3, r7, #16
20002780:	f8c7 3138 	str.w	r3, [r7, #312]	; 0x138
20002784:	2300      	movs	r3, #0
20002786:	f8c7 3118 	str.w	r3, [r7, #280]	; 0x118
2000278a:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000278e:	3301      	adds	r3, #1
20002790:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
20002794:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002798:	781b      	ldrb	r3, [r3, #0]
2000279a:	2b5e      	cmp	r3, #94	; 0x5e
2000279c:	d106      	bne.n	200027ac <sal_sscanf+0x1a8>
2000279e:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200027a2:	f443 7300 	orr.w	r3, r3, #512	; 0x200
200027a6:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200027aa:	e005      	b.n	200027b8 <sal_sscanf+0x1b4>
200027ac:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200027b0:	f443 6380 	orr.w	r3, r3, #1024	; 0x400
200027b4:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200027b8:	e00e      	b.n	200027d8 <sal_sscanf+0x1d4>
200027ba:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200027be:	781b      	ldrb	r3, [r3, #0]
200027c0:	2b25      	cmp	r3, #37	; 0x25
200027c2:	d100      	bne.n	200027c6 <sal_sscanf+0x1c2>
200027c4:	e012      	b.n	200027ec <sal_sscanf+0x1e8>
200027c6:	f8d7 3138 	ldr.w	r3, [r7, #312]	; 0x138
200027ca:	1c5a      	adds	r2, r3, #1
200027cc:	f8c7 2138 	str.w	r2, [r7, #312]	; 0x138
200027d0:	f8d7 2130 	ldr.w	r2, [r7, #304]	; 0x130
200027d4:	7812      	ldrb	r2, [r2, #0]
200027d6:	701a      	strb	r2, [r3, #0]
200027d8:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200027dc:	3301      	adds	r3, #1
200027de:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
200027e2:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200027e6:	781b      	ldrb	r3, [r3, #0]
200027e8:	2b00      	cmp	r3, #0
200027ea:	d1e6      	bne.n	200027ba <sal_sscanf+0x1b6>
200027ec:	f8d7 2138 	ldr.w	r2, [r7, #312]	; 0x138
200027f0:	f107 0310 	add.w	r3, r7, #16
200027f4:	1ad3      	subs	r3, r2, r3
200027f6:	f8c7 313c 	str.w	r3, [r7, #316]	; 0x13c
200027fa:	e01c      	b.n	20002836 <sal_sscanf+0x232>
200027fc:	f107 0210 	add.w	r2, r7, #16
20002800:	f8d7 313c 	ldr.w	r3, [r7, #316]	; 0x13c
20002804:	4413      	add	r3, r2
20002806:	781b      	ldrb	r3, [r3, #0]
20002808:	2b5d      	cmp	r3, #93	; 0x5d
2000280a:	d10f      	bne.n	2000282c <sal_sscanf+0x228>
2000280c:	f107 0210 	add.w	r2, r7, #16
20002810:	f8d7 313c 	ldr.w	r3, [r7, #316]	; 0x13c
20002814:	4413      	add	r3, r2
20002816:	2200      	movs	r2, #0
20002818:	701a      	strb	r2, [r3, #0]
2000281a:	f8d7 313c 	ldr.w	r3, [r7, #316]	; 0x13c
2000281e:	f8c7 3128 	str.w	r3, [r7, #296]	; 0x128
20002822:	f107 0310 	add.w	r3, r7, #16
20002826:	f8c7 312c 	str.w	r3, [r7, #300]	; 0x12c
2000282a:	e008      	b.n	2000283e <sal_sscanf+0x23a>
2000282c:	f8d7 313c 	ldr.w	r3, [r7, #316]	; 0x13c
20002830:	3b01      	subs	r3, #1
20002832:	f8c7 313c 	str.w	r3, [r7, #316]	; 0x13c
20002836:	f8d7 313c 	ldr.w	r3, [r7, #316]	; 0x13c
2000283a:	2b00      	cmp	r3, #0
2000283c:	dcde      	bgt.n	200027fc <sal_sscanf+0x1f8>
2000283e:	f8d7 313c 	ldr.w	r3, [r7, #316]	; 0x13c
20002842:	2b00      	cmp	r3, #0
20002844:	dc78      	bgt.n	20002938 <sal_sscanf+0x334>
20002846:	f8d7 311c 	ldr.w	r3, [r7, #284]	; 0x11c
2000284a:	e117      	b.n	20002a7c <sal_sscanf+0x478>
2000284c:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002850:	781b      	ldrb	r3, [r3, #0]
20002852:	2b6e      	cmp	r3, #110	; 0x6e
20002854:	d109      	bne.n	2000286a <sal_sscanf+0x266>
20002856:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
2000285a:	f443 6300 	orr.w	r3, r3, #2048	; 0x800
2000285e:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
20002862:	2300      	movs	r3, #0
20002864:	f8c7 3118 	str.w	r3, [r7, #280]	; 0x118
20002868:	e066      	b.n	20002938 <sal_sscanf+0x334>
2000286a:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000286e:	781b      	ldrb	r3, [r3, #0]
20002870:	2b73      	cmp	r3, #115	; 0x73
20002872:	d109      	bne.n	20002888 <sal_sscanf+0x284>
20002874:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
20002878:	f043 0320 	orr.w	r3, r3, #32
2000287c:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
20002880:	2300      	movs	r3, #0
20002882:	f8c7 3118 	str.w	r3, [r7, #280]	; 0x118
20002886:	e057      	b.n	20002938 <sal_sscanf+0x334>
20002888:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000288c:	781b      	ldrb	r3, [r3, #0]
2000288e:	2b63      	cmp	r3, #99	; 0x63
20002890:	d109      	bne.n	200028a6 <sal_sscanf+0x2a2>
20002892:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
20002896:	f043 0340 	orr.w	r3, r3, #64	; 0x40
2000289a:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
2000289e:	2300      	movs	r3, #0
200028a0:	f8c7 3118 	str.w	r3, [r7, #280]	; 0x118
200028a4:	e048      	b.n	20002938 <sal_sscanf+0x334>
200028a6:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200028aa:	781b      	ldrb	r3, [r3, #0]
200028ac:	2b68      	cmp	r3, #104	; 0x68
200028ae:	d122      	bne.n	200028f6 <sal_sscanf+0x2f2>
200028b0:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200028b4:	f423 4380 	bic.w	r3, r3, #16384	; 0x4000
200028b8:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200028bc:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200028c0:	3301      	adds	r3, #1
200028c2:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
200028c6:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200028ca:	781b      	ldrb	r3, [r3, #0]
200028cc:	2b68      	cmp	r3, #104	; 0x68
200028ce:	d10b      	bne.n	200028e8 <sal_sscanf+0x2e4>
200028d0:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200028d4:	f443 5380 	orr.w	r3, r3, #4096	; 0x1000
200028d8:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200028dc:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200028e0:	3301      	adds	r3, #1
200028e2:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
200028e6:	e027      	b.n	20002938 <sal_sscanf+0x334>
200028e8:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200028ec:	f443 5300 	orr.w	r3, r3, #8192	; 0x2000
200028f0:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200028f4:	e020      	b.n	20002938 <sal_sscanf+0x334>
200028f6:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200028fa:	781b      	ldrb	r3, [r3, #0]
200028fc:	2b6c      	cmp	r3, #108	; 0x6c
200028fe:	d11b      	bne.n	20002938 <sal_sscanf+0x334>
20002900:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002904:	3301      	adds	r3, #1
20002906:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
2000290a:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000290e:	781b      	ldrb	r3, [r3, #0]
20002910:	2b6c      	cmp	r3, #108	; 0x6c
20002912:	d10b      	bne.n	2000292c <sal_sscanf+0x328>
20002914:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
20002918:	f443 4300 	orr.w	r3, r3, #32768	; 0x8000
2000291c:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
20002920:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002924:	3301      	adds	r3, #1
20002926:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
2000292a:	e005      	b.n	20002938 <sal_sscanf+0x334>
2000292c:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
20002930:	f443 4380 	orr.w	r3, r3, #16384	; 0x4000
20002934:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
20002938:	f8d7 3118 	ldr.w	r3, [r7, #280]	; 0x118
2000293c:	2b00      	cmp	r3, #0
2000293e:	d04b      	beq.n	200029d8 <sal_sscanf+0x3d4>
20002940:	4b52      	ldr	r3, [pc, #328]	; (20002a8c <sal_sscanf+0x488>)
20002942:	681b      	ldr	r3, [r3, #0]
20002944:	f8c7 312c 	str.w	r3, [r7, #300]	; 0x12c
20002948:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000294c:	781b      	ldrb	r3, [r3, #0]
2000294e:	2b75      	cmp	r3, #117	; 0x75
20002950:	d10a      	bne.n	20002968 <sal_sscanf+0x364>
20002952:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
20002956:	f043 0303 	orr.w	r3, r3, #3
2000295a:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
2000295e:	4b4c      	ldr	r3, [pc, #304]	; (20002a90 <sal_sscanf+0x48c>)
20002960:	681b      	ldr	r3, [r3, #0]
20002962:	f8c7 3128 	str.w	r3, [r7, #296]	; 0x128
20002966:	e037      	b.n	200029d8 <sal_sscanf+0x3d4>
20002968:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
2000296c:	781b      	ldrb	r3, [r3, #0]
2000296e:	2b78      	cmp	r3, #120	; 0x78
20002970:	d004      	beq.n	2000297c <sal_sscanf+0x378>
20002972:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002976:	781b      	ldrb	r3, [r3, #0]
20002978:	2b58      	cmp	r3, #88	; 0x58
2000297a:	d10a      	bne.n	20002992 <sal_sscanf+0x38e>
2000297c:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
20002980:	f043 0308 	orr.w	r3, r3, #8
20002984:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
20002988:	4b42      	ldr	r3, [pc, #264]	; (20002a94 <sal_sscanf+0x490>)
2000298a:	681b      	ldr	r3, [r3, #0]
2000298c:	f8c7 3128 	str.w	r3, [r7, #296]	; 0x128
20002990:	e022      	b.n	200029d8 <sal_sscanf+0x3d4>
20002992:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002996:	781b      	ldrb	r3, [r3, #0]
20002998:	2b6f      	cmp	r3, #111	; 0x6f
2000299a:	d10a      	bne.n	200029b2 <sal_sscanf+0x3ae>
2000299c:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200029a0:	f043 0310 	orr.w	r3, r3, #16
200029a4:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200029a8:	4b3b      	ldr	r3, [pc, #236]	; (20002a98 <sal_sscanf+0x494>)
200029aa:	681b      	ldr	r3, [r3, #0]
200029ac:	f8c7 3128 	str.w	r3, [r7, #296]	; 0x128
200029b0:	e012      	b.n	200029d8 <sal_sscanf+0x3d4>
200029b2:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
200029b6:	781b      	ldrb	r3, [r3, #0]
200029b8:	2b64      	cmp	r3, #100	; 0x64
200029ba:	d10a      	bne.n	200029d2 <sal_sscanf+0x3ce>
200029bc:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200029c0:	f043 0304 	orr.w	r3, r3, #4
200029c4:	f8c7 3120 	str.w	r3, [r7, #288]	; 0x120
200029c8:	4b31      	ldr	r3, [pc, #196]	; (20002a90 <sal_sscanf+0x48c>)
200029ca:	681b      	ldr	r3, [r3, #0]
200029cc:	f8c7 3128 	str.w	r3, [r7, #296]	; 0x128
200029d0:	e002      	b.n	200029d8 <sal_sscanf+0x3d4>
200029d2:	f8d7 311c 	ldr.w	r3, [r7, #284]	; 0x11c
200029d6:	e051      	b.n	20002a7c <sal_sscanf+0x478>
200029d8:	f8d7 3120 	ldr.w	r3, [r7, #288]	; 0x120
200029dc:	f403 6300 	and.w	r3, r3, #2048	; 0x800
200029e0:	2b00      	cmp	r3, #0
200029e2:	d008      	beq.n	200029f6 <sal_sscanf+0x3f2>
200029e4:	f8d7 2134 	ldr.w	r2, [r7, #308]	; 0x134
200029e8:	1d3b      	adds	r3, r7, #4
200029ea:	681b      	ldr	r3, [r3, #0]
200029ec:	1ad2      	subs	r2, r2, r3
200029ee:	f8d7 3110 	ldr.w	r3, [r7, #272]	; 0x110
200029f2:	601a      	str	r2, [r3, #0]
200029f4:	e035      	b.n	20002a62 <sal_sscanf+0x45e>
200029f6:	f8d7 3124 	ldr.w	r3, [r7, #292]	; 0x124
200029fa:	9300      	str	r3, [sp, #0]
200029fc:	f8d7 3110 	ldr.w	r3, [r7, #272]	; 0x110
20002a00:	9301      	str	r3, [sp, #4]
20002a02:	f8d7 0134 	ldr.w	r0, [r7, #308]	; 0x134
20002a06:	f8d7 1120 	ldr.w	r1, [r7, #288]	; 0x120
20002a0a:	f8d7 212c 	ldr.w	r2, [r7, #300]	; 0x12c
20002a0e:	f8d7 3128 	ldr.w	r3, [r7, #296]	; 0x128
20002a12:	f7ff fb75 	bl	20002100 <extract_bytes>
20002a16:	f8c7 0114 	str.w	r0, [r7, #276]	; 0x114
20002a1a:	f8d7 3114 	ldr.w	r3, [r7, #276]	; 0x114
20002a1e:	2b00      	cmp	r3, #0
20002a20:	dc02      	bgt.n	20002a28 <sal_sscanf+0x424>
20002a22:	f8d7 311c 	ldr.w	r3, [r7, #284]	; 0x11c
20002a26:	e029      	b.n	20002a7c <sal_sscanf+0x478>
20002a28:	f8d7 3114 	ldr.w	r3, [r7, #276]	; 0x114
20002a2c:	f8d7 2134 	ldr.w	r2, [r7, #308]	; 0x134
20002a30:	4413      	add	r3, r2
20002a32:	f8c7 3134 	str.w	r3, [r7, #308]	; 0x134
20002a36:	f8d7 311c 	ldr.w	r3, [r7, #284]	; 0x11c
20002a3a:	3301      	adds	r3, #1
20002a3c:	f8c7 311c 	str.w	r3, [r7, #284]	; 0x11c
20002a40:	e00f      	b.n	20002a62 <sal_sscanf+0x45e>
20002a42:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002a46:	781a      	ldrb	r2, [r3, #0]
20002a48:	f8d7 3134 	ldr.w	r3, [r7, #308]	; 0x134
20002a4c:	781b      	ldrb	r3, [r3, #0]
20002a4e:	429a      	cmp	r2, r3
20002a50:	d002      	beq.n	20002a58 <sal_sscanf+0x454>
20002a52:	f8d7 311c 	ldr.w	r3, [r7, #284]	; 0x11c
20002a56:	e011      	b.n	20002a7c <sal_sscanf+0x478>
20002a58:	f8d7 3134 	ldr.w	r3, [r7, #308]	; 0x134
20002a5c:	3301      	adds	r3, #1
20002a5e:	f8c7 3134 	str.w	r3, [r7, #308]	; 0x134
20002a62:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002a66:	3301      	adds	r3, #1
20002a68:	f8c7 3130 	str.w	r3, [r7, #304]	; 0x130
20002a6c:	f8d7 3130 	ldr.w	r3, [r7, #304]	; 0x130
20002a70:	781b      	ldrb	r3, [r3, #0]
20002a72:	2b00      	cmp	r3, #0
20002a74:	f47f adf9 	bne.w	2000266a <sal_sscanf+0x66>
20002a78:	f8d7 311c 	ldr.w	r3, [r7, #284]	; 0x11c
20002a7c:	4618      	mov	r0, r3
20002a7e:	f507 77a2 	add.w	r7, r7, #324	; 0x144
20002a82:	46bd      	mov	sp, r7
20002a84:	e8bd 4080 	ldmia.w	sp!, {r7, lr}
20002a88:	b003      	add	sp, #12
20002a8a:	4770      	bx	lr
20002a8c:	200092b0 	.word	0x200092b0
20002a90:	200092b8 	.word	0x200092b8
20002a94:	200092b4 	.word	0x200092b4
20002a98:	200092bc 	.word	0x200092bc

20002a9c <sal_memcmp>:
20002a9c:	b480      	push	{r7}
20002a9e:	b087      	sub	sp, #28
20002aa0:	af00      	add	r7, sp, #0
20002aa2:	60f8      	str	r0, [r7, #12]
20002aa4:	60b9      	str	r1, [r7, #8]
20002aa6:	607a      	str	r2, [r7, #4]
20002aa8:	68fb      	ldr	r3, [r7, #12]
20002aaa:	617b      	str	r3, [r7, #20]
20002aac:	68bb      	ldr	r3, [r7, #8]
20002aae:	613b      	str	r3, [r7, #16]
20002ab0:	68fb      	ldr	r3, [r7, #12]
20002ab2:	2b00      	cmp	r3, #0
20002ab4:	d104      	bne.n	20002ac0 <sal_memcmp+0x24>
20002ab6:	68bb      	ldr	r3, [r7, #8]
20002ab8:	2b00      	cmp	r3, #0
20002aba:	d101      	bne.n	20002ac0 <sal_memcmp+0x24>
20002abc:	2300      	movs	r3, #0
20002abe:	e027      	b.n	20002b10 <sal_memcmp+0x74>
20002ac0:	68fb      	ldr	r3, [r7, #12]
20002ac2:	2b00      	cmp	r3, #0
20002ac4:	d002      	beq.n	20002acc <sal_memcmp+0x30>
20002ac6:	68bb      	ldr	r3, [r7, #8]
20002ac8:	2b00      	cmp	r3, #0
20002aca:	d102      	bne.n	20002ad2 <sal_memcmp+0x36>
20002acc:	f04f 33ff 	mov.w	r3, #4294967295
20002ad0:	e01e      	b.n	20002b10 <sal_memcmp+0x74>
20002ad2:	e019      	b.n	20002b08 <sal_memcmp+0x6c>
20002ad4:	697b      	ldr	r3, [r7, #20]
20002ad6:	781a      	ldrb	r2, [r3, #0]
20002ad8:	693b      	ldr	r3, [r7, #16]
20002ada:	781b      	ldrb	r3, [r3, #0]
20002adc:	429a      	cmp	r2, r3
20002ade:	d202      	bcs.n	20002ae6 <sal_memcmp+0x4a>
20002ae0:	f04f 33ff 	mov.w	r3, #4294967295
20002ae4:	e014      	b.n	20002b10 <sal_memcmp+0x74>
20002ae6:	697b      	ldr	r3, [r7, #20]
20002ae8:	781a      	ldrb	r2, [r3, #0]
20002aea:	693b      	ldr	r3, [r7, #16]
20002aec:	781b      	ldrb	r3, [r3, #0]
20002aee:	429a      	cmp	r2, r3
20002af0:	d901      	bls.n	20002af6 <sal_memcmp+0x5a>
20002af2:	2301      	movs	r3, #1
20002af4:	e00c      	b.n	20002b10 <sal_memcmp+0x74>
20002af6:	697b      	ldr	r3, [r7, #20]
20002af8:	3301      	adds	r3, #1
20002afa:	617b      	str	r3, [r7, #20]
20002afc:	693b      	ldr	r3, [r7, #16]
20002afe:	3301      	adds	r3, #1
20002b00:	613b      	str	r3, [r7, #16]
20002b02:	687b      	ldr	r3, [r7, #4]
20002b04:	3b01      	subs	r3, #1
20002b06:	607b      	str	r3, [r7, #4]
20002b08:	687b      	ldr	r3, [r7, #4]
20002b0a:	2b00      	cmp	r3, #0
20002b0c:	d1e2      	bne.n	20002ad4 <sal_memcmp+0x38>
20002b0e:	2300      	movs	r3, #0
20002b10:	4618      	mov	r0, r3
20002b12:	371c      	adds	r7, #28
20002b14:	46bd      	mov	sp, r7
20002b16:	f85d 7b04 	ldr.w	r7, [sp], #4
20002b1a:	4770      	bx	lr

20002b1c <sal_memcpy>:
20002b1c:	b480      	push	{r7}
20002b1e:	b087      	sub	sp, #28
20002b20:	af00      	add	r7, sp, #0
20002b22:	60f8      	str	r0, [r7, #12]
20002b24:	60b9      	str	r1, [r7, #8]
20002b26:	607a      	str	r2, [r7, #4]
20002b28:	68fb      	ldr	r3, [r7, #12]
20002b2a:	617b      	str	r3, [r7, #20]
20002b2c:	68bb      	ldr	r3, [r7, #8]
20002b2e:	613b      	str	r3, [r7, #16]
20002b30:	693b      	ldr	r3, [r7, #16]
20002b32:	2b00      	cmp	r3, #0
20002b34:	d002      	beq.n	20002b3c <sal_memcpy+0x20>
20002b36:	697b      	ldr	r3, [r7, #20]
20002b38:	2b00      	cmp	r3, #0
20002b3a:	d101      	bne.n	20002b40 <sal_memcpy+0x24>
20002b3c:	2300      	movs	r3, #0
20002b3e:	e00f      	b.n	20002b60 <sal_memcpy+0x44>
20002b40:	e00a      	b.n	20002b58 <sal_memcpy+0x3c>
20002b42:	697b      	ldr	r3, [r7, #20]
20002b44:	1c5a      	adds	r2, r3, #1
20002b46:	617a      	str	r2, [r7, #20]
20002b48:	693a      	ldr	r2, [r7, #16]
20002b4a:	1c51      	adds	r1, r2, #1
20002b4c:	6139      	str	r1, [r7, #16]
20002b4e:	7812      	ldrb	r2, [r2, #0]
20002b50:	701a      	strb	r2, [r3, #0]
20002b52:	687b      	ldr	r3, [r7, #4]
20002b54:	3b01      	subs	r3, #1
20002b56:	607b      	str	r3, [r7, #4]
20002b58:	687b      	ldr	r3, [r7, #4]
20002b5a:	2b00      	cmp	r3, #0
20002b5c:	d1f1      	bne.n	20002b42 <sal_memcpy+0x26>
20002b5e:	68fb      	ldr	r3, [r7, #12]
20002b60:	4618      	mov	r0, r3
20002b62:	371c      	adds	r7, #28
20002b64:	46bd      	mov	sp, r7
20002b66:	f85d 7b04 	ldr.w	r7, [sp], #4
20002b6a:	4770      	bx	lr

20002b6c <sal_memset>:
20002b6c:	b480      	push	{r7}
20002b6e:	b087      	sub	sp, #28
20002b70:	af00      	add	r7, sp, #0
20002b72:	60f8      	str	r0, [r7, #12]
20002b74:	60b9      	str	r1, [r7, #8]
20002b76:	607a      	str	r2, [r7, #4]
20002b78:	68fb      	ldr	r3, [r7, #12]
20002b7a:	2b00      	cmp	r3, #0
20002b7c:	d101      	bne.n	20002b82 <sal_memset+0x16>
20002b7e:	2300      	movs	r3, #0
20002b80:	e00f      	b.n	20002ba2 <sal_memset+0x36>
20002b82:	68fb      	ldr	r3, [r7, #12]
20002b84:	617b      	str	r3, [r7, #20]
20002b86:	e008      	b.n	20002b9a <sal_memset+0x2e>
20002b88:	697b      	ldr	r3, [r7, #20]
20002b8a:	1c5a      	adds	r2, r3, #1
20002b8c:	617a      	str	r2, [r7, #20]
20002b8e:	68ba      	ldr	r2, [r7, #8]
20002b90:	b2d2      	uxtb	r2, r2
20002b92:	701a      	strb	r2, [r3, #0]
20002b94:	687b      	ldr	r3, [r7, #4]
20002b96:	3b01      	subs	r3, #1
20002b98:	607b      	str	r3, [r7, #4]
20002b9a:	687b      	ldr	r3, [r7, #4]
20002b9c:	2b00      	cmp	r3, #0
20002b9e:	d1f3      	bne.n	20002b88 <sal_memset+0x1c>
20002ba0:	68fb      	ldr	r3, [r7, #12]
20002ba2:	4618      	mov	r0, r3
20002ba4:	371c      	adds	r7, #28
20002ba6:	46bd      	mov	sp, r7
20002ba8:	f85d 7b04 	ldr.w	r7, [sp], #4
20002bac:	4770      	bx	lr
20002bae:	bf00      	nop

20002bb0 <sal_memmove>:
20002bb0:	b480      	push	{r7}
20002bb2:	b089      	sub	sp, #36	; 0x24
20002bb4:	af00      	add	r7, sp, #0
20002bb6:	60f8      	str	r0, [r7, #12]
20002bb8:	60b9      	str	r1, [r7, #8]
20002bba:	607a      	str	r2, [r7, #4]
20002bbc:	68fb      	ldr	r3, [r7, #12]
20002bbe:	61fb      	str	r3, [r7, #28]
20002bc0:	68bb      	ldr	r3, [r7, #8]
20002bc2:	61bb      	str	r3, [r7, #24]
20002bc4:	687b      	ldr	r3, [r7, #4]
20002bc6:	617b      	str	r3, [r7, #20]
20002bc8:	69ba      	ldr	r2, [r7, #24]
20002bca:	69fb      	ldr	r3, [r7, #28]
20002bcc:	429a      	cmp	r2, r3
20002bce:	d101      	bne.n	20002bd4 <sal_memmove+0x24>
20002bd0:	69fb      	ldr	r3, [r7, #28]
20002bd2:	e02d      	b.n	20002c30 <sal_memmove+0x80>
20002bd4:	69bb      	ldr	r3, [r7, #24]
20002bd6:	2b00      	cmp	r3, #0
20002bd8:	d002      	beq.n	20002be0 <sal_memmove+0x30>
20002bda:	69fb      	ldr	r3, [r7, #28]
20002bdc:	2b00      	cmp	r3, #0
20002bde:	d101      	bne.n	20002be4 <sal_memmove+0x34>
20002be0:	2300      	movs	r3, #0
20002be2:	e025      	b.n	20002c30 <sal_memmove+0x80>
20002be4:	69ba      	ldr	r2, [r7, #24]
20002be6:	69fb      	ldr	r3, [r7, #28]
20002be8:	429a      	cmp	r2, r3
20002bea:	d90f      	bls.n	20002c0c <sal_memmove+0x5c>
20002bec:	e00a      	b.n	20002c04 <sal_memmove+0x54>
20002bee:	69fb      	ldr	r3, [r7, #28]
20002bf0:	1c5a      	adds	r2, r3, #1
20002bf2:	61fa      	str	r2, [r7, #28]
20002bf4:	69ba      	ldr	r2, [r7, #24]
20002bf6:	1c51      	adds	r1, r2, #1
20002bf8:	61b9      	str	r1, [r7, #24]
20002bfa:	7812      	ldrb	r2, [r2, #0]
20002bfc:	701a      	strb	r2, [r3, #0]
20002bfe:	697b      	ldr	r3, [r7, #20]
20002c00:	3b01      	subs	r3, #1
20002c02:	617b      	str	r3, [r7, #20]
20002c04:	697b      	ldr	r3, [r7, #20]
20002c06:	2b00      	cmp	r3, #0
20002c08:	d1f1      	bne.n	20002bee <sal_memmove+0x3e>
20002c0a:	e010      	b.n	20002c2e <sal_memmove+0x7e>
20002c0c:	e00c      	b.n	20002c28 <sal_memmove+0x78>
20002c0e:	697b      	ldr	r3, [r7, #20]
20002c10:	3b01      	subs	r3, #1
20002c12:	69fa      	ldr	r2, [r7, #28]
20002c14:	4413      	add	r3, r2
20002c16:	697a      	ldr	r2, [r7, #20]
20002c18:	3a01      	subs	r2, #1
20002c1a:	69b9      	ldr	r1, [r7, #24]
20002c1c:	440a      	add	r2, r1
20002c1e:	7812      	ldrb	r2, [r2, #0]
20002c20:	701a      	strb	r2, [r3, #0]
20002c22:	697b      	ldr	r3, [r7, #20]
20002c24:	3b01      	subs	r3, #1
20002c26:	617b      	str	r3, [r7, #20]
20002c28:	697b      	ldr	r3, [r7, #20]
20002c2a:	2b00      	cmp	r3, #0
20002c2c:	d1ef      	bne.n	20002c0e <sal_memmove+0x5e>
20002c2e:	68fb      	ldr	r3, [r7, #12]
20002c30:	4618      	mov	r0, r3
20002c32:	3724      	adds	r7, #36	; 0x24
20002c34:	46bd      	mov	sp, r7
20002c36:	f85d 7b04 	ldr.w	r7, [sp], #4
20002c3a:	4770      	bx	lr

20002c3c <sal_memchr>:
20002c3c:	b480      	push	{r7}
20002c3e:	b087      	sub	sp, #28
20002c40:	af00      	add	r7, sp, #0
20002c42:	60f8      	str	r0, [r7, #12]
20002c44:	60b9      	str	r1, [r7, #8]
20002c46:	607a      	str	r2, [r7, #4]
20002c48:	68fb      	ldr	r3, [r7, #12]
20002c4a:	2b00      	cmp	r3, #0
20002c4c:	d016      	beq.n	20002c7c <sal_memchr+0x40>
20002c4e:	2300      	movs	r3, #0
20002c50:	617b      	str	r3, [r7, #20]
20002c52:	e00f      	b.n	20002c74 <sal_memchr+0x38>
20002c54:	697b      	ldr	r3, [r7, #20]
20002c56:	68fa      	ldr	r2, [r7, #12]
20002c58:	4413      	add	r3, r2
20002c5a:	781b      	ldrb	r3, [r3, #0]
20002c5c:	461a      	mov	r2, r3
20002c5e:	68bb      	ldr	r3, [r7, #8]
20002c60:	b2db      	uxtb	r3, r3
20002c62:	429a      	cmp	r2, r3
20002c64:	d103      	bne.n	20002c6e <sal_memchr+0x32>
20002c66:	697b      	ldr	r3, [r7, #20]
20002c68:	68fa      	ldr	r2, [r7, #12]
20002c6a:	4413      	add	r3, r2
20002c6c:	e007      	b.n	20002c7e <sal_memchr+0x42>
20002c6e:	697b      	ldr	r3, [r7, #20]
20002c70:	3301      	adds	r3, #1
20002c72:	617b      	str	r3, [r7, #20]
20002c74:	697a      	ldr	r2, [r7, #20]
20002c76:	687b      	ldr	r3, [r7, #4]
20002c78:	429a      	cmp	r2, r3
20002c7a:	d3eb      	bcc.n	20002c54 <sal_memchr+0x18>
20002c7c:	2300      	movs	r3, #0
20002c7e:	4618      	mov	r0, r3
20002c80:	371c      	adds	r7, #28
20002c82:	46bd      	mov	sp, r7
20002c84:	f85d 7b04 	ldr.w	r7, [sp], #4
20002c88:	4770      	bx	lr
20002c8a:	bf00      	nop

20002c8c <_sal_swap16>:
20002c8c:	b480      	push	{r7}
20002c8e:	b083      	sub	sp, #12
20002c90:	af00      	add	r7, sp, #0
20002c92:	4603      	mov	r3, r0
20002c94:	80fb      	strh	r3, [r7, #6]
20002c96:	88fb      	ldrh	r3, [r7, #6]
20002c98:	021b      	lsls	r3, r3, #8
20002c9a:	b29a      	uxth	r2, r3
20002c9c:	88fb      	ldrh	r3, [r7, #6]
20002c9e:	0a1b      	lsrs	r3, r3, #8
20002ca0:	b29b      	uxth	r3, r3
20002ca2:	b29b      	uxth	r3, r3
20002ca4:	4313      	orrs	r3, r2
20002ca6:	b29b      	uxth	r3, r3
20002ca8:	b29b      	uxth	r3, r3
20002caa:	4618      	mov	r0, r3
20002cac:	370c      	adds	r7, #12
20002cae:	46bd      	mov	sp, r7
20002cb0:	f85d 7b04 	ldr.w	r7, [sp], #4
20002cb4:	4770      	bx	lr
20002cb6:	bf00      	nop

20002cb8 <_sal_swap32>:
20002cb8:	b480      	push	{r7}
20002cba:	b083      	sub	sp, #12
20002cbc:	af00      	add	r7, sp, #0
20002cbe:	6078      	str	r0, [r7, #4]
20002cc0:	687b      	ldr	r3, [r7, #4]
20002cc2:	ea4f 4333 	mov.w	r3, r3, ror #16
20002cc6:	607b      	str	r3, [r7, #4]
20002cc8:	687b      	ldr	r3, [r7, #4]
20002cca:	f423 037f 	bic.w	r3, r3, #16711680	; 0xff0000
20002cce:	0a1a      	lsrs	r2, r3, #8
20002cd0:	687b      	ldr	r3, [r7, #4]
20002cd2:	f423 437f 	bic.w	r3, r3, #65280	; 0xff00
20002cd6:	021b      	lsls	r3, r3, #8
20002cd8:	4313      	orrs	r3, r2
20002cda:	4618      	mov	r0, r3
20002cdc:	370c      	adds	r7, #12
20002cde:	46bd      	mov	sp, r7
20002ce0:	f85d 7b04 	ldr.w	r7, [sp], #4
20002ce4:	4770      	bx	lr
20002ce6:	bf00      	nop

20002ce8 <_sprintf_r>:
20002ce8:	b40c      	push	{r2, r3}
20002cea:	b5f0      	push	{r4, r5, r6, r7, lr}
20002cec:	b09d      	sub	sp, #116	; 0x74
20002cee:	460e      	mov	r6, r1
20002cf0:	f44f 7e02 	mov.w	lr, #520	; 0x208
20002cf4:	f06f 4500 	mvn.w	r5, #2147483648	; 0x80000000
20002cf8:	ac22      	add	r4, sp, #136	; 0x88
20002cfa:	f64f 77ff 	movw	r7, #65535	; 0xffff
20002cfe:	a902      	add	r1, sp, #8
20002d00:	9602      	str	r6, [sp, #8]
20002d02:	f854 2b04 	ldr.w	r2, [r4], #4
20002d06:	9606      	str	r6, [sp, #24]
20002d08:	4623      	mov	r3, r4
20002d0a:	9401      	str	r4, [sp, #4]
20002d0c:	f8ad e014 	strh.w	lr, [sp, #20]
20002d10:	9504      	str	r5, [sp, #16]
20002d12:	9507      	str	r5, [sp, #28]
20002d14:	f8ad 7016 	strh.w	r7, [sp, #22]
20002d18:	f000 f8d2 	bl	20002ec0 <_svfprintf_r>
20002d1c:	9b02      	ldr	r3, [sp, #8]
20002d1e:	2200      	movs	r2, #0
20002d20:	701a      	strb	r2, [r3, #0]
20002d22:	b01d      	add	sp, #116	; 0x74
20002d24:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
20002d28:	b002      	add	sp, #8
20002d2a:	4770      	bx	lr

20002d2c <sprintf>:
20002d2c:	b40e      	push	{r1, r2, r3}
20002d2e:	b5f0      	push	{r4, r5, r6, r7, lr}
20002d30:	b09c      	sub	sp, #112	; 0x70
20002d32:	4b10      	ldr	r3, [pc, #64]	; (20002d74 <sprintf+0x48>)
20002d34:	4606      	mov	r6, r0
20002d36:	f44f 7e02 	mov.w	lr, #520	; 0x208
20002d3a:	ac21      	add	r4, sp, #132	; 0x84
20002d3c:	f06f 4500 	mvn.w	r5, #2147483648	; 0x80000000
20002d40:	f64f 77ff 	movw	r7, #65535	; 0xffff
20002d44:	6818      	ldr	r0, [r3, #0]
20002d46:	f854 2b04 	ldr.w	r2, [r4], #4
20002d4a:	a902      	add	r1, sp, #8
20002d4c:	9602      	str	r6, [sp, #8]
20002d4e:	4623      	mov	r3, r4
20002d50:	9606      	str	r6, [sp, #24]
20002d52:	9401      	str	r4, [sp, #4]
20002d54:	f8ad e014 	strh.w	lr, [sp, #20]
20002d58:	9504      	str	r5, [sp, #16]
20002d5a:	9507      	str	r5, [sp, #28]
20002d5c:	f8ad 7016 	strh.w	r7, [sp, #22]
20002d60:	f000 f8ae 	bl	20002ec0 <_svfprintf_r>
20002d64:	9b02      	ldr	r3, [sp, #8]
20002d66:	2200      	movs	r2, #0
20002d68:	701a      	strb	r2, [r3, #0]
20002d6a:	b01c      	add	sp, #112	; 0x70
20002d6c:	e8bd 40f0 	ldmia.w	sp!, {r4, r5, r6, r7, lr}
20002d70:	b003      	add	sp, #12
20002d72:	4770      	bx	lr
20002d74:	200096f0 	.word	0x200096f0

20002d78 <strchr>:
20002d78:	b2c9      	uxtb	r1, r1
20002d7a:	b470      	push	{r4, r5, r6}
20002d7c:	2900      	cmp	r1, #0
20002d7e:	d040      	beq.n	20002e02 <strchr+0x8a>
20002d80:	0784      	lsls	r4, r0, #30
20002d82:	d010      	beq.n	20002da6 <strchr+0x2e>
20002d84:	7803      	ldrb	r3, [r0, #0]
20002d86:	2b00      	cmp	r3, #0
20002d88:	d068      	beq.n	20002e5c <strchr+0xe4>
20002d8a:	4299      	cmp	r1, r3
20002d8c:	bf18      	it	ne
20002d8e:	1c43      	addne	r3, r0, #1
20002d90:	d106      	bne.n	20002da0 <strchr+0x28>
20002d92:	e034      	b.n	20002dfe <strchr+0x86>
20002d94:	f813 2b01 	ldrb.w	r2, [r3], #1
20002d98:	2a00      	cmp	r2, #0
20002d9a:	d05c      	beq.n	20002e56 <strchr+0xde>
20002d9c:	4291      	cmp	r1, r2
20002d9e:	d02e      	beq.n	20002dfe <strchr+0x86>
20002da0:	079a      	lsls	r2, r3, #30
20002da2:	4618      	mov	r0, r3
20002da4:	d1f6      	bne.n	20002d94 <strchr+0x1c>
20002da6:	ea41 2601 	orr.w	r6, r1, r1, lsl #8
20002daa:	6804      	ldr	r4, [r0, #0]
20002dac:	ea46 4606 	orr.w	r6, r6, r6, lsl #16
20002db0:	f1a4 3201 	sub.w	r2, r4, #16843009	; 0x1010101
20002db4:	ea84 0506 	eor.w	r5, r4, r6
20002db8:	ea22 0204 	bic.w	r2, r2, r4
20002dbc:	f1a5 3301 	sub.w	r3, r5, #16843009	; 0x1010101
20002dc0:	ea23 0305 	bic.w	r3, r3, r5
20002dc4:	4313      	orrs	r3, r2
20002dc6:	f013 3f80 	tst.w	r3, #2155905152	; 0x80808080
20002dca:	d10f      	bne.n	20002dec <strchr+0x74>
20002dcc:	f850 5f04 	ldr.w	r5, [r0, #4]!
20002dd0:	ea86 0305 	eor.w	r3, r6, r5
20002dd4:	f1a5 3201 	sub.w	r2, r5, #16843009	; 0x1010101
20002dd8:	f1a3 3401 	sub.w	r4, r3, #16843009	; 0x1010101
20002ddc:	ea22 0205 	bic.w	r2, r2, r5
20002de0:	ea24 0303 	bic.w	r3, r4, r3
20002de4:	4313      	orrs	r3, r2
20002de6:	f013 3f80 	tst.w	r3, #2155905152	; 0x80808080
20002dea:	d0ef      	beq.n	20002dcc <strchr+0x54>
20002dec:	7803      	ldrb	r3, [r0, #0]
20002dee:	b923      	cbnz	r3, 20002dfa <strchr+0x82>
20002df0:	e034      	b.n	20002e5c <strchr+0xe4>
20002df2:	f810 3f01 	ldrb.w	r3, [r0, #1]!
20002df6:	2b00      	cmp	r3, #0
20002df8:	d030      	beq.n	20002e5c <strchr+0xe4>
20002dfa:	4299      	cmp	r1, r3
20002dfc:	d1f9      	bne.n	20002df2 <strchr+0x7a>
20002dfe:	bc70      	pop	{r4, r5, r6}
20002e00:	4770      	bx	lr
20002e02:	0781      	lsls	r1, r0, #30
20002e04:	d00d      	beq.n	20002e22 <strchr+0xaa>
20002e06:	7803      	ldrb	r3, [r0, #0]
20002e08:	2b00      	cmp	r3, #0
20002e0a:	d0f8      	beq.n	20002dfe <strchr+0x86>
20002e0c:	1c43      	adds	r3, r0, #1
20002e0e:	e002      	b.n	20002e16 <strchr+0x9e>
20002e10:	7802      	ldrb	r2, [r0, #0]
20002e12:	2a00      	cmp	r2, #0
20002e14:	d0f3      	beq.n	20002dfe <strchr+0x86>
20002e16:	f013 0f03 	tst.w	r3, #3
20002e1a:	4618      	mov	r0, r3
20002e1c:	f103 0301 	add.w	r3, r3, #1
20002e20:	d1f6      	bne.n	20002e10 <strchr+0x98>
20002e22:	6802      	ldr	r2, [r0, #0]
20002e24:	f1a2 3301 	sub.w	r3, r2, #16843009	; 0x1010101
20002e28:	ea23 0302 	bic.w	r3, r3, r2
20002e2c:	f013 3f80 	tst.w	r3, #2155905152	; 0x80808080
20002e30:	d108      	bne.n	20002e44 <strchr+0xcc>
20002e32:	f850 2f04 	ldr.w	r2, [r0, #4]!
20002e36:	f1a2 3301 	sub.w	r3, r2, #16843009	; 0x1010101
20002e3a:	ea23 0302 	bic.w	r3, r3, r2
20002e3e:	f013 3f80 	tst.w	r3, #2155905152	; 0x80808080
20002e42:	d0f6      	beq.n	20002e32 <strchr+0xba>
20002e44:	7803      	ldrb	r3, [r0, #0]
20002e46:	2b00      	cmp	r3, #0
20002e48:	d0d9      	beq.n	20002dfe <strchr+0x86>
20002e4a:	f810 3f01 	ldrb.w	r3, [r0, #1]!
20002e4e:	2b00      	cmp	r3, #0
20002e50:	d1fb      	bne.n	20002e4a <strchr+0xd2>
20002e52:	bc70      	pop	{r4, r5, r6}
20002e54:	4770      	bx	lr
20002e56:	4610      	mov	r0, r2
20002e58:	bc70      	pop	{r4, r5, r6}
20002e5a:	4770      	bx	lr
20002e5c:	4618      	mov	r0, r3
20002e5e:	bc70      	pop	{r4, r5, r6}
20002e60:	4770      	bx	lr
20002e62:	bf00      	nop

20002e64 <strlen>:
20002e64:	f020 0103 	bic.w	r1, r0, #3
20002e68:	f010 0003 	ands.w	r0, r0, #3
20002e6c:	f1c0 0000 	rsb	r0, r0, #0
20002e70:	f851 3b04 	ldr.w	r3, [r1], #4
20002e74:	f100 0c04 	add.w	ip, r0, #4
20002e78:	ea4f 0ccc 	mov.w	ip, ip, lsl #3
20002e7c:	f06f 0200 	mvn.w	r2, #0
20002e80:	bf1c      	itt	ne
20002e82:	fa22 f20c 	lsrne.w	r2, r2, ip
20002e86:	4313      	orrne	r3, r2
20002e88:	f04f 0c01 	mov.w	ip, #1
20002e8c:	ea4c 2c0c 	orr.w	ip, ip, ip, lsl #8
20002e90:	ea4c 4c0c 	orr.w	ip, ip, ip, lsl #16
20002e94:	eba3 020c 	sub.w	r2, r3, ip
20002e98:	ea22 0203 	bic.w	r2, r2, r3
20002e9c:	ea12 12cc 	ands.w	r2, r2, ip, lsl #7
20002ea0:	bf04      	itt	eq
20002ea2:	f851 3b04 	ldreq.w	r3, [r1], #4
20002ea6:	3004      	addeq	r0, #4
20002ea8:	d0f4      	beq.n	20002e94 <strlen+0x30>
20002eaa:	f1c2 0100 	rsb	r1, r2, #0
20002eae:	ea02 0201 	and.w	r2, r2, r1
20002eb2:	fab2 f282 	clz	r2, r2
20002eb6:	f1c2 021f 	rsb	r2, r2, #31
20002eba:	eb00 00d2 	add.w	r0, r0, r2, lsr #3
20002ebe:	4770      	bx	lr

20002ec0 <_svfprintf_r>:
20002ec0:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20002ec4:	b0c1      	sub	sp, #260	; 0x104
20002ec6:	4689      	mov	r9, r1
20002ec8:	9208      	str	r2, [sp, #32]
20002eca:	930c      	str	r3, [sp, #48]	; 0x30
20002ecc:	9009      	str	r0, [sp, #36]	; 0x24
20002ece:	f002 faed 	bl	200054ac <_localeconv_r>
20002ed2:	6803      	ldr	r3, [r0, #0]
20002ed4:	4618      	mov	r0, r3
20002ed6:	9316      	str	r3, [sp, #88]	; 0x58
20002ed8:	f7ff ffc4 	bl	20002e64 <strlen>
20002edc:	f8b9 300c 	ldrh.w	r3, [r9, #12]
20002ee0:	9017      	str	r0, [sp, #92]	; 0x5c
20002ee2:	061d      	lsls	r5, r3, #24
20002ee4:	d504      	bpl.n	20002ef0 <_svfprintf_r+0x30>
20002ee6:	f8d9 3010 	ldr.w	r3, [r9, #16]
20002eea:	2b00      	cmp	r3, #0
20002eec:	f001 8080 	beq.w	20003ff0 <_svfprintf_r+0x1130>
20002ef0:	f10d 08c0 	add.w	r8, sp, #192	; 0xc0
20002ef4:	2300      	movs	r3, #0
20002ef6:	46c3      	mov	fp, r8
20002ef8:	9313      	str	r3, [sp, #76]	; 0x4c
20002efa:	9314      	str	r3, [sp, #80]	; 0x50
20002efc:	9312      	str	r3, [sp, #72]	; 0x48
20002efe:	9315      	str	r3, [sp, #84]	; 0x54
20002f00:	9319      	str	r3, [sp, #100]	; 0x64
20002f02:	930a      	str	r3, [sp, #40]	; 0x28
20002f04:	9325      	str	r3, [sp, #148]	; 0x94
20002f06:	9324      	str	r3, [sp, #144]	; 0x90
20002f08:	f8cd 808c 	str.w	r8, [sp, #140]	; 0x8c
20002f0c:	9a08      	ldr	r2, [sp, #32]
20002f0e:	7813      	ldrb	r3, [r2, #0]
20002f10:	2b25      	cmp	r3, #37	; 0x25
20002f12:	f000 80a6 	beq.w	20003062 <_svfprintf_r+0x1a2>
20002f16:	b90b      	cbnz	r3, 20002f1c <_svfprintf_r+0x5c>
20002f18:	e0a3      	b.n	20003062 <_svfprintf_r+0x1a2>
20002f1a:	b11b      	cbz	r3, 20002f24 <_svfprintf_r+0x64>
20002f1c:	f812 3f01 	ldrb.w	r3, [r2, #1]!
20002f20:	2b25      	cmp	r3, #37	; 0x25
20002f22:	d1fa      	bne.n	20002f1a <_svfprintf_r+0x5a>
20002f24:	9908      	ldr	r1, [sp, #32]
20002f26:	4614      	mov	r4, r2
20002f28:	1a55      	subs	r5, r2, r1
20002f2a:	d010      	beq.n	20002f4e <_svfprintf_r+0x8e>
20002f2c:	9b24      	ldr	r3, [sp, #144]	; 0x90
20002f2e:	9a25      	ldr	r2, [sp, #148]	; 0x94
20002f30:	3301      	adds	r3, #1
20002f32:	f8cb 1000 	str.w	r1, [fp]
20002f36:	442a      	add	r2, r5
20002f38:	f8cb 5004 	str.w	r5, [fp, #4]
20002f3c:	2b07      	cmp	r3, #7
20002f3e:	9324      	str	r3, [sp, #144]	; 0x90
20002f40:	9225      	str	r2, [sp, #148]	; 0x94
20002f42:	dc73      	bgt.n	2000302c <_svfprintf_r+0x16c>
20002f44:	f10b 0b08 	add.w	fp, fp, #8
20002f48:	9b0a      	ldr	r3, [sp, #40]	; 0x28
20002f4a:	442b      	add	r3, r5
20002f4c:	930a      	str	r3, [sp, #40]	; 0x28
20002f4e:	7823      	ldrb	r3, [r4, #0]
20002f50:	2b00      	cmp	r3, #0
20002f52:	d073      	beq.n	2000303c <_svfprintf_r+0x17c>
20002f54:	2300      	movs	r3, #0
20002f56:	f04f 3cff 	mov.w	ip, #4294967295
20002f5a:	1c61      	adds	r1, r4, #1
20002f5c:	7866      	ldrb	r6, [r4, #1]
20002f5e:	461a      	mov	r2, r3
20002f60:	4664      	mov	r4, ip
20002f62:	f88d 306f 	strb.w	r3, [sp, #111]	; 0x6f
20002f66:	930b      	str	r3, [sp, #44]	; 0x2c
20002f68:	9306      	str	r3, [sp, #24]
20002f6a:	1c4b      	adds	r3, r1, #1
20002f6c:	f1a6 0120 	sub.w	r1, r6, #32
20002f70:	2958      	cmp	r1, #88	; 0x58
20002f72:	f200 83b2 	bhi.w	200036da <_svfprintf_r+0x81a>
20002f76:	e8df f011 	tbh	[pc, r1, lsl #1]
20002f7a:	0319      	.short	0x0319
20002f7c:	03b003b0 	.word	0x03b003b0
20002f80:	03b00320 	.word	0x03b00320
20002f84:	03b003b0 	.word	0x03b003b0
20002f88:	03b003b0 	.word	0x03b003b0
20002f8c:	024003b0 	.word	0x024003b0
20002f90:	03b001fb 	.word	0x03b001fb
20002f94:	01ff01e2 	.word	0x01ff01e2
20002f98:	032703b0 	.word	0x032703b0
20002f9c:	032e032e 	.word	0x032e032e
20002fa0:	032e032e 	.word	0x032e032e
20002fa4:	032e032e 	.word	0x032e032e
20002fa8:	032e032e 	.word	0x032e032e
20002fac:	03b0032e 	.word	0x03b0032e
20002fb0:	03b003b0 	.word	0x03b003b0
20002fb4:	03b003b0 	.word	0x03b003b0
20002fb8:	03b003b0 	.word	0x03b003b0
20002fbc:	03b003b0 	.word	0x03b003b0
20002fc0:	033d03b0 	.word	0x033d03b0
20002fc4:	03b00365 	.word	0x03b00365
20002fc8:	03b00365 	.word	0x03b00365
20002fcc:	03b003b0 	.word	0x03b003b0
20002fd0:	02fe03b0 	.word	0x02fe03b0
20002fd4:	03b003b0 	.word	0x03b003b0
20002fd8:	03b00293 	.word	0x03b00293
20002fdc:	03b003b0 	.word	0x03b003b0
20002fe0:	03b003b0 	.word	0x03b003b0
20002fe4:	03b002b0 	.word	0x03b002b0
20002fe8:	02c603b0 	.word	0x02c603b0
20002fec:	03b003b0 	.word	0x03b003b0
20002ff0:	03b003b0 	.word	0x03b003b0
20002ff4:	03b003b0 	.word	0x03b003b0
20002ff8:	03b003b0 	.word	0x03b003b0
20002ffc:	03b003b0 	.word	0x03b003b0
20003000:	021802eb 	.word	0x021802eb
20003004:	03650365 	.word	0x03650365
20003008:	024e0365 	.word	0x024e0365
2000300c:	03b00218 	.word	0x03b00218
20003010:	023603b0 	.word	0x023603b0
20003014:	026003b0 	.word	0x026003b0
20003018:	039901e9 	.word	0x039901e9
2000301c:	03b00259 	.word	0x03b00259
20003020:	03b00272 	.word	0x03b00272
20003024:	03b00076 	.word	0x03b00076
20003028:	030503b0 	.word	0x030503b0
2000302c:	9809      	ldr	r0, [sp, #36]	; 0x24
2000302e:	4649      	mov	r1, r9
20003030:	aa23      	add	r2, sp, #140	; 0x8c
20003032:	f003 fc03 	bl	2000683c <__ssprint_r>
20003036:	b940      	cbnz	r0, 2000304a <_svfprintf_r+0x18a>
20003038:	46c3      	mov	fp, r8
2000303a:	e785      	b.n	20002f48 <_svfprintf_r+0x88>
2000303c:	9b25      	ldr	r3, [sp, #148]	; 0x94
2000303e:	b123      	cbz	r3, 2000304a <_svfprintf_r+0x18a>
20003040:	9809      	ldr	r0, [sp, #36]	; 0x24
20003042:	4649      	mov	r1, r9
20003044:	aa23      	add	r2, sp, #140	; 0x8c
20003046:	f003 fbf9 	bl	2000683c <__ssprint_r>
2000304a:	f8b9 300c 	ldrh.w	r3, [r9, #12]
2000304e:	f013 0f40 	tst.w	r3, #64	; 0x40
20003052:	9b0a      	ldr	r3, [sp, #40]	; 0x28
20003054:	bf18      	it	ne
20003056:	f04f 33ff 	movne.w	r3, #4294967295
2000305a:	4618      	mov	r0, r3
2000305c:	b041      	add	sp, #260	; 0x104
2000305e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20003062:	9c08      	ldr	r4, [sp, #32]
20003064:	e773      	b.n	20002f4e <_svfprintf_r+0x8e>
20003066:	9308      	str	r3, [sp, #32]
20003068:	46a4      	mov	ip, r4
2000306a:	9b06      	ldr	r3, [sp, #24]
2000306c:	0698      	lsls	r0, r3, #26
2000306e:	f140 823e 	bpl.w	200034ee <_svfprintf_r+0x62e>
20003072:	9f0c      	ldr	r7, [sp, #48]	; 0x30
20003074:	2301      	movs	r3, #1
20003076:	3707      	adds	r7, #7
20003078:	f027 0207 	bic.w	r2, r7, #7
2000307c:	f102 0108 	add.w	r1, r2, #8
20003080:	e9d2 4500 	ldrd	r4, r5, [r2]
20003084:	910c      	str	r1, [sp, #48]	; 0x30
20003086:	2200      	movs	r2, #0
20003088:	f8cd c040 	str.w	ip, [sp, #64]	; 0x40
2000308c:	4617      	mov	r7, r2
2000308e:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
20003092:	f1bc 0f00 	cmp.w	ip, #0
20003096:	db03      	blt.n	200030a0 <_svfprintf_r+0x1e0>
20003098:	9a06      	ldr	r2, [sp, #24]
2000309a:	f022 0280 	bic.w	r2, r2, #128	; 0x80
2000309e:	9206      	str	r2, [sp, #24]
200030a0:	f1bc 0f00 	cmp.w	ip, #0
200030a4:	f040 83ac 	bne.w	20003800 <_svfprintf_r+0x940>
200030a8:	ea54 0205 	orrs.w	r2, r4, r5
200030ac:	bf14      	ite	ne
200030ae:	2201      	movne	r2, #1
200030b0:	2200      	moveq	r2, #0
200030b2:	2a00      	cmp	r2, #0
200030b4:	f040 83a4 	bne.w	20003800 <_svfprintf_r+0x940>
200030b8:	2b00      	cmp	r3, #0
200030ba:	f040 8460 	bne.w	2000397e <_svfprintf_r+0xabe>
200030be:	9a06      	ldr	r2, [sp, #24]
200030c0:	07d2      	lsls	r2, r2, #31
200030c2:	f140 84fd 	bpl.w	20003ac0 <_svfprintf_r+0xc00>
200030c6:	f50d 7a80 	add.w	sl, sp, #256	; 0x100
200030ca:	2330      	movs	r3, #48	; 0x30
200030cc:	f80a 3d41 	strb.w	r3, [sl, #-65]!
200030d0:	ebca 0408 	rsb	r4, sl, r8
200030d4:	f8cd a03c 	str.w	sl, [sp, #60]	; 0x3c
200030d8:	45a4      	cmp	ip, r4
200030da:	4663      	mov	r3, ip
200030dc:	bfb8      	it	lt
200030de:	4623      	movlt	r3, r4
200030e0:	9307      	str	r3, [sp, #28]
200030e2:	2300      	movs	r3, #0
200030e4:	9311      	str	r3, [sp, #68]	; 0x44
200030e6:	b117      	cbz	r7, 200030ee <_svfprintf_r+0x22e>
200030e8:	9b07      	ldr	r3, [sp, #28]
200030ea:	3301      	adds	r3, #1
200030ec:	9307      	str	r3, [sp, #28]
200030ee:	9b06      	ldr	r3, [sp, #24]
200030f0:	f013 0302 	ands.w	r3, r3, #2
200030f4:	930d      	str	r3, [sp, #52]	; 0x34
200030f6:	d002      	beq.n	200030fe <_svfprintf_r+0x23e>
200030f8:	9b07      	ldr	r3, [sp, #28]
200030fa:	3302      	adds	r3, #2
200030fc:	9307      	str	r3, [sp, #28]
200030fe:	9b06      	ldr	r3, [sp, #24]
20003100:	f013 0384 	ands.w	r3, r3, #132	; 0x84
20003104:	930e      	str	r3, [sp, #56]	; 0x38
20003106:	f040 8307 	bne.w	20003718 <_svfprintf_r+0x858>
2000310a:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
2000310c:	9a07      	ldr	r2, [sp, #28]
2000310e:	ebc2 0a03 	rsb	sl, r2, r3
20003112:	f1ba 0f00 	cmp.w	sl, #0
20003116:	f340 82ff 	ble.w	20003718 <_svfprintf_r+0x858>
2000311a:	f1ba 0f10 	cmp.w	sl, #16
2000311e:	9925      	ldr	r1, [sp, #148]	; 0x94
20003120:	f340 87dd 	ble.w	200040de <_svfprintf_r+0x121e>
20003124:	4fbf      	ldr	r7, [pc, #764]	; (20003424 <_svfprintf_r+0x564>)
20003126:	2510      	movs	r5, #16
20003128:	9618      	str	r6, [sp, #96]	; 0x60
2000312a:	9a24      	ldr	r2, [sp, #144]	; 0x90
2000312c:	463e      	mov	r6, r7
2000312e:	4627      	mov	r7, r4
20003130:	9c09      	ldr	r4, [sp, #36]	; 0x24
20003132:	e004      	b.n	2000313e <_svfprintf_r+0x27e>
20003134:	f1aa 0a10 	sub.w	sl, sl, #16
20003138:	f1ba 0f10 	cmp.w	sl, #16
2000313c:	dd1b      	ble.n	20003176 <_svfprintf_r+0x2b6>
2000313e:	3201      	adds	r2, #1
20003140:	3110      	adds	r1, #16
20003142:	f8cb 6000 	str.w	r6, [fp]
20003146:	f10b 0b08 	add.w	fp, fp, #8
2000314a:	2a07      	cmp	r2, #7
2000314c:	f84b 5c04 	str.w	r5, [fp, #-4]
20003150:	9125      	str	r1, [sp, #148]	; 0x94
20003152:	9224      	str	r2, [sp, #144]	; 0x90
20003154:	ddee      	ble.n	20003134 <_svfprintf_r+0x274>
20003156:	4620      	mov	r0, r4
20003158:	4649      	mov	r1, r9
2000315a:	aa23      	add	r2, sp, #140	; 0x8c
2000315c:	46c3      	mov	fp, r8
2000315e:	f003 fb6d 	bl	2000683c <__ssprint_r>
20003162:	2800      	cmp	r0, #0
20003164:	f47f af71 	bne.w	2000304a <_svfprintf_r+0x18a>
20003168:	f1aa 0a10 	sub.w	sl, sl, #16
2000316c:	9925      	ldr	r1, [sp, #148]	; 0x94
2000316e:	9a24      	ldr	r2, [sp, #144]	; 0x90
20003170:	f1ba 0f10 	cmp.w	sl, #16
20003174:	dce3      	bgt.n	2000313e <_svfprintf_r+0x27e>
20003176:	463c      	mov	r4, r7
20003178:	4637      	mov	r7, r6
2000317a:	9e18      	ldr	r6, [sp, #96]	; 0x60
2000317c:	3201      	adds	r2, #1
2000317e:	eb0a 0c01 	add.w	ip, sl, r1
20003182:	f8cb 7000 	str.w	r7, [fp]
20003186:	2a07      	cmp	r2, #7
20003188:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
2000318c:	9224      	str	r2, [sp, #144]	; 0x90
2000318e:	f8cb a004 	str.w	sl, [fp, #4]
20003192:	f300 82b6 	bgt.w	20003702 <_svfprintf_r+0x842>
20003196:	f10b 0b08 	add.w	fp, fp, #8
2000319a:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
2000319e:	b197      	cbz	r7, 200031c6 <_svfprintf_r+0x306>
200031a0:	9b24      	ldr	r3, [sp, #144]	; 0x90
200031a2:	2201      	movs	r2, #1
200031a4:	f10c 0c01 	add.w	ip, ip, #1
200031a8:	f10d 016f 	add.w	r1, sp, #111	; 0x6f
200031ac:	4413      	add	r3, r2
200031ae:	f8cb 2004 	str.w	r2, [fp, #4]
200031b2:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
200031b6:	2b07      	cmp	r3, #7
200031b8:	f8cb 1000 	str.w	r1, [fp]
200031bc:	9324      	str	r3, [sp, #144]	; 0x90
200031be:	f300 83fb 	bgt.w	200039b8 <_svfprintf_r+0xaf8>
200031c2:	f10b 0b08 	add.w	fp, fp, #8
200031c6:	9b0d      	ldr	r3, [sp, #52]	; 0x34
200031c8:	b18b      	cbz	r3, 200031ee <_svfprintf_r+0x32e>
200031ca:	9b24      	ldr	r3, [sp, #144]	; 0x90
200031cc:	f10c 0c02 	add.w	ip, ip, #2
200031d0:	a91c      	add	r1, sp, #112	; 0x70
200031d2:	2202      	movs	r2, #2
200031d4:	3301      	adds	r3, #1
200031d6:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
200031da:	f8cb 1000 	str.w	r1, [fp]
200031de:	2b07      	cmp	r3, #7
200031e0:	9324      	str	r3, [sp, #144]	; 0x90
200031e2:	f8cb 2004 	str.w	r2, [fp, #4]
200031e6:	f300 83db 	bgt.w	200039a0 <_svfprintf_r+0xae0>
200031ea:	f10b 0b08 	add.w	fp, fp, #8
200031ee:	9b0e      	ldr	r3, [sp, #56]	; 0x38
200031f0:	2b80      	cmp	r3, #128	; 0x80
200031f2:	f000 8321 	beq.w	20003838 <_svfprintf_r+0x978>
200031f6:	9b10      	ldr	r3, [sp, #64]	; 0x40
200031f8:	1b1f      	subs	r7, r3, r4
200031fa:	2f00      	cmp	r7, #0
200031fc:	dd37      	ble.n	2000326e <_svfprintf_r+0x3ae>
200031fe:	2f10      	cmp	r7, #16
20003200:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003202:	f340 85ec 	ble.w	20003dde <_svfprintf_r+0xf1e>
20003206:	940d      	str	r4, [sp, #52]	; 0x34
20003208:	f04f 0a10 	mov.w	sl, #16
2000320c:	4d86      	ldr	r5, [pc, #536]	; (20003428 <_svfprintf_r+0x568>)
2000320e:	4662      	mov	r2, ip
20003210:	9c09      	ldr	r4, [sp, #36]	; 0x24
20003212:	e002      	b.n	2000321a <_svfprintf_r+0x35a>
20003214:	3f10      	subs	r7, #16
20003216:	2f10      	cmp	r7, #16
20003218:	dd19      	ble.n	2000324e <_svfprintf_r+0x38e>
2000321a:	3301      	adds	r3, #1
2000321c:	3210      	adds	r2, #16
2000321e:	f8cb 5000 	str.w	r5, [fp]
20003222:	f10b 0b08 	add.w	fp, fp, #8
20003226:	2b07      	cmp	r3, #7
20003228:	f84b ac04 	str.w	sl, [fp, #-4]
2000322c:	9225      	str	r2, [sp, #148]	; 0x94
2000322e:	9324      	str	r3, [sp, #144]	; 0x90
20003230:	ddf0      	ble.n	20003214 <_svfprintf_r+0x354>
20003232:	4620      	mov	r0, r4
20003234:	4649      	mov	r1, r9
20003236:	aa23      	add	r2, sp, #140	; 0x8c
20003238:	46c3      	mov	fp, r8
2000323a:	f003 faff 	bl	2000683c <__ssprint_r>
2000323e:	2800      	cmp	r0, #0
20003240:	f47f af03 	bne.w	2000304a <_svfprintf_r+0x18a>
20003244:	3f10      	subs	r7, #16
20003246:	9a25      	ldr	r2, [sp, #148]	; 0x94
20003248:	9b24      	ldr	r3, [sp, #144]	; 0x90
2000324a:	2f10      	cmp	r7, #16
2000324c:	dce5      	bgt.n	2000321a <_svfprintf_r+0x35a>
2000324e:	9c0d      	ldr	r4, [sp, #52]	; 0x34
20003250:	4694      	mov	ip, r2
20003252:	3301      	adds	r3, #1
20003254:	44bc      	add	ip, r7
20003256:	f8cb 5000 	str.w	r5, [fp]
2000325a:	2b07      	cmp	r3, #7
2000325c:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003260:	9324      	str	r3, [sp, #144]	; 0x90
20003262:	f8cb 7004 	str.w	r7, [fp, #4]
20003266:	f300 838f 	bgt.w	20003988 <_svfprintf_r+0xac8>
2000326a:	f10b 0b08 	add.w	fp, fp, #8
2000326e:	9b06      	ldr	r3, [sp, #24]
20003270:	05df      	lsls	r7, r3, #23
20003272:	f100 8254 	bmi.w	2000371e <_svfprintf_r+0x85e>
20003276:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003278:	44a4      	add	ip, r4
2000327a:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
2000327c:	3301      	adds	r3, #1
2000327e:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003282:	f8cb 2000 	str.w	r2, [fp]
20003286:	2b07      	cmp	r3, #7
20003288:	f8cb 4004 	str.w	r4, [fp, #4]
2000328c:	9324      	str	r3, [sp, #144]	; 0x90
2000328e:	f300 8360 	bgt.w	20003952 <_svfprintf_r+0xa92>
20003292:	f10b 0b08 	add.w	fp, fp, #8
20003296:	9b06      	ldr	r3, [sp, #24]
20003298:	075a      	lsls	r2, r3, #29
2000329a:	d540      	bpl.n	2000331e <_svfprintf_r+0x45e>
2000329c:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
2000329e:	9a07      	ldr	r2, [sp, #28]
200032a0:	1a9c      	subs	r4, r3, r2
200032a2:	2c00      	cmp	r4, #0
200032a4:	dd3b      	ble.n	2000331e <_svfprintf_r+0x45e>
200032a6:	2c10      	cmp	r4, #16
200032a8:	9b24      	ldr	r3, [sp, #144]	; 0x90
200032aa:	f340 876b 	ble.w	20004184 <_svfprintf_r+0x12c4>
200032ae:	2510      	movs	r5, #16
200032b0:	4f5c      	ldr	r7, [pc, #368]	; (20003424 <_svfprintf_r+0x564>)
200032b2:	4662      	mov	r2, ip
200032b4:	9e09      	ldr	r6, [sp, #36]	; 0x24
200032b6:	e002      	b.n	200032be <_svfprintf_r+0x3fe>
200032b8:	3c10      	subs	r4, #16
200032ba:	2c10      	cmp	r4, #16
200032bc:	dd19      	ble.n	200032f2 <_svfprintf_r+0x432>
200032be:	3301      	adds	r3, #1
200032c0:	3210      	adds	r2, #16
200032c2:	f8cb 7000 	str.w	r7, [fp]
200032c6:	f10b 0b08 	add.w	fp, fp, #8
200032ca:	2b07      	cmp	r3, #7
200032cc:	f84b 5c04 	str.w	r5, [fp, #-4]
200032d0:	9225      	str	r2, [sp, #148]	; 0x94
200032d2:	9324      	str	r3, [sp, #144]	; 0x90
200032d4:	ddf0      	ble.n	200032b8 <_svfprintf_r+0x3f8>
200032d6:	4630      	mov	r0, r6
200032d8:	4649      	mov	r1, r9
200032da:	aa23      	add	r2, sp, #140	; 0x8c
200032dc:	46c3      	mov	fp, r8
200032de:	f003 faad 	bl	2000683c <__ssprint_r>
200032e2:	2800      	cmp	r0, #0
200032e4:	f47f aeb1 	bne.w	2000304a <_svfprintf_r+0x18a>
200032e8:	3c10      	subs	r4, #16
200032ea:	9a25      	ldr	r2, [sp, #148]	; 0x94
200032ec:	9b24      	ldr	r3, [sp, #144]	; 0x90
200032ee:	2c10      	cmp	r4, #16
200032f0:	dce5      	bgt.n	200032be <_svfprintf_r+0x3fe>
200032f2:	4694      	mov	ip, r2
200032f4:	3301      	adds	r3, #1
200032f6:	44a4      	add	ip, r4
200032f8:	f8cb 7000 	str.w	r7, [fp]
200032fc:	2b07      	cmp	r3, #7
200032fe:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003302:	9324      	str	r3, [sp, #144]	; 0x90
20003304:	f8cb 4004 	str.w	r4, [fp, #4]
20003308:	dd09      	ble.n	2000331e <_svfprintf_r+0x45e>
2000330a:	9809      	ldr	r0, [sp, #36]	; 0x24
2000330c:	4649      	mov	r1, r9
2000330e:	aa23      	add	r2, sp, #140	; 0x8c
20003310:	f003 fa94 	bl	2000683c <__ssprint_r>
20003314:	2800      	cmp	r0, #0
20003316:	f47f ae98 	bne.w	2000304a <_svfprintf_r+0x18a>
2000331a:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
2000331e:	9b0a      	ldr	r3, [sp, #40]	; 0x28
20003320:	9a07      	ldr	r2, [sp, #28]
20003322:	990b      	ldr	r1, [sp, #44]	; 0x2c
20003324:	428a      	cmp	r2, r1
20003326:	bfac      	ite	ge
20003328:	189b      	addge	r3, r3, r2
2000332a:	185b      	addlt	r3, r3, r1
2000332c:	930a      	str	r3, [sp, #40]	; 0x28
2000332e:	f1bc 0f00 	cmp.w	ip, #0
20003332:	f040 831a 	bne.w	2000396a <_svfprintf_r+0xaaa>
20003336:	2300      	movs	r3, #0
20003338:	46c3      	mov	fp, r8
2000333a:	9324      	str	r3, [sp, #144]	; 0x90
2000333c:	e5e6      	b.n	20002f0c <_svfprintf_r+0x4c>
2000333e:	4619      	mov	r1, r3
20003340:	9806      	ldr	r0, [sp, #24]
20003342:	781e      	ldrb	r6, [r3, #0]
20003344:	f040 0004 	orr.w	r0, r0, #4
20003348:	9006      	str	r0, [sp, #24]
2000334a:	e60e      	b.n	20002f6a <_svfprintf_r+0xaa>
2000334c:	9308      	str	r3, [sp, #32]
2000334e:	46a4      	mov	ip, r4
20003350:	9b06      	ldr	r3, [sp, #24]
20003352:	f013 0320 	ands.w	r3, r3, #32
20003356:	f000 80ae 	beq.w	200034b6 <_svfprintf_r+0x5f6>
2000335a:	9f0c      	ldr	r7, [sp, #48]	; 0x30
2000335c:	2300      	movs	r3, #0
2000335e:	3707      	adds	r7, #7
20003360:	f027 0207 	bic.w	r2, r7, #7
20003364:	f102 0108 	add.w	r1, r2, #8
20003368:	e9d2 4500 	ldrd	r4, r5, [r2]
2000336c:	910c      	str	r1, [sp, #48]	; 0x30
2000336e:	e68a      	b.n	20003086 <_svfprintf_r+0x1c6>
20003370:	781e      	ldrb	r6, [r3, #0]
20003372:	4619      	mov	r1, r3
20003374:	222b      	movs	r2, #43	; 0x2b
20003376:	e5f8      	b.n	20002f6a <_svfprintf_r+0xaa>
20003378:	781e      	ldrb	r6, [r3, #0]
2000337a:	1c59      	adds	r1, r3, #1
2000337c:	2e2a      	cmp	r6, #42	; 0x2a
2000337e:	f001 8006 	beq.w	2000438e <_svfprintf_r+0x14ce>
20003382:	f1a6 0030 	sub.w	r0, r6, #48	; 0x30
20003386:	460b      	mov	r3, r1
20003388:	2400      	movs	r4, #0
2000338a:	2809      	cmp	r0, #9
2000338c:	f63f adee 	bhi.w	20002f6c <_svfprintf_r+0xac>
20003390:	eb04 0484 	add.w	r4, r4, r4, lsl #2
20003394:	f813 6b01 	ldrb.w	r6, [r3], #1
20003398:	eb00 0444 	add.w	r4, r0, r4, lsl #1
2000339c:	f1a6 0030 	sub.w	r0, r6, #48	; 0x30
200033a0:	2809      	cmp	r0, #9
200033a2:	d9f5      	bls.n	20003390 <_svfprintf_r+0x4d0>
200033a4:	ea44 74e4 	orr.w	r4, r4, r4, asr #31
200033a8:	e5e0      	b.n	20002f6c <_svfprintf_r+0xac>
200033aa:	9308      	str	r3, [sp, #32]
200033ac:	46a4      	mov	ip, r4
200033ae:	9b06      	ldr	r3, [sp, #24]
200033b0:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
200033b4:	069c      	lsls	r4, r3, #26
200033b6:	f140 8129 	bpl.w	2000360c <_svfprintf_r+0x74c>
200033ba:	9f0c      	ldr	r7, [sp, #48]	; 0x30
200033bc:	3707      	adds	r7, #7
200033be:	f027 0707 	bic.w	r7, r7, #7
200033c2:	e9d7 2300 	ldrd	r2, r3, [r7]
200033c6:	f107 0108 	add.w	r1, r7, #8
200033ca:	4614      	mov	r4, r2
200033cc:	461d      	mov	r5, r3
200033ce:	910c      	str	r1, [sp, #48]	; 0x30
200033d0:	2a00      	cmp	r2, #0
200033d2:	f173 0300 	sbcs.w	r3, r3, #0
200033d6:	f2c0 812a 	blt.w	2000362e <_svfprintf_r+0x76e>
200033da:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
200033de:	2301      	movs	r3, #1
200033e0:	f8cd c040 	str.w	ip, [sp, #64]	; 0x40
200033e4:	e655      	b.n	20003092 <_svfprintf_r+0x1d2>
200033e6:	781e      	ldrb	r6, [r3, #0]
200033e8:	9906      	ldr	r1, [sp, #24]
200033ea:	2e6c      	cmp	r6, #108	; 0x6c
200033ec:	f000 8557 	beq.w	20003e9e <_svfprintf_r+0xfde>
200033f0:	f041 0110 	orr.w	r1, r1, #16
200033f4:	9106      	str	r1, [sp, #24]
200033f6:	4619      	mov	r1, r3
200033f8:	e5b7      	b.n	20002f6a <_svfprintf_r+0xaa>
200033fa:	9d0c      	ldr	r5, [sp, #48]	; 0x30
200033fc:	6829      	ldr	r1, [r5, #0]
200033fe:	4608      	mov	r0, r1
20003400:	910b      	str	r1, [sp, #44]	; 0x2c
20003402:	4629      	mov	r1, r5
20003404:	2800      	cmp	r0, #0
20003406:	f101 0104 	add.w	r1, r1, #4
2000340a:	f2c0 854f 	blt.w	20003eac <_svfprintf_r+0xfec>
2000340e:	910c      	str	r1, [sp, #48]	; 0x30
20003410:	4619      	mov	r1, r3
20003412:	781e      	ldrb	r6, [r3, #0]
20003414:	e5a9      	b.n	20002f6a <_svfprintf_r+0xaa>
20003416:	9906      	ldr	r1, [sp, #24]
20003418:	f041 0140 	orr.w	r1, r1, #64	; 0x40
2000341c:	9106      	str	r1, [sp, #24]
2000341e:	4619      	mov	r1, r3
20003420:	781e      	ldrb	r6, [r3, #0]
20003422:	e5a2      	b.n	20002f6a <_svfprintf_r+0xaa>
20003424:	2000906c 	.word	0x2000906c
20003428:	2000905c 	.word	0x2000905c
2000342c:	9906      	ldr	r1, [sp, #24]
2000342e:	f041 0120 	orr.w	r1, r1, #32
20003432:	9106      	str	r1, [sp, #24]
20003434:	4619      	mov	r1, r3
20003436:	781e      	ldrb	r6, [r3, #0]
20003438:	e597      	b.n	20002f6a <_svfprintf_r+0xaa>
2000343a:	9308      	str	r3, [sp, #32]
2000343c:	9b06      	ldr	r3, [sp, #24]
2000343e:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
20003442:	069e      	lsls	r6, r3, #26
20003444:	f140 84a4 	bpl.w	20003d90 <_svfprintf_r+0xed0>
20003448:	990a      	ldr	r1, [sp, #40]	; 0x28
2000344a:	9a0c      	ldr	r2, [sp, #48]	; 0x30
2000344c:	17cd      	asrs	r5, r1, #31
2000344e:	4608      	mov	r0, r1
20003450:	6813      	ldr	r3, [r2, #0]
20003452:	3204      	adds	r2, #4
20003454:	4629      	mov	r1, r5
20003456:	920c      	str	r2, [sp, #48]	; 0x30
20003458:	e9c3 0100 	strd	r0, r1, [r3]
2000345c:	e556      	b.n	20002f0c <_svfprintf_r+0x4c>
2000345e:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20003460:	2500      	movs	r5, #0
20003462:	9308      	str	r3, [sp, #32]
20003464:	6813      	ldr	r3, [r2, #0]
20003466:	1d17      	adds	r7, r2, #4
20003468:	f88d 506f 	strb.w	r5, [sp, #111]	; 0x6f
2000346c:	930f      	str	r3, [sp, #60]	; 0x3c
2000346e:	2b00      	cmp	r3, #0
20003470:	f000 867a 	beq.w	20004168 <_svfprintf_r+0x12a8>
20003474:	2c00      	cmp	r4, #0
20003476:	980f      	ldr	r0, [sp, #60]	; 0x3c
20003478:	f2c0 8624 	blt.w	200040c4 <_svfprintf_r+0x1204>
2000347c:	4629      	mov	r1, r5
2000347e:	4622      	mov	r2, r4
20003480:	f002 fad0 	bl	20005a24 <memchr>
20003484:	2800      	cmp	r0, #0
20003486:	f000 86df 	beq.w	20004248 <_svfprintf_r+0x1388>
2000348a:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
2000348c:	970c      	str	r7, [sp, #48]	; 0x30
2000348e:	1ac4      	subs	r4, r0, r3
20003490:	9510      	str	r5, [sp, #64]	; 0x40
20003492:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
20003496:	ea24 73e4 	bic.w	r3, r4, r4, asr #31
2000349a:	9511      	str	r5, [sp, #68]	; 0x44
2000349c:	9307      	str	r3, [sp, #28]
2000349e:	e622      	b.n	200030e6 <_svfprintf_r+0x226>
200034a0:	9308      	str	r3, [sp, #32]
200034a2:	46a4      	mov	ip, r4
200034a4:	9b06      	ldr	r3, [sp, #24]
200034a6:	f043 0310 	orr.w	r3, r3, #16
200034aa:	9306      	str	r3, [sp, #24]
200034ac:	9b06      	ldr	r3, [sp, #24]
200034ae:	f013 0320 	ands.w	r3, r3, #32
200034b2:	f47f af52 	bne.w	2000335a <_svfprintf_r+0x49a>
200034b6:	9a06      	ldr	r2, [sp, #24]
200034b8:	f012 0210 	ands.w	r2, r2, #16
200034bc:	f040 8352 	bne.w	20003b64 <_svfprintf_r+0xca4>
200034c0:	9b06      	ldr	r3, [sp, #24]
200034c2:	f013 0340 	ands.w	r3, r3, #64	; 0x40
200034c6:	f000 834d 	beq.w	20003b64 <_svfprintf_r+0xca4>
200034ca:	990c      	ldr	r1, [sp, #48]	; 0x30
200034cc:	4613      	mov	r3, r2
200034ce:	2500      	movs	r5, #0
200034d0:	460a      	mov	r2, r1
200034d2:	880c      	ldrh	r4, [r1, #0]
200034d4:	3204      	adds	r2, #4
200034d6:	920c      	str	r2, [sp, #48]	; 0x30
200034d8:	e5d5      	b.n	20003086 <_svfprintf_r+0x1c6>
200034da:	9308      	str	r3, [sp, #32]
200034dc:	46a4      	mov	ip, r4
200034de:	9b06      	ldr	r3, [sp, #24]
200034e0:	f043 0310 	orr.w	r3, r3, #16
200034e4:	9306      	str	r3, [sp, #24]
200034e6:	9b06      	ldr	r3, [sp, #24]
200034e8:	0698      	lsls	r0, r3, #26
200034ea:	f53f adc2 	bmi.w	20003072 <_svfprintf_r+0x1b2>
200034ee:	9b06      	ldr	r3, [sp, #24]
200034f0:	06d9      	lsls	r1, r3, #27
200034f2:	f140 8432 	bpl.w	20003d5a <_svfprintf_r+0xe9a>
200034f6:	9a0c      	ldr	r2, [sp, #48]	; 0x30
200034f8:	4613      	mov	r3, r2
200034fa:	3204      	adds	r2, #4
200034fc:	681c      	ldr	r4, [r3, #0]
200034fe:	2500      	movs	r5, #0
20003500:	2301      	movs	r3, #1
20003502:	920c      	str	r2, [sp, #48]	; 0x30
20003504:	e5bf      	b.n	20003086 <_svfprintf_r+0x1c6>
20003506:	9308      	str	r3, [sp, #32]
20003508:	46a4      	mov	ip, r4
2000350a:	4bb7      	ldr	r3, [pc, #732]	; (200037e8 <_svfprintf_r+0x928>)
2000350c:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
20003510:	9315      	str	r3, [sp, #84]	; 0x54
20003512:	9b06      	ldr	r3, [sp, #24]
20003514:	069a      	lsls	r2, r3, #26
20003516:	d53e      	bpl.n	20003596 <_svfprintf_r+0x6d6>
20003518:	9f0c      	ldr	r7, [sp, #48]	; 0x30
2000351a:	3707      	adds	r7, #7
2000351c:	f027 0307 	bic.w	r3, r7, #7
20003520:	f103 0208 	add.w	r2, r3, #8
20003524:	e9d3 4500 	ldrd	r4, r5, [r3]
20003528:	920c      	str	r2, [sp, #48]	; 0x30
2000352a:	9b06      	ldr	r3, [sp, #24]
2000352c:	07df      	lsls	r7, r3, #31
2000352e:	f140 8268 	bpl.w	20003a02 <_svfprintf_r+0xb42>
20003532:	ea54 0305 	orrs.w	r3, r4, r5
20003536:	f000 8264 	beq.w	20003a02 <_svfprintf_r+0xb42>
2000353a:	9b06      	ldr	r3, [sp, #24]
2000353c:	2230      	movs	r2, #48	; 0x30
2000353e:	f88d 6071 	strb.w	r6, [sp, #113]	; 0x71
20003542:	f043 0302 	orr.w	r3, r3, #2
20003546:	f88d 2070 	strb.w	r2, [sp, #112]	; 0x70
2000354a:	9306      	str	r3, [sp, #24]
2000354c:	2302      	movs	r3, #2
2000354e:	e59a      	b.n	20003086 <_svfprintf_r+0x1c6>
20003550:	990c      	ldr	r1, [sp, #48]	; 0x30
20003552:	2001      	movs	r0, #1
20003554:	9308      	str	r3, [sp, #32]
20003556:	2300      	movs	r3, #0
20003558:	680a      	ldr	r2, [r1, #0]
2000355a:	3104      	adds	r1, #4
2000355c:	461f      	mov	r7, r3
2000355e:	f88d 306f 	strb.w	r3, [sp, #111]	; 0x6f
20003562:	9310      	str	r3, [sp, #64]	; 0x40
20003564:	4604      	mov	r4, r0
20003566:	9311      	str	r3, [sp, #68]	; 0x44
20003568:	ab26      	add	r3, sp, #152	; 0x98
2000356a:	9007      	str	r0, [sp, #28]
2000356c:	910c      	str	r1, [sp, #48]	; 0x30
2000356e:	f88d 2098 	strb.w	r2, [sp, #152]	; 0x98
20003572:	930f      	str	r3, [sp, #60]	; 0x3c
20003574:	e5bb      	b.n	200030ee <_svfprintf_r+0x22e>
20003576:	9906      	ldr	r1, [sp, #24]
20003578:	f041 0108 	orr.w	r1, r1, #8
2000357c:	9106      	str	r1, [sp, #24]
2000357e:	4619      	mov	r1, r3
20003580:	781e      	ldrb	r6, [r3, #0]
20003582:	e4f2      	b.n	20002f6a <_svfprintf_r+0xaa>
20003584:	9308      	str	r3, [sp, #32]
20003586:	46a4      	mov	ip, r4
20003588:	4b98      	ldr	r3, [pc, #608]	; (200037ec <_svfprintf_r+0x92c>)
2000358a:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
2000358e:	9315      	str	r3, [sp, #84]	; 0x54
20003590:	9b06      	ldr	r3, [sp, #24]
20003592:	069a      	lsls	r2, r3, #26
20003594:	d4c0      	bmi.n	20003518 <_svfprintf_r+0x658>
20003596:	9b06      	ldr	r3, [sp, #24]
20003598:	06db      	lsls	r3, r3, #27
2000359a:	f140 83ec 	bpl.w	20003d76 <_svfprintf_r+0xeb6>
2000359e:	9a0c      	ldr	r2, [sp, #48]	; 0x30
200035a0:	2500      	movs	r5, #0
200035a2:	4613      	mov	r3, r2
200035a4:	6814      	ldr	r4, [r2, #0]
200035a6:	3304      	adds	r3, #4
200035a8:	930c      	str	r3, [sp, #48]	; 0x30
200035aa:	e7be      	b.n	2000352a <_svfprintf_r+0x66a>
200035ac:	781e      	ldrb	r6, [r3, #0]
200035ae:	4619      	mov	r1, r3
200035b0:	2a00      	cmp	r2, #0
200035b2:	f47f acda 	bne.w	20002f6a <_svfprintf_r+0xaa>
200035b6:	2220      	movs	r2, #32
200035b8:	e4d7      	b.n	20002f6a <_svfprintf_r+0xaa>
200035ba:	9906      	ldr	r1, [sp, #24]
200035bc:	f041 0101 	orr.w	r1, r1, #1
200035c0:	9106      	str	r1, [sp, #24]
200035c2:	4619      	mov	r1, r3
200035c4:	781e      	ldrb	r6, [r3, #0]
200035c6:	e4d0      	b.n	20002f6a <_svfprintf_r+0xaa>
200035c8:	9906      	ldr	r1, [sp, #24]
200035ca:	f041 0180 	orr.w	r1, r1, #128	; 0x80
200035ce:	9106      	str	r1, [sp, #24]
200035d0:	4619      	mov	r1, r3
200035d2:	781e      	ldrb	r6, [r3, #0]
200035d4:	e4c9      	b.n	20002f6a <_svfprintf_r+0xaa>
200035d6:	f1a6 0030 	sub.w	r0, r6, #48	; 0x30
200035da:	2100      	movs	r1, #0
200035dc:	eb01 0181 	add.w	r1, r1, r1, lsl #2
200035e0:	f813 6b01 	ldrb.w	r6, [r3], #1
200035e4:	eb00 0141 	add.w	r1, r0, r1, lsl #1
200035e8:	f1a6 0030 	sub.w	r0, r6, #48	; 0x30
200035ec:	2809      	cmp	r0, #9
200035ee:	d9f5      	bls.n	200035dc <_svfprintf_r+0x71c>
200035f0:	910b      	str	r1, [sp, #44]	; 0x2c
200035f2:	e4bb      	b.n	20002f6c <_svfprintf_r+0xac>
200035f4:	9308      	str	r3, [sp, #32]
200035f6:	46a4      	mov	ip, r4
200035f8:	9b06      	ldr	r3, [sp, #24]
200035fa:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
200035fe:	f043 0310 	orr.w	r3, r3, #16
20003602:	9306      	str	r3, [sp, #24]
20003604:	9b06      	ldr	r3, [sp, #24]
20003606:	069c      	lsls	r4, r3, #26
20003608:	f53f aed7 	bmi.w	200033ba <_svfprintf_r+0x4fa>
2000360c:	9b06      	ldr	r3, [sp, #24]
2000360e:	06d8      	lsls	r0, r3, #27
20003610:	f140 83d5 	bpl.w	20003dbe <_svfprintf_r+0xefe>
20003614:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20003616:	4613      	mov	r3, r2
20003618:	681c      	ldr	r4, [r3, #0]
2000361a:	3304      	adds	r3, #4
2000361c:	17e5      	asrs	r5, r4, #31
2000361e:	4622      	mov	r2, r4
20003620:	930c      	str	r3, [sp, #48]	; 0x30
20003622:	2a00      	cmp	r2, #0
20003624:	462b      	mov	r3, r5
20003626:	f173 0300 	sbcs.w	r3, r3, #0
2000362a:	f6bf aed6 	bge.w	200033da <_svfprintf_r+0x51a>
2000362e:	272d      	movs	r7, #45	; 0x2d
20003630:	4264      	negs	r4, r4
20003632:	f8cd c040 	str.w	ip, [sp, #64]	; 0x40
20003636:	f04f 0301 	mov.w	r3, #1
2000363a:	eb65 0545 	sbc.w	r5, r5, r5, lsl #1
2000363e:	f88d 706f 	strb.w	r7, [sp, #111]	; 0x6f
20003642:	e526      	b.n	20003092 <_svfprintf_r+0x1d2>
20003644:	9f0c      	ldr	r7, [sp, #48]	; 0x30
20003646:	9308      	str	r3, [sp, #32]
20003648:	3707      	adds	r7, #7
2000364a:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
2000364e:	f027 0307 	bic.w	r3, r7, #7
20003652:	f103 0208 	add.w	r2, r3, #8
20003656:	920c      	str	r2, [sp, #48]	; 0x30
20003658:	681a      	ldr	r2, [r3, #0]
2000365a:	9213      	str	r2, [sp, #76]	; 0x4c
2000365c:	685b      	ldr	r3, [r3, #4]
2000365e:	9813      	ldr	r0, [sp, #76]	; 0x4c
20003660:	9314      	str	r3, [sp, #80]	; 0x50
20003662:	9914      	ldr	r1, [sp, #80]	; 0x50
20003664:	9407      	str	r4, [sp, #28]
20003666:	f002 ff3b 	bl	200064e0 <__fpclassifyd>
2000366a:	2801      	cmp	r0, #1
2000366c:	f8dd c01c 	ldr.w	ip, [sp, #28]
20003670:	f040 8355 	bne.w	20003d1e <_svfprintf_r+0xe5e>
20003674:	9813      	ldr	r0, [sp, #76]	; 0x4c
20003676:	2200      	movs	r2, #0
20003678:	2300      	movs	r3, #0
2000367a:	9914      	ldr	r1, [sp, #80]	; 0x50
2000367c:	f004 feec 	bl	20008458 <__aeabi_dcmplt>
20003680:	2800      	cmp	r0, #0
20003682:	f040 8582 	bne.w	2000418a <_svfprintf_r+0x12ca>
20003686:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
2000368a:	2303      	movs	r3, #3
2000368c:	9806      	ldr	r0, [sp, #24]
2000368e:	4a58      	ldr	r2, [pc, #352]	; (200037f0 <_svfprintf_r+0x930>)
20003690:	2100      	movs	r1, #0
20003692:	461c      	mov	r4, r3
20003694:	9307      	str	r3, [sp, #28]
20003696:	4b57      	ldr	r3, [pc, #348]	; (200037f4 <_svfprintf_r+0x934>)
20003698:	f020 0080 	bic.w	r0, r0, #128	; 0x80
2000369c:	9110      	str	r1, [sp, #64]	; 0x40
2000369e:	2e47      	cmp	r6, #71	; 0x47
200036a0:	bfd8      	it	le
200036a2:	461a      	movle	r2, r3
200036a4:	9006      	str	r0, [sp, #24]
200036a6:	9111      	str	r1, [sp, #68]	; 0x44
200036a8:	920f      	str	r2, [sp, #60]	; 0x3c
200036aa:	e51c      	b.n	200030e6 <_svfprintf_r+0x226>
200036ac:	9d0c      	ldr	r5, [sp, #48]	; 0x30
200036ae:	2230      	movs	r2, #48	; 0x30
200036b0:	9806      	ldr	r0, [sp, #24]
200036b2:	46a4      	mov	ip, r4
200036b4:	4629      	mov	r1, r5
200036b6:	9308      	str	r3, [sp, #32]
200036b8:	2378      	movs	r3, #120	; 0x78
200036ba:	f040 0002 	orr.w	r0, r0, #2
200036be:	3104      	adds	r1, #4
200036c0:	682c      	ldr	r4, [r5, #0]
200036c2:	461e      	mov	r6, r3
200036c4:	f88d 3071 	strb.w	r3, [sp, #113]	; 0x71
200036c8:	4b48      	ldr	r3, [pc, #288]	; (200037ec <_svfprintf_r+0x92c>)
200036ca:	2500      	movs	r5, #0
200036cc:	9006      	str	r0, [sp, #24]
200036ce:	9315      	str	r3, [sp, #84]	; 0x54
200036d0:	2302      	movs	r3, #2
200036d2:	910c      	str	r1, [sp, #48]	; 0x30
200036d4:	f88d 2070 	strb.w	r2, [sp, #112]	; 0x70
200036d8:	e4d5      	b.n	20003086 <_svfprintf_r+0x1c6>
200036da:	9308      	str	r3, [sp, #32]
200036dc:	f88d 206f 	strb.w	r2, [sp, #111]	; 0x6f
200036e0:	2e00      	cmp	r6, #0
200036e2:	f43f acab 	beq.w	2000303c <_svfprintf_r+0x17c>
200036e6:	2300      	movs	r3, #0
200036e8:	2201      	movs	r2, #1
200036ea:	a926      	add	r1, sp, #152	; 0x98
200036ec:	f88d 6098 	strb.w	r6, [sp, #152]	; 0x98
200036f0:	9207      	str	r2, [sp, #28]
200036f2:	461f      	mov	r7, r3
200036f4:	910f      	str	r1, [sp, #60]	; 0x3c
200036f6:	4614      	mov	r4, r2
200036f8:	f88d 306f 	strb.w	r3, [sp, #111]	; 0x6f
200036fc:	9310      	str	r3, [sp, #64]	; 0x40
200036fe:	9311      	str	r3, [sp, #68]	; 0x44
20003700:	e4f5      	b.n	200030ee <_svfprintf_r+0x22e>
20003702:	9809      	ldr	r0, [sp, #36]	; 0x24
20003704:	4649      	mov	r1, r9
20003706:	aa23      	add	r2, sp, #140	; 0x8c
20003708:	f003 f898 	bl	2000683c <__ssprint_r>
2000370c:	2800      	cmp	r0, #0
2000370e:	f47f ac9c 	bne.w	2000304a <_svfprintf_r+0x18a>
20003712:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
20003716:	46c3      	mov	fp, r8
20003718:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
2000371c:	e53f      	b.n	2000319e <_svfprintf_r+0x2de>
2000371e:	2e65      	cmp	r6, #101	; 0x65
20003720:	f340 80c9 	ble.w	200038b6 <_svfprintf_r+0x9f6>
20003724:	9813      	ldr	r0, [sp, #76]	; 0x4c
20003726:	2200      	movs	r2, #0
20003728:	2300      	movs	r3, #0
2000372a:	9914      	ldr	r1, [sp, #80]	; 0x50
2000372c:	f8cd c034 	str.w	ip, [sp, #52]	; 0x34
20003730:	f004 fe88 	bl	20008444 <__aeabi_dcmpeq>
20003734:	f8dd c034 	ldr.w	ip, [sp, #52]	; 0x34
20003738:	2800      	cmp	r0, #0
2000373a:	f000 8165 	beq.w	20003a08 <_svfprintf_r+0xb48>
2000373e:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003740:	2201      	movs	r2, #1
20003742:	f10c 0c01 	add.w	ip, ip, #1
20003746:	492c      	ldr	r1, [pc, #176]	; (200037f8 <_svfprintf_r+0x938>)
20003748:	4413      	add	r3, r2
2000374a:	f8cb 2004 	str.w	r2, [fp, #4]
2000374e:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003752:	2b07      	cmp	r3, #7
20003754:	f8cb 1000 	str.w	r1, [fp]
20003758:	9324      	str	r3, [sp, #144]	; 0x90
2000375a:	f300 8343 	bgt.w	20003de4 <_svfprintf_r+0xf24>
2000375e:	f10b 0b08 	add.w	fp, fp, #8
20003762:	9b1d      	ldr	r3, [sp, #116]	; 0x74
20003764:	9a12      	ldr	r2, [sp, #72]	; 0x48
20003766:	4293      	cmp	r3, r2
20003768:	db03      	blt.n	20003772 <_svfprintf_r+0x8b2>
2000376a:	9b06      	ldr	r3, [sp, #24]
2000376c:	07de      	lsls	r6, r3, #31
2000376e:	f57f ad92 	bpl.w	20003296 <_svfprintf_r+0x3d6>
20003772:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003774:	9917      	ldr	r1, [sp, #92]	; 0x5c
20003776:	3301      	adds	r3, #1
20003778:	9a16      	ldr	r2, [sp, #88]	; 0x58
2000377a:	448c      	add	ip, r1
2000377c:	f8cb 1004 	str.w	r1, [fp, #4]
20003780:	2b07      	cmp	r3, #7
20003782:	f8cb 2000 	str.w	r2, [fp]
20003786:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
2000378a:	9324      	str	r3, [sp, #144]	; 0x90
2000378c:	f300 841d 	bgt.w	20003fca <_svfprintf_r+0x110a>
20003790:	f10b 0b08 	add.w	fp, fp, #8
20003794:	9b12      	ldr	r3, [sp, #72]	; 0x48
20003796:	1e5c      	subs	r4, r3, #1
20003798:	2c00      	cmp	r4, #0
2000379a:	f77f ad7c 	ble.w	20003296 <_svfprintf_r+0x3d6>
2000379e:	2c10      	cmp	r4, #16
200037a0:	9b24      	ldr	r3, [sp, #144]	; 0x90
200037a2:	f340 85bc 	ble.w	2000431e <_svfprintf_r+0x145e>
200037a6:	2610      	movs	r6, #16
200037a8:	4d14      	ldr	r5, [pc, #80]	; (200037fc <_svfprintf_r+0x93c>)
200037aa:	4662      	mov	r2, ip
200037ac:	9f09      	ldr	r7, [sp, #36]	; 0x24
200037ae:	e003      	b.n	200037b8 <_svfprintf_r+0x8f8>
200037b0:	3c10      	subs	r4, #16
200037b2:	2c10      	cmp	r4, #16
200037b4:	f340 81c8 	ble.w	20003b48 <_svfprintf_r+0xc88>
200037b8:	3301      	adds	r3, #1
200037ba:	3210      	adds	r2, #16
200037bc:	f8cb 5000 	str.w	r5, [fp]
200037c0:	f10b 0b08 	add.w	fp, fp, #8
200037c4:	2b07      	cmp	r3, #7
200037c6:	f84b 6c04 	str.w	r6, [fp, #-4]
200037ca:	9225      	str	r2, [sp, #148]	; 0x94
200037cc:	9324      	str	r3, [sp, #144]	; 0x90
200037ce:	ddef      	ble.n	200037b0 <_svfprintf_r+0x8f0>
200037d0:	4638      	mov	r0, r7
200037d2:	4649      	mov	r1, r9
200037d4:	aa23      	add	r2, sp, #140	; 0x8c
200037d6:	46c3      	mov	fp, r8
200037d8:	f003 f830 	bl	2000683c <__ssprint_r>
200037dc:	2800      	cmp	r0, #0
200037de:	f47f ac34 	bne.w	2000304a <_svfprintf_r+0x18a>
200037e2:	9a25      	ldr	r2, [sp, #148]	; 0x94
200037e4:	9b24      	ldr	r3, [sp, #144]	; 0x90
200037e6:	e7e3      	b.n	200037b0 <_svfprintf_r+0x8f0>
200037e8:	2000908c 	.word	0x2000908c
200037ec:	200090a0 	.word	0x200090a0
200037f0:	20009080 	.word	0x20009080
200037f4:	2000907c 	.word	0x2000907c
200037f8:	200090bc 	.word	0x200090bc
200037fc:	2000905c 	.word	0x2000905c
20003800:	2b01      	cmp	r3, #1
20003802:	f000 814d 	beq.w	20003aa0 <_svfprintf_r+0xbe0>
20003806:	2b02      	cmp	r3, #2
20003808:	f040 80e3 	bne.w	200039d2 <_svfprintf_r+0xb12>
2000380c:	46c2      	mov	sl, r8
2000380e:	9815      	ldr	r0, [sp, #84]	; 0x54
20003810:	0923      	lsrs	r3, r4, #4
20003812:	f004 010f 	and.w	r1, r4, #15
20003816:	092a      	lsrs	r2, r5, #4
20003818:	ea43 7305 	orr.w	r3, r3, r5, lsl #28
2000381c:	4615      	mov	r5, r2
2000381e:	461c      	mov	r4, r3
20003820:	5c43      	ldrb	r3, [r0, r1]
20003822:	f80a 3d01 	strb.w	r3, [sl, #-1]!
20003826:	ea54 0305 	orrs.w	r3, r4, r5
2000382a:	d1f1      	bne.n	20003810 <_svfprintf_r+0x950>
2000382c:	4653      	mov	r3, sl
2000382e:	f8cd a03c 	str.w	sl, [sp, #60]	; 0x3c
20003832:	ebc3 0408 	rsb	r4, r3, r8
20003836:	e44f      	b.n	200030d8 <_svfprintf_r+0x218>
20003838:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
2000383a:	9a07      	ldr	r2, [sp, #28]
2000383c:	1a9f      	subs	r7, r3, r2
2000383e:	2f00      	cmp	r7, #0
20003840:	f77f acd9 	ble.w	200031f6 <_svfprintf_r+0x336>
20003844:	2f10      	cmp	r7, #16
20003846:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003848:	f340 84f9 	ble.w	2000423e <_svfprintf_r+0x137e>
2000384c:	940d      	str	r4, [sp, #52]	; 0x34
2000384e:	f04f 0a10 	mov.w	sl, #16
20003852:	4dc8      	ldr	r5, [pc, #800]	; (20003b74 <_svfprintf_r+0xcb4>)
20003854:	4662      	mov	r2, ip
20003856:	9c09      	ldr	r4, [sp, #36]	; 0x24
20003858:	e002      	b.n	20003860 <_svfprintf_r+0x9a0>
2000385a:	3f10      	subs	r7, #16
2000385c:	2f10      	cmp	r7, #16
2000385e:	dd19      	ble.n	20003894 <_svfprintf_r+0x9d4>
20003860:	3301      	adds	r3, #1
20003862:	3210      	adds	r2, #16
20003864:	f8cb 5000 	str.w	r5, [fp]
20003868:	f10b 0b08 	add.w	fp, fp, #8
2000386c:	2b07      	cmp	r3, #7
2000386e:	f84b ac04 	str.w	sl, [fp, #-4]
20003872:	9225      	str	r2, [sp, #148]	; 0x94
20003874:	9324      	str	r3, [sp, #144]	; 0x90
20003876:	ddf0      	ble.n	2000385a <_svfprintf_r+0x99a>
20003878:	4620      	mov	r0, r4
2000387a:	4649      	mov	r1, r9
2000387c:	aa23      	add	r2, sp, #140	; 0x8c
2000387e:	46c3      	mov	fp, r8
20003880:	f002 ffdc 	bl	2000683c <__ssprint_r>
20003884:	2800      	cmp	r0, #0
20003886:	f47f abe0 	bne.w	2000304a <_svfprintf_r+0x18a>
2000388a:	3f10      	subs	r7, #16
2000388c:	9a25      	ldr	r2, [sp, #148]	; 0x94
2000388e:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003890:	2f10      	cmp	r7, #16
20003892:	dce5      	bgt.n	20003860 <_svfprintf_r+0x9a0>
20003894:	9c0d      	ldr	r4, [sp, #52]	; 0x34
20003896:	4694      	mov	ip, r2
20003898:	3301      	adds	r3, #1
2000389a:	44bc      	add	ip, r7
2000389c:	f8cb 5000 	str.w	r5, [fp]
200038a0:	2b07      	cmp	r3, #7
200038a2:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
200038a6:	9324      	str	r3, [sp, #144]	; 0x90
200038a8:	f8cb 7004 	str.w	r7, [fp, #4]
200038ac:	f300 81fc 	bgt.w	20003ca8 <_svfprintf_r+0xde8>
200038b0:	f10b 0b08 	add.w	fp, fp, #8
200038b4:	e49f      	b.n	200031f6 <_svfprintf_r+0x336>
200038b6:	9b12      	ldr	r3, [sp, #72]	; 0x48
200038b8:	f10c 0601 	add.w	r6, ip, #1
200038bc:	9c24      	ldr	r4, [sp, #144]	; 0x90
200038be:	2b01      	cmp	r3, #1
200038c0:	f340 81c9 	ble.w	20003c56 <_svfprintf_r+0xd96>
200038c4:	2301      	movs	r3, #1
200038c6:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
200038c8:	9625      	str	r6, [sp, #148]	; 0x94
200038ca:	441c      	add	r4, r3
200038cc:	f8cb 2000 	str.w	r2, [fp]
200038d0:	f8cb 3004 	str.w	r3, [fp, #4]
200038d4:	2c07      	cmp	r4, #7
200038d6:	9424      	str	r4, [sp, #144]	; 0x90
200038d8:	f300 81da 	bgt.w	20003c90 <_svfprintf_r+0xdd0>
200038dc:	f10b 0b08 	add.w	fp, fp, #8
200038e0:	9a17      	ldr	r2, [sp, #92]	; 0x5c
200038e2:	3401      	adds	r4, #1
200038e4:	9b16      	ldr	r3, [sp, #88]	; 0x58
200038e6:	4416      	add	r6, r2
200038e8:	2c07      	cmp	r4, #7
200038ea:	f8cb 3000 	str.w	r3, [fp]
200038ee:	9625      	str	r6, [sp, #148]	; 0x94
200038f0:	9424      	str	r4, [sp, #144]	; 0x90
200038f2:	f8cb 2004 	str.w	r2, [fp, #4]
200038f6:	f300 81bf 	bgt.w	20003c78 <_svfprintf_r+0xdb8>
200038fa:	f10b 0b08 	add.w	fp, fp, #8
200038fe:	2300      	movs	r3, #0
20003900:	9813      	ldr	r0, [sp, #76]	; 0x4c
20003902:	2200      	movs	r2, #0
20003904:	9914      	ldr	r1, [sp, #80]	; 0x50
20003906:	f004 fd9d 	bl	20008444 <__aeabi_dcmpeq>
2000390a:	9b12      	ldr	r3, [sp, #72]	; 0x48
2000390c:	2800      	cmp	r0, #0
2000390e:	f040 80dc 	bne.w	20003aca <_svfprintf_r+0xc0a>
20003912:	3b01      	subs	r3, #1
20003914:	3401      	adds	r4, #1
20003916:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
20003918:	441e      	add	r6, r3
2000391a:	2c07      	cmp	r4, #7
2000391c:	f102 0201 	add.w	r2, r2, #1
20003920:	9424      	str	r4, [sp, #144]	; 0x90
20003922:	9625      	str	r6, [sp, #148]	; 0x94
20003924:	f8cb 2000 	str.w	r2, [fp]
20003928:	f8cb 3004 	str.w	r3, [fp, #4]
2000392c:	f300 8100 	bgt.w	20003b30 <_svfprintf_r+0xc70>
20003930:	f10b 0b08 	add.w	fp, fp, #8
20003934:	9a19      	ldr	r2, [sp, #100]	; 0x64
20003936:	3401      	adds	r4, #1
20003938:	ab1f      	add	r3, sp, #124	; 0x7c
2000393a:	eb06 0c02 	add.w	ip, r6, r2
2000393e:	2c07      	cmp	r4, #7
20003940:	f8cb 2004 	str.w	r2, [fp, #4]
20003944:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003948:	9424      	str	r4, [sp, #144]	; 0x90
2000394a:	f8cb 3000 	str.w	r3, [fp]
2000394e:	f77f aca0 	ble.w	20003292 <_svfprintf_r+0x3d2>
20003952:	9809      	ldr	r0, [sp, #36]	; 0x24
20003954:	4649      	mov	r1, r9
20003956:	aa23      	add	r2, sp, #140	; 0x8c
20003958:	f002 ff70 	bl	2000683c <__ssprint_r>
2000395c:	2800      	cmp	r0, #0
2000395e:	f47f ab74 	bne.w	2000304a <_svfprintf_r+0x18a>
20003962:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
20003966:	46c3      	mov	fp, r8
20003968:	e495      	b.n	20003296 <_svfprintf_r+0x3d6>
2000396a:	9809      	ldr	r0, [sp, #36]	; 0x24
2000396c:	4649      	mov	r1, r9
2000396e:	aa23      	add	r2, sp, #140	; 0x8c
20003970:	f002 ff64 	bl	2000683c <__ssprint_r>
20003974:	2800      	cmp	r0, #0
20003976:	f43f acde 	beq.w	20003336 <_svfprintf_r+0x476>
2000397a:	f7ff bb66 	b.w	2000304a <_svfprintf_r+0x18a>
2000397e:	4614      	mov	r4, r2
20003980:	f8cd 803c 	str.w	r8, [sp, #60]	; 0x3c
20003984:	f7ff bba8 	b.w	200030d8 <_svfprintf_r+0x218>
20003988:	9809      	ldr	r0, [sp, #36]	; 0x24
2000398a:	4649      	mov	r1, r9
2000398c:	aa23      	add	r2, sp, #140	; 0x8c
2000398e:	f002 ff55 	bl	2000683c <__ssprint_r>
20003992:	2800      	cmp	r0, #0
20003994:	f47f ab59 	bne.w	2000304a <_svfprintf_r+0x18a>
20003998:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
2000399c:	46c3      	mov	fp, r8
2000399e:	e466      	b.n	2000326e <_svfprintf_r+0x3ae>
200039a0:	9809      	ldr	r0, [sp, #36]	; 0x24
200039a2:	4649      	mov	r1, r9
200039a4:	aa23      	add	r2, sp, #140	; 0x8c
200039a6:	f002 ff49 	bl	2000683c <__ssprint_r>
200039aa:	2800      	cmp	r0, #0
200039ac:	f47f ab4d 	bne.w	2000304a <_svfprintf_r+0x18a>
200039b0:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
200039b4:	46c3      	mov	fp, r8
200039b6:	e41a      	b.n	200031ee <_svfprintf_r+0x32e>
200039b8:	9809      	ldr	r0, [sp, #36]	; 0x24
200039ba:	4649      	mov	r1, r9
200039bc:	aa23      	add	r2, sp, #140	; 0x8c
200039be:	f002 ff3d 	bl	2000683c <__ssprint_r>
200039c2:	2800      	cmp	r0, #0
200039c4:	f47f ab41 	bne.w	2000304a <_svfprintf_r+0x18a>
200039c8:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
200039cc:	46c3      	mov	fp, r8
200039ce:	f7ff bbfa 	b.w	200031c6 <_svfprintf_r+0x306>
200039d2:	4641      	mov	r1, r8
200039d4:	08e2      	lsrs	r2, r4, #3
200039d6:	f004 0307 	and.w	r3, r4, #7
200039da:	08e8      	lsrs	r0, r5, #3
200039dc:	ea42 7245 	orr.w	r2, r2, r5, lsl #29
200039e0:	3330      	adds	r3, #48	; 0x30
200039e2:	4605      	mov	r5, r0
200039e4:	4614      	mov	r4, r2
200039e6:	f801 3d01 	strb.w	r3, [r1, #-1]!
200039ea:	ea54 0205 	orrs.w	r2, r4, r5
200039ee:	d1f1      	bne.n	200039d4 <_svfprintf_r+0xb14>
200039f0:	9a06      	ldr	r2, [sp, #24]
200039f2:	910f      	str	r1, [sp, #60]	; 0x3c
200039f4:	07d0      	lsls	r0, r2, #31
200039f6:	f100 8164 	bmi.w	20003cc2 <_svfprintf_r+0xe02>
200039fa:	ebc1 0408 	rsb	r4, r1, r8
200039fe:	f7ff bb6b 	b.w	200030d8 <_svfprintf_r+0x218>
20003a02:	2302      	movs	r3, #2
20003a04:	f7ff bb3f 	b.w	20003086 <_svfprintf_r+0x1c6>
20003a08:	9c1d      	ldr	r4, [sp, #116]	; 0x74
20003a0a:	2c00      	cmp	r4, #0
20003a0c:	f340 81f6 	ble.w	20003dfc <_svfprintf_r+0xf3c>
20003a10:	9a12      	ldr	r2, [sp, #72]	; 0x48
20003a12:	9911      	ldr	r1, [sp, #68]	; 0x44
20003a14:	4613      	mov	r3, r2
20003a16:	428a      	cmp	r2, r1
20003a18:	bfa8      	it	ge
20003a1a:	460b      	movge	r3, r1
20003a1c:	990f      	ldr	r1, [sp, #60]	; 0x3c
20003a1e:	461c      	mov	r4, r3
20003a20:	188f      	adds	r7, r1, r2
20003a22:	2c00      	cmp	r4, #0
20003a24:	dd0e      	ble.n	20003a44 <_svfprintf_r+0xb84>
20003a26:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003a28:	44a4      	add	ip, r4
20003a2a:	f8cb 1000 	str.w	r1, [fp]
20003a2e:	3301      	adds	r3, #1
20003a30:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003a34:	f8cb 4004 	str.w	r4, [fp, #4]
20003a38:	2b07      	cmp	r3, #7
20003a3a:	9324      	str	r3, [sp, #144]	; 0x90
20003a3c:	f300 8353 	bgt.w	200040e6 <_svfprintf_r+0x1226>
20003a40:	f10b 0b08 	add.w	fp, fp, #8
20003a44:	9b11      	ldr	r3, [sp, #68]	; 0x44
20003a46:	2c00      	cmp	r4, #0
20003a48:	bfa8      	it	ge
20003a4a:	1b1b      	subge	r3, r3, r4
20003a4c:	2b00      	cmp	r3, #0
20003a4e:	461c      	mov	r4, r3
20003a50:	f340 80a1 	ble.w	20003b96 <_svfprintf_r+0xcd6>
20003a54:	2c10      	cmp	r4, #16
20003a56:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003a58:	f340 846c 	ble.w	20004334 <_svfprintf_r+0x1474>
20003a5c:	2610      	movs	r6, #16
20003a5e:	4d45      	ldr	r5, [pc, #276]	; (20003b74 <_svfprintf_r+0xcb4>)
20003a60:	4662      	mov	r2, ip
20003a62:	f8dd a024 	ldr.w	sl, [sp, #36]	; 0x24
20003a66:	e003      	b.n	20003a70 <_svfprintf_r+0xbb0>
20003a68:	3c10      	subs	r4, #16
20003a6a:	2c10      	cmp	r4, #16
20003a6c:	f340 8084 	ble.w	20003b78 <_svfprintf_r+0xcb8>
20003a70:	3301      	adds	r3, #1
20003a72:	3210      	adds	r2, #16
20003a74:	f8cb 5000 	str.w	r5, [fp]
20003a78:	f10b 0b08 	add.w	fp, fp, #8
20003a7c:	2b07      	cmp	r3, #7
20003a7e:	f84b 6c04 	str.w	r6, [fp, #-4]
20003a82:	9225      	str	r2, [sp, #148]	; 0x94
20003a84:	9324      	str	r3, [sp, #144]	; 0x90
20003a86:	ddef      	ble.n	20003a68 <_svfprintf_r+0xba8>
20003a88:	4650      	mov	r0, sl
20003a8a:	4649      	mov	r1, r9
20003a8c:	aa23      	add	r2, sp, #140	; 0x8c
20003a8e:	46c3      	mov	fp, r8
20003a90:	f002 fed4 	bl	2000683c <__ssprint_r>
20003a94:	2800      	cmp	r0, #0
20003a96:	f47f aad8 	bne.w	2000304a <_svfprintf_r+0x18a>
20003a9a:	9a25      	ldr	r2, [sp, #148]	; 0x94
20003a9c:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003a9e:	e7e3      	b.n	20003a68 <_svfprintf_r+0xba8>
20003aa0:	2d00      	cmp	r5, #0
20003aa2:	bf08      	it	eq
20003aa4:	2c0a      	cmpeq	r4, #10
20003aa6:	f080 811a 	bcs.w	20003cde <_svfprintf_r+0xe1e>
20003aaa:	f50d 7a80 	add.w	sl, sp, #256	; 0x100
20003aae:	3430      	adds	r4, #48	; 0x30
20003ab0:	f80a 4d41 	strb.w	r4, [sl, #-65]!
20003ab4:	ebca 0408 	rsb	r4, sl, r8
20003ab8:	f8cd a03c 	str.w	sl, [sp, #60]	; 0x3c
20003abc:	f7ff bb0c 	b.w	200030d8 <_svfprintf_r+0x218>
20003ac0:	461c      	mov	r4, r3
20003ac2:	f8cd 803c 	str.w	r8, [sp, #60]	; 0x3c
20003ac6:	f7ff bb07 	b.w	200030d8 <_svfprintf_r+0x218>
20003aca:	1e5f      	subs	r7, r3, #1
20003acc:	2f00      	cmp	r7, #0
20003ace:	f77f af31 	ble.w	20003934 <_svfprintf_r+0xa74>
20003ad2:	2f10      	cmp	r7, #16
20003ad4:	4d27      	ldr	r5, [pc, #156]	; (20003b74 <_svfprintf_r+0xcb4>)
20003ad6:	bfc8      	it	gt
20003ad8:	f04f 0a10 	movgt.w	sl, #16
20003adc:	dc03      	bgt.n	20003ae6 <_svfprintf_r+0xc26>
20003ade:	e01c      	b.n	20003b1a <_svfprintf_r+0xc5a>
20003ae0:	3f10      	subs	r7, #16
20003ae2:	2f10      	cmp	r7, #16
20003ae4:	dd19      	ble.n	20003b1a <_svfprintf_r+0xc5a>
20003ae6:	3401      	adds	r4, #1
20003ae8:	3610      	adds	r6, #16
20003aea:	f8cb 5000 	str.w	r5, [fp]
20003aee:	f10b 0b08 	add.w	fp, fp, #8
20003af2:	2c07      	cmp	r4, #7
20003af4:	f84b ac04 	str.w	sl, [fp, #-4]
20003af8:	9625      	str	r6, [sp, #148]	; 0x94
20003afa:	9424      	str	r4, [sp, #144]	; 0x90
20003afc:	ddf0      	ble.n	20003ae0 <_svfprintf_r+0xc20>
20003afe:	9809      	ldr	r0, [sp, #36]	; 0x24
20003b00:	4649      	mov	r1, r9
20003b02:	aa23      	add	r2, sp, #140	; 0x8c
20003b04:	46c3      	mov	fp, r8
20003b06:	f002 fe99 	bl	2000683c <__ssprint_r>
20003b0a:	2800      	cmp	r0, #0
20003b0c:	f47f aa9d 	bne.w	2000304a <_svfprintf_r+0x18a>
20003b10:	3f10      	subs	r7, #16
20003b12:	9e25      	ldr	r6, [sp, #148]	; 0x94
20003b14:	9c24      	ldr	r4, [sp, #144]	; 0x90
20003b16:	2f10      	cmp	r7, #16
20003b18:	dce5      	bgt.n	20003ae6 <_svfprintf_r+0xc26>
20003b1a:	3401      	adds	r4, #1
20003b1c:	443e      	add	r6, r7
20003b1e:	f8cb 5000 	str.w	r5, [fp]
20003b22:	2c07      	cmp	r4, #7
20003b24:	9625      	str	r6, [sp, #148]	; 0x94
20003b26:	9424      	str	r4, [sp, #144]	; 0x90
20003b28:	f8cb 7004 	str.w	r7, [fp, #4]
20003b2c:	f77f af00 	ble.w	20003930 <_svfprintf_r+0xa70>
20003b30:	9809      	ldr	r0, [sp, #36]	; 0x24
20003b32:	4649      	mov	r1, r9
20003b34:	aa23      	add	r2, sp, #140	; 0x8c
20003b36:	f002 fe81 	bl	2000683c <__ssprint_r>
20003b3a:	2800      	cmp	r0, #0
20003b3c:	f47f aa85 	bne.w	2000304a <_svfprintf_r+0x18a>
20003b40:	9e25      	ldr	r6, [sp, #148]	; 0x94
20003b42:	46c3      	mov	fp, r8
20003b44:	9c24      	ldr	r4, [sp, #144]	; 0x90
20003b46:	e6f5      	b.n	20003934 <_svfprintf_r+0xa74>
20003b48:	4694      	mov	ip, r2
20003b4a:	3301      	adds	r3, #1
20003b4c:	44a4      	add	ip, r4
20003b4e:	f8cb 5000 	str.w	r5, [fp]
20003b52:	2b07      	cmp	r3, #7
20003b54:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003b58:	9324      	str	r3, [sp, #144]	; 0x90
20003b5a:	f8cb 4004 	str.w	r4, [fp, #4]
20003b5e:	f77f ab98 	ble.w	20003292 <_svfprintf_r+0x3d2>
20003b62:	e6f6      	b.n	20003952 <_svfprintf_r+0xa92>
20003b64:	990c      	ldr	r1, [sp, #48]	; 0x30
20003b66:	2500      	movs	r5, #0
20003b68:	460a      	mov	r2, r1
20003b6a:	680c      	ldr	r4, [r1, #0]
20003b6c:	3204      	adds	r2, #4
20003b6e:	920c      	str	r2, [sp, #48]	; 0x30
20003b70:	f7ff ba89 	b.w	20003086 <_svfprintf_r+0x1c6>
20003b74:	2000905c 	.word	0x2000905c
20003b78:	4694      	mov	ip, r2
20003b7a:	3301      	adds	r3, #1
20003b7c:	44a4      	add	ip, r4
20003b7e:	f8cb 5000 	str.w	r5, [fp]
20003b82:	2b07      	cmp	r3, #7
20003b84:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003b88:	9324      	str	r3, [sp, #144]	; 0x90
20003b8a:	f8cb 4004 	str.w	r4, [fp, #4]
20003b8e:	f300 82df 	bgt.w	20004150 <_svfprintf_r+0x1290>
20003b92:	f10b 0b08 	add.w	fp, fp, #8
20003b96:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
20003b98:	9911      	ldr	r1, [sp, #68]	; 0x44
20003b9a:	9b1d      	ldr	r3, [sp, #116]	; 0x74
20003b9c:	440a      	add	r2, r1
20003b9e:	4616      	mov	r6, r2
20003ba0:	9a12      	ldr	r2, [sp, #72]	; 0x48
20003ba2:	4293      	cmp	r3, r2
20003ba4:	db45      	blt.n	20003c32 <_svfprintf_r+0xd72>
20003ba6:	9a06      	ldr	r2, [sp, #24]
20003ba8:	07d0      	lsls	r0, r2, #31
20003baa:	d442      	bmi.n	20003c32 <_svfprintf_r+0xd72>
20003bac:	9a12      	ldr	r2, [sp, #72]	; 0x48
20003bae:	1bbc      	subs	r4, r7, r6
20003bb0:	1ad3      	subs	r3, r2, r3
20003bb2:	429c      	cmp	r4, r3
20003bb4:	bfa8      	it	ge
20003bb6:	461c      	movge	r4, r3
20003bb8:	2c00      	cmp	r4, #0
20003bba:	4625      	mov	r5, r4
20003bbc:	dd0e      	ble.n	20003bdc <_svfprintf_r+0xd1c>
20003bbe:	9a24      	ldr	r2, [sp, #144]	; 0x90
20003bc0:	44a4      	add	ip, r4
20003bc2:	f8cb 6000 	str.w	r6, [fp]
20003bc6:	3201      	adds	r2, #1
20003bc8:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003bcc:	f8cb 4004 	str.w	r4, [fp, #4]
20003bd0:	2a07      	cmp	r2, #7
20003bd2:	9224      	str	r2, [sp, #144]	; 0x90
20003bd4:	f300 82a0 	bgt.w	20004118 <_svfprintf_r+0x1258>
20003bd8:	f10b 0b08 	add.w	fp, fp, #8
20003bdc:	2d00      	cmp	r5, #0
20003bde:	bfac      	ite	ge
20003be0:	1b5c      	subge	r4, r3, r5
20003be2:	461c      	movlt	r4, r3
20003be4:	2c00      	cmp	r4, #0
20003be6:	f77f ab56 	ble.w	20003296 <_svfprintf_r+0x3d6>
20003bea:	2c10      	cmp	r4, #16
20003bec:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003bee:	f340 8396 	ble.w	2000431e <_svfprintf_r+0x145e>
20003bf2:	2610      	movs	r6, #16
20003bf4:	4db1      	ldr	r5, [pc, #708]	; (20003ebc <_svfprintf_r+0xffc>)
20003bf6:	4662      	mov	r2, ip
20003bf8:	9f09      	ldr	r7, [sp, #36]	; 0x24
20003bfa:	e002      	b.n	20003c02 <_svfprintf_r+0xd42>
20003bfc:	3c10      	subs	r4, #16
20003bfe:	2c10      	cmp	r4, #16
20003c00:	dda2      	ble.n	20003b48 <_svfprintf_r+0xc88>
20003c02:	3301      	adds	r3, #1
20003c04:	3210      	adds	r2, #16
20003c06:	f8cb 5000 	str.w	r5, [fp]
20003c0a:	f10b 0b08 	add.w	fp, fp, #8
20003c0e:	2b07      	cmp	r3, #7
20003c10:	f84b 6c04 	str.w	r6, [fp, #-4]
20003c14:	9225      	str	r2, [sp, #148]	; 0x94
20003c16:	9324      	str	r3, [sp, #144]	; 0x90
20003c18:	ddf0      	ble.n	20003bfc <_svfprintf_r+0xd3c>
20003c1a:	4638      	mov	r0, r7
20003c1c:	4649      	mov	r1, r9
20003c1e:	aa23      	add	r2, sp, #140	; 0x8c
20003c20:	46c3      	mov	fp, r8
20003c22:	f002 fe0b 	bl	2000683c <__ssprint_r>
20003c26:	2800      	cmp	r0, #0
20003c28:	f47f aa0f 	bne.w	2000304a <_svfprintf_r+0x18a>
20003c2c:	9a25      	ldr	r2, [sp, #148]	; 0x94
20003c2e:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003c30:	e7e4      	b.n	20003bfc <_svfprintf_r+0xd3c>
20003c32:	9a24      	ldr	r2, [sp, #144]	; 0x90
20003c34:	9817      	ldr	r0, [sp, #92]	; 0x5c
20003c36:	3201      	adds	r2, #1
20003c38:	9916      	ldr	r1, [sp, #88]	; 0x58
20003c3a:	4484      	add	ip, r0
20003c3c:	f8cb 0004 	str.w	r0, [fp, #4]
20003c40:	2a07      	cmp	r2, #7
20003c42:	f8cb 1000 	str.w	r1, [fp]
20003c46:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003c4a:	9224      	str	r2, [sp, #144]	; 0x90
20003c4c:	f300 8273 	bgt.w	20004136 <_svfprintf_r+0x1276>
20003c50:	f10b 0b08 	add.w	fp, fp, #8
20003c54:	e7aa      	b.n	20003bac <_svfprintf_r+0xcec>
20003c56:	9b06      	ldr	r3, [sp, #24]
20003c58:	07d9      	lsls	r1, r3, #31
20003c5a:	f53f ae33 	bmi.w	200038c4 <_svfprintf_r+0xa04>
20003c5e:	2301      	movs	r3, #1
20003c60:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
20003c62:	9625      	str	r6, [sp, #148]	; 0x94
20003c64:	441c      	add	r4, r3
20003c66:	f8cb 2000 	str.w	r2, [fp]
20003c6a:	f8cb 3004 	str.w	r3, [fp, #4]
20003c6e:	2c07      	cmp	r4, #7
20003c70:	9424      	str	r4, [sp, #144]	; 0x90
20003c72:	f77f ae5d 	ble.w	20003930 <_svfprintf_r+0xa70>
20003c76:	e75b      	b.n	20003b30 <_svfprintf_r+0xc70>
20003c78:	9809      	ldr	r0, [sp, #36]	; 0x24
20003c7a:	4649      	mov	r1, r9
20003c7c:	aa23      	add	r2, sp, #140	; 0x8c
20003c7e:	f002 fddd 	bl	2000683c <__ssprint_r>
20003c82:	2800      	cmp	r0, #0
20003c84:	f47f a9e1 	bne.w	2000304a <_svfprintf_r+0x18a>
20003c88:	9e25      	ldr	r6, [sp, #148]	; 0x94
20003c8a:	46c3      	mov	fp, r8
20003c8c:	9c24      	ldr	r4, [sp, #144]	; 0x90
20003c8e:	e636      	b.n	200038fe <_svfprintf_r+0xa3e>
20003c90:	9809      	ldr	r0, [sp, #36]	; 0x24
20003c92:	4649      	mov	r1, r9
20003c94:	aa23      	add	r2, sp, #140	; 0x8c
20003c96:	f002 fdd1 	bl	2000683c <__ssprint_r>
20003c9a:	2800      	cmp	r0, #0
20003c9c:	f47f a9d5 	bne.w	2000304a <_svfprintf_r+0x18a>
20003ca0:	9e25      	ldr	r6, [sp, #148]	; 0x94
20003ca2:	46c3      	mov	fp, r8
20003ca4:	9c24      	ldr	r4, [sp, #144]	; 0x90
20003ca6:	e61b      	b.n	200038e0 <_svfprintf_r+0xa20>
20003ca8:	9809      	ldr	r0, [sp, #36]	; 0x24
20003caa:	4649      	mov	r1, r9
20003cac:	aa23      	add	r2, sp, #140	; 0x8c
20003cae:	f002 fdc5 	bl	2000683c <__ssprint_r>
20003cb2:	2800      	cmp	r0, #0
20003cb4:	f47f a9c9 	bne.w	2000304a <_svfprintf_r+0x18a>
20003cb8:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
20003cbc:	46c3      	mov	fp, r8
20003cbe:	f7ff ba9a 	b.w	200031f6 <_svfprintf_r+0x336>
20003cc2:	2b30      	cmp	r3, #48	; 0x30
20003cc4:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
20003cc6:	f43f adb4 	beq.w	20003832 <_svfprintf_r+0x972>
20003cca:	3b01      	subs	r3, #1
20003ccc:	461a      	mov	r2, r3
20003cce:	930f      	str	r3, [sp, #60]	; 0x3c
20003cd0:	2330      	movs	r3, #48	; 0x30
20003cd2:	ebc2 0408 	rsb	r4, r2, r8
20003cd6:	f801 3c01 	strb.w	r3, [r1, #-1]
20003cda:	f7ff b9fd 	b.w	200030d8 <_svfprintf_r+0x218>
20003cde:	46c2      	mov	sl, r8
20003ce0:	f8cd c01c 	str.w	ip, [sp, #28]
20003ce4:	4620      	mov	r0, r4
20003ce6:	4629      	mov	r1, r5
20003ce8:	220a      	movs	r2, #10
20003cea:	2300      	movs	r3, #0
20003cec:	f004 fc04 	bl	200084f8 <__aeabi_uldivmod>
20003cf0:	f102 0e30 	add.w	lr, r2, #48	; 0x30
20003cf4:	4620      	mov	r0, r4
20003cf6:	4629      	mov	r1, r5
20003cf8:	2300      	movs	r3, #0
20003cfa:	220a      	movs	r2, #10
20003cfc:	f80a ed01 	strb.w	lr, [sl, #-1]!
20003d00:	f004 fbfa 	bl	200084f8 <__aeabi_uldivmod>
20003d04:	4604      	mov	r4, r0
20003d06:	460d      	mov	r5, r1
20003d08:	ea54 0305 	orrs.w	r3, r4, r5
20003d0c:	d1ea      	bne.n	20003ce4 <_svfprintf_r+0xe24>
20003d0e:	f8dd c01c 	ldr.w	ip, [sp, #28]
20003d12:	ebca 0408 	rsb	r4, sl, r8
20003d16:	f8cd a03c 	str.w	sl, [sp, #60]	; 0x3c
20003d1a:	f7ff b9dd 	b.w	200030d8 <_svfprintf_r+0x218>
20003d1e:	9813      	ldr	r0, [sp, #76]	; 0x4c
20003d20:	9914      	ldr	r1, [sp, #80]	; 0x50
20003d22:	f8cd c01c 	str.w	ip, [sp, #28]
20003d26:	f002 fbdb 	bl	200064e0 <__fpclassifyd>
20003d2a:	f8dd c01c 	ldr.w	ip, [sp, #28]
20003d2e:	2800      	cmp	r0, #0
20003d30:	f040 80cc 	bne.w	20003ecc <_svfprintf_r+0x100c>
20003d34:	9c06      	ldr	r4, [sp, #24]
20003d36:	2103      	movs	r1, #3
20003d38:	4a61      	ldr	r2, [pc, #388]	; (20003ec0 <_svfprintf_r+0x1000>)
20003d3a:	4b62      	ldr	r3, [pc, #392]	; (20003ec4 <_svfprintf_r+0x1004>)
20003d3c:	f024 0480 	bic.w	r4, r4, #128	; 0x80
20003d40:	9010      	str	r0, [sp, #64]	; 0x40
20003d42:	2e47      	cmp	r6, #71	; 0x47
20003d44:	bfd8      	it	le
20003d46:	461a      	movle	r2, r3
20003d48:	9406      	str	r4, [sp, #24]
20003d4a:	9107      	str	r1, [sp, #28]
20003d4c:	460c      	mov	r4, r1
20003d4e:	9011      	str	r0, [sp, #68]	; 0x44
20003d50:	920f      	str	r2, [sp, #60]	; 0x3c
20003d52:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
20003d56:	f7ff b9c6 	b.w	200030e6 <_svfprintf_r+0x226>
20003d5a:	9b06      	ldr	r3, [sp, #24]
20003d5c:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20003d5e:	f013 0f40 	tst.w	r3, #64	; 0x40
20003d62:	4613      	mov	r3, r2
20003d64:	f43f abc9 	beq.w	200034fa <_svfprintf_r+0x63a>
20003d68:	8814      	ldrh	r4, [r2, #0]
20003d6a:	3204      	adds	r2, #4
20003d6c:	2500      	movs	r5, #0
20003d6e:	2301      	movs	r3, #1
20003d70:	920c      	str	r2, [sp, #48]	; 0x30
20003d72:	f7ff b988 	b.w	20003086 <_svfprintf_r+0x1c6>
20003d76:	9b06      	ldr	r3, [sp, #24]
20003d78:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20003d7a:	f013 0f40 	tst.w	r3, #64	; 0x40
20003d7e:	4613      	mov	r3, r2
20003d80:	f000 8130 	beq.w	20003fe4 <_svfprintf_r+0x1124>
20003d84:	3304      	adds	r3, #4
20003d86:	8814      	ldrh	r4, [r2, #0]
20003d88:	2500      	movs	r5, #0
20003d8a:	930c      	str	r3, [sp, #48]	; 0x30
20003d8c:	f7ff bbcd 	b.w	2000352a <_svfprintf_r+0x66a>
20003d90:	9b06      	ldr	r3, [sp, #24]
20003d92:	06dd      	lsls	r5, r3, #27
20003d94:	d40b      	bmi.n	20003dae <_svfprintf_r+0xeee>
20003d96:	9b06      	ldr	r3, [sp, #24]
20003d98:	065c      	lsls	r4, r3, #25
20003d9a:	d508      	bpl.n	20003dae <_svfprintf_r+0xeee>
20003d9c:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20003d9e:	6813      	ldr	r3, [r2, #0]
20003da0:	3204      	adds	r2, #4
20003da2:	920c      	str	r2, [sp, #48]	; 0x30
20003da4:	f8bd 2028 	ldrh.w	r2, [sp, #40]	; 0x28
20003da8:	801a      	strh	r2, [r3, #0]
20003daa:	f7ff b8af 	b.w	20002f0c <_svfprintf_r+0x4c>
20003dae:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20003db0:	6813      	ldr	r3, [r2, #0]
20003db2:	3204      	adds	r2, #4
20003db4:	920c      	str	r2, [sp, #48]	; 0x30
20003db6:	9a0a      	ldr	r2, [sp, #40]	; 0x28
20003db8:	601a      	str	r2, [r3, #0]
20003dba:	f7ff b8a7 	b.w	20002f0c <_svfprintf_r+0x4c>
20003dbe:	9b06      	ldr	r3, [sp, #24]
20003dc0:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20003dc2:	f013 0f40 	tst.w	r3, #64	; 0x40
20003dc6:	4613      	mov	r3, r2
20003dc8:	f43f ac26 	beq.w	20003618 <_svfprintf_r+0x758>
20003dcc:	f9b2 4000 	ldrsh.w	r4, [r2]
20003dd0:	3304      	adds	r3, #4
20003dd2:	17e5      	asrs	r5, r4, #31
20003dd4:	930c      	str	r3, [sp, #48]	; 0x30
20003dd6:	4622      	mov	r2, r4
20003dd8:	462b      	mov	r3, r5
20003dda:	f7ff baf9 	b.w	200033d0 <_svfprintf_r+0x510>
20003dde:	4d37      	ldr	r5, [pc, #220]	; (20003ebc <_svfprintf_r+0xffc>)
20003de0:	f7ff ba37 	b.w	20003252 <_svfprintf_r+0x392>
20003de4:	9809      	ldr	r0, [sp, #36]	; 0x24
20003de6:	4649      	mov	r1, r9
20003de8:	aa23      	add	r2, sp, #140	; 0x8c
20003dea:	f002 fd27 	bl	2000683c <__ssprint_r>
20003dee:	2800      	cmp	r0, #0
20003df0:	f47f a92b 	bne.w	2000304a <_svfprintf_r+0x18a>
20003df4:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
20003df8:	46c3      	mov	fp, r8
20003dfa:	e4b2      	b.n	20003762 <_svfprintf_r+0x8a2>
20003dfc:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003dfe:	2201      	movs	r2, #1
20003e00:	f10c 0c01 	add.w	ip, ip, #1
20003e04:	4930      	ldr	r1, [pc, #192]	; (20003ec8 <_svfprintf_r+0x1008>)
20003e06:	4413      	add	r3, r2
20003e08:	f8cb 2004 	str.w	r2, [fp, #4]
20003e0c:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
20003e10:	2b07      	cmp	r3, #7
20003e12:	f8cb 1000 	str.w	r1, [fp]
20003e16:	9324      	str	r3, [sp, #144]	; 0x90
20003e18:	f300 80fa 	bgt.w	20004010 <_svfprintf_r+0x1150>
20003e1c:	f10b 0b08 	add.w	fp, fp, #8
20003e20:	b92c      	cbnz	r4, 20003e2e <_svfprintf_r+0xf6e>
20003e22:	9b12      	ldr	r3, [sp, #72]	; 0x48
20003e24:	b91b      	cbnz	r3, 20003e2e <_svfprintf_r+0xf6e>
20003e26:	9b06      	ldr	r3, [sp, #24]
20003e28:	07dd      	lsls	r5, r3, #31
20003e2a:	f57f aa34 	bpl.w	20003296 <_svfprintf_r+0x3d6>
20003e2e:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003e30:	9917      	ldr	r1, [sp, #92]	; 0x5c
20003e32:	3301      	adds	r3, #1
20003e34:	9816      	ldr	r0, [sp, #88]	; 0x58
20003e36:	eb0c 0201 	add.w	r2, ip, r1
20003e3a:	f8cb 1004 	str.w	r1, [fp, #4]
20003e3e:	2b07      	cmp	r3, #7
20003e40:	f8cb 0000 	str.w	r0, [fp]
20003e44:	9225      	str	r2, [sp, #148]	; 0x94
20003e46:	9324      	str	r3, [sp, #144]	; 0x90
20003e48:	f300 824c 	bgt.w	200042e4 <_svfprintf_r+0x1424>
20003e4c:	f10b 0b08 	add.w	fp, fp, #8
20003e50:	4264      	negs	r4, r4
20003e52:	2c00      	cmp	r4, #0
20003e54:	f340 8126 	ble.w	200040a4 <_svfprintf_r+0x11e4>
20003e58:	2c10      	cmp	r4, #16
20003e5a:	4d18      	ldr	r5, [pc, #96]	; (20003ebc <_svfprintf_r+0xffc>)
20003e5c:	f340 814f 	ble.w	200040fe <_svfprintf_r+0x123e>
20003e60:	2610      	movs	r6, #16
20003e62:	9f09      	ldr	r7, [sp, #36]	; 0x24
20003e64:	e003      	b.n	20003e6e <_svfprintf_r+0xfae>
20003e66:	3c10      	subs	r4, #16
20003e68:	2c10      	cmp	r4, #16
20003e6a:	f340 8148 	ble.w	200040fe <_svfprintf_r+0x123e>
20003e6e:	3301      	adds	r3, #1
20003e70:	3210      	adds	r2, #16
20003e72:	f8cb 5000 	str.w	r5, [fp]
20003e76:	f10b 0b08 	add.w	fp, fp, #8
20003e7a:	2b07      	cmp	r3, #7
20003e7c:	f84b 6c04 	str.w	r6, [fp, #-4]
20003e80:	9225      	str	r2, [sp, #148]	; 0x94
20003e82:	9324      	str	r3, [sp, #144]	; 0x90
20003e84:	ddef      	ble.n	20003e66 <_svfprintf_r+0xfa6>
20003e86:	4638      	mov	r0, r7
20003e88:	4649      	mov	r1, r9
20003e8a:	aa23      	add	r2, sp, #140	; 0x8c
20003e8c:	46c3      	mov	fp, r8
20003e8e:	f002 fcd5 	bl	2000683c <__ssprint_r>
20003e92:	2800      	cmp	r0, #0
20003e94:	f47f a8d9 	bne.w	2000304a <_svfprintf_r+0x18a>
20003e98:	9a25      	ldr	r2, [sp, #148]	; 0x94
20003e9a:	9b24      	ldr	r3, [sp, #144]	; 0x90
20003e9c:	e7e3      	b.n	20003e66 <_svfprintf_r+0xfa6>
20003e9e:	f041 0120 	orr.w	r1, r1, #32
20003ea2:	785e      	ldrb	r6, [r3, #1]
20003ea4:	9106      	str	r1, [sp, #24]
20003ea6:	1c59      	adds	r1, r3, #1
20003ea8:	f7ff b85f 	b.w	20002f6a <_svfprintf_r+0xaa>
20003eac:	980b      	ldr	r0, [sp, #44]	; 0x2c
20003eae:	910c      	str	r1, [sp, #48]	; 0x30
20003eb0:	4619      	mov	r1, r3
20003eb2:	4240      	negs	r0, r0
20003eb4:	900b      	str	r0, [sp, #44]	; 0x2c
20003eb6:	f7ff ba43 	b.w	20003340 <_svfprintf_r+0x480>
20003eba:	bf00      	nop
20003ebc:	2000905c 	.word	0x2000905c
20003ec0:	20009088 	.word	0x20009088
20003ec4:	20009084 	.word	0x20009084
20003ec8:	200090bc 	.word	0x200090bc
20003ecc:	f1bc 3fff 	cmp.w	ip, #4294967295
20003ed0:	f026 0a20 	bic.w	sl, r6, #32
20003ed4:	f000 80a9 	beq.w	2000402a <_svfprintf_r+0x116a>
20003ed8:	f1ba 0f47 	cmp.w	sl, #71	; 0x47
20003edc:	f040 80a7 	bne.w	2000402e <_svfprintf_r+0x116e>
20003ee0:	f1bc 0f00 	cmp.w	ip, #0
20003ee4:	f040 80a3 	bne.w	2000402e <_svfprintf_r+0x116e>
20003ee8:	9b06      	ldr	r3, [sp, #24]
20003eea:	f04f 0a47 	mov.w	sl, #71	; 0x47
20003eee:	f04f 0c01 	mov.w	ip, #1
20003ef2:	f443 7380 	orr.w	r3, r3, #256	; 0x100
20003ef6:	930e      	str	r3, [sp, #56]	; 0x38
20003ef8:	9b14      	ldr	r3, [sp, #80]	; 0x50
20003efa:	2b00      	cmp	r3, #0
20003efc:	f2c0 81ff 	blt.w	200042fe <_svfprintf_r+0x143e>
20003f00:	461d      	mov	r5, r3
20003f02:	9f13      	ldr	r7, [sp, #76]	; 0x4c
20003f04:	2300      	movs	r3, #0
20003f06:	930d      	str	r3, [sp, #52]	; 0x34
20003f08:	2002      	movs	r0, #2
20003f0a:	a91d      	add	r1, sp, #116	; 0x74
20003f0c:	f8cd c004 	str.w	ip, [sp, #4]
20003f10:	463a      	mov	r2, r7
20003f12:	9000      	str	r0, [sp, #0]
20003f14:	a81e      	add	r0, sp, #120	; 0x78
20003f16:	9102      	str	r1, [sp, #8]
20003f18:	a921      	add	r1, sp, #132	; 0x84
20003f1a:	9003      	str	r0, [sp, #12]
20003f1c:	462b      	mov	r3, r5
20003f1e:	9809      	ldr	r0, [sp, #36]	; 0x24
20003f20:	9104      	str	r1, [sp, #16]
20003f22:	f8cd c01c 	str.w	ip, [sp, #28]
20003f26:	f000 fb1b 	bl	20004560 <_dtoa_r>
20003f2a:	f1ba 0f47 	cmp.w	sl, #71	; 0x47
20003f2e:	900f      	str	r0, [sp, #60]	; 0x3c
20003f30:	f8dd c01c 	ldr.w	ip, [sp, #28]
20003f34:	f040 8242 	bne.w	200043bc <_svfprintf_r+0x14fc>
20003f38:	9b06      	ldr	r3, [sp, #24]
20003f3a:	07db      	lsls	r3, r3, #31
20003f3c:	f140 81f3 	bpl.w	20004326 <_svfprintf_r+0x1466>
20003f40:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
20003f42:	eb03 040c 	add.w	r4, r3, ip
20003f46:	4638      	mov	r0, r7
20003f48:	2200      	movs	r2, #0
20003f4a:	2300      	movs	r3, #0
20003f4c:	4629      	mov	r1, r5
20003f4e:	f8cd c01c 	str.w	ip, [sp, #28]
20003f52:	f004 fa77 	bl	20008444 <__aeabi_dcmpeq>
20003f56:	f8dd c01c 	ldr.w	ip, [sp, #28]
20003f5a:	2800      	cmp	r0, #0
20003f5c:	f040 8172 	bne.w	20004244 <_svfprintf_r+0x1384>
20003f60:	9b21      	ldr	r3, [sp, #132]	; 0x84
20003f62:	429c      	cmp	r4, r3
20003f64:	d906      	bls.n	20003f74 <_svfprintf_r+0x10b4>
20003f66:	2130      	movs	r1, #48	; 0x30
20003f68:	1c5a      	adds	r2, r3, #1
20003f6a:	9221      	str	r2, [sp, #132]	; 0x84
20003f6c:	7019      	strb	r1, [r3, #0]
20003f6e:	9b21      	ldr	r3, [sp, #132]	; 0x84
20003f70:	429c      	cmp	r4, r3
20003f72:	d8f9      	bhi.n	20003f68 <_svfprintf_r+0x10a8>
20003f74:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
20003f76:	f1ba 0f47 	cmp.w	sl, #71	; 0x47
20003f7a:	eba3 0302 	sub.w	r3, r3, r2
20003f7e:	9312      	str	r3, [sp, #72]	; 0x48
20003f80:	9b1d      	ldr	r3, [sp, #116]	; 0x74
20003f82:	f040 8113 	bne.w	200041ac <_svfprintf_r+0x12ec>
20003f86:	459c      	cmp	ip, r3
20003f88:	f2c0 8168 	blt.w	2000425c <_svfprintf_r+0x139c>
20003f8c:	1cd9      	adds	r1, r3, #3
20003f8e:	f2c0 8165 	blt.w	2000425c <_svfprintf_r+0x139c>
20003f92:	2667      	movs	r6, #103	; 0x67
20003f94:	9311      	str	r3, [sp, #68]	; 0x44
20003f96:	9b12      	ldr	r3, [sp, #72]	; 0x48
20003f98:	9a11      	ldr	r2, [sp, #68]	; 0x44
20003f9a:	4293      	cmp	r3, r2
20003f9c:	f300 81b6 	bgt.w	2000430c <_svfprintf_r+0x144c>
20003fa0:	9b06      	ldr	r3, [sp, #24]
20003fa2:	07df      	lsls	r7, r3, #31
20003fa4:	f100 81c1 	bmi.w	2000432a <_svfprintf_r+0x146a>
20003fa8:	4614      	mov	r4, r2
20003faa:	ea22 7ce2 	bic.w	ip, r2, r2, asr #31
20003fae:	9b0d      	ldr	r3, [sp, #52]	; 0x34
20003fb0:	2b00      	cmp	r3, #0
20003fb2:	f040 80f0 	bne.w	20004196 <_svfprintf_r+0x12d6>
20003fb6:	9b0e      	ldr	r3, [sp, #56]	; 0x38
20003fb8:	f8cd c01c 	str.w	ip, [sp, #28]
20003fbc:	9306      	str	r3, [sp, #24]
20003fbe:	9b0d      	ldr	r3, [sp, #52]	; 0x34
20003fc0:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
20003fc4:	9310      	str	r3, [sp, #64]	; 0x40
20003fc6:	f7ff b88e 	b.w	200030e6 <_svfprintf_r+0x226>
20003fca:	9809      	ldr	r0, [sp, #36]	; 0x24
20003fcc:	4649      	mov	r1, r9
20003fce:	aa23      	add	r2, sp, #140	; 0x8c
20003fd0:	f002 fc34 	bl	2000683c <__ssprint_r>
20003fd4:	2800      	cmp	r0, #0
20003fd6:	f47f a838 	bne.w	2000304a <_svfprintf_r+0x18a>
20003fda:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
20003fde:	46c3      	mov	fp, r8
20003fe0:	f7ff bbd8 	b.w	20003794 <_svfprintf_r+0x8d4>
20003fe4:	3304      	adds	r3, #4
20003fe6:	6814      	ldr	r4, [r2, #0]
20003fe8:	2500      	movs	r5, #0
20003fea:	930c      	str	r3, [sp, #48]	; 0x30
20003fec:	f7ff ba9d 	b.w	2000352a <_svfprintf_r+0x66a>
20003ff0:	9809      	ldr	r0, [sp, #36]	; 0x24
20003ff2:	2140      	movs	r1, #64	; 0x40
20003ff4:	f001 fa6a 	bl	200054cc <_malloc_r>
20003ff8:	f8c9 0000 	str.w	r0, [r9]
20003ffc:	f8c9 0010 	str.w	r0, [r9, #16]
20004000:	2800      	cmp	r0, #0
20004002:	f000 81d4 	beq.w	200043ae <_svfprintf_r+0x14ee>
20004006:	2340      	movs	r3, #64	; 0x40
20004008:	f8c9 3014 	str.w	r3, [r9, #20]
2000400c:	f7fe bf70 	b.w	20002ef0 <_svfprintf_r+0x30>
20004010:	9809      	ldr	r0, [sp, #36]	; 0x24
20004012:	4649      	mov	r1, r9
20004014:	aa23      	add	r2, sp, #140	; 0x8c
20004016:	f002 fc11 	bl	2000683c <__ssprint_r>
2000401a:	2800      	cmp	r0, #0
2000401c:	f47f a815 	bne.w	2000304a <_svfprintf_r+0x18a>
20004020:	9c1d      	ldr	r4, [sp, #116]	; 0x74
20004022:	46c3      	mov	fp, r8
20004024:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
20004028:	e6fa      	b.n	20003e20 <_svfprintf_r+0xf60>
2000402a:	f04f 0c06 	mov.w	ip, #6
2000402e:	9b06      	ldr	r3, [sp, #24]
20004030:	f443 7380 	orr.w	r3, r3, #256	; 0x100
20004034:	930e      	str	r3, [sp, #56]	; 0x38
20004036:	9b14      	ldr	r3, [sp, #80]	; 0x50
20004038:	2b00      	cmp	r3, #0
2000403a:	f2c0 8160 	blt.w	200042fe <_svfprintf_r+0x143e>
2000403e:	461d      	mov	r5, r3
20004040:	9f13      	ldr	r7, [sp, #76]	; 0x4c
20004042:	2300      	movs	r3, #0
20004044:	930d      	str	r3, [sp, #52]	; 0x34
20004046:	f1ba 0f46 	cmp.w	sl, #70	; 0x46
2000404a:	f000 80c4 	beq.w	200041d6 <_svfprintf_r+0x1316>
2000404e:	f1ba 0f45 	cmp.w	sl, #69	; 0x45
20004052:	f47f af59 	bne.w	20003f08 <_svfprintf_r+0x1048>
20004056:	f10c 0401 	add.w	r4, ip, #1
2000405a:	f8cd c01c 	str.w	ip, [sp, #28]
2000405e:	f10d 0e74 	add.w	lr, sp, #116	; 0x74
20004062:	f10d 0c84 	add.w	ip, sp, #132	; 0x84
20004066:	a81e      	add	r0, sp, #120	; 0x78
20004068:	2102      	movs	r1, #2
2000406a:	f8cd c010 	str.w	ip, [sp, #16]
2000406e:	463a      	mov	r2, r7
20004070:	9003      	str	r0, [sp, #12]
20004072:	462b      	mov	r3, r5
20004074:	9401      	str	r4, [sp, #4]
20004076:	9100      	str	r1, [sp, #0]
20004078:	f8cd e008 	str.w	lr, [sp, #8]
2000407c:	9809      	ldr	r0, [sp, #36]	; 0x24
2000407e:	f000 fa6f 	bl	20004560 <_dtoa_r>
20004082:	f8dd c01c 	ldr.w	ip, [sp, #28]
20004086:	900f      	str	r0, [sp, #60]	; 0x3c
20004088:	9b0f      	ldr	r3, [sp, #60]	; 0x3c
2000408a:	441c      	add	r4, r3
2000408c:	e75b      	b.n	20003f46 <_svfprintf_r+0x1086>
2000408e:	9809      	ldr	r0, [sp, #36]	; 0x24
20004090:	4649      	mov	r1, r9
20004092:	aa23      	add	r2, sp, #140	; 0x8c
20004094:	f002 fbd2 	bl	2000683c <__ssprint_r>
20004098:	2800      	cmp	r0, #0
2000409a:	f47e afd6 	bne.w	2000304a <_svfprintf_r+0x18a>
2000409e:	9a25      	ldr	r2, [sp, #148]	; 0x94
200040a0:	46c3      	mov	fp, r8
200040a2:	9b24      	ldr	r3, [sp, #144]	; 0x90
200040a4:	9812      	ldr	r0, [sp, #72]	; 0x48
200040a6:	3301      	adds	r3, #1
200040a8:	eb02 0c00 	add.w	ip, r2, r0
200040ac:	2b07      	cmp	r3, #7
200040ae:	9a0f      	ldr	r2, [sp, #60]	; 0x3c
200040b0:	f8cd c094 	str.w	ip, [sp, #148]	; 0x94
200040b4:	f8cb 2000 	str.w	r2, [fp]
200040b8:	9324      	str	r3, [sp, #144]	; 0x90
200040ba:	f8cb 0004 	str.w	r0, [fp, #4]
200040be:	f77f a8e8 	ble.w	20003292 <_svfprintf_r+0x3d2>
200040c2:	e446      	b.n	20003952 <_svfprintf_r+0xa92>
200040c4:	9510      	str	r5, [sp, #64]	; 0x40
200040c6:	f7fe fecd 	bl	20002e64 <strlen>
200040ca:	ea20 73e0 	bic.w	r3, r0, r0, asr #31
200040ce:	970c      	str	r7, [sp, #48]	; 0x30
200040d0:	4604      	mov	r4, r0
200040d2:	9511      	str	r5, [sp, #68]	; 0x44
200040d4:	9307      	str	r3, [sp, #28]
200040d6:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
200040da:	f7ff b804 	b.w	200030e6 <_svfprintf_r+0x226>
200040de:	9a24      	ldr	r2, [sp, #144]	; 0x90
200040e0:	4fb7      	ldr	r7, [pc, #732]	; (200043c0 <_svfprintf_r+0x1500>)
200040e2:	f7ff b84b 	b.w	2000317c <_svfprintf_r+0x2bc>
200040e6:	9809      	ldr	r0, [sp, #36]	; 0x24
200040e8:	4649      	mov	r1, r9
200040ea:	aa23      	add	r2, sp, #140	; 0x8c
200040ec:	f002 fba6 	bl	2000683c <__ssprint_r>
200040f0:	2800      	cmp	r0, #0
200040f2:	f47e afaa 	bne.w	2000304a <_svfprintf_r+0x18a>
200040f6:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
200040fa:	46c3      	mov	fp, r8
200040fc:	e4a2      	b.n	20003a44 <_svfprintf_r+0xb84>
200040fe:	3301      	adds	r3, #1
20004100:	4422      	add	r2, r4
20004102:	f8cb 5000 	str.w	r5, [fp]
20004106:	2b07      	cmp	r3, #7
20004108:	9225      	str	r2, [sp, #148]	; 0x94
2000410a:	9324      	str	r3, [sp, #144]	; 0x90
2000410c:	f8cb 4004 	str.w	r4, [fp, #4]
20004110:	dcbd      	bgt.n	2000408e <_svfprintf_r+0x11ce>
20004112:	f10b 0b08 	add.w	fp, fp, #8
20004116:	e7c5      	b.n	200040a4 <_svfprintf_r+0x11e4>
20004118:	9809      	ldr	r0, [sp, #36]	; 0x24
2000411a:	4649      	mov	r1, r9
2000411c:	aa23      	add	r2, sp, #140	; 0x8c
2000411e:	f002 fb8d 	bl	2000683c <__ssprint_r>
20004122:	2800      	cmp	r0, #0
20004124:	f47e af91 	bne.w	2000304a <_svfprintf_r+0x18a>
20004128:	9b1d      	ldr	r3, [sp, #116]	; 0x74
2000412a:	46c3      	mov	fp, r8
2000412c:	9a12      	ldr	r2, [sp, #72]	; 0x48
2000412e:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
20004132:	1ad3      	subs	r3, r2, r3
20004134:	e552      	b.n	20003bdc <_svfprintf_r+0xd1c>
20004136:	9809      	ldr	r0, [sp, #36]	; 0x24
20004138:	4649      	mov	r1, r9
2000413a:	aa23      	add	r2, sp, #140	; 0x8c
2000413c:	f002 fb7e 	bl	2000683c <__ssprint_r>
20004140:	2800      	cmp	r0, #0
20004142:	f47e af82 	bne.w	2000304a <_svfprintf_r+0x18a>
20004146:	9b1d      	ldr	r3, [sp, #116]	; 0x74
20004148:	46c3      	mov	fp, r8
2000414a:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
2000414e:	e52d      	b.n	20003bac <_svfprintf_r+0xcec>
20004150:	9809      	ldr	r0, [sp, #36]	; 0x24
20004152:	4649      	mov	r1, r9
20004154:	aa23      	add	r2, sp, #140	; 0x8c
20004156:	f002 fb71 	bl	2000683c <__ssprint_r>
2000415a:	2800      	cmp	r0, #0
2000415c:	f47e af75 	bne.w	2000304a <_svfprintf_r+0x18a>
20004160:	f8dd c094 	ldr.w	ip, [sp, #148]	; 0x94
20004164:	46c3      	mov	fp, r8
20004166:	e516      	b.n	20003b96 <_svfprintf_r+0xcd6>
20004168:	2c06      	cmp	r4, #6
2000416a:	970c      	str	r7, [sp, #48]	; 0x30
2000416c:	9310      	str	r3, [sp, #64]	; 0x40
2000416e:	461f      	mov	r7, r3
20004170:	bf28      	it	cs
20004172:	2406      	movcs	r4, #6
20004174:	9711      	str	r7, [sp, #68]	; 0x44
20004176:	ea24 73e4 	bic.w	r3, r4, r4, asr #31
2000417a:	9307      	str	r3, [sp, #28]
2000417c:	4b91      	ldr	r3, [pc, #580]	; (200043c4 <_svfprintf_r+0x1504>)
2000417e:	930f      	str	r3, [sp, #60]	; 0x3c
20004180:	f7fe bfb1 	b.w	200030e6 <_svfprintf_r+0x226>
20004184:	4f8e      	ldr	r7, [pc, #568]	; (200043c0 <_svfprintf_r+0x1500>)
20004186:	f7ff b8b5 	b.w	200032f4 <_svfprintf_r+0x434>
2000418a:	232d      	movs	r3, #45	; 0x2d
2000418c:	461f      	mov	r7, r3
2000418e:	f88d 306f 	strb.w	r3, [sp, #111]	; 0x6f
20004192:	f7ff ba7a 	b.w	2000368a <_svfprintf_r+0x7ca>
20004196:	9b0e      	ldr	r3, [sp, #56]	; 0x38
20004198:	272d      	movs	r7, #45	; 0x2d
2000419a:	f8cd c01c 	str.w	ip, [sp, #28]
2000419e:	9306      	str	r3, [sp, #24]
200041a0:	2300      	movs	r3, #0
200041a2:	f88d 706f 	strb.w	r7, [sp, #111]	; 0x6f
200041a6:	9310      	str	r3, [sp, #64]	; 0x40
200041a8:	f7fe bf9e 	b.w	200030e8 <_svfprintf_r+0x228>
200041ac:	2e65      	cmp	r6, #101	; 0x65
200041ae:	dd56      	ble.n	2000425e <_svfprintf_r+0x139e>
200041b0:	2e66      	cmp	r6, #102	; 0x66
200041b2:	9311      	str	r3, [sp, #68]	; 0x44
200041b4:	f47f aeef 	bne.w	20003f96 <_svfprintf_r+0x10d6>
200041b8:	2b00      	cmp	r3, #0
200041ba:	f340 80d6 	ble.w	2000436a <_svfprintf_r+0x14aa>
200041be:	f1bc 0f00 	cmp.w	ip, #0
200041c2:	f040 80c1 	bne.w	20004348 <_svfprintf_r+0x1488>
200041c6:	9b06      	ldr	r3, [sp, #24]
200041c8:	07da      	lsls	r2, r3, #31
200041ca:	f100 80bd 	bmi.w	20004348 <_svfprintf_r+0x1488>
200041ce:	9c11      	ldr	r4, [sp, #68]	; 0x44
200041d0:	ea24 7ce4 	bic.w	ip, r4, r4, asr #31
200041d4:	e6eb      	b.n	20003fae <_svfprintf_r+0x10ee>
200041d6:	2003      	movs	r0, #3
200041d8:	a91d      	add	r1, sp, #116	; 0x74
200041da:	463a      	mov	r2, r7
200041dc:	f8cd c004 	str.w	ip, [sp, #4]
200041e0:	9000      	str	r0, [sp, #0]
200041e2:	a81e      	add	r0, sp, #120	; 0x78
200041e4:	9102      	str	r1, [sp, #8]
200041e6:	a921      	add	r1, sp, #132	; 0x84
200041e8:	462b      	mov	r3, r5
200041ea:	9003      	str	r0, [sp, #12]
200041ec:	9104      	str	r1, [sp, #16]
200041ee:	9809      	ldr	r0, [sp, #36]	; 0x24
200041f0:	f8cd c01c 	str.w	ip, [sp, #28]
200041f4:	f000 f9b4 	bl	20004560 <_dtoa_r>
200041f8:	f8dd c01c 	ldr.w	ip, [sp, #28]
200041fc:	4602      	mov	r2, r0
200041fe:	7803      	ldrb	r3, [r0, #0]
20004200:	4462      	add	r2, ip
20004202:	900f      	str	r0, [sp, #60]	; 0x3c
20004204:	2b30      	cmp	r3, #48	; 0x30
20004206:	9207      	str	r2, [sp, #28]
20004208:	f040 808b 	bne.w	20004322 <_svfprintf_r+0x1462>
2000420c:	4638      	mov	r0, r7
2000420e:	2200      	movs	r2, #0
20004210:	2300      	movs	r3, #0
20004212:	4629      	mov	r1, r5
20004214:	f8cd c040 	str.w	ip, [sp, #64]	; 0x40
20004218:	f04f 0401 	mov.w	r4, #1
2000421c:	f004 f912 	bl	20008444 <__aeabi_dcmpeq>
20004220:	f8dd c040 	ldr.w	ip, [sp, #64]	; 0x40
20004224:	b108      	cbz	r0, 2000422a <_svfprintf_r+0x136a>
20004226:	f04f 0400 	mov.w	r4, #0
2000422a:	f014 0fff 	tst.w	r4, #255	; 0xff
2000422e:	d078      	beq.n	20004322 <_svfprintf_r+0x1462>
20004230:	f1cc 0401 	rsb	r4, ip, #1
20004234:	941d      	str	r4, [sp, #116]	; 0x74
20004236:	9b07      	ldr	r3, [sp, #28]
20004238:	4423      	add	r3, r4
2000423a:	461c      	mov	r4, r3
2000423c:	e683      	b.n	20003f46 <_svfprintf_r+0x1086>
2000423e:	4d62      	ldr	r5, [pc, #392]	; (200043c8 <_svfprintf_r+0x1508>)
20004240:	f7ff bb2a 	b.w	20003898 <_svfprintf_r+0x9d8>
20004244:	4623      	mov	r3, r4
20004246:	e695      	b.n	20003f74 <_svfprintf_r+0x10b4>
20004248:	ea24 73e4 	bic.w	r3, r4, r4, asr #31
2000424c:	970c      	str	r7, [sp, #48]	; 0x30
2000424e:	9010      	str	r0, [sp, #64]	; 0x40
20004250:	f89d 706f 	ldrb.w	r7, [sp, #111]	; 0x6f
20004254:	9307      	str	r3, [sp, #28]
20004256:	9011      	str	r0, [sp, #68]	; 0x44
20004258:	f7fe bf45 	b.w	200030e6 <_svfprintf_r+0x226>
2000425c:	3e02      	subs	r6, #2
2000425e:	3b01      	subs	r3, #1
20004260:	f88d 607c 	strb.w	r6, [sp, #124]	; 0x7c
20004264:	2b00      	cmp	r3, #0
20004266:	931d      	str	r3, [sp, #116]	; 0x74
20004268:	db75      	blt.n	20004356 <_svfprintf_r+0x1496>
2000426a:	222b      	movs	r2, #43	; 0x2b
2000426c:	f88d 207d 	strb.w	r2, [sp, #125]	; 0x7d
20004270:	2b09      	cmp	r3, #9
20004272:	dd61      	ble.n	20004338 <_svfprintf_r+0x1478>
20004274:	f10d 008b 	add.w	r0, sp, #139	; 0x8b
20004278:	4601      	mov	r1, r0
2000427a:	4a54      	ldr	r2, [pc, #336]	; (200043cc <_svfprintf_r+0x150c>)
2000427c:	fb82 2403 	smull	r2, r4, r2, r3
20004280:	17da      	asrs	r2, r3, #31
20004282:	ebc2 04a4 	rsb	r4, r2, r4, asr #2
20004286:	eb04 0284 	add.w	r2, r4, r4, lsl #2
2000428a:	2c09      	cmp	r4, #9
2000428c:	eba3 0242 	sub.w	r2, r3, r2, lsl #1
20004290:	4623      	mov	r3, r4
20004292:	f102 0230 	add.w	r2, r2, #48	; 0x30
20004296:	f801 2d01 	strb.w	r2, [r1, #-1]!
2000429a:	dcee      	bgt.n	2000427a <_svfprintf_r+0x13ba>
2000429c:	3330      	adds	r3, #48	; 0x30
2000429e:	460a      	mov	r2, r1
200042a0:	b2db      	uxtb	r3, r3
200042a2:	f802 3d01 	strb.w	r3, [r2, #-1]!
200042a6:	4290      	cmp	r0, r2
200042a8:	d97e      	bls.n	200043a8 <_svfprintf_r+0x14e8>
200042aa:	460a      	mov	r2, r1
200042ac:	f10d 047e 	add.w	r4, sp, #126	; 0x7e
200042b0:	e001      	b.n	200042b6 <_svfprintf_r+0x13f6>
200042b2:	f812 3b01 	ldrb.w	r3, [r2], #1
200042b6:	4282      	cmp	r2, r0
200042b8:	f804 3b01 	strb.w	r3, [r4], #1
200042bc:	d1f9      	bne.n	200042b2 <_svfprintf_r+0x13f2>
200042be:	ab23      	add	r3, sp, #140	; 0x8c
200042c0:	f10d 027e 	add.w	r2, sp, #126	; 0x7e
200042c4:	1a5b      	subs	r3, r3, r1
200042c6:	4413      	add	r3, r2
200042c8:	aa1f      	add	r2, sp, #124	; 0x7c
200042ca:	1a9b      	subs	r3, r3, r2
200042cc:	9a12      	ldr	r2, [sp, #72]	; 0x48
200042ce:	2a01      	cmp	r2, #1
200042d0:	9319      	str	r3, [sp, #100]	; 0x64
200042d2:	eb02 0403 	add.w	r4, r2, r3
200042d6:	dd43      	ble.n	20004360 <_svfprintf_r+0x14a0>
200042d8:	3401      	adds	r4, #1
200042da:	2300      	movs	r3, #0
200042dc:	9311      	str	r3, [sp, #68]	; 0x44
200042de:	ea24 7ce4 	bic.w	ip, r4, r4, asr #31
200042e2:	e664      	b.n	20003fae <_svfprintf_r+0x10ee>
200042e4:	9809      	ldr	r0, [sp, #36]	; 0x24
200042e6:	4649      	mov	r1, r9
200042e8:	aa23      	add	r2, sp, #140	; 0x8c
200042ea:	f002 faa7 	bl	2000683c <__ssprint_r>
200042ee:	2800      	cmp	r0, #0
200042f0:	f47e aeab 	bne.w	2000304a <_svfprintf_r+0x18a>
200042f4:	9c1d      	ldr	r4, [sp, #116]	; 0x74
200042f6:	46c3      	mov	fp, r8
200042f8:	9a25      	ldr	r2, [sp, #148]	; 0x94
200042fa:	9b24      	ldr	r3, [sp, #144]	; 0x90
200042fc:	e5a8      	b.n	20003e50 <_svfprintf_r+0xf90>
200042fe:	9b14      	ldr	r3, [sp, #80]	; 0x50
20004300:	9f13      	ldr	r7, [sp, #76]	; 0x4c
20004302:	f103 4500 	add.w	r5, r3, #2147483648	; 0x80000000
20004306:	232d      	movs	r3, #45	; 0x2d
20004308:	930d      	str	r3, [sp, #52]	; 0x34
2000430a:	e69c      	b.n	20004046 <_svfprintf_r+0x1186>
2000430c:	9b11      	ldr	r3, [sp, #68]	; 0x44
2000430e:	2b00      	cmp	r3, #0
20004310:	dd35      	ble.n	2000437e <_svfprintf_r+0x14be>
20004312:	2401      	movs	r4, #1
20004314:	9b12      	ldr	r3, [sp, #72]	; 0x48
20004316:	441c      	add	r4, r3
20004318:	ea24 7ce4 	bic.w	ip, r4, r4, asr #31
2000431c:	e647      	b.n	20003fae <_svfprintf_r+0x10ee>
2000431e:	4d2a      	ldr	r5, [pc, #168]	; (200043c8 <_svfprintf_r+0x1508>)
20004320:	e413      	b.n	20003b4a <_svfprintf_r+0xc8a>
20004322:	9c1d      	ldr	r4, [sp, #116]	; 0x74
20004324:	e787      	b.n	20004236 <_svfprintf_r+0x1376>
20004326:	9b21      	ldr	r3, [sp, #132]	; 0x84
20004328:	e624      	b.n	20003f74 <_svfprintf_r+0x10b4>
2000432a:	9b11      	ldr	r3, [sp, #68]	; 0x44
2000432c:	1c5c      	adds	r4, r3, #1
2000432e:	ea24 7ce4 	bic.w	ip, r4, r4, asr #31
20004332:	e63c      	b.n	20003fae <_svfprintf_r+0x10ee>
20004334:	4d24      	ldr	r5, [pc, #144]	; (200043c8 <_svfprintf_r+0x1508>)
20004336:	e420      	b.n	20003b7a <_svfprintf_r+0xcba>
20004338:	3330      	adds	r3, #48	; 0x30
2000433a:	2230      	movs	r2, #48	; 0x30
2000433c:	f88d 307f 	strb.w	r3, [sp, #127]	; 0x7f
20004340:	ab20      	add	r3, sp, #128	; 0x80
20004342:	f88d 207e 	strb.w	r2, [sp, #126]	; 0x7e
20004346:	e7bf      	b.n	200042c8 <_svfprintf_r+0x1408>
20004348:	f10c 0401 	add.w	r4, ip, #1
2000434c:	9b11      	ldr	r3, [sp, #68]	; 0x44
2000434e:	441c      	add	r4, r3
20004350:	ea24 7ce4 	bic.w	ip, r4, r4, asr #31
20004354:	e62b      	b.n	20003fae <_svfprintf_r+0x10ee>
20004356:	222d      	movs	r2, #45	; 0x2d
20004358:	425b      	negs	r3, r3
2000435a:	f88d 207d 	strb.w	r2, [sp, #125]	; 0x7d
2000435e:	e787      	b.n	20004270 <_svfprintf_r+0x13b0>
20004360:	9b06      	ldr	r3, [sp, #24]
20004362:	f013 0301 	ands.w	r3, r3, #1
20004366:	d0b9      	beq.n	200042dc <_svfprintf_r+0x141c>
20004368:	e7b6      	b.n	200042d8 <_svfprintf_r+0x1418>
2000436a:	f1bc 0f00 	cmp.w	ip, #0
2000436e:	d109      	bne.n	20004384 <_svfprintf_r+0x14c4>
20004370:	9b06      	ldr	r3, [sp, #24]
20004372:	07db      	lsls	r3, r3, #31
20004374:	d406      	bmi.n	20004384 <_svfprintf_r+0x14c4>
20004376:	f04f 0c01 	mov.w	ip, #1
2000437a:	4664      	mov	r4, ip
2000437c:	e617      	b.n	20003fae <_svfprintf_r+0x10ee>
2000437e:	f1c3 0402 	rsb	r4, r3, #2
20004382:	e7c7      	b.n	20004314 <_svfprintf_r+0x1454>
20004384:	f10c 0402 	add.w	r4, ip, #2
20004388:	ea24 7ce4 	bic.w	ip, r4, r4, asr #31
2000438c:	e60f      	b.n	20003fae <_svfprintf_r+0x10ee>
2000438e:	9d0c      	ldr	r5, [sp, #48]	; 0x30
20004390:	785e      	ldrb	r6, [r3, #1]
20004392:	4628      	mov	r0, r5
20004394:	682c      	ldr	r4, [r5, #0]
20004396:	3004      	adds	r0, #4
20004398:	2c00      	cmp	r4, #0
2000439a:	900c      	str	r0, [sp, #48]	; 0x30
2000439c:	f6be ade5 	bge.w	20002f6a <_svfprintf_r+0xaa>
200043a0:	f04f 34ff 	mov.w	r4, #4294967295
200043a4:	f7fe bde1 	b.w	20002f6a <_svfprintf_r+0xaa>
200043a8:	f10d 037e 	add.w	r3, sp, #126	; 0x7e
200043ac:	e78c      	b.n	200042c8 <_svfprintf_r+0x1408>
200043ae:	230c      	movs	r3, #12
200043b0:	9a09      	ldr	r2, [sp, #36]	; 0x24
200043b2:	f04f 30ff 	mov.w	r0, #4294967295
200043b6:	6013      	str	r3, [r2, #0]
200043b8:	f7fe be50 	b.w	2000305c <_svfprintf_r+0x19c>
200043bc:	4664      	mov	r4, ip
200043be:	e663      	b.n	20004088 <_svfprintf_r+0x11c8>
200043c0:	2000906c 	.word	0x2000906c
200043c4:	200090b4 	.word	0x200090b4
200043c8:	2000905c 	.word	0x2000905c
200043cc:	66666667 	.word	0x66666667

200043d0 <_vsprintf_r>:
200043d0:	b5f0      	push	{r4, r5, r6, r7, lr}
200043d2:	b09b      	sub	sp, #108	; 0x6c
200043d4:	460d      	mov	r5, r1
200043d6:	f06f 4400 	mvn.w	r4, #2147483648	; 0x80000000
200043da:	f44f 7702 	mov.w	r7, #520	; 0x208
200043de:	f64f 76ff 	movw	r6, #65535	; 0xffff
200043e2:	4669      	mov	r1, sp
200043e4:	9500      	str	r5, [sp, #0]
200043e6:	9504      	str	r5, [sp, #16]
200043e8:	f8ad 700c 	strh.w	r7, [sp, #12]
200043ec:	9402      	str	r4, [sp, #8]
200043ee:	9405      	str	r4, [sp, #20]
200043f0:	f8ad 600e 	strh.w	r6, [sp, #14]
200043f4:	f7fe fd64 	bl	20002ec0 <_svfprintf_r>
200043f8:	9b00      	ldr	r3, [sp, #0]
200043fa:	2200      	movs	r2, #0
200043fc:	701a      	strb	r2, [r3, #0]
200043fe:	b01b      	add	sp, #108	; 0x6c
20004400:	bdf0      	pop	{r4, r5, r6, r7, pc}
20004402:	bf00      	nop

20004404 <vsprintf>:
20004404:	b430      	push	{r4, r5}
20004406:	460d      	mov	r5, r1
20004408:	4c03      	ldr	r4, [pc, #12]	; (20004418 <vsprintf+0x14>)
2000440a:	4613      	mov	r3, r2
2000440c:	4601      	mov	r1, r0
2000440e:	462a      	mov	r2, r5
20004410:	6820      	ldr	r0, [r4, #0]
20004412:	bc30      	pop	{r4, r5}
20004414:	f7ff bfdc 	b.w	200043d0 <_vsprintf_r>
20004418:	200096f0 	.word	0x200096f0

2000441c <quorem>:
2000441c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20004420:	6903      	ldr	r3, [r0, #16]
20004422:	b083      	sub	sp, #12
20004424:	690f      	ldr	r7, [r1, #16]
20004426:	429f      	cmp	r7, r3
20004428:	f300 8093 	bgt.w	20004552 <quorem+0x136>
2000442c:	3f01      	subs	r7, #1
2000442e:	f101 0614 	add.w	r6, r1, #20
20004432:	f100 0a14 	add.w	sl, r0, #20
20004436:	00bb      	lsls	r3, r7, #2
20004438:	f856 2027 	ldr.w	r2, [r6, r7, lsl #2]
2000443c:	461d      	mov	r5, r3
2000443e:	9300      	str	r3, [sp, #0]
20004440:	3201      	adds	r2, #1
20004442:	f85a 3027 	ldr.w	r3, [sl, r7, lsl #2]
20004446:	eb0a 0405 	add.w	r4, sl, r5
2000444a:	eb06 0905 	add.w	r9, r6, r5
2000444e:	fbb3 f8f2 	udiv	r8, r3, r2
20004452:	9401      	str	r4, [sp, #4]
20004454:	f1b8 0f00 	cmp.w	r8, #0
20004458:	d040      	beq.n	200044dc <quorem+0xc0>
2000445a:	2500      	movs	r5, #0
2000445c:	46b4      	mov	ip, r6
2000445e:	46d6      	mov	lr, sl
20004460:	462b      	mov	r3, r5
20004462:	f85c bb04 	ldr.w	fp, [ip], #4
20004466:	f8de 2000 	ldr.w	r2, [lr]
2000446a:	fa1f f48b 	uxth.w	r4, fp
2000446e:	45e1      	cmp	r9, ip
20004470:	ea4f 4b1b 	mov.w	fp, fp, lsr #16
20004474:	fb04 5508 	mla	r5, r4, r8, r5
20004478:	fb0b fb08 	mul.w	fp, fp, r8
2000447c:	b2ac      	uxth	r4, r5
2000447e:	eb0b 4515 	add.w	r5, fp, r5, lsr #16
20004482:	eba3 0304 	sub.w	r3, r3, r4
20004486:	fa1f fb85 	uxth.w	fp, r5
2000448a:	fa13 f482 	uxtah	r4, r3, r2
2000448e:	ea4f 4515 	mov.w	r5, r5, lsr #16
20004492:	ebcb 4212 	rsb	r2, fp, r2, lsr #16
20004496:	fa1f fb84 	uxth.w	fp, r4
2000449a:	eb02 4324 	add.w	r3, r2, r4, asr #16
2000449e:	ea4b 4203 	orr.w	r2, fp, r3, lsl #16
200044a2:	ea4f 4323 	mov.w	r3, r3, asr #16
200044a6:	f84e 2b04 	str.w	r2, [lr], #4
200044aa:	d2da      	bcs.n	20004462 <quorem+0x46>
200044ac:	9b00      	ldr	r3, [sp, #0]
200044ae:	f85a 3003 	ldr.w	r3, [sl, r3]
200044b2:	b99b      	cbnz	r3, 200044dc <quorem+0xc0>
200044b4:	9c01      	ldr	r4, [sp, #4]
200044b6:	1f23      	subs	r3, r4, #4
200044b8:	459a      	cmp	sl, r3
200044ba:	d20e      	bcs.n	200044da <quorem+0xbe>
200044bc:	f854 3c04 	ldr.w	r3, [r4, #-4]
200044c0:	b95b      	cbnz	r3, 200044da <quorem+0xbe>
200044c2:	f1a4 0308 	sub.w	r3, r4, #8
200044c6:	e001      	b.n	200044cc <quorem+0xb0>
200044c8:	6812      	ldr	r2, [r2, #0]
200044ca:	b932      	cbnz	r2, 200044da <quorem+0xbe>
200044cc:	459a      	cmp	sl, r3
200044ce:	461a      	mov	r2, r3
200044d0:	f107 37ff 	add.w	r7, r7, #4294967295
200044d4:	f1a3 0304 	sub.w	r3, r3, #4
200044d8:	d3f6      	bcc.n	200044c8 <quorem+0xac>
200044da:	6107      	str	r7, [r0, #16]
200044dc:	4604      	mov	r4, r0
200044de:	f001 fdef 	bl	200060c0 <__mcmp>
200044e2:	2800      	cmp	r0, #0
200044e4:	db31      	blt.n	2000454a <quorem+0x12e>
200044e6:	f108 0801 	add.w	r8, r8, #1
200044ea:	4655      	mov	r5, sl
200044ec:	2000      	movs	r0, #0
200044ee:	f856 1b04 	ldr.w	r1, [r6], #4
200044f2:	682a      	ldr	r2, [r5, #0]
200044f4:	b28b      	uxth	r3, r1
200044f6:	45b1      	cmp	r9, r6
200044f8:	ea4f 4111 	mov.w	r1, r1, lsr #16
200044fc:	eba0 0303 	sub.w	r3, r0, r3
20004500:	ebc1 4112 	rsb	r1, r1, r2, lsr #16
20004504:	fa13 f382 	uxtah	r3, r3, r2
20004508:	eb01 4223 	add.w	r2, r1, r3, asr #16
2000450c:	b29b      	uxth	r3, r3
2000450e:	ea4f 4022 	mov.w	r0, r2, asr #16
20004512:	ea43 4302 	orr.w	r3, r3, r2, lsl #16
20004516:	f845 3b04 	str.w	r3, [r5], #4
2000451a:	d2e8      	bcs.n	200044ee <quorem+0xd2>
2000451c:	f85a 2027 	ldr.w	r2, [sl, r7, lsl #2]
20004520:	eb0a 0387 	add.w	r3, sl, r7, lsl #2
20004524:	b98a      	cbnz	r2, 2000454a <quorem+0x12e>
20004526:	1f1a      	subs	r2, r3, #4
20004528:	4592      	cmp	sl, r2
2000452a:	d20d      	bcs.n	20004548 <quorem+0x12c>
2000452c:	f853 2c04 	ldr.w	r2, [r3, #-4]
20004530:	b952      	cbnz	r2, 20004548 <quorem+0x12c>
20004532:	3b08      	subs	r3, #8
20004534:	e001      	b.n	2000453a <quorem+0x11e>
20004536:	6812      	ldr	r2, [r2, #0]
20004538:	b932      	cbnz	r2, 20004548 <quorem+0x12c>
2000453a:	459a      	cmp	sl, r3
2000453c:	461a      	mov	r2, r3
2000453e:	f107 37ff 	add.w	r7, r7, #4294967295
20004542:	f1a3 0304 	sub.w	r3, r3, #4
20004546:	d3f6      	bcc.n	20004536 <quorem+0x11a>
20004548:	6127      	str	r7, [r4, #16]
2000454a:	4640      	mov	r0, r8
2000454c:	b003      	add	sp, #12
2000454e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20004552:	2000      	movs	r0, #0
20004554:	b003      	add	sp, #12
20004556:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
2000455a:	bf00      	nop
2000455c:	0000      	movs	r0, r0
	...

20004560 <_dtoa_r>:
20004560:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20004564:	469b      	mov	fp, r3
20004566:	b099      	sub	sp, #100	; 0x64
20004568:	6c03      	ldr	r3, [r0, #64]	; 0x40
2000456a:	4604      	mov	r4, r0
2000456c:	4692      	mov	sl, r2
2000456e:	9d25      	ldr	r5, [sp, #148]	; 0x94
20004570:	b14b      	cbz	r3, 20004586 <_dtoa_r+0x26>
20004572:	6c46      	ldr	r6, [r0, #68]	; 0x44
20004574:	2201      	movs	r2, #1
20004576:	4619      	mov	r1, r3
20004578:	40b2      	lsls	r2, r6
2000457a:	605e      	str	r6, [r3, #4]
2000457c:	609a      	str	r2, [r3, #8]
2000457e:	f001 fb5f 	bl	20005c40 <_Bfree>
20004582:	2300      	movs	r3, #0
20004584:	6423      	str	r3, [r4, #64]	; 0x40
20004586:	f1bb 0f00 	cmp.w	fp, #0
2000458a:	46d9      	mov	r9, fp
2000458c:	db33      	blt.n	200045f6 <_dtoa_r+0x96>
2000458e:	2300      	movs	r3, #0
20004590:	602b      	str	r3, [r5, #0]
20004592:	4b99      	ldr	r3, [pc, #612]	; (200047f8 <_dtoa_r+0x298>)
20004594:	461a      	mov	r2, r3
20004596:	ea09 0303 	and.w	r3, r9, r3
2000459a:	4293      	cmp	r3, r2
2000459c:	d014      	beq.n	200045c8 <_dtoa_r+0x68>
2000459e:	2200      	movs	r2, #0
200045a0:	2300      	movs	r3, #0
200045a2:	4650      	mov	r0, sl
200045a4:	4659      	mov	r1, fp
200045a6:	f003 ff4d 	bl	20008444 <__aeabi_dcmpeq>
200045aa:	4680      	mov	r8, r0
200045ac:	b348      	cbz	r0, 20004602 <_dtoa_r+0xa2>
200045ae:	2301      	movs	r3, #1
200045b0:	9a24      	ldr	r2, [sp, #144]	; 0x90
200045b2:	6013      	str	r3, [r2, #0]
200045b4:	9b26      	ldr	r3, [sp, #152]	; 0x98
200045b6:	2b00      	cmp	r3, #0
200045b8:	f000 80cb 	beq.w	20004752 <_dtoa_r+0x1f2>
200045bc:	488f      	ldr	r0, [pc, #572]	; (200047fc <_dtoa_r+0x29c>)
200045be:	6018      	str	r0, [r3, #0]
200045c0:	3801      	subs	r0, #1
200045c2:	b019      	add	sp, #100	; 0x64
200045c4:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
200045c8:	f242 730f 	movw	r3, #9999	; 0x270f
200045cc:	9a24      	ldr	r2, [sp, #144]	; 0x90
200045ce:	6013      	str	r3, [r2, #0]
200045d0:	f1ba 0f00 	cmp.w	sl, #0
200045d4:	f000 80a6 	beq.w	20004724 <_dtoa_r+0x1c4>
200045d8:	4889      	ldr	r0, [pc, #548]	; (20004800 <_dtoa_r+0x2a0>)
200045da:	9b26      	ldr	r3, [sp, #152]	; 0x98
200045dc:	2b00      	cmp	r3, #0
200045de:	d0f0      	beq.n	200045c2 <_dtoa_r+0x62>
200045e0:	78c3      	ldrb	r3, [r0, #3]
200045e2:	2b00      	cmp	r3, #0
200045e4:	f000 80b7 	beq.w	20004756 <_dtoa_r+0x1f6>
200045e8:	f100 0308 	add.w	r3, r0, #8
200045ec:	9a26      	ldr	r2, [sp, #152]	; 0x98
200045ee:	6013      	str	r3, [r2, #0]
200045f0:	b019      	add	sp, #100	; 0x64
200045f2:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
200045f6:	2301      	movs	r3, #1
200045f8:	f02b 4900 	bic.w	r9, fp, #2147483648	; 0x80000000
200045fc:	602b      	str	r3, [r5, #0]
200045fe:	46cb      	mov	fp, r9
20004600:	e7c7      	b.n	20004592 <_dtoa_r+0x32>
20004602:	ad17      	add	r5, sp, #92	; 0x5c
20004604:	a916      	add	r1, sp, #88	; 0x58
20004606:	4620      	mov	r0, r4
20004608:	4652      	mov	r2, sl
2000460a:	9500      	str	r5, [sp, #0]
2000460c:	465b      	mov	r3, fp
2000460e:	9101      	str	r1, [sp, #4]
20004610:	f001 fe74 	bl	200062fc <__d2b>
20004614:	9006      	str	r0, [sp, #24]
20004616:	ea5f 5519 	movs.w	r5, r9, lsr #20
2000461a:	f040 808c 	bne.w	20004736 <_dtoa_r+0x1d6>
2000461e:	f8dd 8058 	ldr.w	r8, [sp, #88]	; 0x58
20004622:	f46f 6382 	mvn.w	r3, #1040	; 0x410
20004626:	9d17      	ldr	r5, [sp, #92]	; 0x5c
20004628:	4445      	add	r5, r8
2000462a:	429d      	cmp	r5, r3
2000462c:	f2c0 81cc 	blt.w	200049c8 <_dtoa_r+0x468>
20004630:	331f      	adds	r3, #31
20004632:	f205 4212 	addw	r2, r5, #1042	; 0x412
20004636:	1b5b      	subs	r3, r3, r5
20004638:	fa2a f002 	lsr.w	r0, sl, r2
2000463c:	fa09 f903 	lsl.w	r9, r9, r3
20004640:	ea49 0000 	orr.w	r0, r9, r0
20004644:	f003 fc20 	bl	20007e88 <__aeabi_ui2d>
20004648:	3d01      	subs	r5, #1
2000464a:	2301      	movs	r3, #1
2000464c:	f1a1 71f8 	sub.w	r1, r1, #32505856	; 0x1f00000
20004650:	9311      	str	r3, [sp, #68]	; 0x44
20004652:	2200      	movs	r2, #0
20004654:	4b6b      	ldr	r3, [pc, #428]	; (20004804 <_dtoa_r+0x2a4>)
20004656:	f003 fad9 	bl	20007c0c <__aeabi_dsub>
2000465a:	a361      	add	r3, pc, #388	; (adr r3, 200047e0 <_dtoa_r+0x280>)
2000465c:	e9d3 2300 	ldrd	r2, r3, [r3]
20004660:	f003 fc88 	bl	20007f74 <__aeabi_dmul>
20004664:	a360      	add	r3, pc, #384	; (adr r3, 200047e8 <_dtoa_r+0x288>)
20004666:	e9d3 2300 	ldrd	r2, r3, [r3]
2000466a:	f003 fad1 	bl	20007c10 <__adddf3>
2000466e:	4606      	mov	r6, r0
20004670:	460f      	mov	r7, r1
20004672:	4628      	mov	r0, r5
20004674:	f003 fc18 	bl	20007ea8 <__aeabi_i2d>
20004678:	a35d      	add	r3, pc, #372	; (adr r3, 200047f0 <_dtoa_r+0x290>)
2000467a:	e9d3 2300 	ldrd	r2, r3, [r3]
2000467e:	f003 fc79 	bl	20007f74 <__aeabi_dmul>
20004682:	4602      	mov	r2, r0
20004684:	460b      	mov	r3, r1
20004686:	4630      	mov	r0, r6
20004688:	4639      	mov	r1, r7
2000468a:	f003 fac1 	bl	20007c10 <__adddf3>
2000468e:	4606      	mov	r6, r0
20004690:	460f      	mov	r7, r1
20004692:	f003 ff09 	bl	200084a8 <__aeabi_d2iz>
20004696:	2200      	movs	r2, #0
20004698:	9002      	str	r0, [sp, #8]
2000469a:	4639      	mov	r1, r7
2000469c:	4630      	mov	r0, r6
2000469e:	2300      	movs	r3, #0
200046a0:	f003 feda 	bl	20008458 <__aeabi_dcmplt>
200046a4:	2800      	cmp	r0, #0
200046a6:	f040 8171 	bne.w	2000498c <_dtoa_r+0x42c>
200046aa:	9b02      	ldr	r3, [sp, #8]
200046ac:	2b16      	cmp	r3, #22
200046ae:	f200 8091 	bhi.w	200047d4 <_dtoa_r+0x274>
200046b2:	9802      	ldr	r0, [sp, #8]
200046b4:	4652      	mov	r2, sl
200046b6:	4954      	ldr	r1, [pc, #336]	; (20004808 <_dtoa_r+0x2a8>)
200046b8:	465b      	mov	r3, fp
200046ba:	eb01 01c0 	add.w	r1, r1, r0, lsl #3
200046be:	e9d1 0100 	ldrd	r0, r1, [r1]
200046c2:	f003 fee7 	bl	20008494 <__aeabi_dcmpgt>
200046c6:	2800      	cmp	r0, #0
200046c8:	f000 817c 	beq.w	200049c4 <_dtoa_r+0x464>
200046cc:	9b02      	ldr	r3, [sp, #8]
200046ce:	3b01      	subs	r3, #1
200046d0:	9302      	str	r3, [sp, #8]
200046d2:	2300      	movs	r3, #0
200046d4:	930d      	str	r3, [sp, #52]	; 0x34
200046d6:	ebc5 0508 	rsb	r5, r5, r8
200046da:	1e6b      	subs	r3, r5, #1
200046dc:	9303      	str	r3, [sp, #12]
200046de:	f100 816c 	bmi.w	200049ba <_dtoa_r+0x45a>
200046e2:	2300      	movs	r3, #0
200046e4:	9307      	str	r3, [sp, #28]
200046e6:	9b02      	ldr	r3, [sp, #8]
200046e8:	2b00      	cmp	r3, #0
200046ea:	f2c0 815d 	blt.w	200049a8 <_dtoa_r+0x448>
200046ee:	9a03      	ldr	r2, [sp, #12]
200046f0:	930c      	str	r3, [sp, #48]	; 0x30
200046f2:	4611      	mov	r1, r2
200046f4:	4419      	add	r1, r3
200046f6:	2300      	movs	r3, #0
200046f8:	9103      	str	r1, [sp, #12]
200046fa:	930a      	str	r3, [sp, #40]	; 0x28
200046fc:	9b22      	ldr	r3, [sp, #136]	; 0x88
200046fe:	2b09      	cmp	r3, #9
20004700:	d82b      	bhi.n	2000475a <_dtoa_r+0x1fa>
20004702:	2b05      	cmp	r3, #5
20004704:	f340 867e 	ble.w	20005404 <_dtoa_r+0xea4>
20004708:	3b04      	subs	r3, #4
2000470a:	2500      	movs	r5, #0
2000470c:	9322      	str	r3, [sp, #136]	; 0x88
2000470e:	9b22      	ldr	r3, [sp, #136]	; 0x88
20004710:	3b02      	subs	r3, #2
20004712:	2b03      	cmp	r3, #3
20004714:	f200 8679 	bhi.w	2000540a <_dtoa_r+0xeaa>
20004718:	e8df f013 	tbh	[pc, r3, lsl #1]
2000471c:	0289029b 	.word	0x0289029b
20004720:	068d0180 	.word	0x068d0180
20004724:	f3c9 0013 	ubfx	r0, r9, #0, #20
20004728:	4b35      	ldr	r3, [pc, #212]	; (20004800 <_dtoa_r+0x2a0>)
2000472a:	4a38      	ldr	r2, [pc, #224]	; (2000480c <_dtoa_r+0x2ac>)
2000472c:	2800      	cmp	r0, #0
2000472e:	bf14      	ite	ne
20004730:	4618      	movne	r0, r3
20004732:	4610      	moveq	r0, r2
20004734:	e751      	b.n	200045da <_dtoa_r+0x7a>
20004736:	f3cb 0313 	ubfx	r3, fp, #0, #20
2000473a:	f8cd 8044 	str.w	r8, [sp, #68]	; 0x44
2000473e:	4650      	mov	r0, sl
20004740:	f2a5 35ff 	subw	r5, r5, #1023	; 0x3ff
20004744:	f043 517f 	orr.w	r1, r3, #1069547520	; 0x3fc00000
20004748:	f8dd 8058 	ldr.w	r8, [sp, #88]	; 0x58
2000474c:	f441 1140 	orr.w	r1, r1, #3145728	; 0x300000
20004750:	e77f      	b.n	20004652 <_dtoa_r+0xf2>
20004752:	482f      	ldr	r0, [pc, #188]	; (20004810 <_dtoa_r+0x2b0>)
20004754:	e735      	b.n	200045c2 <_dtoa_r+0x62>
20004756:	1cc3      	adds	r3, r0, #3
20004758:	e748      	b.n	200045ec <_dtoa_r+0x8c>
2000475a:	f04f 33ff 	mov.w	r3, #4294967295
2000475e:	2100      	movs	r1, #0
20004760:	4620      	mov	r0, r4
20004762:	461d      	mov	r5, r3
20004764:	9310      	str	r3, [sp, #64]	; 0x40
20004766:	2301      	movs	r3, #1
20004768:	6461      	str	r1, [r4, #68]	; 0x44
2000476a:	9123      	str	r1, [sp, #140]	; 0x8c
2000476c:	930b      	str	r3, [sp, #44]	; 0x2c
2000476e:	f001 fa3f 	bl	20005bf0 <_Balloc>
20004772:	9b23      	ldr	r3, [sp, #140]	; 0x8c
20004774:	9005      	str	r0, [sp, #20]
20004776:	9322      	str	r3, [sp, #136]	; 0x88
20004778:	9504      	str	r5, [sp, #16]
2000477a:	6420      	str	r0, [r4, #64]	; 0x40
2000477c:	9a02      	ldr	r2, [sp, #8]
2000477e:	9b17      	ldr	r3, [sp, #92]	; 0x5c
20004780:	2a0e      	cmp	r2, #14
20004782:	dc49      	bgt.n	20004818 <_dtoa_r+0x2b8>
20004784:	2b00      	cmp	r3, #0
20004786:	db47      	blt.n	20004818 <_dtoa_r+0x2b8>
20004788:	4b1f      	ldr	r3, [pc, #124]	; (20004808 <_dtoa_r+0x2a8>)
2000478a:	9a02      	ldr	r2, [sp, #8]
2000478c:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
20004790:	e9d3 8900 	ldrd	r8, r9, [r3]
20004794:	9b04      	ldr	r3, [sp, #16]
20004796:	2b00      	cmp	r3, #0
20004798:	f300 825e 	bgt.w	20004c58 <_dtoa_r+0x6f8>
2000479c:	9b23      	ldr	r3, [sp, #140]	; 0x8c
2000479e:	2b00      	cmp	r3, #0
200047a0:	f280 825a 	bge.w	20004c58 <_dtoa_r+0x6f8>
200047a4:	9b04      	ldr	r3, [sp, #16]
200047a6:	2b00      	cmp	r3, #0
200047a8:	f040 85bf 	bne.w	2000532a <_dtoa_r+0xdca>
200047ac:	2200      	movs	r2, #0
200047ae:	4b19      	ldr	r3, [pc, #100]	; (20004814 <_dtoa_r+0x2b4>)
200047b0:	4640      	mov	r0, r8
200047b2:	4649      	mov	r1, r9
200047b4:	f003 fbde 	bl	20007f74 <__aeabi_dmul>
200047b8:	4652      	mov	r2, sl
200047ba:	465b      	mov	r3, fp
200047bc:	f003 fe60 	bl	20008480 <__aeabi_dcmpge>
200047c0:	9f04      	ldr	r7, [sp, #16]
200047c2:	9708      	str	r7, [sp, #32]
200047c4:	2800      	cmp	r0, #0
200047c6:	f000 80bd 	beq.w	20004944 <_dtoa_r+0x3e4>
200047ca:	9b23      	ldr	r3, [sp, #140]	; 0x8c
200047cc:	9d05      	ldr	r5, [sp, #20]
200047ce:	43db      	mvns	r3, r3
200047d0:	9302      	str	r3, [sp, #8]
200047d2:	e0be      	b.n	20004952 <_dtoa_r+0x3f2>
200047d4:	2301      	movs	r3, #1
200047d6:	930d      	str	r3, [sp, #52]	; 0x34
200047d8:	e77d      	b.n	200046d6 <_dtoa_r+0x176>
200047da:	bf00      	nop
200047dc:	f3af 8000 	nop.w
200047e0:	636f4361 	.word	0x636f4361
200047e4:	3fd287a7 	.word	0x3fd287a7
200047e8:	8b60c8b3 	.word	0x8b60c8b3
200047ec:	3fc68a28 	.word	0x3fc68a28
200047f0:	509f79fb 	.word	0x509f79fb
200047f4:	3fd34413 	.word	0x3fd34413
200047f8:	7ff00000 	.word	0x7ff00000
200047fc:	200090bd 	.word	0x200090bd
20004800:	200090cc 	.word	0x200090cc
20004804:	3ff80000 	.word	0x3ff80000
20004808:	200090e8 	.word	0x200090e8
2000480c:	200090c0 	.word	0x200090c0
20004810:	200090bc 	.word	0x200090bc
20004814:	40140000 	.word	0x40140000
20004818:	9a0b      	ldr	r2, [sp, #44]	; 0x2c
2000481a:	2a00      	cmp	r2, #0
2000481c:	f040 80e1 	bne.w	200049e2 <_dtoa_r+0x482>
20004820:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
20004822:	9e0a      	ldr	r6, [sp, #40]	; 0x28
20004824:	9d07      	ldr	r5, [sp, #28]
20004826:	9308      	str	r3, [sp, #32]
20004828:	9903      	ldr	r1, [sp, #12]
2000482a:	2900      	cmp	r1, #0
2000482c:	460b      	mov	r3, r1
2000482e:	dd0a      	ble.n	20004846 <_dtoa_r+0x2e6>
20004830:	2d00      	cmp	r5, #0
20004832:	dd08      	ble.n	20004846 <_dtoa_r+0x2e6>
20004834:	42a9      	cmp	r1, r5
20004836:	9a07      	ldr	r2, [sp, #28]
20004838:	bfa8      	it	ge
2000483a:	462b      	movge	r3, r5
2000483c:	1ad2      	subs	r2, r2, r3
2000483e:	1aed      	subs	r5, r5, r3
20004840:	1acb      	subs	r3, r1, r3
20004842:	9207      	str	r2, [sp, #28]
20004844:	9303      	str	r3, [sp, #12]
20004846:	9b0a      	ldr	r3, [sp, #40]	; 0x28
20004848:	2b00      	cmp	r3, #0
2000484a:	dd1b      	ble.n	20004884 <_dtoa_r+0x324>
2000484c:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
2000484e:	2b00      	cmp	r3, #0
20004850:	f000 84ce 	beq.w	200051f0 <_dtoa_r+0xc90>
20004854:	2e00      	cmp	r6, #0
20004856:	dd11      	ble.n	2000487c <_dtoa_r+0x31c>
20004858:	9908      	ldr	r1, [sp, #32]
2000485a:	4632      	mov	r2, r6
2000485c:	4620      	mov	r0, r4
2000485e:	f001 fb85 	bl	20005f6c <__pow5mult>
20004862:	9f06      	ldr	r7, [sp, #24]
20004864:	4601      	mov	r1, r0
20004866:	9008      	str	r0, [sp, #32]
20004868:	463a      	mov	r2, r7
2000486a:	4620      	mov	r0, r4
2000486c:	f001 fadc 	bl	20005e28 <__multiply>
20004870:	4603      	mov	r3, r0
20004872:	4639      	mov	r1, r7
20004874:	4620      	mov	r0, r4
20004876:	9306      	str	r3, [sp, #24]
20004878:	f001 f9e2 	bl	20005c40 <_Bfree>
2000487c:	9b0a      	ldr	r3, [sp, #40]	; 0x28
2000487e:	1b9a      	subs	r2, r3, r6
20004880:	f040 84ff 	bne.w	20005282 <_dtoa_r+0xd22>
20004884:	4620      	mov	r0, r4
20004886:	2101      	movs	r1, #1
20004888:	f001 fac4 	bl	20005e14 <__i2b>
2000488c:	9b0c      	ldr	r3, [sp, #48]	; 0x30
2000488e:	4607      	mov	r7, r0
20004890:	2b00      	cmp	r3, #0
20004892:	f340 8282 	ble.w	20004d9a <_dtoa_r+0x83a>
20004896:	4601      	mov	r1, r0
20004898:	461a      	mov	r2, r3
2000489a:	4620      	mov	r0, r4
2000489c:	f001 fb66 	bl	20005f6c <__pow5mult>
200048a0:	9b22      	ldr	r3, [sp, #136]	; 0x88
200048a2:	4607      	mov	r7, r0
200048a4:	2b01      	cmp	r3, #1
200048a6:	f340 84f3 	ble.w	20005290 <_dtoa_r+0xd30>
200048aa:	f04f 0800 	mov.w	r8, #0
200048ae:	693b      	ldr	r3, [r7, #16]
200048b0:	eb07 0383 	add.w	r3, r7, r3, lsl #2
200048b4:	6918      	ldr	r0, [r3, #16]
200048b6:	f001 fa59 	bl	20005d6c <__hi0bits>
200048ba:	f1c0 0020 	rsb	r0, r0, #32
200048be:	9b03      	ldr	r3, [sp, #12]
200048c0:	4418      	add	r0, r3
200048c2:	f010 001f 	ands.w	r0, r0, #31
200048c6:	f000 82a0 	beq.w	20004e0a <_dtoa_r+0x8aa>
200048ca:	f1c0 0320 	rsb	r3, r0, #32
200048ce:	2b04      	cmp	r3, #4
200048d0:	f340 8592 	ble.w	200053f8 <_dtoa_r+0xe98>
200048d4:	f1c0 001c 	rsb	r0, r0, #28
200048d8:	9b07      	ldr	r3, [sp, #28]
200048da:	4405      	add	r5, r0
200048dc:	4403      	add	r3, r0
200048de:	9307      	str	r3, [sp, #28]
200048e0:	9b03      	ldr	r3, [sp, #12]
200048e2:	4403      	add	r3, r0
200048e4:	9303      	str	r3, [sp, #12]
200048e6:	9b07      	ldr	r3, [sp, #28]
200048e8:	2b00      	cmp	r3, #0
200048ea:	dd05      	ble.n	200048f8 <_dtoa_r+0x398>
200048ec:	9906      	ldr	r1, [sp, #24]
200048ee:	461a      	mov	r2, r3
200048f0:	4620      	mov	r0, r4
200048f2:	f001 fb8b 	bl	2000600c <__lshift>
200048f6:	9006      	str	r0, [sp, #24]
200048f8:	9b03      	ldr	r3, [sp, #12]
200048fa:	2b00      	cmp	r3, #0
200048fc:	dd05      	ble.n	2000490a <_dtoa_r+0x3aa>
200048fe:	4639      	mov	r1, r7
20004900:	461a      	mov	r2, r3
20004902:	4620      	mov	r0, r4
20004904:	f001 fb82 	bl	2000600c <__lshift>
20004908:	4607      	mov	r7, r0
2000490a:	9b0d      	ldr	r3, [sp, #52]	; 0x34
2000490c:	2b00      	cmp	r3, #0
2000490e:	f040 838b 	bne.w	20005028 <_dtoa_r+0xac8>
20004912:	9b22      	ldr	r3, [sp, #136]	; 0x88
20004914:	2b02      	cmp	r3, #2
20004916:	f340 827a 	ble.w	20004e0e <_dtoa_r+0x8ae>
2000491a:	9b04      	ldr	r3, [sp, #16]
2000491c:	2b00      	cmp	r3, #0
2000491e:	f300 8276 	bgt.w	20004e0e <_dtoa_r+0x8ae>
20004922:	9b04      	ldr	r3, [sp, #16]
20004924:	2b00      	cmp	r3, #0
20004926:	f47f af50 	bne.w	200047ca <_dtoa_r+0x26a>
2000492a:	4639      	mov	r1, r7
2000492c:	2205      	movs	r2, #5
2000492e:	4620      	mov	r0, r4
20004930:	f001 f990 	bl	20005c54 <__multadd>
20004934:	4607      	mov	r7, r0
20004936:	9806      	ldr	r0, [sp, #24]
20004938:	4639      	mov	r1, r7
2000493a:	f001 fbc1 	bl	200060c0 <__mcmp>
2000493e:	2800      	cmp	r0, #0
20004940:	f77f af43 	ble.w	200047ca <_dtoa_r+0x26a>
20004944:	9a02      	ldr	r2, [sp, #8]
20004946:	2331      	movs	r3, #49	; 0x31
20004948:	3201      	adds	r2, #1
2000494a:	9202      	str	r2, [sp, #8]
2000494c:	9a05      	ldr	r2, [sp, #20]
2000494e:	1c55      	adds	r5, r2, #1
20004950:	7013      	strb	r3, [r2, #0]
20004952:	4639      	mov	r1, r7
20004954:	4620      	mov	r0, r4
20004956:	f001 f973 	bl	20005c40 <_Bfree>
2000495a:	9b08      	ldr	r3, [sp, #32]
2000495c:	b11b      	cbz	r3, 20004966 <_dtoa_r+0x406>
2000495e:	9908      	ldr	r1, [sp, #32]
20004960:	4620      	mov	r0, r4
20004962:	f001 f96d 	bl	20005c40 <_Bfree>
20004966:	4620      	mov	r0, r4
20004968:	9906      	ldr	r1, [sp, #24]
2000496a:	f001 f969 	bl	20005c40 <_Bfree>
2000496e:	2200      	movs	r2, #0
20004970:	9b02      	ldr	r3, [sp, #8]
20004972:	702a      	strb	r2, [r5, #0]
20004974:	3301      	adds	r3, #1
20004976:	9a24      	ldr	r2, [sp, #144]	; 0x90
20004978:	6013      	str	r3, [r2, #0]
2000497a:	9b26      	ldr	r3, [sp, #152]	; 0x98
2000497c:	2b00      	cmp	r3, #0
2000497e:	f000 82a3 	beq.w	20004ec8 <_dtoa_r+0x968>
20004982:	9805      	ldr	r0, [sp, #20]
20004984:	601d      	str	r5, [r3, #0]
20004986:	b019      	add	sp, #100	; 0x64
20004988:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
2000498c:	9802      	ldr	r0, [sp, #8]
2000498e:	f003 fa8b 	bl	20007ea8 <__aeabi_i2d>
20004992:	4632      	mov	r2, r6
20004994:	463b      	mov	r3, r7
20004996:	f003 fd55 	bl	20008444 <__aeabi_dcmpeq>
2000499a:	2800      	cmp	r0, #0
2000499c:	f47f ae85 	bne.w	200046aa <_dtoa_r+0x14a>
200049a0:	9b02      	ldr	r3, [sp, #8]
200049a2:	3b01      	subs	r3, #1
200049a4:	9302      	str	r3, [sp, #8]
200049a6:	e680      	b.n	200046aa <_dtoa_r+0x14a>
200049a8:	9a07      	ldr	r2, [sp, #28]
200049aa:	9b02      	ldr	r3, [sp, #8]
200049ac:	1ad2      	subs	r2, r2, r3
200049ae:	425b      	negs	r3, r3
200049b0:	930a      	str	r3, [sp, #40]	; 0x28
200049b2:	2300      	movs	r3, #0
200049b4:	9207      	str	r2, [sp, #28]
200049b6:	930c      	str	r3, [sp, #48]	; 0x30
200049b8:	e6a0      	b.n	200046fc <_dtoa_r+0x19c>
200049ba:	425b      	negs	r3, r3
200049bc:	9307      	str	r3, [sp, #28]
200049be:	2300      	movs	r3, #0
200049c0:	9303      	str	r3, [sp, #12]
200049c2:	e690      	b.n	200046e6 <_dtoa_r+0x186>
200049c4:	900d      	str	r0, [sp, #52]	; 0x34
200049c6:	e686      	b.n	200046d6 <_dtoa_r+0x176>
200049c8:	4bbd      	ldr	r3, [pc, #756]	; (20004cc0 <_dtoa_r+0x760>)
200049ca:	1b5b      	subs	r3, r3, r5
200049cc:	fa0a f003 	lsl.w	r0, sl, r3
200049d0:	e638      	b.n	20004644 <_dtoa_r+0xe4>
200049d2:	2100      	movs	r1, #0
200049d4:	f04f 32ff 	mov.w	r2, #4294967295
200049d8:	9123      	str	r1, [sp, #140]	; 0x8c
200049da:	2101      	movs	r1, #1
200049dc:	9204      	str	r2, [sp, #16]
200049de:	910b      	str	r1, [sp, #44]	; 0x2c
200049e0:	9210      	str	r2, [sp, #64]	; 0x40
200049e2:	9a22      	ldr	r2, [sp, #136]	; 0x88
200049e4:	2a01      	cmp	r2, #1
200049e6:	f340 8460 	ble.w	200052aa <_dtoa_r+0xd4a>
200049ea:	9b04      	ldr	r3, [sp, #16]
200049ec:	1e5e      	subs	r6, r3, #1
200049ee:	9b0a      	ldr	r3, [sp, #40]	; 0x28
200049f0:	42b3      	cmp	r3, r6
200049f2:	f2c0 8405 	blt.w	20005200 <_dtoa_r+0xca0>
200049f6:	1b9e      	subs	r6, r3, r6
200049f8:	9b04      	ldr	r3, [sp, #16]
200049fa:	2b00      	cmp	r3, #0
200049fc:	f2c0 8461 	blt.w	200052c2 <_dtoa_r+0xd62>
20004a00:	9d07      	ldr	r5, [sp, #28]
20004a02:	9b04      	ldr	r3, [sp, #16]
20004a04:	9a07      	ldr	r2, [sp, #28]
20004a06:	4620      	mov	r0, r4
20004a08:	2101      	movs	r1, #1
20004a0a:	441a      	add	r2, r3
20004a0c:	9207      	str	r2, [sp, #28]
20004a0e:	9a03      	ldr	r2, [sp, #12]
20004a10:	441a      	add	r2, r3
20004a12:	9203      	str	r2, [sp, #12]
20004a14:	f001 f9fe 	bl	20005e14 <__i2b>
20004a18:	9008      	str	r0, [sp, #32]
20004a1a:	e705      	b.n	20004828 <_dtoa_r+0x2c8>
20004a1c:	2301      	movs	r3, #1
20004a1e:	930b      	str	r3, [sp, #44]	; 0x2c
20004a20:	9b23      	ldr	r3, [sp, #140]	; 0x8c
20004a22:	2b00      	cmp	r3, #0
20004a24:	f340 83df 	ble.w	200051e6 <_dtoa_r+0xc86>
20004a28:	2b0e      	cmp	r3, #14
20004a2a:	bf8c      	ite	hi
20004a2c:	2500      	movhi	r5, #0
20004a2e:	f005 0501 	andls.w	r5, r5, #1
20004a32:	461e      	mov	r6, r3
20004a34:	9310      	str	r3, [sp, #64]	; 0x40
20004a36:	9304      	str	r3, [sp, #16]
20004a38:	2100      	movs	r1, #0
20004a3a:	2e17      	cmp	r6, #23
20004a3c:	6461      	str	r1, [r4, #68]	; 0x44
20004a3e:	d909      	bls.n	20004a54 <_dtoa_r+0x4f4>
20004a40:	2201      	movs	r2, #1
20004a42:	2304      	movs	r3, #4
20004a44:	005b      	lsls	r3, r3, #1
20004a46:	4611      	mov	r1, r2
20004a48:	3201      	adds	r2, #1
20004a4a:	f103 0014 	add.w	r0, r3, #20
20004a4e:	42b0      	cmp	r0, r6
20004a50:	d9f8      	bls.n	20004a44 <_dtoa_r+0x4e4>
20004a52:	6461      	str	r1, [r4, #68]	; 0x44
20004a54:	4620      	mov	r0, r4
20004a56:	f001 f8cb 	bl	20005bf0 <_Balloc>
20004a5a:	9005      	str	r0, [sp, #20]
20004a5c:	6420      	str	r0, [r4, #64]	; 0x40
20004a5e:	2d00      	cmp	r5, #0
20004a60:	f43f ae8c 	beq.w	2000477c <_dtoa_r+0x21c>
20004a64:	9802      	ldr	r0, [sp, #8]
20004a66:	2800      	cmp	r0, #0
20004a68:	e9cd ab0e 	strd	sl, fp, [sp, #56]	; 0x38
20004a6c:	f340 8233 	ble.w	20004ed6 <_dtoa_r+0x976>
20004a70:	4a94      	ldr	r2, [pc, #592]	; (20004cc4 <_dtoa_r+0x764>)
20004a72:	f000 030f 	and.w	r3, r0, #15
20004a76:	1105      	asrs	r5, r0, #4
20004a78:	eb02 03c3 	add.w	r3, r2, r3, lsl #3
20004a7c:	06ea      	lsls	r2, r5, #27
20004a7e:	e9d3 6700 	ldrd	r6, r7, [r3]
20004a82:	f140 821c 	bpl.w	20004ebe <_dtoa_r+0x95e>
20004a86:	4b90      	ldr	r3, [pc, #576]	; (20004cc8 <_dtoa_r+0x768>)
20004a88:	4650      	mov	r0, sl
20004a8a:	4659      	mov	r1, fp
20004a8c:	f005 050f 	and.w	r5, r5, #15
20004a90:	f04f 0803 	mov.w	r8, #3
20004a94:	e9d3 2308 	ldrd	r2, r3, [r3, #32]
20004a98:	f003 fb96 	bl	200081c8 <__aeabi_ddiv>
20004a9c:	4682      	mov	sl, r0
20004a9e:	468b      	mov	fp, r1
20004aa0:	b18d      	cbz	r5, 20004ac6 <_dtoa_r+0x566>
20004aa2:	f8df 9224 	ldr.w	r9, [pc, #548]	; 20004cc8 <_dtoa_r+0x768>
20004aa6:	07eb      	lsls	r3, r5, #31
20004aa8:	4630      	mov	r0, r6
20004aaa:	4639      	mov	r1, r7
20004aac:	d507      	bpl.n	20004abe <_dtoa_r+0x55e>
20004aae:	f108 0801 	add.w	r8, r8, #1
20004ab2:	e9d9 2300 	ldrd	r2, r3, [r9]
20004ab6:	f003 fa5d 	bl	20007f74 <__aeabi_dmul>
20004aba:	4606      	mov	r6, r0
20004abc:	460f      	mov	r7, r1
20004abe:	106d      	asrs	r5, r5, #1
20004ac0:	f109 0908 	add.w	r9, r9, #8
20004ac4:	d1ef      	bne.n	20004aa6 <_dtoa_r+0x546>
20004ac6:	4650      	mov	r0, sl
20004ac8:	4659      	mov	r1, fp
20004aca:	4632      	mov	r2, r6
20004acc:	463b      	mov	r3, r7
20004ace:	f003 fb7b 	bl	200081c8 <__aeabi_ddiv>
20004ad2:	e9cd 0108 	strd	r0, r1, [sp, #32]
20004ad6:	9b0d      	ldr	r3, [sp, #52]	; 0x34
20004ad8:	b143      	cbz	r3, 20004aec <_dtoa_r+0x58c>
20004ada:	2200      	movs	r2, #0
20004adc:	4b7b      	ldr	r3, [pc, #492]	; (20004ccc <_dtoa_r+0x76c>)
20004ade:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
20004ae2:	f003 fcb9 	bl	20008458 <__aeabi_dcmplt>
20004ae6:	2800      	cmp	r0, #0
20004ae8:	f040 8393 	bne.w	20005212 <_dtoa_r+0xcb2>
20004aec:	4640      	mov	r0, r8
20004aee:	f003 f9db 	bl	20007ea8 <__aeabi_i2d>
20004af2:	e9dd 2308 	ldrd	r2, r3, [sp, #32]
20004af6:	f003 fa3d 	bl	20007f74 <__aeabi_dmul>
20004afa:	4b75      	ldr	r3, [pc, #468]	; (20004cd0 <_dtoa_r+0x770>)
20004afc:	2200      	movs	r2, #0
20004afe:	f003 f887 	bl	20007c10 <__adddf3>
20004b02:	9b04      	ldr	r3, [sp, #16]
20004b04:	4606      	mov	r6, r0
20004b06:	f1a1 7750 	sub.w	r7, r1, #54525952	; 0x3400000
20004b0a:	2b00      	cmp	r3, #0
20004b0c:	f000 8161 	beq.w	20004dd2 <_dtoa_r+0x872>
20004b10:	9b02      	ldr	r3, [sp, #8]
20004b12:	f8dd c010 	ldr.w	ip, [sp, #16]
20004b16:	9314      	str	r3, [sp, #80]	; 0x50
20004b18:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
20004b1a:	2b00      	cmp	r3, #0
20004b1c:	f000 820b 	beq.w	20004f36 <_dtoa_r+0x9d6>
20004b20:	4b68      	ldr	r3, [pc, #416]	; (20004cc4 <_dtoa_r+0x764>)
20004b22:	2000      	movs	r0, #0
20004b24:	9a05      	ldr	r2, [sp, #20]
20004b26:	eb03 03cc 	add.w	r3, r3, ip, lsl #3
20004b2a:	496a      	ldr	r1, [pc, #424]	; (20004cd4 <_dtoa_r+0x774>)
20004b2c:	1c55      	adds	r5, r2, #1
20004b2e:	f8cd c048 	str.w	ip, [sp, #72]	; 0x48
20004b32:	e953 2302 	ldrd	r2, r3, [r3, #-8]
20004b36:	f003 fb47 	bl	200081c8 <__aeabi_ddiv>
20004b3a:	4632      	mov	r2, r6
20004b3c:	463b      	mov	r3, r7
20004b3e:	f003 f865 	bl	20007c0c <__aeabi_dsub>
20004b42:	4682      	mov	sl, r0
20004b44:	468b      	mov	fp, r1
20004b46:	e9dd 8908 	ldrd	r8, r9, [sp, #32]
20004b4a:	4649      	mov	r1, r9
20004b4c:	4640      	mov	r0, r8
20004b4e:	f003 fcab 	bl	200084a8 <__aeabi_d2iz>
20004b52:	4606      	mov	r6, r0
20004b54:	f003 f9a8 	bl	20007ea8 <__aeabi_i2d>
20004b58:	4602      	mov	r2, r0
20004b5a:	3630      	adds	r6, #48	; 0x30
20004b5c:	460b      	mov	r3, r1
20004b5e:	4640      	mov	r0, r8
20004b60:	4649      	mov	r1, r9
20004b62:	f003 f853 	bl	20007c0c <__aeabi_dsub>
20004b66:	9f05      	ldr	r7, [sp, #20]
20004b68:	4680      	mov	r8, r0
20004b6a:	4689      	mov	r9, r1
20004b6c:	b2f6      	uxtb	r6, r6
20004b6e:	4650      	mov	r0, sl
20004b70:	4659      	mov	r1, fp
20004b72:	4642      	mov	r2, r8
20004b74:	464b      	mov	r3, r9
20004b76:	703e      	strb	r6, [r7, #0]
20004b78:	f003 fc8c 	bl	20008494 <__aeabi_dcmpgt>
20004b7c:	2800      	cmp	r0, #0
20004b7e:	f040 8243 	bne.w	20005008 <_dtoa_r+0xaa8>
20004b82:	4642      	mov	r2, r8
20004b84:	464b      	mov	r3, r9
20004b86:	2000      	movs	r0, #0
20004b88:	4950      	ldr	r1, [pc, #320]	; (20004ccc <_dtoa_r+0x76c>)
20004b8a:	f003 f83f 	bl	20007c0c <__aeabi_dsub>
20004b8e:	4602      	mov	r2, r0
20004b90:	460b      	mov	r3, r1
20004b92:	4650      	mov	r0, sl
20004b94:	4659      	mov	r1, fp
20004b96:	f003 fc7d 	bl	20008494 <__aeabi_dcmpgt>
20004b9a:	2800      	cmp	r0, #0
20004b9c:	f040 80eb 	bne.w	20004d76 <_dtoa_r+0x816>
20004ba0:	f8dd c048 	ldr.w	ip, [sp, #72]	; 0x48
20004ba4:	f1bc 0f01 	cmp.w	ip, #1
20004ba8:	f340 8192 	ble.w	20004ed0 <_dtoa_r+0x970>
20004bac:	9f05      	ldr	r7, [sp, #20]
20004bae:	9412      	str	r4, [sp, #72]	; 0x48
20004bb0:	4467      	add	r7, ip
20004bb2:	9708      	str	r7, [sp, #32]
20004bb4:	e00e      	b.n	20004bd4 <_dtoa_r+0x674>
20004bb6:	2000      	movs	r0, #0
20004bb8:	4944      	ldr	r1, [pc, #272]	; (20004ccc <_dtoa_r+0x76c>)
20004bba:	f003 f827 	bl	20007c0c <__aeabi_dsub>
20004bbe:	4652      	mov	r2, sl
20004bc0:	465b      	mov	r3, fp
20004bc2:	f003 fc49 	bl	20008458 <__aeabi_dcmplt>
20004bc6:	2800      	cmp	r0, #0
20004bc8:	f040 83a1 	bne.w	2000530e <_dtoa_r+0xdae>
20004bcc:	9b08      	ldr	r3, [sp, #32]
20004bce:	429d      	cmp	r5, r3
20004bd0:	f000 817d 	beq.w	20004ece <_dtoa_r+0x96e>
20004bd4:	4650      	mov	r0, sl
20004bd6:	4659      	mov	r1, fp
20004bd8:	2200      	movs	r2, #0
20004bda:	4b3f      	ldr	r3, [pc, #252]	; (20004cd8 <_dtoa_r+0x778>)
20004bdc:	f003 f9ca 	bl	20007f74 <__aeabi_dmul>
20004be0:	2200      	movs	r2, #0
20004be2:	4b3d      	ldr	r3, [pc, #244]	; (20004cd8 <_dtoa_r+0x778>)
20004be4:	4682      	mov	sl, r0
20004be6:	468b      	mov	fp, r1
20004be8:	4640      	mov	r0, r8
20004bea:	4649      	mov	r1, r9
20004bec:	f003 f9c2 	bl	20007f74 <__aeabi_dmul>
20004bf0:	4606      	mov	r6, r0
20004bf2:	460f      	mov	r7, r1
20004bf4:	f003 fc58 	bl	200084a8 <__aeabi_d2iz>
20004bf8:	4604      	mov	r4, r0
20004bfa:	f003 f955 	bl	20007ea8 <__aeabi_i2d>
20004bfe:	4602      	mov	r2, r0
20004c00:	460b      	mov	r3, r1
20004c02:	3430      	adds	r4, #48	; 0x30
20004c04:	4630      	mov	r0, r6
20004c06:	4639      	mov	r1, r7
20004c08:	b2e6      	uxtb	r6, r4
20004c0a:	f002 ffff 	bl	20007c0c <__aeabi_dsub>
20004c0e:	4680      	mov	r8, r0
20004c10:	4689      	mov	r9, r1
20004c12:	4652      	mov	r2, sl
20004c14:	465b      	mov	r3, fp
20004c16:	f805 6b01 	strb.w	r6, [r5], #1
20004c1a:	f003 fc1d 	bl	20008458 <__aeabi_dcmplt>
20004c1e:	4642      	mov	r2, r8
20004c20:	464b      	mov	r3, r9
20004c22:	2800      	cmp	r0, #0
20004c24:	d0c7      	beq.n	20004bb6 <_dtoa_r+0x656>
20004c26:	9b14      	ldr	r3, [sp, #80]	; 0x50
20004c28:	9c12      	ldr	r4, [sp, #72]	; 0x48
20004c2a:	9302      	str	r3, [sp, #8]
20004c2c:	e69b      	b.n	20004966 <_dtoa_r+0x406>
20004c2e:	2300      	movs	r3, #0
20004c30:	930b      	str	r3, [sp, #44]	; 0x2c
20004c32:	9b23      	ldr	r3, [sp, #140]	; 0x8c
20004c34:	9a02      	ldr	r2, [sp, #8]
20004c36:	4413      	add	r3, r2
20004c38:	9310      	str	r3, [sp, #64]	; 0x40
20004c3a:	3301      	adds	r3, #1
20004c3c:	2b00      	cmp	r3, #0
20004c3e:	9304      	str	r3, [sp, #16]
20004c40:	f340 82c9 	ble.w	200051d6 <_dtoa_r+0xc76>
20004c44:	9e04      	ldr	r6, [sp, #16]
20004c46:	2e0e      	cmp	r6, #14
20004c48:	bf8c      	ite	hi
20004c4a:	2500      	movhi	r5, #0
20004c4c:	f005 0501 	andls.w	r5, r5, #1
20004c50:	e6f2      	b.n	20004a38 <_dtoa_r+0x4d8>
20004c52:	2300      	movs	r3, #0
20004c54:	930b      	str	r3, [sp, #44]	; 0x2c
20004c56:	e6e3      	b.n	20004a20 <_dtoa_r+0x4c0>
20004c58:	9905      	ldr	r1, [sp, #20]
20004c5a:	4642      	mov	r2, r8
20004c5c:	464b      	mov	r3, r9
20004c5e:	4650      	mov	r0, sl
20004c60:	1c4d      	adds	r5, r1, #1
20004c62:	4659      	mov	r1, fp
20004c64:	f003 fab0 	bl	200081c8 <__aeabi_ddiv>
20004c68:	4656      	mov	r6, sl
20004c6a:	f003 fc1d 	bl	200084a8 <__aeabi_d2iz>
20004c6e:	4682      	mov	sl, r0
20004c70:	f003 f91a 	bl	20007ea8 <__aeabi_i2d>
20004c74:	4642      	mov	r2, r8
20004c76:	464b      	mov	r3, r9
20004c78:	f003 f97c 	bl	20007f74 <__aeabi_dmul>
20004c7c:	4602      	mov	r2, r0
20004c7e:	460b      	mov	r3, r1
20004c80:	4630      	mov	r0, r6
20004c82:	4659      	mov	r1, fp
20004c84:	f002 ffc2 	bl	20007c0c <__aeabi_dsub>
20004c88:	f10a 0330 	add.w	r3, sl, #48	; 0x30
20004c8c:	9a05      	ldr	r2, [sp, #20]
20004c8e:	4606      	mov	r6, r0
20004c90:	460f      	mov	r7, r1
20004c92:	7013      	strb	r3, [r2, #0]
20004c94:	9b04      	ldr	r3, [sp, #16]
20004c96:	2b01      	cmp	r3, #1
20004c98:	d04d      	beq.n	20004d36 <_dtoa_r+0x7d6>
20004c9a:	2200      	movs	r2, #0
20004c9c:	4b0e      	ldr	r3, [pc, #56]	; (20004cd8 <_dtoa_r+0x778>)
20004c9e:	f003 f969 	bl	20007f74 <__aeabi_dmul>
20004ca2:	2200      	movs	r2, #0
20004ca4:	2300      	movs	r3, #0
20004ca6:	4606      	mov	r6, r0
20004ca8:	460f      	mov	r7, r1
20004caa:	f003 fbcb 	bl	20008444 <__aeabi_dcmpeq>
20004cae:	2800      	cmp	r0, #0
20004cb0:	f47f ae59 	bne.w	20004966 <_dtoa_r+0x406>
20004cb4:	9403      	str	r4, [sp, #12]
20004cb6:	f8dd b014 	ldr.w	fp, [sp, #20]
20004cba:	9c04      	ldr	r4, [sp, #16]
20004cbc:	e019      	b.n	20004cf2 <_dtoa_r+0x792>
20004cbe:	bf00      	nop
20004cc0:	fffffbee 	.word	0xfffffbee
20004cc4:	200090e8 	.word	0x200090e8
20004cc8:	200091c0 	.word	0x200091c0
20004ccc:	3ff00000 	.word	0x3ff00000
20004cd0:	401c0000 	.word	0x401c0000
20004cd4:	3fe00000 	.word	0x3fe00000
20004cd8:	40240000 	.word	0x40240000
20004cdc:	f003 f94a 	bl	20007f74 <__aeabi_dmul>
20004ce0:	2200      	movs	r2, #0
20004ce2:	2300      	movs	r3, #0
20004ce4:	4606      	mov	r6, r0
20004ce6:	460f      	mov	r7, r1
20004ce8:	f003 fbac 	bl	20008444 <__aeabi_dcmpeq>
20004cec:	2800      	cmp	r0, #0
20004cee:	f040 82e5 	bne.w	200052bc <_dtoa_r+0xd5c>
20004cf2:	4642      	mov	r2, r8
20004cf4:	464b      	mov	r3, r9
20004cf6:	4630      	mov	r0, r6
20004cf8:	4639      	mov	r1, r7
20004cfa:	f003 fa65 	bl	200081c8 <__aeabi_ddiv>
20004cfe:	f003 fbd3 	bl	200084a8 <__aeabi_d2iz>
20004d02:	4682      	mov	sl, r0
20004d04:	f003 f8d0 	bl	20007ea8 <__aeabi_i2d>
20004d08:	4642      	mov	r2, r8
20004d0a:	464b      	mov	r3, r9
20004d0c:	f003 f932 	bl	20007f74 <__aeabi_dmul>
20004d10:	4602      	mov	r2, r0
20004d12:	460b      	mov	r3, r1
20004d14:	4630      	mov	r0, r6
20004d16:	4639      	mov	r1, r7
20004d18:	f002 ff78 	bl	20007c0c <__aeabi_dsub>
20004d1c:	f10a 0e30 	add.w	lr, sl, #48	; 0x30
20004d20:	4606      	mov	r6, r0
20004d22:	460f      	mov	r7, r1
20004d24:	f805 eb01 	strb.w	lr, [r5], #1
20004d28:	ebcb 0e05 	rsb	lr, fp, r5
20004d2c:	2200      	movs	r2, #0
20004d2e:	4bb8      	ldr	r3, [pc, #736]	; (20005010 <_dtoa_r+0xab0>)
20004d30:	4574      	cmp	r4, lr
20004d32:	d1d3      	bne.n	20004cdc <_dtoa_r+0x77c>
20004d34:	9c03      	ldr	r4, [sp, #12]
20004d36:	4632      	mov	r2, r6
20004d38:	463b      	mov	r3, r7
20004d3a:	4630      	mov	r0, r6
20004d3c:	4639      	mov	r1, r7
20004d3e:	f002 ff67 	bl	20007c10 <__adddf3>
20004d42:	4606      	mov	r6, r0
20004d44:	460f      	mov	r7, r1
20004d46:	4640      	mov	r0, r8
20004d48:	4649      	mov	r1, r9
20004d4a:	4632      	mov	r2, r6
20004d4c:	463b      	mov	r3, r7
20004d4e:	f003 fb83 	bl	20008458 <__aeabi_dcmplt>
20004d52:	b960      	cbnz	r0, 20004d6e <_dtoa_r+0x80e>
20004d54:	4640      	mov	r0, r8
20004d56:	4649      	mov	r1, r9
20004d58:	4632      	mov	r2, r6
20004d5a:	463b      	mov	r3, r7
20004d5c:	f003 fb72 	bl	20008444 <__aeabi_dcmpeq>
20004d60:	2800      	cmp	r0, #0
20004d62:	f43f ae00 	beq.w	20004966 <_dtoa_r+0x406>
20004d66:	f01a 0f01 	tst.w	sl, #1
20004d6a:	f43f adfc 	beq.w	20004966 <_dtoa_r+0x406>
20004d6e:	9b02      	ldr	r3, [sp, #8]
20004d70:	9314      	str	r3, [sp, #80]	; 0x50
20004d72:	f815 6c01 	ldrb.w	r6, [r5, #-1]
20004d76:	1e6a      	subs	r2, r5, #1
20004d78:	9b05      	ldr	r3, [sp, #20]
20004d7a:	e004      	b.n	20004d86 <_dtoa_r+0x826>
20004d7c:	429a      	cmp	r2, r3
20004d7e:	f000 8277 	beq.w	20005270 <_dtoa_r+0xd10>
20004d82:	f812 6d01 	ldrb.w	r6, [r2, #-1]!
20004d86:	2e39      	cmp	r6, #57	; 0x39
20004d88:	f102 0501 	add.w	r5, r2, #1
20004d8c:	d0f6      	beq.n	20004d7c <_dtoa_r+0x81c>
20004d8e:	1c73      	adds	r3, r6, #1
20004d90:	9914      	ldr	r1, [sp, #80]	; 0x50
20004d92:	b2db      	uxtb	r3, r3
20004d94:	9102      	str	r1, [sp, #8]
20004d96:	7013      	strb	r3, [r2, #0]
20004d98:	e5e5      	b.n	20004966 <_dtoa_r+0x406>
20004d9a:	9b22      	ldr	r3, [sp, #136]	; 0x88
20004d9c:	2b01      	cmp	r3, #1
20004d9e:	f340 8161 	ble.w	20005064 <_dtoa_r+0xb04>
20004da2:	f04f 0800 	mov.w	r8, #0
20004da6:	9b0c      	ldr	r3, [sp, #48]	; 0x30
20004da8:	2b00      	cmp	r3, #0
20004daa:	f47f ad80 	bne.w	200048ae <_dtoa_r+0x34e>
20004dae:	2001      	movs	r0, #1
20004db0:	e585      	b.n	200048be <_dtoa_r+0x35e>
20004db2:	4640      	mov	r0, r8
20004db4:	f003 f878 	bl	20007ea8 <__aeabi_i2d>
20004db8:	4602      	mov	r2, r0
20004dba:	460b      	mov	r3, r1
20004dbc:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
20004dc0:	f003 f8d8 	bl	20007f74 <__aeabi_dmul>
20004dc4:	2200      	movs	r2, #0
20004dc6:	4b93      	ldr	r3, [pc, #588]	; (20005014 <_dtoa_r+0xab4>)
20004dc8:	f002 ff22 	bl	20007c10 <__adddf3>
20004dcc:	4606      	mov	r6, r0
20004dce:	f1a1 7750 	sub.w	r7, r1, #54525952	; 0x3400000
20004dd2:	2200      	movs	r2, #0
20004dd4:	4b90      	ldr	r3, [pc, #576]	; (20005018 <_dtoa_r+0xab8>)
20004dd6:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
20004dda:	f002 ff17 	bl	20007c0c <__aeabi_dsub>
20004dde:	4632      	mov	r2, r6
20004de0:	463b      	mov	r3, r7
20004de2:	4680      	mov	r8, r0
20004de4:	4689      	mov	r9, r1
20004de6:	f003 fb55 	bl	20008494 <__aeabi_dcmpgt>
20004dea:	4605      	mov	r5, r0
20004dec:	2800      	cmp	r0, #0
20004dee:	f040 809f 	bne.w	20004f30 <_dtoa_r+0x9d0>
20004df2:	4632      	mov	r2, r6
20004df4:	4640      	mov	r0, r8
20004df6:	4649      	mov	r1, r9
20004df8:	f107 4300 	add.w	r3, r7, #2147483648	; 0x80000000
20004dfc:	f003 fb2c 	bl	20008458 <__aeabi_dcmplt>
20004e00:	2800      	cmp	r0, #0
20004e02:	d065      	beq.n	20004ed0 <_dtoa_r+0x970>
20004e04:	462f      	mov	r7, r5
20004e06:	9508      	str	r5, [sp, #32]
20004e08:	e4df      	b.n	200047ca <_dtoa_r+0x26a>
20004e0a:	201c      	movs	r0, #28
20004e0c:	e564      	b.n	200048d8 <_dtoa_r+0x378>
20004e0e:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
20004e10:	2b00      	cmp	r3, #0
20004e12:	f040 814f 	bne.w	200050b4 <_dtoa_r+0xb54>
20004e16:	f8dd a014 	ldr.w	sl, [sp, #20]
20004e1a:	9e06      	ldr	r6, [sp, #24]
20004e1c:	4655      	mov	r5, sl
20004e1e:	f8dd 8010 	ldr.w	r8, [sp, #16]
20004e22:	e002      	b.n	20004e2a <_dtoa_r+0x8ca>
20004e24:	f000 ff16 	bl	20005c54 <__multadd>
20004e28:	4606      	mov	r6, r0
20004e2a:	4639      	mov	r1, r7
20004e2c:	4630      	mov	r0, r6
20004e2e:	f7ff faf5 	bl	2000441c <quorem>
20004e32:	f100 0930 	add.w	r9, r0, #48	; 0x30
20004e36:	4631      	mov	r1, r6
20004e38:	4620      	mov	r0, r4
20004e3a:	f805 9b01 	strb.w	r9, [r5], #1
20004e3e:	ebca 0e05 	rsb	lr, sl, r5
20004e42:	220a      	movs	r2, #10
20004e44:	2300      	movs	r3, #0
20004e46:	45c6      	cmp	lr, r8
20004e48:	dbec      	blt.n	20004e24 <_dtoa_r+0x8c4>
20004e4a:	9b05      	ldr	r3, [sp, #20]
20004e4c:	f04f 0a00 	mov.w	sl, #0
20004e50:	9a04      	ldr	r2, [sp, #16]
20004e52:	9606      	str	r6, [sp, #24]
20004e54:	2a01      	cmp	r2, #1
20004e56:	bfac      	ite	ge
20004e58:	189b      	addge	r3, r3, r2
20004e5a:	3301      	addlt	r3, #1
20004e5c:	461d      	mov	r5, r3
20004e5e:	9906      	ldr	r1, [sp, #24]
20004e60:	2201      	movs	r2, #1
20004e62:	4620      	mov	r0, r4
20004e64:	f001 f8d2 	bl	2000600c <__lshift>
20004e68:	4639      	mov	r1, r7
20004e6a:	9006      	str	r0, [sp, #24]
20004e6c:	f001 f928 	bl	200060c0 <__mcmp>
20004e70:	2800      	cmp	r0, #0
20004e72:	f340 824e 	ble.w	20005312 <_dtoa_r+0xdb2>
20004e76:	f815 2c01 	ldrb.w	r2, [r5, #-1]
20004e7a:	1e6b      	subs	r3, r5, #1
20004e7c:	9905      	ldr	r1, [sp, #20]
20004e7e:	e004      	b.n	20004e8a <_dtoa_r+0x92a>
20004e80:	428b      	cmp	r3, r1
20004e82:	f000 820b 	beq.w	2000529c <_dtoa_r+0xd3c>
20004e86:	f813 2d01 	ldrb.w	r2, [r3, #-1]!
20004e8a:	2a39      	cmp	r2, #57	; 0x39
20004e8c:	f103 0501 	add.w	r5, r3, #1
20004e90:	d0f6      	beq.n	20004e80 <_dtoa_r+0x920>
20004e92:	3201      	adds	r2, #1
20004e94:	701a      	strb	r2, [r3, #0]
20004e96:	4639      	mov	r1, r7
20004e98:	4620      	mov	r0, r4
20004e9a:	f000 fed1 	bl	20005c40 <_Bfree>
20004e9e:	9b08      	ldr	r3, [sp, #32]
20004ea0:	2b00      	cmp	r3, #0
20004ea2:	f43f ad60 	beq.w	20004966 <_dtoa_r+0x406>
20004ea6:	459a      	cmp	sl, r3
20004ea8:	f43f ad59 	beq.w	2000495e <_dtoa_r+0x3fe>
20004eac:	f1ba 0f00 	cmp.w	sl, #0
20004eb0:	f43f ad55 	beq.w	2000495e <_dtoa_r+0x3fe>
20004eb4:	4651      	mov	r1, sl
20004eb6:	4620      	mov	r0, r4
20004eb8:	f000 fec2 	bl	20005c40 <_Bfree>
20004ebc:	e54f      	b.n	2000495e <_dtoa_r+0x3fe>
20004ebe:	e9dd ab0e 	ldrd	sl, fp, [sp, #56]	; 0x38
20004ec2:	f04f 0802 	mov.w	r8, #2
20004ec6:	e5eb      	b.n	20004aa0 <_dtoa_r+0x540>
20004ec8:	9805      	ldr	r0, [sp, #20]
20004eca:	f7ff bb7a 	b.w	200045c2 <_dtoa_r+0x62>
20004ece:	9c12      	ldr	r4, [sp, #72]	; 0x48
20004ed0:	e9dd ab0e 	ldrd	sl, fp, [sp, #56]	; 0x38
20004ed4:	e452      	b.n	2000477c <_dtoa_r+0x21c>
20004ed6:	9b02      	ldr	r3, [sp, #8]
20004ed8:	425d      	negs	r5, r3
20004eda:	2d00      	cmp	r5, #0
20004edc:	f000 81c1 	beq.w	20005262 <_dtoa_r+0xd02>
20004ee0:	f005 020f 	and.w	r2, r5, #15
20004ee4:	4b4d      	ldr	r3, [pc, #308]	; (2000501c <_dtoa_r+0xabc>)
20004ee6:	eb03 03c2 	add.w	r3, r3, r2, lsl #3
20004eea:	e9dd 010e 	ldrd	r0, r1, [sp, #56]	; 0x38
20004eee:	e9d3 2300 	ldrd	r2, r3, [r3]
20004ef2:	f003 f83f 	bl	20007f74 <__aeabi_dmul>
20004ef6:	112d      	asrs	r5, r5, #4
20004ef8:	e9cd 0108 	strd	r0, r1, [sp, #32]
20004efc:	f000 8275 	beq.w	200053ea <_dtoa_r+0xe8a>
20004f00:	4e47      	ldr	r6, [pc, #284]	; (20005020 <_dtoa_r+0xac0>)
20004f02:	f04f 0802 	mov.w	r8, #2
20004f06:	4602      	mov	r2, r0
20004f08:	460b      	mov	r3, r1
20004f0a:	07ef      	lsls	r7, r5, #31
20004f0c:	4610      	mov	r0, r2
20004f0e:	4619      	mov	r1, r3
20004f10:	d507      	bpl.n	20004f22 <_dtoa_r+0x9c2>
20004f12:	f108 0801 	add.w	r8, r8, #1
20004f16:	e9d6 2300 	ldrd	r2, r3, [r6]
20004f1a:	f003 f82b 	bl	20007f74 <__aeabi_dmul>
20004f1e:	4602      	mov	r2, r0
20004f20:	460b      	mov	r3, r1
20004f22:	106d      	asrs	r5, r5, #1
20004f24:	f106 0608 	add.w	r6, r6, #8
20004f28:	d1ef      	bne.n	20004f0a <_dtoa_r+0x9aa>
20004f2a:	e9cd 2308 	strd	r2, r3, [sp, #32]
20004f2e:	e5d2      	b.n	20004ad6 <_dtoa_r+0x576>
20004f30:	2700      	movs	r7, #0
20004f32:	9708      	str	r7, [sp, #32]
20004f34:	e506      	b.n	20004944 <_dtoa_r+0x3e4>
20004f36:	f10c 3bff 	add.w	fp, ip, #4294967295
20004f3a:	4938      	ldr	r1, [pc, #224]	; (2000501c <_dtoa_r+0xabc>)
20004f3c:	4632      	mov	r2, r6
20004f3e:	463b      	mov	r3, r7
20004f40:	eb01 01cb 	add.w	r1, r1, fp, lsl #3
20004f44:	f8cd c054 	str.w	ip, [sp, #84]	; 0x54
20004f48:	9f05      	ldr	r7, [sp, #20]
20004f4a:	e9d1 0100 	ldrd	r0, r1, [r1]
20004f4e:	f003 f811 	bl	20007f74 <__aeabi_dmul>
20004f52:	1c7d      	adds	r5, r7, #1
20004f54:	e9dd 8908 	ldrd	r8, r9, [sp, #32]
20004f58:	e9cd 0112 	strd	r0, r1, [sp, #72]	; 0x48
20004f5c:	4649      	mov	r1, r9
20004f5e:	4640      	mov	r0, r8
20004f60:	f003 faa2 	bl	200084a8 <__aeabi_d2iz>
20004f64:	4606      	mov	r6, r0
20004f66:	f002 ff9f 	bl	20007ea8 <__aeabi_i2d>
20004f6a:	4602      	mov	r2, r0
20004f6c:	460b      	mov	r3, r1
20004f6e:	4640      	mov	r0, r8
20004f70:	4649      	mov	r1, r9
20004f72:	3630      	adds	r6, #48	; 0x30
20004f74:	f002 fe4a 	bl	20007c0c <__aeabi_dsub>
20004f78:	f8dd c054 	ldr.w	ip, [sp, #84]	; 0x54
20004f7c:	4680      	mov	r8, r0
20004f7e:	4689      	mov	r9, r1
20004f80:	f1bc 0f01 	cmp.w	ip, #1
20004f84:	703e      	strb	r6, [r7, #0]
20004f86:	d020      	beq.n	20004fca <_dtoa_r+0xa6a>
20004f88:	9b05      	ldr	r3, [sp, #20]
20004f8a:	4640      	mov	r0, r8
20004f8c:	46aa      	mov	sl, r5
20004f8e:	46a8      	mov	r8, r5
20004f90:	eb03 070c 	add.w	r7, r3, ip
20004f94:	46b9      	mov	r9, r7
20004f96:	2200      	movs	r2, #0
20004f98:	4b1d      	ldr	r3, [pc, #116]	; (20005010 <_dtoa_r+0xab0>)
20004f9a:	f002 ffeb 	bl	20007f74 <__aeabi_dmul>
20004f9e:	460f      	mov	r7, r1
20004fa0:	4606      	mov	r6, r0
20004fa2:	f003 fa81 	bl	200084a8 <__aeabi_d2iz>
20004fa6:	4605      	mov	r5, r0
20004fa8:	f002 ff7e 	bl	20007ea8 <__aeabi_i2d>
20004fac:	4602      	mov	r2, r0
20004fae:	460b      	mov	r3, r1
20004fb0:	3530      	adds	r5, #48	; 0x30
20004fb2:	4630      	mov	r0, r6
20004fb4:	4639      	mov	r1, r7
20004fb6:	f002 fe29 	bl	20007c0c <__aeabi_dsub>
20004fba:	f80a 5b01 	strb.w	r5, [sl], #1
20004fbe:	45ca      	cmp	sl, r9
20004fc0:	d1e9      	bne.n	20004f96 <_dtoa_r+0xa36>
20004fc2:	4645      	mov	r5, r8
20004fc4:	4689      	mov	r9, r1
20004fc6:	4680      	mov	r8, r0
20004fc8:	445d      	add	r5, fp
20004fca:	2200      	movs	r2, #0
20004fcc:	4b15      	ldr	r3, [pc, #84]	; (20005024 <_dtoa_r+0xac4>)
20004fce:	e9dd 0112 	ldrd	r0, r1, [sp, #72]	; 0x48
20004fd2:	f002 fe1d 	bl	20007c10 <__adddf3>
20004fd6:	4642      	mov	r2, r8
20004fd8:	464b      	mov	r3, r9
20004fda:	f003 fa3d 	bl	20008458 <__aeabi_dcmplt>
20004fde:	2800      	cmp	r0, #0
20004fe0:	f47f aec7 	bne.w	20004d72 <_dtoa_r+0x812>
20004fe4:	2000      	movs	r0, #0
20004fe6:	490f      	ldr	r1, [pc, #60]	; (20005024 <_dtoa_r+0xac4>)
20004fe8:	e9dd 2312 	ldrd	r2, r3, [sp, #72]	; 0x48
20004fec:	f002 fe0e 	bl	20007c0c <__aeabi_dsub>
20004ff0:	4642      	mov	r2, r8
20004ff2:	464b      	mov	r3, r9
20004ff4:	f003 fa4e 	bl	20008494 <__aeabi_dcmpgt>
20004ff8:	b908      	cbnz	r0, 20004ffe <_dtoa_r+0xa9e>
20004ffa:	e769      	b.n	20004ed0 <_dtoa_r+0x970>
20004ffc:	4615      	mov	r5, r2
20004ffe:	f815 3c01 	ldrb.w	r3, [r5, #-1]
20005002:	1e6a      	subs	r2, r5, #1
20005004:	2b30      	cmp	r3, #48	; 0x30
20005006:	d0f9      	beq.n	20004ffc <_dtoa_r+0xa9c>
20005008:	9b14      	ldr	r3, [sp, #80]	; 0x50
2000500a:	9302      	str	r3, [sp, #8]
2000500c:	e4ab      	b.n	20004966 <_dtoa_r+0x406>
2000500e:	bf00      	nop
20005010:	40240000 	.word	0x40240000
20005014:	401c0000 	.word	0x401c0000
20005018:	40140000 	.word	0x40140000
2000501c:	200090e8 	.word	0x200090e8
20005020:	200091c0 	.word	0x200091c0
20005024:	3fe00000 	.word	0x3fe00000
20005028:	9806      	ldr	r0, [sp, #24]
2000502a:	4639      	mov	r1, r7
2000502c:	f001 f848 	bl	200060c0 <__mcmp>
20005030:	2800      	cmp	r0, #0
20005032:	f6bf ac6e 	bge.w	20004912 <_dtoa_r+0x3b2>
20005036:	9e02      	ldr	r6, [sp, #8]
20005038:	2300      	movs	r3, #0
2000503a:	9906      	ldr	r1, [sp, #24]
2000503c:	4620      	mov	r0, r4
2000503e:	3e01      	subs	r6, #1
20005040:	220a      	movs	r2, #10
20005042:	9602      	str	r6, [sp, #8]
20005044:	f000 fe06 	bl	20005c54 <__multadd>
20005048:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
2000504a:	9006      	str	r0, [sp, #24]
2000504c:	bb1b      	cbnz	r3, 20005096 <_dtoa_r+0xb36>
2000504e:	9b22      	ldr	r3, [sp, #136]	; 0x88
20005050:	2b02      	cmp	r3, #2
20005052:	f340 81ce 	ble.w	200053f2 <_dtoa_r+0xe92>
20005056:	9b10      	ldr	r3, [sp, #64]	; 0x40
20005058:	2b00      	cmp	r3, #0
2000505a:	f300 81ca 	bgt.w	200053f2 <_dtoa_r+0xe92>
2000505e:	9b10      	ldr	r3, [sp, #64]	; 0x40
20005060:	9304      	str	r3, [sp, #16]
20005062:	e45e      	b.n	20004922 <_dtoa_r+0x3c2>
20005064:	f1ba 0f00 	cmp.w	sl, #0
20005068:	f47f ae9b 	bne.w	20004da2 <_dtoa_r+0x842>
2000506c:	f3cb 0313 	ubfx	r3, fp, #0, #20
20005070:	2b00      	cmp	r3, #0
20005072:	f47f ae96 	bne.w	20004da2 <_dtoa_r+0x842>
20005076:	f02b 4600 	bic.w	r6, fp, #2147483648	; 0x80000000
2000507a:	0d36      	lsrs	r6, r6, #20
2000507c:	0536      	lsls	r6, r6, #20
2000507e:	2e00      	cmp	r6, #0
20005080:	f000 818a 	beq.w	20005398 <_dtoa_r+0xe38>
20005084:	9b07      	ldr	r3, [sp, #28]
20005086:	f04f 0801 	mov.w	r8, #1
2000508a:	3301      	adds	r3, #1
2000508c:	9307      	str	r3, [sp, #28]
2000508e:	9b03      	ldr	r3, [sp, #12]
20005090:	3301      	adds	r3, #1
20005092:	9303      	str	r3, [sp, #12]
20005094:	e687      	b.n	20004da6 <_dtoa_r+0x846>
20005096:	2300      	movs	r3, #0
20005098:	9908      	ldr	r1, [sp, #32]
2000509a:	4620      	mov	r0, r4
2000509c:	220a      	movs	r2, #10
2000509e:	f000 fdd9 	bl	20005c54 <__multadd>
200050a2:	9b22      	ldr	r3, [sp, #136]	; 0x88
200050a4:	9008      	str	r0, [sp, #32]
200050a6:	2b02      	cmp	r3, #2
200050a8:	dd02      	ble.n	200050b0 <_dtoa_r+0xb50>
200050aa:	9b10      	ldr	r3, [sp, #64]	; 0x40
200050ac:	2b00      	cmp	r3, #0
200050ae:	ddd6      	ble.n	2000505e <_dtoa_r+0xafe>
200050b0:	9b10      	ldr	r3, [sp, #64]	; 0x40
200050b2:	9304      	str	r3, [sp, #16]
200050b4:	2d00      	cmp	r5, #0
200050b6:	dd05      	ble.n	200050c4 <_dtoa_r+0xb64>
200050b8:	9908      	ldr	r1, [sp, #32]
200050ba:	462a      	mov	r2, r5
200050bc:	4620      	mov	r0, r4
200050be:	f000 ffa5 	bl	2000600c <__lshift>
200050c2:	9008      	str	r0, [sp, #32]
200050c4:	f1b8 0f00 	cmp.w	r8, #0
200050c8:	f040 8133 	bne.w	20005332 <_dtoa_r+0xdd2>
200050cc:	9e08      	ldr	r6, [sp, #32]
200050ce:	f00a 0101 	and.w	r1, sl, #1
200050d2:	9b04      	ldr	r3, [sp, #16]
200050d4:	9a22      	ldr	r2, [sp, #136]	; 0x88
200050d6:	3b01      	subs	r3, #1
200050d8:	910b      	str	r1, [sp, #44]	; 0x2c
200050da:	9905      	ldr	r1, [sp, #20]
200050dc:	fab2 f282 	clz	r2, r2
200050e0:	970a      	str	r7, [sp, #40]	; 0x28
200050e2:	18cb      	adds	r3, r1, r3
200050e4:	4688      	mov	r8, r1
200050e6:	f8dd b020 	ldr.w	fp, [sp, #32]
200050ea:	930c      	str	r3, [sp, #48]	; 0x30
200050ec:	0953      	lsrs	r3, r2, #5
200050ee:	9f06      	ldr	r7, [sp, #24]
200050f0:	9303      	str	r3, [sp, #12]
200050f2:	9d0a      	ldr	r5, [sp, #40]	; 0x28
200050f4:	4638      	mov	r0, r7
200050f6:	4629      	mov	r1, r5
200050f8:	f7ff f990 	bl	2000441c <quorem>
200050fc:	4659      	mov	r1, fp
200050fe:	4681      	mov	r9, r0
20005100:	4638      	mov	r0, r7
20005102:	f000 ffdd 	bl	200060c0 <__mcmp>
20005106:	4629      	mov	r1, r5
20005108:	4632      	mov	r2, r6
2000510a:	4682      	mov	sl, r0
2000510c:	4620      	mov	r0, r4
2000510e:	f000 fffb 	bl	20006108 <__mdiff>
20005112:	68c2      	ldr	r2, [r0, #12]
20005114:	4605      	mov	r5, r0
20005116:	f109 0c30 	add.w	ip, r9, #48	; 0x30
2000511a:	9207      	str	r2, [sp, #28]
2000511c:	2a00      	cmp	r2, #0
2000511e:	d150      	bne.n	200051c2 <_dtoa_r+0xc62>
20005120:	4629      	mov	r1, r5
20005122:	4638      	mov	r0, r7
20005124:	f8cd c018 	str.w	ip, [sp, #24]
20005128:	f000 ffca 	bl	200060c0 <__mcmp>
2000512c:	4629      	mov	r1, r5
2000512e:	9004      	str	r0, [sp, #16]
20005130:	4620      	mov	r0, r4
20005132:	f000 fd85 	bl	20005c40 <_Bfree>
20005136:	9a03      	ldr	r2, [sp, #12]
20005138:	9b04      	ldr	r3, [sp, #16]
2000513a:	f8dd c018 	ldr.w	ip, [sp, #24]
2000513e:	b132      	cbz	r2, 2000514e <_dtoa_r+0xbee>
20005140:	b92b      	cbnz	r3, 2000514e <_dtoa_r+0xbee>
20005142:	9b0b      	ldr	r3, [sp, #44]	; 0x2c
20005144:	9a07      	ldr	r2, [sp, #28]
20005146:	2b00      	cmp	r3, #0
20005148:	f000 8128 	beq.w	2000539c <_dtoa_r+0xe3c>
2000514c:	4613      	mov	r3, r2
2000514e:	f1ba 0f00 	cmp.w	sl, #0
20005152:	f2c0 80bc 	blt.w	200052ce <_dtoa_r+0xd6e>
20005156:	9a03      	ldr	r2, [sp, #12]
20005158:	b132      	cbz	r2, 20005168 <_dtoa_r+0xc08>
2000515a:	f1ba 0f00 	cmp.w	sl, #0
2000515e:	d103      	bne.n	20005168 <_dtoa_r+0xc08>
20005160:	9a0b      	ldr	r2, [sp, #44]	; 0x2c
20005162:	2a00      	cmp	r2, #0
20005164:	f000 80b3 	beq.w	200052ce <_dtoa_r+0xd6e>
20005168:	2b00      	cmp	r3, #0
2000516a:	f300 80fe 	bgt.w	2000536a <_dtoa_r+0xe0a>
2000516e:	9b0c      	ldr	r3, [sp, #48]	; 0x30
20005170:	f108 0901 	add.w	r9, r8, #1
20005174:	f888 c000 	strb.w	ip, [r8]
20005178:	4598      	cmp	r8, r3
2000517a:	464d      	mov	r5, r9
2000517c:	f000 8106 	beq.w	2000538c <_dtoa_r+0xe2c>
20005180:	4639      	mov	r1, r7
20005182:	220a      	movs	r2, #10
20005184:	2300      	movs	r3, #0
20005186:	4620      	mov	r0, r4
20005188:	f000 fd64 	bl	20005c54 <__multadd>
2000518c:	45b3      	cmp	fp, r6
2000518e:	4607      	mov	r7, r0
20005190:	4659      	mov	r1, fp
20005192:	4620      	mov	r0, r4
20005194:	f04f 020a 	mov.w	r2, #10
20005198:	f04f 0300 	mov.w	r3, #0
2000519c:	d00b      	beq.n	200051b6 <_dtoa_r+0xc56>
2000519e:	f000 fd59 	bl	20005c54 <__multadd>
200051a2:	4631      	mov	r1, r6
200051a4:	4683      	mov	fp, r0
200051a6:	220a      	movs	r2, #10
200051a8:	4620      	mov	r0, r4
200051aa:	2300      	movs	r3, #0
200051ac:	f000 fd52 	bl	20005c54 <__multadd>
200051b0:	46c8      	mov	r8, r9
200051b2:	4606      	mov	r6, r0
200051b4:	e79d      	b.n	200050f2 <_dtoa_r+0xb92>
200051b6:	f000 fd4d 	bl	20005c54 <__multadd>
200051ba:	46c8      	mov	r8, r9
200051bc:	4683      	mov	fp, r0
200051be:	4606      	mov	r6, r0
200051c0:	e797      	b.n	200050f2 <_dtoa_r+0xb92>
200051c2:	4601      	mov	r1, r0
200051c4:	4620      	mov	r0, r4
200051c6:	f8cd c010 	str.w	ip, [sp, #16]
200051ca:	f000 fd39 	bl	20005c40 <_Bfree>
200051ce:	2301      	movs	r3, #1
200051d0:	f8dd c010 	ldr.w	ip, [sp, #16]
200051d4:	e7bb      	b.n	2000514e <_dtoa_r+0xbee>
200051d6:	2b0e      	cmp	r3, #14
200051d8:	bf8c      	ite	hi
200051da:	2300      	movhi	r3, #0
200051dc:	2301      	movls	r3, #1
200051de:	2100      	movs	r1, #0
200051e0:	401d      	ands	r5, r3
200051e2:	6461      	str	r1, [r4, #68]	; 0x44
200051e4:	e436      	b.n	20004a54 <_dtoa_r+0x4f4>
200051e6:	2301      	movs	r3, #1
200051e8:	9323      	str	r3, [sp, #140]	; 0x8c
200051ea:	9310      	str	r3, [sp, #64]	; 0x40
200051ec:	9304      	str	r3, [sp, #16]
200051ee:	e7f6      	b.n	200051de <_dtoa_r+0xc7e>
200051f0:	9906      	ldr	r1, [sp, #24]
200051f2:	4620      	mov	r0, r4
200051f4:	9a0a      	ldr	r2, [sp, #40]	; 0x28
200051f6:	f000 feb9 	bl	20005f6c <__pow5mult>
200051fa:	9006      	str	r0, [sp, #24]
200051fc:	f7ff bb42 	b.w	20004884 <_dtoa_r+0x324>
20005200:	9b0a      	ldr	r3, [sp, #40]	; 0x28
20005202:	9a0c      	ldr	r2, [sp, #48]	; 0x30
20005204:	1af3      	subs	r3, r6, r3
20005206:	960a      	str	r6, [sp, #40]	; 0x28
20005208:	2600      	movs	r6, #0
2000520a:	441a      	add	r2, r3
2000520c:	920c      	str	r2, [sp, #48]	; 0x30
2000520e:	f7ff bbf3 	b.w	200049f8 <_dtoa_r+0x498>
20005212:	9b04      	ldr	r3, [sp, #16]
20005214:	2b00      	cmp	r3, #0
20005216:	f43f adcc 	beq.w	20004db2 <_dtoa_r+0x852>
2000521a:	9d10      	ldr	r5, [sp, #64]	; 0x40
2000521c:	2d00      	cmp	r5, #0
2000521e:	f77f ae57 	ble.w	20004ed0 <_dtoa_r+0x970>
20005222:	2200      	movs	r2, #0
20005224:	4b86      	ldr	r3, [pc, #536]	; (20005440 <_dtoa_r+0xee0>)
20005226:	e9dd 0108 	ldrd	r0, r1, [sp, #32]
2000522a:	f002 fea3 	bl	20007f74 <__aeabi_dmul>
2000522e:	9b02      	ldr	r3, [sp, #8]
20005230:	4606      	mov	r6, r0
20005232:	460f      	mov	r7, r1
20005234:	3b01      	subs	r3, #1
20005236:	f108 0001 	add.w	r0, r8, #1
2000523a:	e9cd 6708 	strd	r6, r7, [sp, #32]
2000523e:	9314      	str	r3, [sp, #80]	; 0x50
20005240:	f002 fe32 	bl	20007ea8 <__aeabi_i2d>
20005244:	4602      	mov	r2, r0
20005246:	460b      	mov	r3, r1
20005248:	4630      	mov	r0, r6
2000524a:	4639      	mov	r1, r7
2000524c:	f002 fe92 	bl	20007f74 <__aeabi_dmul>
20005250:	2200      	movs	r2, #0
20005252:	4b7c      	ldr	r3, [pc, #496]	; (20005444 <_dtoa_r+0xee4>)
20005254:	f002 fcdc 	bl	20007c10 <__adddf3>
20005258:	46ac      	mov	ip, r5
2000525a:	4606      	mov	r6, r0
2000525c:	f1a1 7750 	sub.w	r7, r1, #54525952	; 0x3400000
20005260:	e45a      	b.n	20004b18 <_dtoa_r+0x5b8>
20005262:	e9dd 230e 	ldrd	r2, r3, [sp, #56]	; 0x38
20005266:	f04f 0802 	mov.w	r8, #2
2000526a:	e9cd 2308 	strd	r2, r3, [sp, #32]
2000526e:	e432      	b.n	20004ad6 <_dtoa_r+0x576>
20005270:	9b14      	ldr	r3, [sp, #80]	; 0x50
20005272:	2130      	movs	r1, #48	; 0x30
20005274:	3301      	adds	r3, #1
20005276:	7011      	strb	r1, [r2, #0]
20005278:	9302      	str	r3, [sp, #8]
2000527a:	2331      	movs	r3, #49	; 0x31
2000527c:	7013      	strb	r3, [r2, #0]
2000527e:	f7ff bb72 	b.w	20004966 <_dtoa_r+0x406>
20005282:	9906      	ldr	r1, [sp, #24]
20005284:	4620      	mov	r0, r4
20005286:	f000 fe71 	bl	20005f6c <__pow5mult>
2000528a:	9006      	str	r0, [sp, #24]
2000528c:	f7ff bafa 	b.w	20004884 <_dtoa_r+0x324>
20005290:	f1ba 0f00 	cmp.w	sl, #0
20005294:	f43f aeea 	beq.w	2000506c <_dtoa_r+0xb0c>
20005298:	f7ff bb07 	b.w	200048aa <_dtoa_r+0x34a>
2000529c:	9a02      	ldr	r2, [sp, #8]
2000529e:	2331      	movs	r3, #49	; 0x31
200052a0:	3201      	adds	r2, #1
200052a2:	9202      	str	r2, [sp, #8]
200052a4:	9a05      	ldr	r2, [sp, #20]
200052a6:	7013      	strb	r3, [r2, #0]
200052a8:	e5f5      	b.n	20004e96 <_dtoa_r+0x936>
200052aa:	9a11      	ldr	r2, [sp, #68]	; 0x44
200052ac:	2a00      	cmp	r2, #0
200052ae:	d055      	beq.n	2000535c <_dtoa_r+0xdfc>
200052b0:	f203 4333 	addw	r3, r3, #1075	; 0x433
200052b4:	9e0a      	ldr	r6, [sp, #40]	; 0x28
200052b6:	9d07      	ldr	r5, [sp, #28]
200052b8:	f7ff bba4 	b.w	20004a04 <_dtoa_r+0x4a4>
200052bc:	9c03      	ldr	r4, [sp, #12]
200052be:	f7ff bb52 	b.w	20004966 <_dtoa_r+0x406>
200052c2:	9b07      	ldr	r3, [sp, #28]
200052c4:	9a04      	ldr	r2, [sp, #16]
200052c6:	1a9d      	subs	r5, r3, r2
200052c8:	2300      	movs	r3, #0
200052ca:	f7ff bb9b 	b.w	20004a04 <_dtoa_r+0x4a4>
200052ce:	2b00      	cmp	r3, #0
200052d0:	9706      	str	r7, [sp, #24]
200052d2:	46e1      	mov	r9, ip
200052d4:	f8cd b020 	str.w	fp, [sp, #32]
200052d8:	9f0a      	ldr	r7, [sp, #40]	; 0x28
200052da:	4665      	mov	r5, ip
200052dc:	dd0f      	ble.n	200052fe <_dtoa_r+0xd9e>
200052de:	9906      	ldr	r1, [sp, #24]
200052e0:	2201      	movs	r2, #1
200052e2:	4620      	mov	r0, r4
200052e4:	f000 fe92 	bl	2000600c <__lshift>
200052e8:	4639      	mov	r1, r7
200052ea:	9006      	str	r0, [sp, #24]
200052ec:	f000 fee8 	bl	200060c0 <__mcmp>
200052f0:	2800      	cmp	r0, #0
200052f2:	dd75      	ble.n	200053e0 <_dtoa_r+0xe80>
200052f4:	f1b9 0f39 	cmp.w	r9, #57	; 0x39
200052f8:	d066      	beq.n	200053c8 <_dtoa_r+0xe68>
200052fa:	f109 0501 	add.w	r5, r9, #1
200052fe:	f8dd a020 	ldr.w	sl, [sp, #32]
20005302:	f888 5000 	strb.w	r5, [r8]
20005306:	f108 0501 	add.w	r5, r8, #1
2000530a:	9608      	str	r6, [sp, #32]
2000530c:	e5c3      	b.n	20004e96 <_dtoa_r+0x936>
2000530e:	9c12      	ldr	r4, [sp, #72]	; 0x48
20005310:	e531      	b.n	20004d76 <_dtoa_r+0x816>
20005312:	d104      	bne.n	2000531e <_dtoa_r+0xdbe>
20005314:	f019 0f01 	tst.w	r9, #1
20005318:	d001      	beq.n	2000531e <_dtoa_r+0xdbe>
2000531a:	e5ac      	b.n	20004e76 <_dtoa_r+0x916>
2000531c:	4615      	mov	r5, r2
2000531e:	f815 3c01 	ldrb.w	r3, [r5, #-1]
20005322:	1e6a      	subs	r2, r5, #1
20005324:	2b30      	cmp	r3, #48	; 0x30
20005326:	d0f9      	beq.n	2000531c <_dtoa_r+0xdbc>
20005328:	e5b5      	b.n	20004e96 <_dtoa_r+0x936>
2000532a:	2700      	movs	r7, #0
2000532c:	9708      	str	r7, [sp, #32]
2000532e:	f7ff ba4c 	b.w	200047ca <_dtoa_r+0x26a>
20005332:	9e08      	ldr	r6, [sp, #32]
20005334:	4620      	mov	r0, r4
20005336:	6871      	ldr	r1, [r6, #4]
20005338:	f000 fc5a 	bl	20005bf0 <_Balloc>
2000533c:	6933      	ldr	r3, [r6, #16]
2000533e:	4605      	mov	r5, r0
20005340:	f106 010c 	add.w	r1, r6, #12
20005344:	1c9a      	adds	r2, r3, #2
20005346:	300c      	adds	r0, #12
20005348:	0092      	lsls	r2, r2, #2
2000534a:	f000 fbb3 	bl	20005ab4 <memcpy>
2000534e:	4620      	mov	r0, r4
20005350:	4629      	mov	r1, r5
20005352:	2201      	movs	r2, #1
20005354:	f000 fe5a 	bl	2000600c <__lshift>
20005358:	4606      	mov	r6, r0
2000535a:	e6b8      	b.n	200050ce <_dtoa_r+0xb6e>
2000535c:	9b16      	ldr	r3, [sp, #88]	; 0x58
2000535e:	9e0a      	ldr	r6, [sp, #40]	; 0x28
20005360:	9d07      	ldr	r5, [sp, #28]
20005362:	f1c3 0336 	rsb	r3, r3, #54	; 0x36
20005366:	f7ff bb4d 	b.w	20004a04 <_dtoa_r+0x4a4>
2000536a:	f1bc 0f39 	cmp.w	ip, #57	; 0x39
2000536e:	9706      	str	r7, [sp, #24]
20005370:	f8cd b020 	str.w	fp, [sp, #32]
20005374:	9f0a      	ldr	r7, [sp, #40]	; 0x28
20005376:	d030      	beq.n	200053da <_dtoa_r+0xe7a>
20005378:	f10c 0901 	add.w	r9, ip, #1
2000537c:	f8dd a020 	ldr.w	sl, [sp, #32]
20005380:	f108 0501 	add.w	r5, r8, #1
20005384:	9608      	str	r6, [sp, #32]
20005386:	f888 9000 	strb.w	r9, [r8]
2000538a:	e584      	b.n	20004e96 <_dtoa_r+0x936>
2000538c:	9706      	str	r7, [sp, #24]
2000538e:	46e1      	mov	r9, ip
20005390:	46da      	mov	sl, fp
20005392:	9f0a      	ldr	r7, [sp, #40]	; 0x28
20005394:	9608      	str	r6, [sp, #32]
20005396:	e562      	b.n	20004e5e <_dtoa_r+0x8fe>
20005398:	46b0      	mov	r8, r6
2000539a:	e504      	b.n	20004da6 <_dtoa_r+0x846>
2000539c:	f1bc 0f39 	cmp.w	ip, #57	; 0x39
200053a0:	9706      	str	r7, [sp, #24]
200053a2:	f8cd b020 	str.w	fp, [sp, #32]
200053a6:	4655      	mov	r5, sl
200053a8:	46cb      	mov	fp, r9
200053aa:	9f0a      	ldr	r7, [sp, #40]	; 0x28
200053ac:	46e1      	mov	r9, ip
200053ae:	d00b      	beq.n	200053c8 <_dtoa_r+0xe68>
200053b0:	2d00      	cmp	r5, #0
200053b2:	dd01      	ble.n	200053b8 <_dtoa_r+0xe58>
200053b4:	f10b 0931 	add.w	r9, fp, #49	; 0x31
200053b8:	f8dd a020 	ldr.w	sl, [sp, #32]
200053bc:	f108 0501 	add.w	r5, r8, #1
200053c0:	f888 9000 	strb.w	r9, [r8]
200053c4:	9608      	str	r6, [sp, #32]
200053c6:	e566      	b.n	20004e96 <_dtoa_r+0x936>
200053c8:	f8dd a020 	ldr.w	sl, [sp, #32]
200053cc:	9608      	str	r6, [sp, #32]
200053ce:	2239      	movs	r2, #57	; 0x39
200053d0:	f108 0501 	add.w	r5, r8, #1
200053d4:	f888 2000 	strb.w	r2, [r8]
200053d8:	e54f      	b.n	20004e7a <_dtoa_r+0x91a>
200053da:	46da      	mov	sl, fp
200053dc:	9608      	str	r6, [sp, #32]
200053de:	e7f6      	b.n	200053ce <_dtoa_r+0xe6e>
200053e0:	d18d      	bne.n	200052fe <_dtoa_r+0xd9e>
200053e2:	f019 0f01 	tst.w	r9, #1
200053e6:	d08a      	beq.n	200052fe <_dtoa_r+0xd9e>
200053e8:	e784      	b.n	200052f4 <_dtoa_r+0xd94>
200053ea:	f04f 0802 	mov.w	r8, #2
200053ee:	f7ff bb72 	b.w	20004ad6 <_dtoa_r+0x576>
200053f2:	9b10      	ldr	r3, [sp, #64]	; 0x40
200053f4:	9304      	str	r3, [sp, #16]
200053f6:	e50e      	b.n	20004e16 <_dtoa_r+0x8b6>
200053f8:	f43f aa75 	beq.w	200048e6 <_dtoa_r+0x386>
200053fc:	f1c0 003c 	rsb	r0, r0, #60	; 0x3c
20005400:	f7ff ba6a 	b.w	200048d8 <_dtoa_r+0x378>
20005404:	2501      	movs	r5, #1
20005406:	f7ff b982 	b.w	2000470e <_dtoa_r+0x1ae>
2000540a:	2500      	movs	r5, #0
2000540c:	4620      	mov	r0, r4
2000540e:	6465      	str	r5, [r4, #68]	; 0x44
20005410:	4629      	mov	r1, r5
20005412:	f000 fbed 	bl	20005bf0 <_Balloc>
20005416:	9a02      	ldr	r2, [sp, #8]
20005418:	9005      	str	r0, [sp, #20]
2000541a:	2a0e      	cmp	r2, #14
2000541c:	9b17      	ldr	r3, [sp, #92]	; 0x5c
2000541e:	6420      	str	r0, [r4, #64]	; 0x40
20005420:	f73f aad7 	bgt.w	200049d2 <_dtoa_r+0x472>
20005424:	42ab      	cmp	r3, r5
20005426:	f6ff aad4 	blt.w	200049d2 <_dtoa_r+0x472>
2000542a:	f04f 33ff 	mov.w	r3, #4294967295
2000542e:	9523      	str	r5, [sp, #140]	; 0x8c
20005430:	9304      	str	r3, [sp, #16]
20005432:	f7ff b9a9 	b.w	20004788 <_dtoa_r+0x228>
20005436:	2301      	movs	r3, #1
20005438:	930b      	str	r3, [sp, #44]	; 0x2c
2000543a:	f7ff bbfa 	b.w	20004c32 <_dtoa_r+0x6d2>
2000543e:	bf00      	nop
20005440:	40240000 	.word	0x40240000
20005444:	401c0000 	.word	0x401c0000

20005448 <_setlocale_r>:
20005448:	b1c2      	cbz	r2, 2000547c <_setlocale_r+0x34>
2000544a:	4610      	mov	r0, r2
2000544c:	490c      	ldr	r1, [pc, #48]	; (20005480 <_setlocale_r+0x38>)
2000544e:	b510      	push	{r4, lr}
20005450:	4614      	mov	r4, r2
20005452:	f001 f889 	bl	20006568 <strcmp>
20005456:	b908      	cbnz	r0, 2000545c <_setlocale_r+0x14>
20005458:	480a      	ldr	r0, [pc, #40]	; (20005484 <_setlocale_r+0x3c>)
2000545a:	bd10      	pop	{r4, pc}
2000545c:	4620      	mov	r0, r4
2000545e:	4909      	ldr	r1, [pc, #36]	; (20005484 <_setlocale_r+0x3c>)
20005460:	f001 f882 	bl	20006568 <strcmp>
20005464:	2800      	cmp	r0, #0
20005466:	d0f7      	beq.n	20005458 <_setlocale_r+0x10>
20005468:	4620      	mov	r0, r4
2000546a:	4907      	ldr	r1, [pc, #28]	; (20005488 <_setlocale_r+0x40>)
2000546c:	f001 f87c 	bl	20006568 <strcmp>
20005470:	4b04      	ldr	r3, [pc, #16]	; (20005484 <_setlocale_r+0x3c>)
20005472:	2800      	cmp	r0, #0
20005474:	bf0c      	ite	eq
20005476:	4618      	moveq	r0, r3
20005478:	2000      	movne	r0, #0
2000547a:	bd10      	pop	{r4, pc}
2000547c:	4801      	ldr	r0, [pc, #4]	; (20005484 <_setlocale_r+0x3c>)
2000547e:	4770      	bx	lr
20005480:	200090d8 	.word	0x200090d8
20005484:	200090d0 	.word	0x200090d0
20005488:	2000909c 	.word	0x2000909c

2000548c <__locale_charset>:
2000548c:	4800      	ldr	r0, [pc, #0]	; (20005490 <__locale_charset+0x4>)
2000548e:	4770      	bx	lr
20005490:	2000974c 	.word	0x2000974c

20005494 <__locale_mb_cur_max>:
20005494:	4b01      	ldr	r3, [pc, #4]	; (2000549c <__locale_mb_cur_max+0x8>)
20005496:	6818      	ldr	r0, [r3, #0]
20005498:	4770      	bx	lr
2000549a:	bf00      	nop
2000549c:	2000976c 	.word	0x2000976c

200054a0 <__locale_msgcharset>:
200054a0:	4800      	ldr	r0, [pc, #0]	; (200054a4 <__locale_msgcharset+0x4>)
200054a2:	4770      	bx	lr
200054a4:	200096f4 	.word	0x200096f4

200054a8 <__locale_cjk_lang>:
200054a8:	2000      	movs	r0, #0
200054aa:	4770      	bx	lr

200054ac <_localeconv_r>:
200054ac:	4800      	ldr	r0, [pc, #0]	; (200054b0 <_localeconv_r+0x4>)
200054ae:	4770      	bx	lr
200054b0:	20009714 	.word	0x20009714

200054b4 <setlocale>:
200054b4:	4b02      	ldr	r3, [pc, #8]	; (200054c0 <setlocale+0xc>)
200054b6:	460a      	mov	r2, r1
200054b8:	4601      	mov	r1, r0
200054ba:	6818      	ldr	r0, [r3, #0]
200054bc:	f7ff bfc4 	b.w	20005448 <_setlocale_r>
200054c0:	200096f0 	.word	0x200096f0

200054c4 <localeconv>:
200054c4:	4800      	ldr	r0, [pc, #0]	; (200054c8 <localeconv+0x4>)
200054c6:	4770      	bx	lr
200054c8:	20009714 	.word	0x20009714

200054cc <_malloc_r>:
200054cc:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
200054d0:	f101 040b 	add.w	r4, r1, #11
200054d4:	b083      	sub	sp, #12
200054d6:	4605      	mov	r5, r0
200054d8:	2c16      	cmp	r4, #22
200054da:	d927      	bls.n	2000552c <_malloc_r+0x60>
200054dc:	f034 0407 	bics.w	r4, r4, #7
200054e0:	d42a      	bmi.n	20005538 <_malloc_r+0x6c>
200054e2:	42a1      	cmp	r1, r4
200054e4:	d828      	bhi.n	20005538 <_malloc_r+0x6c>
200054e6:	f000 fb7f 	bl	20005be8 <__malloc_lock>
200054ea:	f5b4 7ffc 	cmp.w	r4, #504	; 0x1f8
200054ee:	d22b      	bcs.n	20005548 <_malloc_r+0x7c>
200054f0:	ea4f 0cd4 	mov.w	ip, r4, lsr #3
200054f4:	4ec0      	ldr	r6, [pc, #768]	; (200057f8 <_malloc_r+0x32c>)
200054f6:	eb06 02cc 	add.w	r2, r6, ip, lsl #3
200054fa:	68d3      	ldr	r3, [r2, #12]
200054fc:	4293      	cmp	r3, r2
200054fe:	f000 8171 	beq.w	200057e4 <_malloc_r+0x318>
20005502:	685a      	ldr	r2, [r3, #4]
20005504:	f103 0808 	add.w	r8, r3, #8
20005508:	68d9      	ldr	r1, [r3, #12]
2000550a:	4628      	mov	r0, r5
2000550c:	f022 0203 	bic.w	r2, r2, #3
20005510:	689c      	ldr	r4, [r3, #8]
20005512:	4413      	add	r3, r2
20005514:	60e1      	str	r1, [r4, #12]
20005516:	608c      	str	r4, [r1, #8]
20005518:	685a      	ldr	r2, [r3, #4]
2000551a:	f042 0201 	orr.w	r2, r2, #1
2000551e:	605a      	str	r2, [r3, #4]
20005520:	f000 fb64 	bl	20005bec <__malloc_unlock>
20005524:	4640      	mov	r0, r8
20005526:	b003      	add	sp, #12
20005528:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
2000552c:	2910      	cmp	r1, #16
2000552e:	d803      	bhi.n	20005538 <_malloc_r+0x6c>
20005530:	f000 fb5a 	bl	20005be8 <__malloc_lock>
20005534:	2410      	movs	r4, #16
20005536:	e7db      	b.n	200054f0 <_malloc_r+0x24>
20005538:	f04f 0800 	mov.w	r8, #0
2000553c:	230c      	movs	r3, #12
2000553e:	4640      	mov	r0, r8
20005540:	602b      	str	r3, [r5, #0]
20005542:	b003      	add	sp, #12
20005544:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20005548:	ea5f 2c54 	movs.w	ip, r4, lsr #9
2000554c:	f000 8089 	beq.w	20005662 <_malloc_r+0x196>
20005550:	f1bc 0f04 	cmp.w	ip, #4
20005554:	f200 80c4 	bhi.w	200056e0 <_malloc_r+0x214>
20005558:	ea4f 1c94 	mov.w	ip, r4, lsr #6
2000555c:	f10c 0c38 	add.w	ip, ip, #56	; 0x38
20005560:	ea4f 004c 	mov.w	r0, ip, lsl #1
20005564:	4ea4      	ldr	r6, [pc, #656]	; (200057f8 <_malloc_r+0x32c>)
20005566:	eb06 0080 	add.w	r0, r6, r0, lsl #2
2000556a:	68c3      	ldr	r3, [r0, #12]
2000556c:	4298      	cmp	r0, r3
2000556e:	d105      	bne.n	2000557c <_malloc_r+0xb0>
20005570:	e00c      	b.n	2000558c <_malloc_r+0xc0>
20005572:	2900      	cmp	r1, #0
20005574:	da79      	bge.n	2000566a <_malloc_r+0x19e>
20005576:	68db      	ldr	r3, [r3, #12]
20005578:	4298      	cmp	r0, r3
2000557a:	d007      	beq.n	2000558c <_malloc_r+0xc0>
2000557c:	685a      	ldr	r2, [r3, #4]
2000557e:	f022 0203 	bic.w	r2, r2, #3
20005582:	1b11      	subs	r1, r2, r4
20005584:	290f      	cmp	r1, #15
20005586:	ddf4      	ble.n	20005572 <_malloc_r+0xa6>
20005588:	f10c 3cff 	add.w	ip, ip, #4294967295
2000558c:	f10c 0c01 	add.w	ip, ip, #1
20005590:	4f99      	ldr	r7, [pc, #612]	; (200057f8 <_malloc_r+0x32c>)
20005592:	6933      	ldr	r3, [r6, #16]
20005594:	f107 0e08 	add.w	lr, r7, #8
20005598:	4573      	cmp	r3, lr
2000559a:	f000 8119 	beq.w	200057d0 <_malloc_r+0x304>
2000559e:	685a      	ldr	r2, [r3, #4]
200055a0:	f022 0203 	bic.w	r2, r2, #3
200055a4:	1b11      	subs	r1, r2, r4
200055a6:	290f      	cmp	r1, #15
200055a8:	f300 80ca 	bgt.w	20005740 <_malloc_r+0x274>
200055ac:	2900      	cmp	r1, #0
200055ae:	f8c7 e014 	str.w	lr, [r7, #20]
200055b2:	f8c7 e010 	str.w	lr, [r7, #16]
200055b6:	da6a      	bge.n	2000568e <_malloc_r+0x1c2>
200055b8:	f5b2 7f00 	cmp.w	r2, #512	; 0x200
200055bc:	f080 809f 	bcs.w	200056fe <_malloc_r+0x232>
200055c0:	08d2      	lsrs	r2, r2, #3
200055c2:	2001      	movs	r0, #1
200055c4:	6879      	ldr	r1, [r7, #4]
200055c6:	ea4f 08a2 	mov.w	r8, r2, asr #2
200055ca:	eb07 02c2 	add.w	r2, r7, r2, lsl #3
200055ce:	fa00 f008 	lsl.w	r0, r0, r8
200055d2:	f8d2 8008 	ldr.w	r8, [r2, #8]
200055d6:	4308      	orrs	r0, r1
200055d8:	60da      	str	r2, [r3, #12]
200055da:	f8c3 8008 	str.w	r8, [r3, #8]
200055de:	6078      	str	r0, [r7, #4]
200055e0:	6093      	str	r3, [r2, #8]
200055e2:	f8c8 300c 	str.w	r3, [r8, #12]
200055e6:	ea4f 03ac 	mov.w	r3, ip, asr #2
200055ea:	2101      	movs	r1, #1
200055ec:	4099      	lsls	r1, r3
200055ee:	4281      	cmp	r1, r0
200055f0:	d85b      	bhi.n	200056aa <_malloc_r+0x1de>
200055f2:	4208      	tst	r0, r1
200055f4:	d106      	bne.n	20005604 <_malloc_r+0x138>
200055f6:	f02c 0c03 	bic.w	ip, ip, #3
200055fa:	0049      	lsls	r1, r1, #1
200055fc:	f10c 0c04 	add.w	ip, ip, #4
20005600:	4208      	tst	r0, r1
20005602:	d0fa      	beq.n	200055fa <_malloc_r+0x12e>
20005604:	eb06 08cc 	add.w	r8, r6, ip, lsl #3
20005608:	46e1      	mov	r9, ip
2000560a:	4640      	mov	r0, r8
2000560c:	68c7      	ldr	r7, [r0, #12]
2000560e:	42b8      	cmp	r0, r7
20005610:	d107      	bne.n	20005622 <_malloc_r+0x156>
20005612:	e0df      	b.n	200057d4 <_malloc_r+0x308>
20005614:	2a00      	cmp	r2, #0
20005616:	f280 80f7 	bge.w	20005808 <_malloc_r+0x33c>
2000561a:	68ff      	ldr	r7, [r7, #12]
2000561c:	42b8      	cmp	r0, r7
2000561e:	f000 80d9 	beq.w	200057d4 <_malloc_r+0x308>
20005622:	687b      	ldr	r3, [r7, #4]
20005624:	f023 0303 	bic.w	r3, r3, #3
20005628:	1b1a      	subs	r2, r3, r4
2000562a:	2a0f      	cmp	r2, #15
2000562c:	ddf2      	ble.n	20005614 <_malloc_r+0x148>
2000562e:	46b8      	mov	r8, r7
20005630:	68fb      	ldr	r3, [r7, #12]
20005632:	f044 0901 	orr.w	r9, r4, #1
20005636:	f042 0c01 	orr.w	ip, r2, #1
2000563a:	f858 1f08 	ldr.w	r1, [r8, #8]!
2000563e:	443c      	add	r4, r7
20005640:	f8c7 9004 	str.w	r9, [r7, #4]
20005644:	4628      	mov	r0, r5
20005646:	60cb      	str	r3, [r1, #12]
20005648:	6099      	str	r1, [r3, #8]
2000564a:	6174      	str	r4, [r6, #20]
2000564c:	6134      	str	r4, [r6, #16]
2000564e:	f8c4 e00c 	str.w	lr, [r4, #12]
20005652:	f8c4 e008 	str.w	lr, [r4, #8]
20005656:	f8c4 c004 	str.w	ip, [r4, #4]
2000565a:	50a2      	str	r2, [r4, r2]
2000565c:	f000 fac6 	bl	20005bec <__malloc_unlock>
20005660:	e760      	b.n	20005524 <_malloc_r+0x58>
20005662:	207e      	movs	r0, #126	; 0x7e
20005664:	f04f 0c3f 	mov.w	ip, #63	; 0x3f
20005668:	e77c      	b.n	20005564 <_malloc_r+0x98>
2000566a:	441a      	add	r2, r3
2000566c:	68dc      	ldr	r4, [r3, #12]
2000566e:	689e      	ldr	r6, [r3, #8]
20005670:	f103 0808 	add.w	r8, r3, #8
20005674:	6851      	ldr	r1, [r2, #4]
20005676:	4628      	mov	r0, r5
20005678:	60f4      	str	r4, [r6, #12]
2000567a:	f041 0301 	orr.w	r3, r1, #1
2000567e:	60a6      	str	r6, [r4, #8]
20005680:	6053      	str	r3, [r2, #4]
20005682:	f000 fab3 	bl	20005bec <__malloc_unlock>
20005686:	4640      	mov	r0, r8
20005688:	b003      	add	sp, #12
2000568a:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
2000568e:	441a      	add	r2, r3
20005690:	f103 0808 	add.w	r8, r3, #8
20005694:	4628      	mov	r0, r5
20005696:	6853      	ldr	r3, [r2, #4]
20005698:	f043 0301 	orr.w	r3, r3, #1
2000569c:	6053      	str	r3, [r2, #4]
2000569e:	f000 faa5 	bl	20005bec <__malloc_unlock>
200056a2:	4640      	mov	r0, r8
200056a4:	b003      	add	sp, #12
200056a6:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
200056aa:	68b7      	ldr	r7, [r6, #8]
200056ac:	687a      	ldr	r2, [r7, #4]
200056ae:	f022 0803 	bic.w	r8, r2, #3
200056b2:	ebc4 0208 	rsb	r2, r4, r8
200056b6:	2a0f      	cmp	r2, #15
200056b8:	dd56      	ble.n	20005768 <_malloc_r+0x29c>
200056ba:	45a0      	cmp	r8, r4
200056bc:	d354      	bcc.n	20005768 <_malloc_r+0x29c>
200056be:	f044 0301 	orr.w	r3, r4, #1
200056c2:	f042 0201 	orr.w	r2, r2, #1
200056c6:	443c      	add	r4, r7
200056c8:	f107 0808 	add.w	r8, r7, #8
200056cc:	607b      	str	r3, [r7, #4]
200056ce:	4628      	mov	r0, r5
200056d0:	60b4      	str	r4, [r6, #8]
200056d2:	6062      	str	r2, [r4, #4]
200056d4:	f000 fa8a 	bl	20005bec <__malloc_unlock>
200056d8:	4640      	mov	r0, r8
200056da:	b003      	add	sp, #12
200056dc:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
200056e0:	f1bc 0f14 	cmp.w	ip, #20
200056e4:	f240 809f 	bls.w	20005826 <_malloc_r+0x35a>
200056e8:	f1bc 0f54 	cmp.w	ip, #84	; 0x54
200056ec:	f200 8127 	bhi.w	2000593e <_malloc_r+0x472>
200056f0:	ea4f 3c14 	mov.w	ip, r4, lsr #12
200056f4:	f10c 0c6e 	add.w	ip, ip, #110	; 0x6e
200056f8:	ea4f 004c 	mov.w	r0, ip, lsl #1
200056fc:	e732      	b.n	20005564 <_malloc_r+0x98>
200056fe:	0a51      	lsrs	r1, r2, #9
20005700:	2904      	cmp	r1, #4
20005702:	f240 8095 	bls.w	20005830 <_malloc_r+0x364>
20005706:	2914      	cmp	r1, #20
20005708:	f200 8142 	bhi.w	20005990 <_malloc_r+0x4c4>
2000570c:	f101 075b 	add.w	r7, r1, #91	; 0x5b
20005710:	0078      	lsls	r0, r7, #1
20005712:	eb06 0080 	add.w	r0, r6, r0, lsl #2
20005716:	f8df 90e0 	ldr.w	r9, [pc, #224]	; 200057f8 <_malloc_r+0x32c>
2000571a:	6881      	ldr	r1, [r0, #8]
2000571c:	4281      	cmp	r1, r0
2000571e:	f000 811b 	beq.w	20005958 <_malloc_r+0x48c>
20005722:	684f      	ldr	r7, [r1, #4]
20005724:	f027 0703 	bic.w	r7, r7, #3
20005728:	42ba      	cmp	r2, r7
2000572a:	d202      	bcs.n	20005732 <_malloc_r+0x266>
2000572c:	6889      	ldr	r1, [r1, #8]
2000572e:	4288      	cmp	r0, r1
20005730:	d1f7      	bne.n	20005722 <_malloc_r+0x256>
20005732:	68ca      	ldr	r2, [r1, #12]
20005734:	6870      	ldr	r0, [r6, #4]
20005736:	60da      	str	r2, [r3, #12]
20005738:	6099      	str	r1, [r3, #8]
2000573a:	6093      	str	r3, [r2, #8]
2000573c:	60cb      	str	r3, [r1, #12]
2000573e:	e752      	b.n	200055e6 <_malloc_r+0x11a>
20005740:	f044 0601 	orr.w	r6, r4, #1
20005744:	f041 0201 	orr.w	r2, r1, #1
20005748:	441c      	add	r4, r3
2000574a:	4628      	mov	r0, r5
2000574c:	605e      	str	r6, [r3, #4]
2000574e:	f103 0808 	add.w	r8, r3, #8
20005752:	617c      	str	r4, [r7, #20]
20005754:	613c      	str	r4, [r7, #16]
20005756:	f8c4 e00c 	str.w	lr, [r4, #12]
2000575a:	f8c4 e008 	str.w	lr, [r4, #8]
2000575e:	6062      	str	r2, [r4, #4]
20005760:	5061      	str	r1, [r4, r1]
20005762:	f000 fa43 	bl	20005bec <__malloc_unlock>
20005766:	e6dd      	b.n	20005524 <_malloc_r+0x58>
20005768:	4a24      	ldr	r2, [pc, #144]	; (200057fc <_malloc_r+0x330>)
2000576a:	4b25      	ldr	r3, [pc, #148]	; (20005800 <_malloc_r+0x334>)
2000576c:	6811      	ldr	r1, [r2, #0]
2000576e:	681b      	ldr	r3, [r3, #0]
20005770:	3101      	adds	r1, #1
20005772:	4423      	add	r3, r4
20005774:	f000 8109 	beq.w	2000598a <_malloc_r+0x4be>
20005778:	f241 010f 	movw	r1, #4111	; 0x100f
2000577c:	f8df 9084 	ldr.w	r9, [pc, #132]	; 20005804 <_malloc_r+0x338>
20005780:	4419      	add	r1, r3
20005782:	ea01 0909 	and.w	r9, r1, r9
20005786:	4628      	mov	r0, r5
20005788:	4649      	mov	r1, r9
2000578a:	9201      	str	r2, [sp, #4]
2000578c:	f000 feca 	bl	20006524 <_sbrk_r>
20005790:	f1b0 3fff 	cmp.w	r0, #4294967295
20005794:	4682      	mov	sl, r0
20005796:	9a01      	ldr	r2, [sp, #4]
20005798:	f000 8101 	beq.w	2000599e <_malloc_r+0x4d2>
2000579c:	1bbb      	subs	r3, r7, r6
2000579e:	eb07 0108 	add.w	r1, r7, r8
200057a2:	4815      	ldr	r0, [pc, #84]	; (200057f8 <_malloc_r+0x32c>)
200057a4:	bf18      	it	ne
200057a6:	2301      	movne	r3, #1
200057a8:	4551      	cmp	r1, sl
200057aa:	d967      	bls.n	2000587c <_malloc_r+0x3b0>
200057ac:	2b00      	cmp	r3, #0
200057ae:	d065      	beq.n	2000587c <_malloc_r+0x3b0>
200057b0:	6887      	ldr	r7, [r0, #8]
200057b2:	687b      	ldr	r3, [r7, #4]
200057b4:	f023 0303 	bic.w	r3, r3, #3
200057b8:	1b1a      	subs	r2, r3, r4
200057ba:	2a0f      	cmp	r2, #15
200057bc:	dd02      	ble.n	200057c4 <_malloc_r+0x2f8>
200057be:	429c      	cmp	r4, r3
200057c0:	f67f af7d 	bls.w	200056be <_malloc_r+0x1f2>
200057c4:	4628      	mov	r0, r5
200057c6:	f04f 0800 	mov.w	r8, #0
200057ca:	f000 fa0f 	bl	20005bec <__malloc_unlock>
200057ce:	e6a9      	b.n	20005524 <_malloc_r+0x58>
200057d0:	6878      	ldr	r0, [r7, #4]
200057d2:	e708      	b.n	200055e6 <_malloc_r+0x11a>
200057d4:	f109 0901 	add.w	r9, r9, #1
200057d8:	3008      	adds	r0, #8
200057da:	f019 0f03 	tst.w	r9, #3
200057de:	f47f af15 	bne.w	2000560c <_malloc_r+0x140>
200057e2:	e02f      	b.n	20005844 <_malloc_r+0x378>
200057e4:	f103 0208 	add.w	r2, r3, #8
200057e8:	695b      	ldr	r3, [r3, #20]
200057ea:	429a      	cmp	r2, r3
200057ec:	bf08      	it	eq
200057ee:	f10c 0c02 	addeq.w	ip, ip, #2
200057f2:	f43f aecd 	beq.w	20005590 <_malloc_r+0xc4>
200057f6:	e684      	b.n	20005502 <_malloc_r+0x36>
200057f8:	20009770 	.word	0x20009770
200057fc:	20009b7c 	.word	0x20009b7c
20005800:	20009c34 	.word	0x20009c34
20005804:	fffff000 	.word	0xfffff000
20005808:	18fa      	adds	r2, r7, r3
2000580a:	46b8      	mov	r8, r7
2000580c:	68f9      	ldr	r1, [r7, #12]
2000580e:	4628      	mov	r0, r5
20005810:	6853      	ldr	r3, [r2, #4]
20005812:	f858 4f08 	ldr.w	r4, [r8, #8]!
20005816:	f043 0301 	orr.w	r3, r3, #1
2000581a:	6053      	str	r3, [r2, #4]
2000581c:	60e1      	str	r1, [r4, #12]
2000581e:	608c      	str	r4, [r1, #8]
20005820:	f000 f9e4 	bl	20005bec <__malloc_unlock>
20005824:	e67e      	b.n	20005524 <_malloc_r+0x58>
20005826:	f10c 0c5b 	add.w	ip, ip, #91	; 0x5b
2000582a:	ea4f 004c 	mov.w	r0, ip, lsl #1
2000582e:	e699      	b.n	20005564 <_malloc_r+0x98>
20005830:	0991      	lsrs	r1, r2, #6
20005832:	f101 0738 	add.w	r7, r1, #56	; 0x38
20005836:	0078      	lsls	r0, r7, #1
20005838:	e76b      	b.n	20005712 <_malloc_r+0x246>
2000583a:	f8d8 8000 	ldr.w	r8, [r8]
2000583e:	4598      	cmp	r8, r3
20005840:	f040 80e6 	bne.w	20005a10 <_malloc_r+0x544>
20005844:	f01c 0f03 	tst.w	ip, #3
20005848:	f1a8 0308 	sub.w	r3, r8, #8
2000584c:	f10c 3cff 	add.w	ip, ip, #4294967295
20005850:	d1f3      	bne.n	2000583a <_malloc_r+0x36e>
20005852:	6873      	ldr	r3, [r6, #4]
20005854:	ea23 0301 	bic.w	r3, r3, r1
20005858:	6073      	str	r3, [r6, #4]
2000585a:	0049      	lsls	r1, r1, #1
2000585c:	4299      	cmp	r1, r3
2000585e:	f63f af24 	bhi.w	200056aa <_malloc_r+0x1de>
20005862:	2900      	cmp	r1, #0
20005864:	f43f af21 	beq.w	200056aa <_malloc_r+0x1de>
20005868:	420b      	tst	r3, r1
2000586a:	46cc      	mov	ip, r9
2000586c:	f47f aeca 	bne.w	20005604 <_malloc_r+0x138>
20005870:	0049      	lsls	r1, r1, #1
20005872:	f10c 0c04 	add.w	ip, ip, #4
20005876:	420b      	tst	r3, r1
20005878:	d0fa      	beq.n	20005870 <_malloc_r+0x3a4>
2000587a:	e6c3      	b.n	20005604 <_malloc_r+0x138>
2000587c:	f8df b19c 	ldr.w	fp, [pc, #412]	; 20005a1c <_malloc_r+0x550>
20005880:	4551      	cmp	r1, sl
20005882:	f8db 0000 	ldr.w	r0, [fp]
20005886:	4448      	add	r0, r9
20005888:	f8cb 0000 	str.w	r0, [fp]
2000588c:	f000 808c 	beq.w	200059a8 <_malloc_r+0x4dc>
20005890:	6812      	ldr	r2, [r2, #0]
20005892:	f8df e18c 	ldr.w	lr, [pc, #396]	; 20005a20 <_malloc_r+0x554>
20005896:	3201      	adds	r2, #1
20005898:	f000 80a3 	beq.w	200059e2 <_malloc_r+0x516>
2000589c:	ebc1 010a 	rsb	r1, r1, sl
200058a0:	4408      	add	r0, r1
200058a2:	f8cb 0000 	str.w	r0, [fp]
200058a6:	f01a 0107 	ands.w	r1, sl, #7
200058aa:	d052      	beq.n	20005952 <_malloc_r+0x486>
200058ac:	f1c1 0008 	rsb	r0, r1, #8
200058b0:	f241 0208 	movw	r2, #4104	; 0x1008
200058b4:	4482      	add	sl, r0
200058b6:	1a52      	subs	r2, r2, r1
200058b8:	eb0a 0109 	add.w	r1, sl, r9
200058bc:	4628      	mov	r0, r5
200058be:	9301      	str	r3, [sp, #4]
200058c0:	f3c1 010b 	ubfx	r1, r1, #0, #12
200058c4:	ebc1 0902 	rsb	r9, r1, r2
200058c8:	4649      	mov	r1, r9
200058ca:	f000 fe2b 	bl	20006524 <_sbrk_r>
200058ce:	1c43      	adds	r3, r0, #1
200058d0:	9b01      	ldr	r3, [sp, #4]
200058d2:	d07a      	beq.n	200059ca <_malloc_r+0x4fe>
200058d4:	ebca 0200 	rsb	r2, sl, r0
200058d8:	444a      	add	r2, r9
200058da:	f042 0201 	orr.w	r2, r2, #1
200058de:	f8db 0000 	ldr.w	r0, [fp]
200058e2:	f8c6 a008 	str.w	sl, [r6, #8]
200058e6:	4448      	add	r0, r9
200058e8:	f8ca 2004 	str.w	r2, [sl, #4]
200058ec:	f8df 912c 	ldr.w	r9, [pc, #300]	; 20005a1c <_malloc_r+0x550>
200058f0:	f8cb 0000 	str.w	r0, [fp]
200058f4:	b1a3      	cbz	r3, 20005920 <_malloc_r+0x454>
200058f6:	f1b8 0f0f 	cmp.w	r8, #15
200058fa:	d937      	bls.n	2000596c <_malloc_r+0x4a0>
200058fc:	687b      	ldr	r3, [r7, #4]
200058fe:	f1a8 020c 	sub.w	r2, r8, #12
20005902:	f04f 0e05 	mov.w	lr, #5
20005906:	f022 0207 	bic.w	r2, r2, #7
2000590a:	f003 0301 	and.w	r3, r3, #1
2000590e:	18b9      	adds	r1, r7, r2
20005910:	4313      	orrs	r3, r2
20005912:	2a0f      	cmp	r2, #15
20005914:	607b      	str	r3, [r7, #4]
20005916:	f8c1 e004 	str.w	lr, [r1, #4]
2000591a:	f8c1 e008 	str.w	lr, [r1, #8]
2000591e:	d858      	bhi.n	200059d2 <_malloc_r+0x506>
20005920:	4b3c      	ldr	r3, [pc, #240]	; (20005a14 <_malloc_r+0x548>)
20005922:	681a      	ldr	r2, [r3, #0]
20005924:	4290      	cmp	r0, r2
20005926:	d900      	bls.n	2000592a <_malloc_r+0x45e>
20005928:	6018      	str	r0, [r3, #0]
2000592a:	4b3b      	ldr	r3, [pc, #236]	; (20005a18 <_malloc_r+0x54c>)
2000592c:	68b7      	ldr	r7, [r6, #8]
2000592e:	681a      	ldr	r2, [r3, #0]
20005930:	4290      	cmp	r0, r2
20005932:	d900      	bls.n	20005936 <_malloc_r+0x46a>
20005934:	6018      	str	r0, [r3, #0]
20005936:	687b      	ldr	r3, [r7, #4]
20005938:	f023 0303 	bic.w	r3, r3, #3
2000593c:	e73c      	b.n	200057b8 <_malloc_r+0x2ec>
2000593e:	f5bc 7faa 	cmp.w	ip, #340	; 0x154
20005942:	d817      	bhi.n	20005974 <_malloc_r+0x4a8>
20005944:	ea4f 3cd4 	mov.w	ip, r4, lsr #15
20005948:	f10c 0c77 	add.w	ip, ip, #119	; 0x77
2000594c:	ea4f 004c 	mov.w	r0, ip, lsl #1
20005950:	e608      	b.n	20005564 <_malloc_r+0x98>
20005952:	f44f 5280 	mov.w	r2, #4096	; 0x1000
20005956:	e7af      	b.n	200058b8 <_malloc_r+0x3ec>
20005958:	10bf      	asrs	r7, r7, #2
2000595a:	2001      	movs	r0, #1
2000595c:	460a      	mov	r2, r1
2000595e:	40b8      	lsls	r0, r7
20005960:	f8d9 7004 	ldr.w	r7, [r9, #4]
20005964:	4338      	orrs	r0, r7
20005966:	f8c9 0004 	str.w	r0, [r9, #4]
2000596a:	e6e4      	b.n	20005736 <_malloc_r+0x26a>
2000596c:	2301      	movs	r3, #1
2000596e:	f8ca 3004 	str.w	r3, [sl, #4]
20005972:	e727      	b.n	200057c4 <_malloc_r+0x2f8>
20005974:	f240 5354 	movw	r3, #1364	; 0x554
20005978:	459c      	cmp	ip, r3
2000597a:	d822      	bhi.n	200059c2 <_malloc_r+0x4f6>
2000597c:	ea4f 4c94 	mov.w	ip, r4, lsr #18
20005980:	f10c 0c7c 	add.w	ip, ip, #124	; 0x7c
20005984:	ea4f 004c 	mov.w	r0, ip, lsl #1
20005988:	e5ec      	b.n	20005564 <_malloc_r+0x98>
2000598a:	f103 0910 	add.w	r9, r3, #16
2000598e:	e6fa      	b.n	20005786 <_malloc_r+0x2ba>
20005990:	2954      	cmp	r1, #84	; 0x54
20005992:	d829      	bhi.n	200059e8 <_malloc_r+0x51c>
20005994:	0b11      	lsrs	r1, r2, #12
20005996:	f101 076e 	add.w	r7, r1, #110	; 0x6e
2000599a:	0078      	lsls	r0, r7, #1
2000599c:	e6b9      	b.n	20005712 <_malloc_r+0x246>
2000599e:	68b7      	ldr	r7, [r6, #8]
200059a0:	687b      	ldr	r3, [r7, #4]
200059a2:	f023 0303 	bic.w	r3, r3, #3
200059a6:	e707      	b.n	200057b8 <_malloc_r+0x2ec>
200059a8:	f3c1 0e0b 	ubfx	lr, r1, #0, #12
200059ac:	f1be 0f00 	cmp.w	lr, #0
200059b0:	f47f af6e 	bne.w	20005890 <_malloc_r+0x3c4>
200059b4:	eb09 0308 	add.w	r3, r9, r8
200059b8:	68b2      	ldr	r2, [r6, #8]
200059ba:	f043 0301 	orr.w	r3, r3, #1
200059be:	6053      	str	r3, [r2, #4]
200059c0:	e7ae      	b.n	20005920 <_malloc_r+0x454>
200059c2:	20fc      	movs	r0, #252	; 0xfc
200059c4:	f04f 0c7e 	mov.w	ip, #126	; 0x7e
200059c8:	e5cc      	b.n	20005564 <_malloc_r+0x98>
200059ca:	2201      	movs	r2, #1
200059cc:	f04f 0900 	mov.w	r9, #0
200059d0:	e785      	b.n	200058de <_malloc_r+0x412>
200059d2:	4628      	mov	r0, r5
200059d4:	f107 0108 	add.w	r1, r7, #8
200059d8:	f001 fd40 	bl	2000745c <_free_r>
200059dc:	f8d9 0000 	ldr.w	r0, [r9]
200059e0:	e79e      	b.n	20005920 <_malloc_r+0x454>
200059e2:	f8ce a000 	str.w	sl, [lr]
200059e6:	e75e      	b.n	200058a6 <_malloc_r+0x3da>
200059e8:	f5b1 7faa 	cmp.w	r1, #340	; 0x154
200059ec:	d804      	bhi.n	200059f8 <_malloc_r+0x52c>
200059ee:	0bd1      	lsrs	r1, r2, #15
200059f0:	f101 0777 	add.w	r7, r1, #119	; 0x77
200059f4:	0078      	lsls	r0, r7, #1
200059f6:	e68c      	b.n	20005712 <_malloc_r+0x246>
200059f8:	f240 5054 	movw	r0, #1364	; 0x554
200059fc:	4281      	cmp	r1, r0
200059fe:	d804      	bhi.n	20005a0a <_malloc_r+0x53e>
20005a00:	0c91      	lsrs	r1, r2, #18
20005a02:	f101 077c 	add.w	r7, r1, #124	; 0x7c
20005a06:	0078      	lsls	r0, r7, #1
20005a08:	e683      	b.n	20005712 <_malloc_r+0x246>
20005a0a:	20fc      	movs	r0, #252	; 0xfc
20005a0c:	277e      	movs	r7, #126	; 0x7e
20005a0e:	e680      	b.n	20005712 <_malloc_r+0x246>
20005a10:	6873      	ldr	r3, [r6, #4]
20005a12:	e722      	b.n	2000585a <_malloc_r+0x38e>
20005a14:	20009c30 	.word	0x20009c30
20005a18:	20009c2c 	.word	0x20009c2c
20005a1c:	20009c38 	.word	0x20009c38
20005a20:	20009b7c 	.word	0x20009b7c

20005a24 <memchr>:
20005a24:	0783      	lsls	r3, r0, #30
20005a26:	b2c9      	uxtb	r1, r1
20005a28:	b470      	push	{r4, r5, r6}
20005a2a:	d03f      	beq.n	20005aac <memchr+0x88>
20005a2c:	1e54      	subs	r4, r2, #1
20005a2e:	2a00      	cmp	r2, #0
20005a30:	d03e      	beq.n	20005ab0 <memchr+0x8c>
20005a32:	7803      	ldrb	r3, [r0, #0]
20005a34:	428b      	cmp	r3, r1
20005a36:	bf18      	it	ne
20005a38:	1c43      	addne	r3, r0, #1
20005a3a:	d105      	bne.n	20005a48 <memchr+0x24>
20005a3c:	e01c      	b.n	20005a78 <memchr+0x54>
20005a3e:	b1ec      	cbz	r4, 20005a7c <memchr+0x58>
20005a40:	7802      	ldrb	r2, [r0, #0]
20005a42:	3c01      	subs	r4, #1
20005a44:	428a      	cmp	r2, r1
20005a46:	d017      	beq.n	20005a78 <memchr+0x54>
20005a48:	f013 0f03 	tst.w	r3, #3
20005a4c:	4618      	mov	r0, r3
20005a4e:	f103 0301 	add.w	r3, r3, #1
20005a52:	d1f4      	bne.n	20005a3e <memchr+0x1a>
20005a54:	2c03      	cmp	r4, #3
20005a56:	d814      	bhi.n	20005a82 <memchr+0x5e>
20005a58:	b184      	cbz	r4, 20005a7c <memchr+0x58>
20005a5a:	7803      	ldrb	r3, [r0, #0]
20005a5c:	428b      	cmp	r3, r1
20005a5e:	d00b      	beq.n	20005a78 <memchr+0x54>
20005a60:	1905      	adds	r5, r0, r4
20005a62:	1c43      	adds	r3, r0, #1
20005a64:	e002      	b.n	20005a6c <memchr+0x48>
20005a66:	7802      	ldrb	r2, [r0, #0]
20005a68:	428a      	cmp	r2, r1
20005a6a:	d005      	beq.n	20005a78 <memchr+0x54>
20005a6c:	42ab      	cmp	r3, r5
20005a6e:	4618      	mov	r0, r3
20005a70:	f103 0301 	add.w	r3, r3, #1
20005a74:	d1f7      	bne.n	20005a66 <memchr+0x42>
20005a76:	2000      	movs	r0, #0
20005a78:	bc70      	pop	{r4, r5, r6}
20005a7a:	4770      	bx	lr
20005a7c:	4620      	mov	r0, r4
20005a7e:	bc70      	pop	{r4, r5, r6}
20005a80:	4770      	bx	lr
20005a82:	ea41 2601 	orr.w	r6, r1, r1, lsl #8
20005a86:	4602      	mov	r2, r0
20005a88:	ea46 4606 	orr.w	r6, r6, r6, lsl #16
20005a8c:	4610      	mov	r0, r2
20005a8e:	3204      	adds	r2, #4
20005a90:	6803      	ldr	r3, [r0, #0]
20005a92:	4073      	eors	r3, r6
20005a94:	f1a3 3501 	sub.w	r5, r3, #16843009	; 0x1010101
20005a98:	ea25 0303 	bic.w	r3, r5, r3
20005a9c:	f013 3f80 	tst.w	r3, #2155905152	; 0x80808080
20005aa0:	d1da      	bne.n	20005a58 <memchr+0x34>
20005aa2:	3c04      	subs	r4, #4
20005aa4:	4610      	mov	r0, r2
20005aa6:	2c03      	cmp	r4, #3
20005aa8:	d8f0      	bhi.n	20005a8c <memchr+0x68>
20005aaa:	e7d5      	b.n	20005a58 <memchr+0x34>
20005aac:	4614      	mov	r4, r2
20005aae:	e7d1      	b.n	20005a54 <memchr+0x30>
20005ab0:	4610      	mov	r0, r2
20005ab2:	e7e1      	b.n	20005a78 <memchr+0x54>

20005ab4 <memcpy>:
20005ab4:	4684      	mov	ip, r0
20005ab6:	ea41 0300 	orr.w	r3, r1, r0
20005aba:	f013 0303 	ands.w	r3, r3, #3
20005abe:	d16d      	bne.n	20005b9c <memcpy+0xe8>
20005ac0:	3a40      	subs	r2, #64	; 0x40
20005ac2:	d341      	bcc.n	20005b48 <memcpy+0x94>
20005ac4:	f851 3b04 	ldr.w	r3, [r1], #4
20005ac8:	f840 3b04 	str.w	r3, [r0], #4
20005acc:	f851 3b04 	ldr.w	r3, [r1], #4
20005ad0:	f840 3b04 	str.w	r3, [r0], #4
20005ad4:	f851 3b04 	ldr.w	r3, [r1], #4
20005ad8:	f840 3b04 	str.w	r3, [r0], #4
20005adc:	f851 3b04 	ldr.w	r3, [r1], #4
20005ae0:	f840 3b04 	str.w	r3, [r0], #4
20005ae4:	f851 3b04 	ldr.w	r3, [r1], #4
20005ae8:	f840 3b04 	str.w	r3, [r0], #4
20005aec:	f851 3b04 	ldr.w	r3, [r1], #4
20005af0:	f840 3b04 	str.w	r3, [r0], #4
20005af4:	f851 3b04 	ldr.w	r3, [r1], #4
20005af8:	f840 3b04 	str.w	r3, [r0], #4
20005afc:	f851 3b04 	ldr.w	r3, [r1], #4
20005b00:	f840 3b04 	str.w	r3, [r0], #4
20005b04:	f851 3b04 	ldr.w	r3, [r1], #4
20005b08:	f840 3b04 	str.w	r3, [r0], #4
20005b0c:	f851 3b04 	ldr.w	r3, [r1], #4
20005b10:	f840 3b04 	str.w	r3, [r0], #4
20005b14:	f851 3b04 	ldr.w	r3, [r1], #4
20005b18:	f840 3b04 	str.w	r3, [r0], #4
20005b1c:	f851 3b04 	ldr.w	r3, [r1], #4
20005b20:	f840 3b04 	str.w	r3, [r0], #4
20005b24:	f851 3b04 	ldr.w	r3, [r1], #4
20005b28:	f840 3b04 	str.w	r3, [r0], #4
20005b2c:	f851 3b04 	ldr.w	r3, [r1], #4
20005b30:	f840 3b04 	str.w	r3, [r0], #4
20005b34:	f851 3b04 	ldr.w	r3, [r1], #4
20005b38:	f840 3b04 	str.w	r3, [r0], #4
20005b3c:	f851 3b04 	ldr.w	r3, [r1], #4
20005b40:	f840 3b04 	str.w	r3, [r0], #4
20005b44:	3a40      	subs	r2, #64	; 0x40
20005b46:	d2bd      	bcs.n	20005ac4 <memcpy+0x10>
20005b48:	3230      	adds	r2, #48	; 0x30
20005b4a:	d311      	bcc.n	20005b70 <memcpy+0xbc>
20005b4c:	f851 3b04 	ldr.w	r3, [r1], #4
20005b50:	f840 3b04 	str.w	r3, [r0], #4
20005b54:	f851 3b04 	ldr.w	r3, [r1], #4
20005b58:	f840 3b04 	str.w	r3, [r0], #4
20005b5c:	f851 3b04 	ldr.w	r3, [r1], #4
20005b60:	f840 3b04 	str.w	r3, [r0], #4
20005b64:	f851 3b04 	ldr.w	r3, [r1], #4
20005b68:	f840 3b04 	str.w	r3, [r0], #4
20005b6c:	3a10      	subs	r2, #16
20005b6e:	d2ed      	bcs.n	20005b4c <memcpy+0x98>
20005b70:	320c      	adds	r2, #12
20005b72:	d305      	bcc.n	20005b80 <memcpy+0xcc>
20005b74:	f851 3b04 	ldr.w	r3, [r1], #4
20005b78:	f840 3b04 	str.w	r3, [r0], #4
20005b7c:	3a04      	subs	r2, #4
20005b7e:	d2f9      	bcs.n	20005b74 <memcpy+0xc0>
20005b80:	3204      	adds	r2, #4
20005b82:	d008      	beq.n	20005b96 <memcpy+0xe2>
20005b84:	07d2      	lsls	r2, r2, #31
20005b86:	bf1c      	itt	ne
20005b88:	f811 3b01 	ldrbne.w	r3, [r1], #1
20005b8c:	f800 3b01 	strbne.w	r3, [r0], #1
20005b90:	d301      	bcc.n	20005b96 <memcpy+0xe2>
20005b92:	880b      	ldrh	r3, [r1, #0]
20005b94:	8003      	strh	r3, [r0, #0]
20005b96:	4660      	mov	r0, ip
20005b98:	4770      	bx	lr
20005b9a:	bf00      	nop
20005b9c:	2a08      	cmp	r2, #8
20005b9e:	d313      	bcc.n	20005bc8 <memcpy+0x114>
20005ba0:	078b      	lsls	r3, r1, #30
20005ba2:	d08d      	beq.n	20005ac0 <memcpy+0xc>
20005ba4:	f010 0303 	ands.w	r3, r0, #3
20005ba8:	d08a      	beq.n	20005ac0 <memcpy+0xc>
20005baa:	f1c3 0304 	rsb	r3, r3, #4
20005bae:	1ad2      	subs	r2, r2, r3
20005bb0:	07db      	lsls	r3, r3, #31
20005bb2:	bf1c      	itt	ne
20005bb4:	f811 3b01 	ldrbne.w	r3, [r1], #1
20005bb8:	f800 3b01 	strbne.w	r3, [r0], #1
20005bbc:	d380      	bcc.n	20005ac0 <memcpy+0xc>
20005bbe:	f831 3b02 	ldrh.w	r3, [r1], #2
20005bc2:	f820 3b02 	strh.w	r3, [r0], #2
20005bc6:	e77b      	b.n	20005ac0 <memcpy+0xc>
20005bc8:	3a04      	subs	r2, #4
20005bca:	d3d9      	bcc.n	20005b80 <memcpy+0xcc>
20005bcc:	3a01      	subs	r2, #1
20005bce:	f811 3b01 	ldrb.w	r3, [r1], #1
20005bd2:	f800 3b01 	strb.w	r3, [r0], #1
20005bd6:	d2f9      	bcs.n	20005bcc <memcpy+0x118>
20005bd8:	780b      	ldrb	r3, [r1, #0]
20005bda:	7003      	strb	r3, [r0, #0]
20005bdc:	784b      	ldrb	r3, [r1, #1]
20005bde:	7043      	strb	r3, [r0, #1]
20005be0:	788b      	ldrb	r3, [r1, #2]
20005be2:	7083      	strb	r3, [r0, #2]
20005be4:	4660      	mov	r0, ip
20005be6:	4770      	bx	lr

20005be8 <__malloc_lock>:
20005be8:	4770      	bx	lr
20005bea:	bf00      	nop

20005bec <__malloc_unlock>:
20005bec:	4770      	bx	lr
20005bee:	bf00      	nop

20005bf0 <_Balloc>:
20005bf0:	6cc2      	ldr	r2, [r0, #76]	; 0x4c
20005bf2:	b538      	push	{r3, r4, r5, lr}
20005bf4:	4605      	mov	r5, r0
20005bf6:	460c      	mov	r4, r1
20005bf8:	b152      	cbz	r2, 20005c10 <_Balloc+0x20>
20005bfa:	f852 3024 	ldr.w	r3, [r2, r4, lsl #2]
20005bfe:	b18b      	cbz	r3, 20005c24 <_Balloc+0x34>
20005c00:	6819      	ldr	r1, [r3, #0]
20005c02:	f842 1024 	str.w	r1, [r2, r4, lsl #2]
20005c06:	2200      	movs	r2, #0
20005c08:	4618      	mov	r0, r3
20005c0a:	611a      	str	r2, [r3, #16]
20005c0c:	60da      	str	r2, [r3, #12]
20005c0e:	bd38      	pop	{r3, r4, r5, pc}
20005c10:	2221      	movs	r2, #33	; 0x21
20005c12:	2104      	movs	r1, #4
20005c14:	f001 fba4 	bl	20007360 <_calloc_r>
20005c18:	64e8      	str	r0, [r5, #76]	; 0x4c
20005c1a:	4602      	mov	r2, r0
20005c1c:	2800      	cmp	r0, #0
20005c1e:	d1ec      	bne.n	20005bfa <_Balloc+0xa>
20005c20:	2000      	movs	r0, #0
20005c22:	bd38      	pop	{r3, r4, r5, pc}
20005c24:	2101      	movs	r1, #1
20005c26:	4628      	mov	r0, r5
20005c28:	fa01 f504 	lsl.w	r5, r1, r4
20005c2c:	1d6a      	adds	r2, r5, #5
20005c2e:	0092      	lsls	r2, r2, #2
20005c30:	f001 fb96 	bl	20007360 <_calloc_r>
20005c34:	4603      	mov	r3, r0
20005c36:	2800      	cmp	r0, #0
20005c38:	d0f2      	beq.n	20005c20 <_Balloc+0x30>
20005c3a:	6044      	str	r4, [r0, #4]
20005c3c:	6085      	str	r5, [r0, #8]
20005c3e:	e7e2      	b.n	20005c06 <_Balloc+0x16>

20005c40 <_Bfree>:
20005c40:	b131      	cbz	r1, 20005c50 <_Bfree+0x10>
20005c42:	6cc3      	ldr	r3, [r0, #76]	; 0x4c
20005c44:	684a      	ldr	r2, [r1, #4]
20005c46:	f853 0022 	ldr.w	r0, [r3, r2, lsl #2]
20005c4a:	6008      	str	r0, [r1, #0]
20005c4c:	f843 1022 	str.w	r1, [r3, r2, lsl #2]
20005c50:	4770      	bx	lr
20005c52:	bf00      	nop

20005c54 <__multadd>:
20005c54:	b5f0      	push	{r4, r5, r6, r7, lr}
20005c56:	460c      	mov	r4, r1
20005c58:	b083      	sub	sp, #12
20005c5a:	4605      	mov	r5, r0
20005c5c:	690e      	ldr	r6, [r1, #16]
20005c5e:	f101 0e14 	add.w	lr, r1, #20
20005c62:	2700      	movs	r7, #0
20005c64:	f8de 1000 	ldr.w	r1, [lr]
20005c68:	3701      	adds	r7, #1
20005c6a:	b288      	uxth	r0, r1
20005c6c:	42be      	cmp	r6, r7
20005c6e:	ea4f 4111 	mov.w	r1, r1, lsr #16
20005c72:	fb02 3300 	mla	r3, r2, r0, r3
20005c76:	fb02 f101 	mul.w	r1, r2, r1
20005c7a:	b298      	uxth	r0, r3
20005c7c:	eb01 4313 	add.w	r3, r1, r3, lsr #16
20005c80:	eb00 4103 	add.w	r1, r0, r3, lsl #16
20005c84:	ea4f 4313 	mov.w	r3, r3, lsr #16
20005c88:	f84e 1b04 	str.w	r1, [lr], #4
20005c8c:	dcea      	bgt.n	20005c64 <__multadd+0x10>
20005c8e:	b13b      	cbz	r3, 20005ca0 <__multadd+0x4c>
20005c90:	68a2      	ldr	r2, [r4, #8]
20005c92:	4296      	cmp	r6, r2
20005c94:	da07      	bge.n	20005ca6 <__multadd+0x52>
20005c96:	eb04 0286 	add.w	r2, r4, r6, lsl #2
20005c9a:	3601      	adds	r6, #1
20005c9c:	6153      	str	r3, [r2, #20]
20005c9e:	6126      	str	r6, [r4, #16]
20005ca0:	4620      	mov	r0, r4
20005ca2:	b003      	add	sp, #12
20005ca4:	bdf0      	pop	{r4, r5, r6, r7, pc}
20005ca6:	6861      	ldr	r1, [r4, #4]
20005ca8:	4628      	mov	r0, r5
20005caa:	9301      	str	r3, [sp, #4]
20005cac:	3101      	adds	r1, #1
20005cae:	f7ff ff9f 	bl	20005bf0 <_Balloc>
20005cb2:	6922      	ldr	r2, [r4, #16]
20005cb4:	f104 010c 	add.w	r1, r4, #12
20005cb8:	4607      	mov	r7, r0
20005cba:	3202      	adds	r2, #2
20005cbc:	300c      	adds	r0, #12
20005cbe:	0092      	lsls	r2, r2, #2
20005cc0:	f7ff fef8 	bl	20005ab4 <memcpy>
20005cc4:	6cea      	ldr	r2, [r5, #76]	; 0x4c
20005cc6:	6861      	ldr	r1, [r4, #4]
20005cc8:	9b01      	ldr	r3, [sp, #4]
20005cca:	f852 0021 	ldr.w	r0, [r2, r1, lsl #2]
20005cce:	6020      	str	r0, [r4, #0]
20005cd0:	f842 4021 	str.w	r4, [r2, r1, lsl #2]
20005cd4:	463c      	mov	r4, r7
20005cd6:	e7de      	b.n	20005c96 <__multadd+0x42>

20005cd8 <__s2b>:
20005cd8:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
20005cdc:	461f      	mov	r7, r3
20005cde:	4d22      	ldr	r5, [pc, #136]	; (20005d68 <__s2b+0x90>)
20005ce0:	3308      	adds	r3, #8
20005ce2:	460c      	mov	r4, r1
20005ce4:	4690      	mov	r8, r2
20005ce6:	4606      	mov	r6, r0
20005ce8:	fb85 1203 	smull	r1, r2, r5, r3
20005cec:	17db      	asrs	r3, r3, #31
20005cee:	ebc3 0262 	rsb	r2, r3, r2, asr #1
20005cf2:	2a01      	cmp	r2, #1
20005cf4:	dd35      	ble.n	20005d62 <__s2b+0x8a>
20005cf6:	2301      	movs	r3, #1
20005cf8:	2100      	movs	r1, #0
20005cfa:	005b      	lsls	r3, r3, #1
20005cfc:	3101      	adds	r1, #1
20005cfe:	429a      	cmp	r2, r3
20005d00:	dcfb      	bgt.n	20005cfa <__s2b+0x22>
20005d02:	4630      	mov	r0, r6
20005d04:	f7ff ff74 	bl	20005bf0 <_Balloc>
20005d08:	2301      	movs	r3, #1
20005d0a:	9a08      	ldr	r2, [sp, #32]
20005d0c:	f1b8 0f09 	cmp.w	r8, #9
20005d10:	6103      	str	r3, [r0, #16]
20005d12:	6142      	str	r2, [r0, #20]
20005d14:	dd21      	ble.n	20005d5a <__s2b+0x82>
20005d16:	f104 0909 	add.w	r9, r4, #9
20005d1a:	4444      	add	r4, r8
20005d1c:	464d      	mov	r5, r9
20005d1e:	f815 3b01 	ldrb.w	r3, [r5], #1
20005d22:	4601      	mov	r1, r0
20005d24:	220a      	movs	r2, #10
20005d26:	4630      	mov	r0, r6
20005d28:	3b30      	subs	r3, #48	; 0x30
20005d2a:	f7ff ff93 	bl	20005c54 <__multadd>
20005d2e:	42a5      	cmp	r5, r4
20005d30:	d1f5      	bne.n	20005d1e <__s2b+0x46>
20005d32:	eb09 0408 	add.w	r4, r9, r8
20005d36:	3c08      	subs	r4, #8
20005d38:	4547      	cmp	r7, r8
20005d3a:	dd0c      	ble.n	20005d56 <__s2b+0x7e>
20005d3c:	ebc8 0707 	rsb	r7, r8, r7
20005d40:	4427      	add	r7, r4
20005d42:	f814 3b01 	ldrb.w	r3, [r4], #1
20005d46:	4601      	mov	r1, r0
20005d48:	220a      	movs	r2, #10
20005d4a:	4630      	mov	r0, r6
20005d4c:	3b30      	subs	r3, #48	; 0x30
20005d4e:	f7ff ff81 	bl	20005c54 <__multadd>
20005d52:	42bc      	cmp	r4, r7
20005d54:	d1f5      	bne.n	20005d42 <__s2b+0x6a>
20005d56:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
20005d5a:	340a      	adds	r4, #10
20005d5c:	f04f 0809 	mov.w	r8, #9
20005d60:	e7ea      	b.n	20005d38 <__s2b+0x60>
20005d62:	2100      	movs	r1, #0
20005d64:	e7cd      	b.n	20005d02 <__s2b+0x2a>
20005d66:	bf00      	nop
20005d68:	38e38e39 	.word	0x38e38e39

20005d6c <__hi0bits>:
20005d6c:	4b0f      	ldr	r3, [pc, #60]	; (20005dac <__hi0bits+0x40>)
20005d6e:	4003      	ands	r3, r0
20005d70:	b9b3      	cbnz	r3, 20005da0 <__hi0bits+0x34>
20005d72:	0400      	lsls	r0, r0, #16
20005d74:	2310      	movs	r3, #16
20005d76:	f010 4f7f 	tst.w	r0, #4278190080	; 0xff000000
20005d7a:	d101      	bne.n	20005d80 <__hi0bits+0x14>
20005d7c:	3308      	adds	r3, #8
20005d7e:	0200      	lsls	r0, r0, #8
20005d80:	f010 4f70 	tst.w	r0, #4026531840	; 0xf0000000
20005d84:	d101      	bne.n	20005d8a <__hi0bits+0x1e>
20005d86:	3304      	adds	r3, #4
20005d88:	0100      	lsls	r0, r0, #4
20005d8a:	f010 4f40 	tst.w	r0, #3221225472	; 0xc0000000
20005d8e:	d101      	bne.n	20005d94 <__hi0bits+0x28>
20005d90:	3302      	adds	r3, #2
20005d92:	0080      	lsls	r0, r0, #2
20005d94:	2800      	cmp	r0, #0
20005d96:	db07      	blt.n	20005da8 <__hi0bits+0x3c>
20005d98:	0042      	lsls	r2, r0, #1
20005d9a:	d403      	bmi.n	20005da4 <__hi0bits+0x38>
20005d9c:	2020      	movs	r0, #32
20005d9e:	4770      	bx	lr
20005da0:	2300      	movs	r3, #0
20005da2:	e7e8      	b.n	20005d76 <__hi0bits+0xa>
20005da4:	1c58      	adds	r0, r3, #1
20005da6:	4770      	bx	lr
20005da8:	4618      	mov	r0, r3
20005daa:	4770      	bx	lr
20005dac:	ffff0000 	.word	0xffff0000

20005db0 <__lo0bits>:
20005db0:	6803      	ldr	r3, [r0, #0]
20005db2:	f013 0207 	ands.w	r2, r3, #7
20005db6:	d008      	beq.n	20005dca <__lo0bits+0x1a>
20005db8:	07d9      	lsls	r1, r3, #31
20005dba:	d422      	bmi.n	20005e02 <__lo0bits+0x52>
20005dbc:	079a      	lsls	r2, r3, #30
20005dbe:	d423      	bmi.n	20005e08 <__lo0bits+0x58>
20005dc0:	089b      	lsrs	r3, r3, #2
20005dc2:	2202      	movs	r2, #2
20005dc4:	6003      	str	r3, [r0, #0]
20005dc6:	4610      	mov	r0, r2
20005dc8:	4770      	bx	lr
20005dca:	b299      	uxth	r1, r3
20005dcc:	b909      	cbnz	r1, 20005dd2 <__lo0bits+0x22>
20005dce:	0c1b      	lsrs	r3, r3, #16
20005dd0:	2210      	movs	r2, #16
20005dd2:	f013 0fff 	tst.w	r3, #255	; 0xff
20005dd6:	d101      	bne.n	20005ddc <__lo0bits+0x2c>
20005dd8:	3208      	adds	r2, #8
20005dda:	0a1b      	lsrs	r3, r3, #8
20005ddc:	0719      	lsls	r1, r3, #28
20005dde:	d101      	bne.n	20005de4 <__lo0bits+0x34>
20005de0:	3204      	adds	r2, #4
20005de2:	091b      	lsrs	r3, r3, #4
20005de4:	0799      	lsls	r1, r3, #30
20005de6:	d101      	bne.n	20005dec <__lo0bits+0x3c>
20005de8:	3202      	adds	r2, #2
20005dea:	089b      	lsrs	r3, r3, #2
20005dec:	07d9      	lsls	r1, r3, #31
20005dee:	d405      	bmi.n	20005dfc <__lo0bits+0x4c>
20005df0:	085b      	lsrs	r3, r3, #1
20005df2:	d102      	bne.n	20005dfa <__lo0bits+0x4a>
20005df4:	2220      	movs	r2, #32
20005df6:	4610      	mov	r0, r2
20005df8:	4770      	bx	lr
20005dfa:	3201      	adds	r2, #1
20005dfc:	6003      	str	r3, [r0, #0]
20005dfe:	4610      	mov	r0, r2
20005e00:	4770      	bx	lr
20005e02:	2200      	movs	r2, #0
20005e04:	4610      	mov	r0, r2
20005e06:	4770      	bx	lr
20005e08:	085b      	lsrs	r3, r3, #1
20005e0a:	2201      	movs	r2, #1
20005e0c:	6003      	str	r3, [r0, #0]
20005e0e:	4610      	mov	r0, r2
20005e10:	4770      	bx	lr
20005e12:	bf00      	nop

20005e14 <__i2b>:
20005e14:	b510      	push	{r4, lr}
20005e16:	460c      	mov	r4, r1
20005e18:	2101      	movs	r1, #1
20005e1a:	f7ff fee9 	bl	20005bf0 <_Balloc>
20005e1e:	2201      	movs	r2, #1
20005e20:	6144      	str	r4, [r0, #20]
20005e22:	6102      	str	r2, [r0, #16]
20005e24:	bd10      	pop	{r4, pc}
20005e26:	bf00      	nop

20005e28 <__multiply>:
20005e28:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20005e2c:	690e      	ldr	r6, [r1, #16]
20005e2e:	b085      	sub	sp, #20
20005e30:	6915      	ldr	r5, [r2, #16]
20005e32:	460c      	mov	r4, r1
20005e34:	4691      	mov	r9, r2
20005e36:	42ae      	cmp	r6, r5
20005e38:	f2c0 8094 	blt.w	20005f64 <__multiply+0x13c>
20005e3c:	462b      	mov	r3, r5
20005e3e:	4635      	mov	r5, r6
20005e40:	461e      	mov	r6, r3
20005e42:	eb05 0806 	add.w	r8, r5, r6
20005e46:	68a3      	ldr	r3, [r4, #8]
20005e48:	6861      	ldr	r1, [r4, #4]
20005e4a:	4598      	cmp	r8, r3
20005e4c:	dd00      	ble.n	20005e50 <__multiply+0x28>
20005e4e:	3101      	adds	r1, #1
20005e50:	f7ff fece 	bl	20005bf0 <_Balloc>
20005e54:	f100 0a14 	add.w	sl, r0, #20
20005e58:	9001      	str	r0, [sp, #4]
20005e5a:	eb0a 0b88 	add.w	fp, sl, r8, lsl #2
20005e5e:	45da      	cmp	sl, fp
20005e60:	d205      	bcs.n	20005e6e <__multiply+0x46>
20005e62:	4653      	mov	r3, sl
20005e64:	2100      	movs	r1, #0
20005e66:	f843 1b04 	str.w	r1, [r3], #4
20005e6a:	459b      	cmp	fp, r3
20005e6c:	d8fb      	bhi.n	20005e66 <__multiply+0x3e>
20005e6e:	f109 0914 	add.w	r9, r9, #20
20005e72:	f104 0314 	add.w	r3, r4, #20
20005e76:	eb09 0286 	add.w	r2, r9, r6, lsl #2
20005e7a:	eb03 0c85 	add.w	ip, r3, r5, lsl #2
20005e7e:	4591      	cmp	r9, r2
20005e80:	d25b      	bcs.n	20005f3a <__multiply+0x112>
20005e82:	f8cd b008 	str.w	fp, [sp, #8]
20005e86:	4693      	mov	fp, r2
20005e88:	f8cd 800c 	str.w	r8, [sp, #12]
20005e8c:	4698      	mov	r8, r3
20005e8e:	f859 6b04 	ldr.w	r6, [r9], #4
20005e92:	fa1f fe86 	uxth.w	lr, r6
20005e96:	f1be 0f00 	cmp.w	lr, #0
20005e9a:	d021      	beq.n	20005ee0 <__multiply+0xb8>
20005e9c:	4647      	mov	r7, r8
20005e9e:	4656      	mov	r6, sl
20005ea0:	2100      	movs	r1, #0
20005ea2:	e000      	b.n	20005ea6 <__multiply+0x7e>
20005ea4:	4626      	mov	r6, r4
20005ea6:	f857 2b04 	ldr.w	r2, [r7], #4
20005eaa:	4634      	mov	r4, r6
20005eac:	6835      	ldr	r5, [r6, #0]
20005eae:	b290      	uxth	r0, r2
20005eb0:	45bc      	cmp	ip, r7
20005eb2:	b2ab      	uxth	r3, r5
20005eb4:	ea4f 4212 	mov.w	r2, r2, lsr #16
20005eb8:	fb0e 3000 	mla	r0, lr, r0, r3
20005ebc:	ea4f 4315 	mov.w	r3, r5, lsr #16
20005ec0:	4401      	add	r1, r0
20005ec2:	fb0e 3302 	mla	r3, lr, r2, r3
20005ec6:	b28a      	uxth	r2, r1
20005ec8:	eb03 4111 	add.w	r1, r3, r1, lsr #16
20005ecc:	ea42 4301 	orr.w	r3, r2, r1, lsl #16
20005ed0:	ea4f 4111 	mov.w	r1, r1, lsr #16
20005ed4:	f844 3b04 	str.w	r3, [r4], #4
20005ed8:	d8e4      	bhi.n	20005ea4 <__multiply+0x7c>
20005eda:	6071      	str	r1, [r6, #4]
20005edc:	f859 6c04 	ldr.w	r6, [r9, #-4]
20005ee0:	0c36      	lsrs	r6, r6, #16
20005ee2:	d022      	beq.n	20005f2a <__multiply+0x102>
20005ee4:	f8da 3000 	ldr.w	r3, [sl]
20005ee8:	2700      	movs	r7, #0
20005eea:	4655      	mov	r5, sl
20005eec:	4640      	mov	r0, r8
20005eee:	461a      	mov	r2, r3
20005ef0:	46be      	mov	lr, r7
20005ef2:	e000      	b.n	20005ef6 <__multiply+0xce>
20005ef4:	4625      	mov	r5, r4
20005ef6:	8807      	ldrh	r7, [r0, #0]
20005ef8:	0c12      	lsrs	r2, r2, #16
20005efa:	b299      	uxth	r1, r3
20005efc:	462c      	mov	r4, r5
20005efe:	fb06 2207 	mla	r2, r6, r7, r2
20005f02:	eb02 070e 	add.w	r7, r2, lr
20005f06:	ea41 4307 	orr.w	r3, r1, r7, lsl #16
20005f0a:	f844 3b04 	str.w	r3, [r4], #4
20005f0e:	f850 1b04 	ldr.w	r1, [r0], #4
20005f12:	686a      	ldr	r2, [r5, #4]
20005f14:	0c09      	lsrs	r1, r1, #16
20005f16:	4584      	cmp	ip, r0
20005f18:	b293      	uxth	r3, r2
20005f1a:	fb06 3101 	mla	r1, r6, r1, r3
20005f1e:	eb01 4317 	add.w	r3, r1, r7, lsr #16
20005f22:	ea4f 4e13 	mov.w	lr, r3, lsr #16
20005f26:	d8e5      	bhi.n	20005ef4 <__multiply+0xcc>
20005f28:	606b      	str	r3, [r5, #4]
20005f2a:	45cb      	cmp	fp, r9
20005f2c:	f10a 0a04 	add.w	sl, sl, #4
20005f30:	d8ad      	bhi.n	20005e8e <__multiply+0x66>
20005f32:	f8dd b008 	ldr.w	fp, [sp, #8]
20005f36:	f8dd 800c 	ldr.w	r8, [sp, #12]
20005f3a:	f1b8 0f00 	cmp.w	r8, #0
20005f3e:	dd0b      	ble.n	20005f58 <__multiply+0x130>
20005f40:	f85b 3c04 	ldr.w	r3, [fp, #-4]
20005f44:	f1ab 0b04 	sub.w	fp, fp, #4
20005f48:	b11b      	cbz	r3, 20005f52 <__multiply+0x12a>
20005f4a:	e005      	b.n	20005f58 <__multiply+0x130>
20005f4c:	f85b 3d04 	ldr.w	r3, [fp, #-4]!
20005f50:	b913      	cbnz	r3, 20005f58 <__multiply+0x130>
20005f52:	f1b8 0801 	subs.w	r8, r8, #1
20005f56:	d1f9      	bne.n	20005f4c <__multiply+0x124>
20005f58:	9801      	ldr	r0, [sp, #4]
20005f5a:	f8c0 8010 	str.w	r8, [r0, #16]
20005f5e:	b005      	add	sp, #20
20005f60:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20005f64:	4614      	mov	r4, r2
20005f66:	4689      	mov	r9, r1
20005f68:	e76b      	b.n	20005e42 <__multiply+0x1a>
20005f6a:	bf00      	nop

20005f6c <__pow5mult>:
20005f6c:	f012 0303 	ands.w	r3, r2, #3
20005f70:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
20005f74:	4614      	mov	r4, r2
20005f76:	4607      	mov	r7, r0
20005f78:	460e      	mov	r6, r1
20005f7a:	d12d      	bne.n	20005fd8 <__pow5mult+0x6c>
20005f7c:	10a4      	asrs	r4, r4, #2
20005f7e:	d01c      	beq.n	20005fba <__pow5mult+0x4e>
20005f80:	6cbd      	ldr	r5, [r7, #72]	; 0x48
20005f82:	b395      	cbz	r5, 20005fea <__pow5mult+0x7e>
20005f84:	07e3      	lsls	r3, r4, #31
20005f86:	f04f 0800 	mov.w	r8, #0
20005f8a:	d406      	bmi.n	20005f9a <__pow5mult+0x2e>
20005f8c:	1064      	asrs	r4, r4, #1
20005f8e:	d014      	beq.n	20005fba <__pow5mult+0x4e>
20005f90:	6828      	ldr	r0, [r5, #0]
20005f92:	b1a8      	cbz	r0, 20005fc0 <__pow5mult+0x54>
20005f94:	4605      	mov	r5, r0
20005f96:	07e3      	lsls	r3, r4, #31
20005f98:	d5f8      	bpl.n	20005f8c <__pow5mult+0x20>
20005f9a:	4638      	mov	r0, r7
20005f9c:	4631      	mov	r1, r6
20005f9e:	462a      	mov	r2, r5
20005fa0:	f7ff ff42 	bl	20005e28 <__multiply>
20005fa4:	b1b6      	cbz	r6, 20005fd4 <__pow5mult+0x68>
20005fa6:	6872      	ldr	r2, [r6, #4]
20005fa8:	1064      	asrs	r4, r4, #1
20005faa:	6cfb      	ldr	r3, [r7, #76]	; 0x4c
20005fac:	f853 1022 	ldr.w	r1, [r3, r2, lsl #2]
20005fb0:	6031      	str	r1, [r6, #0]
20005fb2:	f843 6022 	str.w	r6, [r3, r2, lsl #2]
20005fb6:	4606      	mov	r6, r0
20005fb8:	d1ea      	bne.n	20005f90 <__pow5mult+0x24>
20005fba:	4630      	mov	r0, r6
20005fbc:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
20005fc0:	4629      	mov	r1, r5
20005fc2:	462a      	mov	r2, r5
20005fc4:	4638      	mov	r0, r7
20005fc6:	f7ff ff2f 	bl	20005e28 <__multiply>
20005fca:	6028      	str	r0, [r5, #0]
20005fcc:	f8c0 8000 	str.w	r8, [r0]
20005fd0:	4605      	mov	r5, r0
20005fd2:	e7e0      	b.n	20005f96 <__pow5mult+0x2a>
20005fd4:	4606      	mov	r6, r0
20005fd6:	e7d9      	b.n	20005f8c <__pow5mult+0x20>
20005fd8:	1e5a      	subs	r2, r3, #1
20005fda:	4d0b      	ldr	r5, [pc, #44]	; (20006008 <__pow5mult+0x9c>)
20005fdc:	2300      	movs	r3, #0
20005fde:	f855 2022 	ldr.w	r2, [r5, r2, lsl #2]
20005fe2:	f7ff fe37 	bl	20005c54 <__multadd>
20005fe6:	4606      	mov	r6, r0
20005fe8:	e7c8      	b.n	20005f7c <__pow5mult+0x10>
20005fea:	2101      	movs	r1, #1
20005fec:	4638      	mov	r0, r7
20005fee:	f7ff fdff 	bl	20005bf0 <_Balloc>
20005ff2:	f240 2171 	movw	r1, #625	; 0x271
20005ff6:	2201      	movs	r2, #1
20005ff8:	2300      	movs	r3, #0
20005ffa:	6141      	str	r1, [r0, #20]
20005ffc:	4605      	mov	r5, r0
20005ffe:	6102      	str	r2, [r0, #16]
20006000:	64b8      	str	r0, [r7, #72]	; 0x48
20006002:	6003      	str	r3, [r0, #0]
20006004:	e7be      	b.n	20005f84 <__pow5mult+0x18>
20006006:	bf00      	nop
20006008:	200091b0 	.word	0x200091b0

2000600c <__lshift>:
2000600c:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
20006010:	690e      	ldr	r6, [r1, #16]
20006012:	ea4f 1962 	mov.w	r9, r2, asr #5
20006016:	688b      	ldr	r3, [r1, #8]
20006018:	460d      	mov	r5, r1
2000601a:	444e      	add	r6, r9
2000601c:	4690      	mov	r8, r2
2000601e:	4607      	mov	r7, r0
20006020:	6849      	ldr	r1, [r1, #4]
20006022:	1c74      	adds	r4, r6, #1
20006024:	429c      	cmp	r4, r3
20006026:	dd03      	ble.n	20006030 <__lshift+0x24>
20006028:	005b      	lsls	r3, r3, #1
2000602a:	3101      	adds	r1, #1
2000602c:	429c      	cmp	r4, r3
2000602e:	dcfb      	bgt.n	20006028 <__lshift+0x1c>
20006030:	4638      	mov	r0, r7
20006032:	f7ff fddd 	bl	20005bf0 <_Balloc>
20006036:	f1b9 0f00 	cmp.w	r9, #0
2000603a:	4684      	mov	ip, r0
2000603c:	f100 0114 	add.w	r1, r0, #20
20006040:	dd09      	ble.n	20006056 <__lshift+0x4a>
20006042:	2300      	movs	r3, #0
20006044:	460a      	mov	r2, r1
20006046:	4618      	mov	r0, r3
20006048:	3301      	adds	r3, #1
2000604a:	f842 0b04 	str.w	r0, [r2], #4
2000604e:	454b      	cmp	r3, r9
20006050:	d1fa      	bne.n	20006048 <__lshift+0x3c>
20006052:	eb01 0183 	add.w	r1, r1, r3, lsl #2
20006056:	692a      	ldr	r2, [r5, #16]
20006058:	f105 0314 	add.w	r3, r5, #20
2000605c:	eb03 0e82 	add.w	lr, r3, r2, lsl #2
20006060:	f018 021f 	ands.w	r2, r8, #31
20006064:	d023      	beq.n	200060ae <__lshift+0xa2>
20006066:	f1c2 0920 	rsb	r9, r2, #32
2000606a:	f04f 0a00 	mov.w	sl, #0
2000606e:	6818      	ldr	r0, [r3, #0]
20006070:	4688      	mov	r8, r1
20006072:	4090      	lsls	r0, r2
20006074:	ea4a 0000 	orr.w	r0, sl, r0
20006078:	f841 0b04 	str.w	r0, [r1], #4
2000607c:	f853 0b04 	ldr.w	r0, [r3], #4
20006080:	459e      	cmp	lr, r3
20006082:	fa20 fa09 	lsr.w	sl, r0, r9
20006086:	d8f2      	bhi.n	2000606e <__lshift+0x62>
20006088:	f8c8 a004 	str.w	sl, [r8, #4]
2000608c:	f1ba 0f00 	cmp.w	sl, #0
20006090:	d000      	beq.n	20006094 <__lshift+0x88>
20006092:	1cb4      	adds	r4, r6, #2
20006094:	6cfb      	ldr	r3, [r7, #76]	; 0x4c
20006096:	3c01      	subs	r4, #1
20006098:	686a      	ldr	r2, [r5, #4]
2000609a:	4660      	mov	r0, ip
2000609c:	f8cc 4010 	str.w	r4, [ip, #16]
200060a0:	f853 1022 	ldr.w	r1, [r3, r2, lsl #2]
200060a4:	6029      	str	r1, [r5, #0]
200060a6:	f843 5022 	str.w	r5, [r3, r2, lsl #2]
200060aa:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
200060ae:	3904      	subs	r1, #4
200060b0:	f853 2b04 	ldr.w	r2, [r3], #4
200060b4:	459e      	cmp	lr, r3
200060b6:	f841 2f04 	str.w	r2, [r1, #4]!
200060ba:	d8f9      	bhi.n	200060b0 <__lshift+0xa4>
200060bc:	e7ea      	b.n	20006094 <__lshift+0x88>
200060be:	bf00      	nop

200060c0 <__mcmp>:
200060c0:	6902      	ldr	r2, [r0, #16]
200060c2:	690b      	ldr	r3, [r1, #16]
200060c4:	1ad2      	subs	r2, r2, r3
200060c6:	d113      	bne.n	200060f0 <__mcmp+0x30>
200060c8:	009a      	lsls	r2, r3, #2
200060ca:	3014      	adds	r0, #20
200060cc:	3114      	adds	r1, #20
200060ce:	1883      	adds	r3, r0, r2
200060d0:	4411      	add	r1, r2
200060d2:	b410      	push	{r4}
200060d4:	e001      	b.n	200060da <__mcmp+0x1a>
200060d6:	4298      	cmp	r0, r3
200060d8:	d20c      	bcs.n	200060f4 <__mcmp+0x34>
200060da:	f853 4d04 	ldr.w	r4, [r3, #-4]!
200060de:	f851 2d04 	ldr.w	r2, [r1, #-4]!
200060e2:	4294      	cmp	r4, r2
200060e4:	d0f7      	beq.n	200060d6 <__mcmp+0x16>
200060e6:	d309      	bcc.n	200060fc <__mcmp+0x3c>
200060e8:	2001      	movs	r0, #1
200060ea:	f85d 4b04 	ldr.w	r4, [sp], #4
200060ee:	4770      	bx	lr
200060f0:	4610      	mov	r0, r2
200060f2:	4770      	bx	lr
200060f4:	2000      	movs	r0, #0
200060f6:	f85d 4b04 	ldr.w	r4, [sp], #4
200060fa:	4770      	bx	lr
200060fc:	f04f 30ff 	mov.w	r0, #4294967295
20006100:	f85d 4b04 	ldr.w	r4, [sp], #4
20006104:	4770      	bx	lr
20006106:	bf00      	nop

20006108 <__mdiff>:
20006108:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
2000610c:	460c      	mov	r4, r1
2000610e:	4605      	mov	r5, r0
20006110:	4611      	mov	r1, r2
20006112:	4617      	mov	r7, r2
20006114:	4620      	mov	r0, r4
20006116:	f7ff ffd3 	bl	200060c0 <__mcmp>
2000611a:	1e06      	subs	r6, r0, #0
2000611c:	d05c      	beq.n	200061d8 <__mdiff+0xd0>
2000611e:	db55      	blt.n	200061cc <__mdiff+0xc4>
20006120:	f04f 0800 	mov.w	r8, #0
20006124:	6861      	ldr	r1, [r4, #4]
20006126:	4628      	mov	r0, r5
20006128:	f7ff fd62 	bl	20005bf0 <_Balloc>
2000612c:	f107 0514 	add.w	r5, r7, #20
20006130:	693b      	ldr	r3, [r7, #16]
20006132:	f104 0114 	add.w	r1, r4, #20
20006136:	6926      	ldr	r6, [r4, #16]
20006138:	4684      	mov	ip, r0
2000613a:	eb05 0e83 	add.w	lr, r5, r3, lsl #2
2000613e:	f100 0414 	add.w	r4, r0, #20
20006142:	eb01 0786 	add.w	r7, r1, r6, lsl #2
20006146:	2300      	movs	r3, #0
20006148:	f8c0 800c 	str.w	r8, [r0, #12]
2000614c:	f851 9b04 	ldr.w	r9, [r1], #4
20006150:	f855 2b04 	ldr.w	r2, [r5], #4
20006154:	fa13 f889 	uxtah	r8, r3, r9
20006158:	4608      	mov	r0, r1
2000615a:	b293      	uxth	r3, r2
2000615c:	45ae      	cmp	lr, r5
2000615e:	ea4f 4212 	mov.w	r2, r2, lsr #16
20006162:	ebc3 0308 	rsb	r3, r3, r8
20006166:	ebc2 4219 	rsb	r2, r2, r9, lsr #16
2000616a:	fa1f f883 	uxth.w	r8, r3
2000616e:	eb02 4323 	add.w	r3, r2, r3, asr #16
20006172:	ea48 4203 	orr.w	r2, r8, r3, lsl #16
20006176:	ea4f 4323 	mov.w	r3, r3, asr #16
2000617a:	f844 2b04 	str.w	r2, [r4], #4
2000617e:	d8e5      	bhi.n	2000614c <__mdiff+0x44>
20006180:	428f      	cmp	r7, r1
20006182:	4625      	mov	r5, r4
20006184:	d916      	bls.n	200061b4 <__mdiff+0xac>
20006186:	f850 2b04 	ldr.w	r2, [r0], #4
2000618a:	fa13 f382 	uxtah	r3, r3, r2
2000618e:	0c12      	lsrs	r2, r2, #16
20006190:	4287      	cmp	r7, r0
20006192:	fa1f fe83 	uxth.w	lr, r3
20006196:	eb02 4323 	add.w	r3, r2, r3, asr #16
2000619a:	ea4e 4203 	orr.w	r2, lr, r3, lsl #16
2000619e:	ea4f 4323 	mov.w	r3, r3, asr #16
200061a2:	f844 2b04 	str.w	r2, [r4], #4
200061a6:	d8ee      	bhi.n	20006186 <__mdiff+0x7e>
200061a8:	43c9      	mvns	r1, r1
200061aa:	4439      	add	r1, r7
200061ac:	f021 0403 	bic.w	r4, r1, #3
200061b0:	3404      	adds	r4, #4
200061b2:	442c      	add	r4, r5
200061b4:	3c04      	subs	r4, #4
200061b6:	b922      	cbnz	r2, 200061c2 <__mdiff+0xba>
200061b8:	f854 3d04 	ldr.w	r3, [r4, #-4]!
200061bc:	3e01      	subs	r6, #1
200061be:	2b00      	cmp	r3, #0
200061c0:	d0fa      	beq.n	200061b8 <__mdiff+0xb0>
200061c2:	4660      	mov	r0, ip
200061c4:	f8cc 6010 	str.w	r6, [ip, #16]
200061c8:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}
200061cc:	4623      	mov	r3, r4
200061ce:	f04f 0801 	mov.w	r8, #1
200061d2:	463c      	mov	r4, r7
200061d4:	461f      	mov	r7, r3
200061d6:	e7a5      	b.n	20006124 <__mdiff+0x1c>
200061d8:	4628      	mov	r0, r5
200061da:	4631      	mov	r1, r6
200061dc:	f7ff fd08 	bl	20005bf0 <_Balloc>
200061e0:	2201      	movs	r2, #1
200061e2:	4603      	mov	r3, r0
200061e4:	6146      	str	r6, [r0, #20]
200061e6:	611a      	str	r2, [r3, #16]
200061e8:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}

200061ec <__ulp>:
200061ec:	4b10      	ldr	r3, [pc, #64]	; (20006230 <__ulp+0x44>)
200061ee:	400b      	ands	r3, r1
200061f0:	f1a3 7350 	sub.w	r3, r3, #54525952	; 0x3400000
200061f4:	2b00      	cmp	r3, #0
200061f6:	dd02      	ble.n	200061fe <__ulp+0x12>
200061f8:	2000      	movs	r0, #0
200061fa:	4619      	mov	r1, r3
200061fc:	4770      	bx	lr
200061fe:	425b      	negs	r3, r3
20006200:	151b      	asrs	r3, r3, #20
20006202:	2b13      	cmp	r3, #19
20006204:	dd0d      	ble.n	20006222 <__ulp+0x36>
20006206:	2b32      	cmp	r3, #50	; 0x32
20006208:	f04f 0100 	mov.w	r1, #0
2000620c:	dd02      	ble.n	20006214 <__ulp+0x28>
2000620e:	2301      	movs	r3, #1
20006210:	4618      	mov	r0, r3
20006212:	4770      	bx	lr
20006214:	2201      	movs	r2, #1
20006216:	f1c3 0333 	rsb	r3, r3, #51	; 0x33
2000621a:	fa02 f303 	lsl.w	r3, r2, r3
2000621e:	4618      	mov	r0, r3
20006220:	4770      	bx	lr
20006222:	f44f 2200 	mov.w	r2, #524288	; 0x80000
20006226:	2000      	movs	r0, #0
20006228:	fa42 f103 	asr.w	r1, r2, r3
2000622c:	4770      	bx	lr
2000622e:	bf00      	nop
20006230:	7ff00000 	.word	0x7ff00000

20006234 <__b2d>:
20006234:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
20006238:	6904      	ldr	r4, [r0, #16]
2000623a:	f100 0814 	add.w	r8, r0, #20
2000623e:	460e      	mov	r6, r1
20006240:	eb08 0484 	add.w	r4, r8, r4, lsl #2
20006244:	f854 7c04 	ldr.w	r7, [r4, #-4]
20006248:	1f25      	subs	r5, r4, #4
2000624a:	4638      	mov	r0, r7
2000624c:	f7ff fd8e 	bl	20005d6c <__hi0bits>
20006250:	f1c0 0320 	rsb	r3, r0, #32
20006254:	280a      	cmp	r0, #10
20006256:	6033      	str	r3, [r6, #0]
20006258:	dc14      	bgt.n	20006284 <__b2d+0x50>
2000625a:	f1c0 060b 	rsb	r6, r0, #11
2000625e:	45a8      	cmp	r8, r5
20006260:	fa27 f106 	lsr.w	r1, r7, r6
20006264:	f041 537f 	orr.w	r3, r1, #1069547520	; 0x3fc00000
20006268:	f443 1340 	orr.w	r3, r3, #3145728	; 0x300000
2000626c:	d242      	bcs.n	200062f4 <__b2d+0xc0>
2000626e:	f854 1c08 	ldr.w	r1, [r4, #-8]
20006272:	40f1      	lsrs	r1, r6
20006274:	3015      	adds	r0, #21
20006276:	4087      	lsls	r7, r0
20006278:	ea41 0207 	orr.w	r2, r1, r7
2000627c:	4619      	mov	r1, r3
2000627e:	4610      	mov	r0, r2
20006280:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
20006284:	45a8      	cmp	r8, r5
20006286:	d21f      	bcs.n	200062c8 <__b2d+0x94>
20006288:	f1b0 0e0b 	subs.w	lr, r0, #11
2000628c:	f1a4 0c08 	sub.w	ip, r4, #8
20006290:	f854 1c08 	ldr.w	r1, [r4, #-8]
20006294:	d025      	beq.n	200062e2 <__b2d+0xae>
20006296:	f1c0 062b 	rsb	r6, r0, #43	; 0x2b
2000629a:	fa07 f70e 	lsl.w	r7, r7, lr
2000629e:	45e0      	cmp	r8, ip
200062a0:	fa21 f506 	lsr.w	r5, r1, r6
200062a4:	ea47 0705 	orr.w	r7, r7, r5
200062a8:	f047 537f 	orr.w	r3, r7, #1069547520	; 0x3fc00000
200062ac:	f443 1340 	orr.w	r3, r3, #3145728	; 0x300000
200062b0:	d222      	bcs.n	200062f8 <__b2d+0xc4>
200062b2:	f854 0c0c 	ldr.w	r0, [r4, #-12]
200062b6:	40f0      	lsrs	r0, r6
200062b8:	fa01 f10e 	lsl.w	r1, r1, lr
200062bc:	ea41 0200 	orr.w	r2, r1, r0
200062c0:	4619      	mov	r1, r3
200062c2:	4610      	mov	r0, r2
200062c4:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
200062c8:	f1b0 0e0b 	subs.w	lr, r0, #11
200062cc:	d008      	beq.n	200062e0 <__b2d+0xac>
200062ce:	fa07 f70e 	lsl.w	r7, r7, lr
200062d2:	2000      	movs	r0, #0
200062d4:	f047 537f 	orr.w	r3, r7, #1069547520	; 0x3fc00000
200062d8:	4601      	mov	r1, r0
200062da:	f443 1340 	orr.w	r3, r3, #3145728	; 0x300000
200062de:	e7eb      	b.n	200062b8 <__b2d+0x84>
200062e0:	4671      	mov	r1, lr
200062e2:	f047 537f 	orr.w	r3, r7, #1069547520	; 0x3fc00000
200062e6:	460a      	mov	r2, r1
200062e8:	f443 1340 	orr.w	r3, r3, #3145728	; 0x300000
200062ec:	4610      	mov	r0, r2
200062ee:	4619      	mov	r1, r3
200062f0:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
200062f4:	2100      	movs	r1, #0
200062f6:	e7bd      	b.n	20006274 <__b2d+0x40>
200062f8:	2000      	movs	r0, #0
200062fa:	e7dd      	b.n	200062b8 <__b2d+0x84>

200062fc <__d2b>:
200062fc:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
20006300:	461c      	mov	r4, r3
20006302:	b082      	sub	sp, #8
20006304:	2101      	movs	r1, #1
20006306:	4616      	mov	r6, r2
20006308:	f3c4 550a 	ubfx	r5, r4, #20, #11
2000630c:	f7ff fc70 	bl	20005bf0 <_Balloc>
20006310:	f3c4 0413 	ubfx	r4, r4, #0, #20
20006314:	4607      	mov	r7, r0
20006316:	b10d      	cbz	r5, 2000631c <__d2b+0x20>
20006318:	f444 1480 	orr.w	r4, r4, #1048576	; 0x100000
2000631c:	9401      	str	r4, [sp, #4]
2000631e:	b30e      	cbz	r6, 20006364 <__d2b+0x68>
20006320:	a802      	add	r0, sp, #8
20006322:	f840 6d08 	str.w	r6, [r0, #-8]!
20006326:	f7ff fd43 	bl	20005db0 <__lo0bits>
2000632a:	2800      	cmp	r0, #0
2000632c:	d132      	bne.n	20006394 <__d2b+0x98>
2000632e:	e89d 000c 	ldmia.w	sp, {r2, r3}
20006332:	617a      	str	r2, [r7, #20]
20006334:	2b00      	cmp	r3, #0
20006336:	61bb      	str	r3, [r7, #24]
20006338:	bf14      	ite	ne
2000633a:	2402      	movne	r4, #2
2000633c:	2401      	moveq	r4, #1
2000633e:	613c      	str	r4, [r7, #16]
20006340:	b9dd      	cbnz	r5, 2000637a <__d2b+0x7e>
20006342:	eb07 0384 	add.w	r3, r7, r4, lsl #2
20006346:	9a08      	ldr	r2, [sp, #32]
20006348:	f2a0 4032 	subw	r0, r0, #1074	; 0x432
2000634c:	6010      	str	r0, [r2, #0]
2000634e:	6918      	ldr	r0, [r3, #16]
20006350:	f7ff fd0c 	bl	20005d6c <__hi0bits>
20006354:	9b09      	ldr	r3, [sp, #36]	; 0x24
20006356:	ebc0 1044 	rsb	r0, r0, r4, lsl #5
2000635a:	6018      	str	r0, [r3, #0]
2000635c:	4638      	mov	r0, r7
2000635e:	b002      	add	sp, #8
20006360:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
20006364:	a801      	add	r0, sp, #4
20006366:	f7ff fd23 	bl	20005db0 <__lo0bits>
2000636a:	2301      	movs	r3, #1
2000636c:	9a01      	ldr	r2, [sp, #4]
2000636e:	3020      	adds	r0, #32
20006370:	461c      	mov	r4, r3
20006372:	613b      	str	r3, [r7, #16]
20006374:	617a      	str	r2, [r7, #20]
20006376:	2d00      	cmp	r5, #0
20006378:	d0e3      	beq.n	20006342 <__d2b+0x46>
2000637a:	f2a5 4833 	subw	r8, r5, #1075	; 0x433
2000637e:	f1c0 0335 	rsb	r3, r0, #53	; 0x35
20006382:	9a08      	ldr	r2, [sp, #32]
20006384:	4440      	add	r0, r8
20006386:	6010      	str	r0, [r2, #0]
20006388:	4638      	mov	r0, r7
2000638a:	9a09      	ldr	r2, [sp, #36]	; 0x24
2000638c:	6013      	str	r3, [r2, #0]
2000638e:	b002      	add	sp, #8
20006390:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}
20006394:	9b01      	ldr	r3, [sp, #4]
20006396:	f1c0 0120 	rsb	r1, r0, #32
2000639a:	9a00      	ldr	r2, [sp, #0]
2000639c:	fa03 f101 	lsl.w	r1, r3, r1
200063a0:	40c3      	lsrs	r3, r0
200063a2:	430a      	orrs	r2, r1
200063a4:	9301      	str	r3, [sp, #4]
200063a6:	617a      	str	r2, [r7, #20]
200063a8:	e7c4      	b.n	20006334 <__d2b+0x38>
200063aa:	bf00      	nop

200063ac <__ratio>:
200063ac:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
200063b0:	b083      	sub	sp, #12
200063b2:	4688      	mov	r8, r1
200063b4:	4681      	mov	r9, r0
200063b6:	4669      	mov	r1, sp
200063b8:	f7ff ff3c 	bl	20006234 <__b2d>
200063bc:	4604      	mov	r4, r0
200063be:	460d      	mov	r5, r1
200063c0:	4640      	mov	r0, r8
200063c2:	a901      	add	r1, sp, #4
200063c4:	f7ff ff36 	bl	20006234 <__b2d>
200063c8:	f8d9 2010 	ldr.w	r2, [r9, #16]
200063cc:	f8d8 3010 	ldr.w	r3, [r8, #16]
200063d0:	4606      	mov	r6, r0
200063d2:	460f      	mov	r7, r1
200063d4:	ebc3 0e02 	rsb	lr, r3, r2
200063d8:	9b00      	ldr	r3, [sp, #0]
200063da:	9a01      	ldr	r2, [sp, #4]
200063dc:	1a9b      	subs	r3, r3, r2
200063de:	eb03 134e 	add.w	r3, r3, lr, lsl #5
200063e2:	2b00      	cmp	r3, #0
200063e4:	dd0b      	ble.n	200063fe <__ratio+0x52>
200063e6:	eb05 5103 	add.w	r1, r5, r3, lsl #20
200063ea:	460d      	mov	r5, r1
200063ec:	4632      	mov	r2, r6
200063ee:	463b      	mov	r3, r7
200063f0:	4620      	mov	r0, r4
200063f2:	4629      	mov	r1, r5
200063f4:	f001 fee8 	bl	200081c8 <__aeabi_ddiv>
200063f8:	b003      	add	sp, #12
200063fa:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
200063fe:	eba1 5903 	sub.w	r9, r1, r3, lsl #20
20006402:	4606      	mov	r6, r0
20006404:	464f      	mov	r7, r9
20006406:	e7f1      	b.n	200063ec <__ratio+0x40>

20006408 <_mprec_log10>:
20006408:	2817      	cmp	r0, #23
2000640a:	b510      	push	{r4, lr}
2000640c:	4604      	mov	r4, r0
2000640e:	dd08      	ble.n	20006422 <_mprec_log10+0x1a>
20006410:	2000      	movs	r0, #0
20006412:	4907      	ldr	r1, [pc, #28]	; (20006430 <_mprec_log10+0x28>)
20006414:	2200      	movs	r2, #0
20006416:	4b07      	ldr	r3, [pc, #28]	; (20006434 <_mprec_log10+0x2c>)
20006418:	f001 fdac 	bl	20007f74 <__aeabi_dmul>
2000641c:	3c01      	subs	r4, #1
2000641e:	d1f9      	bne.n	20006414 <_mprec_log10+0xc>
20006420:	bd10      	pop	{r4, pc}
20006422:	4b05      	ldr	r3, [pc, #20]	; (20006438 <_mprec_log10+0x30>)
20006424:	eb03 04c0 	add.w	r4, r3, r0, lsl #3
20006428:	e9d4 0100 	ldrd	r0, r1, [r4]
2000642c:	bd10      	pop	{r4, pc}
2000642e:	bf00      	nop
20006430:	3ff00000 	.word	0x3ff00000
20006434:	40240000 	.word	0x40240000
20006438:	200090e8 	.word	0x200090e8

2000643c <__copybits>:
2000643c:	b470      	push	{r4, r5, r6}
2000643e:	f102 0314 	add.w	r3, r2, #20
20006442:	3901      	subs	r1, #1
20006444:	6914      	ldr	r4, [r2, #16]
20006446:	114e      	asrs	r6, r1, #5
20006448:	eb03 0484 	add.w	r4, r3, r4, lsl #2
2000644c:	3601      	adds	r6, #1
2000644e:	42a3      	cmp	r3, r4
20006450:	eb00 0686 	add.w	r6, r0, r6, lsl #2
20006454:	d20c      	bcs.n	20006470 <__copybits+0x34>
20006456:	1f01      	subs	r1, r0, #4
20006458:	f853 5b04 	ldr.w	r5, [r3], #4
2000645c:	429c      	cmp	r4, r3
2000645e:	f841 5f04 	str.w	r5, [r1, #4]!
20006462:	d8f9      	bhi.n	20006458 <__copybits+0x1c>
20006464:	1aa3      	subs	r3, r4, r2
20006466:	3b15      	subs	r3, #21
20006468:	f023 0303 	bic.w	r3, r3, #3
2000646c:	3304      	adds	r3, #4
2000646e:	4418      	add	r0, r3
20006470:	4286      	cmp	r6, r0
20006472:	d904      	bls.n	2000647e <__copybits+0x42>
20006474:	2300      	movs	r3, #0
20006476:	f840 3b04 	str.w	r3, [r0], #4
2000647a:	4286      	cmp	r6, r0
2000647c:	d8fb      	bhi.n	20006476 <__copybits+0x3a>
2000647e:	bc70      	pop	{r4, r5, r6}
20006480:	4770      	bx	lr
20006482:	bf00      	nop

20006484 <__any_on>:
20006484:	6903      	ldr	r3, [r0, #16]
20006486:	114a      	asrs	r2, r1, #5
20006488:	4293      	cmp	r3, r2
2000648a:	b410      	push	{r4}
2000648c:	f100 0414 	add.w	r4, r0, #20
20006490:	da10      	bge.n	200064b4 <__any_on+0x30>
20006492:	eb04 0383 	add.w	r3, r4, r3, lsl #2
20006496:	429c      	cmp	r4, r3
20006498:	d220      	bcs.n	200064dc <__any_on+0x58>
2000649a:	f853 0c04 	ldr.w	r0, [r3, #-4]
2000649e:	3b04      	subs	r3, #4
200064a0:	b118      	cbz	r0, 200064aa <__any_on+0x26>
200064a2:	e014      	b.n	200064ce <__any_on+0x4a>
200064a4:	f853 2d04 	ldr.w	r2, [r3, #-4]!
200064a8:	b98a      	cbnz	r2, 200064ce <__any_on+0x4a>
200064aa:	429c      	cmp	r4, r3
200064ac:	d3fa      	bcc.n	200064a4 <__any_on+0x20>
200064ae:	f85d 4b04 	ldr.w	r4, [sp], #4
200064b2:	4770      	bx	lr
200064b4:	dd0f      	ble.n	200064d6 <__any_on+0x52>
200064b6:	f011 011f 	ands.w	r1, r1, #31
200064ba:	d00c      	beq.n	200064d6 <__any_on+0x52>
200064bc:	f854 0022 	ldr.w	r0, [r4, r2, lsl #2]
200064c0:	eb04 0382 	add.w	r3, r4, r2, lsl #2
200064c4:	fa20 f201 	lsr.w	r2, r0, r1
200064c8:	408a      	lsls	r2, r1
200064ca:	4282      	cmp	r2, r0
200064cc:	d0e3      	beq.n	20006496 <__any_on+0x12>
200064ce:	2001      	movs	r0, #1
200064d0:	f85d 4b04 	ldr.w	r4, [sp], #4
200064d4:	4770      	bx	lr
200064d6:	eb04 0382 	add.w	r3, r4, r2, lsl #2
200064da:	e7dc      	b.n	20006496 <__any_on+0x12>
200064dc:	2000      	movs	r0, #0
200064de:	e7e6      	b.n	200064ae <__any_on+0x2a>

200064e0 <__fpclassifyd>:
200064e0:	fab0 f080 	clz	r0, r0
200064e4:	f031 4100 	bics.w	r1, r1, #2147483648	; 0x80000000
200064e8:	ea4f 1050 	mov.w	r0, r0, lsr #5
200064ec:	d102      	bne.n	200064f4 <__fpclassifyd+0x14>
200064ee:	b108      	cbz	r0, 200064f4 <__fpclassifyd+0x14>
200064f0:	2002      	movs	r0, #2
200064f2:	4770      	bx	lr
200064f4:	f5a1 1280 	sub.w	r2, r1, #1048576	; 0x100000
200064f8:	4b08      	ldr	r3, [pc, #32]	; (2000651c <__fpclassifyd+0x3c>)
200064fa:	429a      	cmp	r2, r3
200064fc:	d801      	bhi.n	20006502 <__fpclassifyd+0x22>
200064fe:	2004      	movs	r0, #4
20006500:	4770      	bx	lr
20006502:	f5b1 1f80 	cmp.w	r1, #1048576	; 0x100000
20006506:	d201      	bcs.n	2000650c <__fpclassifyd+0x2c>
20006508:	2003      	movs	r0, #3
2000650a:	4770      	bx	lr
2000650c:	4b04      	ldr	r3, [pc, #16]	; (20006520 <__fpclassifyd+0x40>)
2000650e:	4299      	cmp	r1, r3
20006510:	bf14      	ite	ne
20006512:	2000      	movne	r0, #0
20006514:	f000 0001 	andeq.w	r0, r0, #1
20006518:	4770      	bx	lr
2000651a:	bf00      	nop
2000651c:	7fdfffff 	.word	0x7fdfffff
20006520:	7ff00000 	.word	0x7ff00000

20006524 <_sbrk_r>:
20006524:	b538      	push	{r3, r4, r5, lr}
20006526:	2300      	movs	r3, #0
20006528:	4c06      	ldr	r4, [pc, #24]	; (20006544 <_sbrk_r+0x20>)
2000652a:	4605      	mov	r5, r0
2000652c:	4608      	mov	r0, r1
2000652e:	6023      	str	r3, [r4, #0]
20006530:	f7f9 fe06 	bl	20000140 <_sbrk>
20006534:	1c43      	adds	r3, r0, #1
20006536:	d000      	beq.n	2000653a <_sbrk_r+0x16>
20006538:	bd38      	pop	{r3, r4, r5, pc}
2000653a:	6823      	ldr	r3, [r4, #0]
2000653c:	2b00      	cmp	r3, #0
2000653e:	d0fb      	beq.n	20006538 <_sbrk_r+0x14>
20006540:	602b      	str	r3, [r5, #0]
20006542:	bd38      	pop	{r3, r4, r5, pc}
20006544:	20009c60 	.word	0x20009c60
	...
20006560:	eba2 0003 	sub.w	r0, r2, r3
20006564:	4770      	bx	lr
20006566:	bf00      	nop

20006568 <strcmp>:
20006568:	7802      	ldrb	r2, [r0, #0]
2000656a:	780b      	ldrb	r3, [r1, #0]
2000656c:	2a01      	cmp	r2, #1
2000656e:	bf28      	it	cs
20006570:	429a      	cmpcs	r2, r3
20006572:	d1f5      	bne.n	20006560 <_sbrk_r+0x3c>
20006574:	e96d 4504 	strd	r4, r5, [sp, #-16]!
20006578:	ea40 0401 	orr.w	r4, r0, r1
2000657c:	e9cd 6702 	strd	r6, r7, [sp, #8]
20006580:	f06f 0c00 	mvn.w	ip, #0
20006584:	ea4f 7244 	mov.w	r2, r4, lsl #29
20006588:	b312      	cbz	r2, 200065d0 <strcmp+0x68>
2000658a:	ea80 0401 	eor.w	r4, r0, r1
2000658e:	f014 0f07 	tst.w	r4, #7
20006592:	d16a      	bne.n	2000666a <strcmp+0x102>
20006594:	f000 0407 	and.w	r4, r0, #7
20006598:	f020 0007 	bic.w	r0, r0, #7
2000659c:	f004 0503 	and.w	r5, r4, #3
200065a0:	f021 0107 	bic.w	r1, r1, #7
200065a4:	ea4f 05c5 	mov.w	r5, r5, lsl #3
200065a8:	e8f0 2304 	ldrd	r2, r3, [r0], #16
200065ac:	f014 0f04 	tst.w	r4, #4
200065b0:	e8f1 6704 	ldrd	r6, r7, [r1], #16
200065b4:	fa0c f405 	lsl.w	r4, ip, r5
200065b8:	ea62 0204 	orn	r2, r2, r4
200065bc:	ea66 0604 	orn	r6, r6, r4
200065c0:	d00a      	beq.n	200065d8 <strcmp+0x70>
200065c2:	ea63 0304 	orn	r3, r3, r4
200065c6:	4662      	mov	r2, ip
200065c8:	ea67 0704 	orn	r7, r7, r4
200065cc:	4666      	mov	r6, ip
200065ce:	e003      	b.n	200065d8 <strcmp+0x70>
200065d0:	e8f0 2304 	ldrd	r2, r3, [r0], #16
200065d4:	e8f1 6704 	ldrd	r6, r7, [r1], #16
200065d8:	fa82 f54c 	uadd8	r5, r2, ip
200065dc:	ea82 0406 	eor.w	r4, r2, r6
200065e0:	faa4 f48c 	sel	r4, r4, ip
200065e4:	bb6c      	cbnz	r4, 20006642 <strcmp+0xda>
200065e6:	fa83 f54c 	uadd8	r5, r3, ip
200065ea:	ea83 0507 	eor.w	r5, r3, r7
200065ee:	faa5 f58c 	sel	r5, r5, ip
200065f2:	b995      	cbnz	r5, 2000661a <strcmp+0xb2>
200065f4:	e950 2302 	ldrd	r2, r3, [r0, #-8]
200065f8:	e951 6702 	ldrd	r6, r7, [r1, #-8]
200065fc:	fa82 f54c 	uadd8	r5, r2, ip
20006600:	ea82 0406 	eor.w	r4, r2, r6
20006604:	faa4 f48c 	sel	r4, r4, ip
20006608:	fa83 f54c 	uadd8	r5, r3, ip
2000660c:	ea83 0507 	eor.w	r5, r3, r7
20006610:	faa5 f58c 	sel	r5, r5, ip
20006614:	4325      	orrs	r5, r4
20006616:	d0db      	beq.n	200065d0 <strcmp+0x68>
20006618:	b99c      	cbnz	r4, 20006642 <strcmp+0xda>
2000661a:	ba2d      	rev	r5, r5
2000661c:	fab5 f485 	clz	r4, r5
20006620:	f024 0407 	bic.w	r4, r4, #7
20006624:	fa27 f104 	lsr.w	r1, r7, r4
20006628:	e9dd 6702 	ldrd	r6, r7, [sp, #8]
2000662c:	fa23 f304 	lsr.w	r3, r3, r4
20006630:	f003 00ff 	and.w	r0, r3, #255	; 0xff
20006634:	f001 01ff 	and.w	r1, r1, #255	; 0xff
20006638:	e8fd 4504 	ldrd	r4, r5, [sp], #16
2000663c:	eba0 0001 	sub.w	r0, r0, r1
20006640:	4770      	bx	lr
20006642:	ba24      	rev	r4, r4
20006644:	fab4 f484 	clz	r4, r4
20006648:	f024 0407 	bic.w	r4, r4, #7
2000664c:	fa26 f104 	lsr.w	r1, r6, r4
20006650:	e9dd 6702 	ldrd	r6, r7, [sp, #8]
20006654:	fa22 f204 	lsr.w	r2, r2, r4
20006658:	f002 00ff 	and.w	r0, r2, #255	; 0xff
2000665c:	f001 01ff 	and.w	r1, r1, #255	; 0xff
20006660:	e8fd 4504 	ldrd	r4, r5, [sp], #16
20006664:	eba0 0001 	sub.w	r0, r0, r1
20006668:	4770      	bx	lr
2000666a:	f014 0f03 	tst.w	r4, #3
2000666e:	d13c      	bne.n	200066ea <strcmp+0x182>
20006670:	f010 0403 	ands.w	r4, r0, #3
20006674:	d128      	bne.n	200066c8 <strcmp+0x160>
20006676:	f850 2b08 	ldr.w	r2, [r0], #8
2000667a:	f851 3b08 	ldr.w	r3, [r1], #8
2000667e:	fa82 f54c 	uadd8	r5, r2, ip
20006682:	ea82 0503 	eor.w	r5, r2, r3
20006686:	faa5 f58c 	sel	r5, r5, ip
2000668a:	b95d      	cbnz	r5, 200066a4 <strcmp+0x13c>
2000668c:	f850 2c04 	ldr.w	r2, [r0, #-4]
20006690:	f851 3c04 	ldr.w	r3, [r1, #-4]
20006694:	fa82 f54c 	uadd8	r5, r2, ip
20006698:	ea82 0503 	eor.w	r5, r2, r3
2000669c:	faa5 f58c 	sel	r5, r5, ip
200066a0:	2d00      	cmp	r5, #0
200066a2:	d0e8      	beq.n	20006676 <strcmp+0x10e>
200066a4:	ba2d      	rev	r5, r5
200066a6:	fab5 f485 	clz	r4, r5
200066aa:	f024 0407 	bic.w	r4, r4, #7
200066ae:	fa23 f104 	lsr.w	r1, r3, r4
200066b2:	fa22 f204 	lsr.w	r2, r2, r4
200066b6:	f002 00ff 	and.w	r0, r2, #255	; 0xff
200066ba:	f001 01ff 	and.w	r1, r1, #255	; 0xff
200066be:	e8fd 4504 	ldrd	r4, r5, [sp], #16
200066c2:	eba0 0001 	sub.w	r0, r0, r1
200066c6:	4770      	bx	lr
200066c8:	ea4f 04c4 	mov.w	r4, r4, lsl #3
200066cc:	f020 0003 	bic.w	r0, r0, #3
200066d0:	f850 2b08 	ldr.w	r2, [r0], #8
200066d4:	f021 0103 	bic.w	r1, r1, #3
200066d8:	f851 3b08 	ldr.w	r3, [r1], #8
200066dc:	fa0c f404 	lsl.w	r4, ip, r4
200066e0:	ea62 0204 	orn	r2, r2, r4
200066e4:	ea63 0304 	orn	r3, r3, r4
200066e8:	e7c9      	b.n	2000667e <strcmp+0x116>
200066ea:	f010 0403 	ands.w	r4, r0, #3
200066ee:	d01a      	beq.n	20006726 <strcmp+0x1be>
200066f0:	eba1 0104 	sub.w	r1, r1, r4
200066f4:	f020 0003 	bic.w	r0, r0, #3
200066f8:	07e4      	lsls	r4, r4, #31
200066fa:	f850 2b04 	ldr.w	r2, [r0], #4
200066fe:	d006      	beq.n	2000670e <strcmp+0x1a6>
20006700:	d20f      	bcs.n	20006722 <strcmp+0x1ba>
20006702:	788b      	ldrb	r3, [r1, #2]
20006704:	fa5f f4a2 	uxtb.w	r4, r2, ror #16
20006708:	1ae4      	subs	r4, r4, r3
2000670a:	d106      	bne.n	2000671a <strcmp+0x1b2>
2000670c:	b12b      	cbz	r3, 2000671a <strcmp+0x1b2>
2000670e:	78cb      	ldrb	r3, [r1, #3]
20006710:	fa5f f4b2 	uxtb.w	r4, r2, ror #24
20006714:	1ae4      	subs	r4, r4, r3
20006716:	d100      	bne.n	2000671a <strcmp+0x1b2>
20006718:	b91b      	cbnz	r3, 20006722 <strcmp+0x1ba>
2000671a:	4620      	mov	r0, r4
2000671c:	f85d 4b10 	ldr.w	r4, [sp], #16
20006720:	4770      	bx	lr
20006722:	f101 0104 	add.w	r1, r1, #4
20006726:	f850 2b04 	ldr.w	r2, [r0], #4
2000672a:	07cc      	lsls	r4, r1, #31
2000672c:	f021 0103 	bic.w	r1, r1, #3
20006730:	f851 3b04 	ldr.w	r3, [r1], #4
20006734:	d848      	bhi.n	200067c8 <strcmp+0x260>
20006736:	d224      	bcs.n	20006782 <strcmp+0x21a>
20006738:	f022 447f 	bic.w	r4, r2, #4278190080	; 0xff000000
2000673c:	fa82 f54c 	uadd8	r5, r2, ip
20006740:	ea94 2513 	eors.w	r5, r4, r3, lsr #8
20006744:	faa5 f58c 	sel	r5, r5, ip
20006748:	d10a      	bne.n	20006760 <strcmp+0x1f8>
2000674a:	b965      	cbnz	r5, 20006766 <strcmp+0x1fe>
2000674c:	f851 3b04 	ldr.w	r3, [r1], #4
20006750:	ea84 0402 	eor.w	r4, r4, r2
20006754:	ebb4 6f03 	cmp.w	r4, r3, lsl #24
20006758:	d10e      	bne.n	20006778 <strcmp+0x210>
2000675a:	f850 2b04 	ldr.w	r2, [r0], #4
2000675e:	e7eb      	b.n	20006738 <strcmp+0x1d0>
20006760:	ea4f 2313 	mov.w	r3, r3, lsr #8
20006764:	e055      	b.n	20006812 <strcmp+0x2aa>
20006766:	f035 457f 	bics.w	r5, r5, #4278190080	; 0xff000000
2000676a:	d14d      	bne.n	20006808 <strcmp+0x2a0>
2000676c:	7808      	ldrb	r0, [r1, #0]
2000676e:	e8fd 4504 	ldrd	r4, r5, [sp], #16
20006772:	f1c0 0000 	rsb	r0, r0, #0
20006776:	4770      	bx	lr
20006778:	ea4f 6212 	mov.w	r2, r2, lsr #24
2000677c:	f003 03ff 	and.w	r3, r3, #255	; 0xff
20006780:	e047      	b.n	20006812 <strcmp+0x2aa>
20006782:	ea02 441c 	and.w	r4, r2, ip, lsr #16
20006786:	fa82 f54c 	uadd8	r5, r2, ip
2000678a:	ea94 4513 	eors.w	r5, r4, r3, lsr #16
2000678e:	faa5 f58c 	sel	r5, r5, ip
20006792:	d10a      	bne.n	200067aa <strcmp+0x242>
20006794:	b965      	cbnz	r5, 200067b0 <strcmp+0x248>
20006796:	f851 3b04 	ldr.w	r3, [r1], #4
2000679a:	ea84 0402 	eor.w	r4, r4, r2
2000679e:	ebb4 4f03 	cmp.w	r4, r3, lsl #16
200067a2:	d10c      	bne.n	200067be <strcmp+0x256>
200067a4:	f850 2b04 	ldr.w	r2, [r0], #4
200067a8:	e7eb      	b.n	20006782 <strcmp+0x21a>
200067aa:	ea4f 4313 	mov.w	r3, r3, lsr #16
200067ae:	e030      	b.n	20006812 <strcmp+0x2aa>
200067b0:	ea15 451c 	ands.w	r5, r5, ip, lsr #16
200067b4:	d128      	bne.n	20006808 <strcmp+0x2a0>
200067b6:	880b      	ldrh	r3, [r1, #0]
200067b8:	ea4f 4212 	mov.w	r2, r2, lsr #16
200067bc:	e029      	b.n	20006812 <strcmp+0x2aa>
200067be:	ea4f 4212 	mov.w	r2, r2, lsr #16
200067c2:	ea03 431c 	and.w	r3, r3, ip, lsr #16
200067c6:	e024      	b.n	20006812 <strcmp+0x2aa>
200067c8:	f002 04ff 	and.w	r4, r2, #255	; 0xff
200067cc:	fa82 f54c 	uadd8	r5, r2, ip
200067d0:	ea94 6513 	eors.w	r5, r4, r3, lsr #24
200067d4:	faa5 f58c 	sel	r5, r5, ip
200067d8:	d10a      	bne.n	200067f0 <strcmp+0x288>
200067da:	b965      	cbnz	r5, 200067f6 <strcmp+0x28e>
200067dc:	f851 3b04 	ldr.w	r3, [r1], #4
200067e0:	ea84 0402 	eor.w	r4, r4, r2
200067e4:	ebb4 2f03 	cmp.w	r4, r3, lsl #8
200067e8:	d109      	bne.n	200067fe <strcmp+0x296>
200067ea:	f850 2b04 	ldr.w	r2, [r0], #4
200067ee:	e7eb      	b.n	200067c8 <strcmp+0x260>
200067f0:	ea4f 6313 	mov.w	r3, r3, lsr #24
200067f4:	e00d      	b.n	20006812 <strcmp+0x2aa>
200067f6:	f015 0fff 	tst.w	r5, #255	; 0xff
200067fa:	d105      	bne.n	20006808 <strcmp+0x2a0>
200067fc:	680b      	ldr	r3, [r1, #0]
200067fe:	ea4f 2212 	mov.w	r2, r2, lsr #8
20006802:	f023 437f 	bic.w	r3, r3, #4278190080	; 0xff000000
20006806:	e004      	b.n	20006812 <strcmp+0x2aa>
20006808:	f04f 0000 	mov.w	r0, #0
2000680c:	e8fd 4504 	ldrd	r4, r5, [sp], #16
20006810:	4770      	bx	lr
20006812:	ba12      	rev	r2, r2
20006814:	ba1b      	rev	r3, r3
20006816:	fa82 f44c 	uadd8	r4, r2, ip
2000681a:	ea82 0403 	eor.w	r4, r2, r3
2000681e:	faa4 f58c 	sel	r5, r4, ip
20006822:	fab5 f485 	clz	r4, r5
20006826:	fa02 f204 	lsl.w	r2, r2, r4
2000682a:	fa03 f304 	lsl.w	r3, r3, r4
2000682e:	ea4f 6012 	mov.w	r0, r2, lsr #24
20006832:	e8fd 4504 	ldrd	r4, r5, [sp], #16
20006836:	eba0 6013 	sub.w	r0, r0, r3, lsr #24
2000683a:	4770      	bx	lr

2000683c <__ssprint_r>:
2000683c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20006840:	6893      	ldr	r3, [r2, #8]
20006842:	b083      	sub	sp, #12
20006844:	4691      	mov	r9, r2
20006846:	f8d2 8000 	ldr.w	r8, [r2]
2000684a:	9001      	str	r0, [sp, #4]
2000684c:	2b00      	cmp	r3, #0
2000684e:	d073      	beq.n	20006938 <__ssprint_r+0xfc>
20006850:	f04f 0b00 	mov.w	fp, #0
20006854:	460d      	mov	r5, r1
20006856:	6808      	ldr	r0, [r1, #0]
20006858:	688b      	ldr	r3, [r1, #8]
2000685a:	465c      	mov	r4, fp
2000685c:	2c00      	cmp	r4, #0
2000685e:	d046      	beq.n	200068ee <__ssprint_r+0xb2>
20006860:	429c      	cmp	r4, r3
20006862:	461e      	mov	r6, r3
20006864:	469a      	mov	sl, r3
20006866:	d349      	bcc.n	200068fc <__ssprint_r+0xc0>
20006868:	f8b5 e00c 	ldrh.w	lr, [r5, #12]
2000686c:	f41e 6f90 	tst.w	lr, #1152	; 0x480
20006870:	d02d      	beq.n	200068ce <__ssprint_r+0x92>
20006872:	696b      	ldr	r3, [r5, #20]
20006874:	1c62      	adds	r2, r4, #1
20006876:	6929      	ldr	r1, [r5, #16]
20006878:	eb03 0343 	add.w	r3, r3, r3, lsl #1
2000687c:	1a46      	subs	r6, r0, r1
2000687e:	4610      	mov	r0, r2
20006880:	eb03 73d3 	add.w	r3, r3, r3, lsr #31
20006884:	4430      	add	r0, r6
20006886:	105f      	asrs	r7, r3, #1
20006888:	4287      	cmp	r7, r0
2000688a:	463a      	mov	r2, r7
2000688c:	d201      	bcs.n	20006892 <__ssprint_r+0x56>
2000688e:	4607      	mov	r7, r0
20006890:	4602      	mov	r2, r0
20006892:	f41e 6f80 	tst.w	lr, #1024	; 0x400
20006896:	d034      	beq.n	20006902 <__ssprint_r+0xc6>
20006898:	4611      	mov	r1, r2
2000689a:	9801      	ldr	r0, [sp, #4]
2000689c:	f7fe fe16 	bl	200054cc <_malloc_r>
200068a0:	4682      	mov	sl, r0
200068a2:	2800      	cmp	r0, #0
200068a4:	d04e      	beq.n	20006944 <__ssprint_r+0x108>
200068a6:	4632      	mov	r2, r6
200068a8:	6929      	ldr	r1, [r5, #16]
200068aa:	f7ff f903 	bl	20005ab4 <memcpy>
200068ae:	89aa      	ldrh	r2, [r5, #12]
200068b0:	f422 6290 	bic.w	r2, r2, #1152	; 0x480
200068b4:	f042 0280 	orr.w	r2, r2, #128	; 0x80
200068b8:	81aa      	strh	r2, [r5, #12]
200068ba:	eb0a 0006 	add.w	r0, sl, r6
200068be:	1bba      	subs	r2, r7, r6
200068c0:	f8c5 a010 	str.w	sl, [r5, #16]
200068c4:	4626      	mov	r6, r4
200068c6:	46a2      	mov	sl, r4
200068c8:	6028      	str	r0, [r5, #0]
200068ca:	60aa      	str	r2, [r5, #8]
200068cc:	616f      	str	r7, [r5, #20]
200068ce:	4652      	mov	r2, sl
200068d0:	4659      	mov	r1, fp
200068d2:	f000 fe9b 	bl	2000760c <memmove>
200068d6:	f8d9 2008 	ldr.w	r2, [r9, #8]
200068da:	68ab      	ldr	r3, [r5, #8]
200068dc:	6828      	ldr	r0, [r5, #0]
200068de:	1b14      	subs	r4, r2, r4
200068e0:	1b9b      	subs	r3, r3, r6
200068e2:	4450      	add	r0, sl
200068e4:	60ab      	str	r3, [r5, #8]
200068e6:	6028      	str	r0, [r5, #0]
200068e8:	f8c9 4008 	str.w	r4, [r9, #8]
200068ec:	b324      	cbz	r4, 20006938 <__ssprint_r+0xfc>
200068ee:	f8d8 b000 	ldr.w	fp, [r8]
200068f2:	f108 0808 	add.w	r8, r8, #8
200068f6:	f858 4c04 	ldr.w	r4, [r8, #-4]
200068fa:	e7af      	b.n	2000685c <__ssprint_r+0x20>
200068fc:	4626      	mov	r6, r4
200068fe:	46a2      	mov	sl, r4
20006900:	e7e5      	b.n	200068ce <__ssprint_r+0x92>
20006902:	9801      	ldr	r0, [sp, #4]
20006904:	f000 ff34 	bl	20007770 <_realloc_r>
20006908:	4682      	mov	sl, r0
2000690a:	2800      	cmp	r0, #0
2000690c:	d1d5      	bne.n	200068ba <__ssprint_r+0x7e>
2000690e:	9c01      	ldr	r4, [sp, #4]
20006910:	6929      	ldr	r1, [r5, #16]
20006912:	4620      	mov	r0, r4
20006914:	f000 fda2 	bl	2000745c <_free_r>
20006918:	230c      	movs	r3, #12
2000691a:	6023      	str	r3, [r4, #0]
2000691c:	89ab      	ldrh	r3, [r5, #12]
2000691e:	2200      	movs	r2, #0
20006920:	f04f 30ff 	mov.w	r0, #4294967295
20006924:	f043 0340 	orr.w	r3, r3, #64	; 0x40
20006928:	81ab      	strh	r3, [r5, #12]
2000692a:	f8c9 2008 	str.w	r2, [r9, #8]
2000692e:	f8c9 2004 	str.w	r2, [r9, #4]
20006932:	b003      	add	sp, #12
20006934:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20006938:	2000      	movs	r0, #0
2000693a:	f8c9 0004 	str.w	r0, [r9, #4]
2000693e:	b003      	add	sp, #12
20006940:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20006944:	230c      	movs	r3, #12
20006946:	9a01      	ldr	r2, [sp, #4]
20006948:	6013      	str	r3, [r2, #0]
2000694a:	e7e7      	b.n	2000691c <__ssprint_r+0xe0>

2000694c <_svfiprintf_r>:
2000694c:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20006950:	b0ad      	sub	sp, #180	; 0xb4
20006952:	4693      	mov	fp, r2
20006954:	9306      	str	r3, [sp, #24]
20006956:	898b      	ldrh	r3, [r1, #12]
20006958:	9100      	str	r1, [sp, #0]
2000695a:	061c      	lsls	r4, r3, #24
2000695c:	9003      	str	r0, [sp, #12]
2000695e:	d503      	bpl.n	20006968 <_svfiprintf_r+0x1c>
20006960:	690b      	ldr	r3, [r1, #16]
20006962:	2b00      	cmp	r3, #0
20006964:	f000 84c0 	beq.w	200072e8 <_svfiprintf_r+0x99c>
20006968:	f10d 0970 	add.w	r9, sp, #112	; 0x70
2000696c:	2300      	movs	r3, #0
2000696e:	46ca      	mov	sl, r9
20006970:	930a      	str	r3, [sp, #40]	; 0x28
20006972:	9311      	str	r3, [sp, #68]	; 0x44
20006974:	9302      	str	r3, [sp, #8]
20006976:	9310      	str	r3, [sp, #64]	; 0x40
20006978:	f8cd 903c 	str.w	r9, [sp, #60]	; 0x3c
2000697c:	f89b 3000 	ldrb.w	r3, [fp]
20006980:	465c      	mov	r4, fp
20006982:	2b25      	cmp	r3, #37	; 0x25
20006984:	d01b      	beq.n	200069be <_svfiprintf_r+0x72>
20006986:	b90b      	cbnz	r3, 2000698c <_svfiprintf_r+0x40>
20006988:	e019      	b.n	200069be <_svfiprintf_r+0x72>
2000698a:	b11b      	cbz	r3, 20006994 <_svfiprintf_r+0x48>
2000698c:	f814 3f01 	ldrb.w	r3, [r4, #1]!
20006990:	2b25      	cmp	r3, #37	; 0x25
20006992:	d1fa      	bne.n	2000698a <_svfiprintf_r+0x3e>
20006994:	ebb4 050b 	subs.w	r5, r4, fp
20006998:	d011      	beq.n	200069be <_svfiprintf_r+0x72>
2000699a:	9b10      	ldr	r3, [sp, #64]	; 0x40
2000699c:	9a11      	ldr	r2, [sp, #68]	; 0x44
2000699e:	3301      	adds	r3, #1
200069a0:	f8ca b000 	str.w	fp, [sl]
200069a4:	442a      	add	r2, r5
200069a6:	f8ca 5004 	str.w	r5, [sl, #4]
200069aa:	2b07      	cmp	r3, #7
200069ac:	9310      	str	r3, [sp, #64]	; 0x40
200069ae:	9211      	str	r2, [sp, #68]	; 0x44
200069b0:	f300 83b3 	bgt.w	2000711a <_svfiprintf_r+0x7ce>
200069b4:	f10a 0a08 	add.w	sl, sl, #8
200069b8:	9b02      	ldr	r3, [sp, #8]
200069ba:	442b      	add	r3, r5
200069bc:	9302      	str	r3, [sp, #8]
200069be:	7823      	ldrb	r3, [r4, #0]
200069c0:	2b00      	cmp	r3, #0
200069c2:	f000 833f 	beq.w	20007044 <_svfiprintf_r+0x6f8>
200069c6:	2100      	movs	r1, #0
200069c8:	f04f 0300 	mov.w	r3, #0
200069cc:	f104 0b01 	add.w	fp, r4, #1
200069d0:	f88d 3037 	strb.w	r3, [sp, #55]	; 0x37
200069d4:	4608      	mov	r0, r1
200069d6:	7863      	ldrb	r3, [r4, #1]
200069d8:	460f      	mov	r7, r1
200069da:	f04f 36ff 	mov.w	r6, #4294967295
200069de:	f10b 0b01 	add.w	fp, fp, #1
200069e2:	f1a3 0220 	sub.w	r2, r3, #32
200069e6:	2a58      	cmp	r2, #88	; 0x58
200069e8:	f200 823e 	bhi.w	20006e68 <_svfiprintf_r+0x51c>
200069ec:	e8df f012 	tbh	[pc, r2, lsl #1]
200069f0:	023c012c 	.word	0x023c012c
200069f4:	0127023c 	.word	0x0127023c
200069f8:	023c023c 	.word	0x023c023c
200069fc:	023c023c 	.word	0x023c023c
20006a00:	023c023c 	.word	0x023c023c
20006a04:	01230059 	.word	0x01230059
20006a08:	0063023c 	.word	0x0063023c
20006a0c:	023c021f 	.word	0x023c021f
20006a10:	019f021a 	.word	0x019f021a
20006a14:	019f019f 	.word	0x019f019f
20006a18:	019f019f 	.word	0x019f019f
20006a1c:	019f019f 	.word	0x019f019f
20006a20:	019f019f 	.word	0x019f019f
20006a24:	023c023c 	.word	0x023c023c
20006a28:	023c023c 	.word	0x023c023c
20006a2c:	023c023c 	.word	0x023c023c
20006a30:	023c023c 	.word	0x023c023c
20006a34:	023c023c 	.word	0x023c023c
20006a38:	023c0184 	.word	0x023c0184
20006a3c:	023c023c 	.word	0x023c023c
20006a40:	023c023c 	.word	0x023c023c
20006a44:	023c023c 	.word	0x023c023c
20006a48:	023c023c 	.word	0x023c023c
20006a4c:	016e023c 	.word	0x016e023c
20006a50:	023c023c 	.word	0x023c023c
20006a54:	023c023c 	.word	0x023c023c
20006a58:	015a023c 	.word	0x015a023c
20006a5c:	023c023c 	.word	0x023c023c
20006a60:	023c0146 	.word	0x023c0146
20006a64:	023c023c 	.word	0x023c023c
20006a68:	023c023c 	.word	0x023c023c
20006a6c:	023c023c 	.word	0x023c023c
20006a70:	023c023c 	.word	0x023c023c
20006a74:	0133023c 	.word	0x0133023c
20006a78:	023c01ce 	.word	0x023c01ce
20006a7c:	023c023c 	.word	0x023c023c
20006a80:	01ce01c9 	.word	0x01ce01c9
20006a84:	023c023c 	.word	0x023c023c
20006a88:	023c01c0 	.word	0x023c01c0
20006a8c:	00f401ad 	.word	0x00f401ad
20006a90:	02020207 	.word	0x02020207
20006a94:	01e3023c 	.word	0x01e3023c
20006a98:	0068023c 	.word	0x0068023c
20006a9c:	023c023c 	.word	0x023c023c
20006aa0:	0103      	.short	0x0103
20006aa2:	9a06      	ldr	r2, [sp, #24]
20006aa4:	6810      	ldr	r0, [r2, #0]
20006aa6:	4613      	mov	r3, r2
20006aa8:	2800      	cmp	r0, #0
20006aaa:	f103 0304 	add.w	r3, r3, #4
20006aae:	f280 83a8 	bge.w	20007202 <_svfiprintf_r+0x8b6>
20006ab2:	4240      	negs	r0, r0
20006ab4:	9306      	str	r3, [sp, #24]
20006ab6:	f047 0704 	orr.w	r7, r7, #4
20006aba:	f89b 3000 	ldrb.w	r3, [fp]
20006abe:	e78e      	b.n	200069de <_svfiprintf_r+0x92>
20006ac0:	9004      	str	r0, [sp, #16]
20006ac2:	06b8      	lsls	r0, r7, #26
20006ac4:	9a06      	ldr	r2, [sp, #24]
20006ac6:	f140 80f4 	bpl.w	20006cb2 <_svfiprintf_r+0x366>
20006aca:	3207      	adds	r2, #7
20006acc:	2301      	movs	r3, #1
20006ace:	f022 0207 	bic.w	r2, r2, #7
20006ad2:	f102 0108 	add.w	r1, r2, #8
20006ad6:	e9d2 4500 	ldrd	r4, r5, [r2]
20006ada:	9106      	str	r1, [sp, #24]
20006adc:	f04f 0200 	mov.w	r2, #0
20006ae0:	f04f 0800 	mov.w	r8, #0
20006ae4:	9605      	str	r6, [sp, #20]
20006ae6:	f88d 2037 	strb.w	r2, [sp, #55]	; 0x37
20006aea:	2e00      	cmp	r6, #0
20006aec:	db01      	blt.n	20006af2 <_svfiprintf_r+0x1a6>
20006aee:	f027 0780 	bic.w	r7, r7, #128	; 0x80
20006af2:	2e00      	cmp	r6, #0
20006af4:	f040 828c 	bne.w	20007010 <_svfiprintf_r+0x6c4>
20006af8:	ea54 0205 	orrs.w	r2, r4, r5
20006afc:	bf14      	ite	ne
20006afe:	2601      	movne	r6, #1
20006b00:	2600      	moveq	r6, #0
20006b02:	2e00      	cmp	r6, #0
20006b04:	f040 8284 	bne.w	20007010 <_svfiprintf_r+0x6c4>
20006b08:	2b00      	cmp	r3, #0
20006b0a:	f040 82af 	bne.w	2000706c <_svfiprintf_r+0x720>
20006b0e:	07fa      	lsls	r2, r7, #31
20006b10:	f140 82ab 	bpl.w	2000706a <_svfiprintf_r+0x71e>
20006b14:	ab2c      	add	r3, sp, #176	; 0xb0
20006b16:	2230      	movs	r2, #48	; 0x30
20006b18:	f803 2d41 	strb.w	r2, [r3, #-65]!
20006b1c:	ebc3 0609 	rsb	r6, r3, r9
20006b20:	9309      	str	r3, [sp, #36]	; 0x24
20006b22:	9b05      	ldr	r3, [sp, #20]
20006b24:	42b3      	cmp	r3, r6
20006b26:	bfb8      	it	lt
20006b28:	4633      	movlt	r3, r6
20006b2a:	9301      	str	r3, [sp, #4]
20006b2c:	f1b8 0f00 	cmp.w	r8, #0
20006b30:	d001      	beq.n	20006b36 <_svfiprintf_r+0x1ea>
20006b32:	3301      	adds	r3, #1
20006b34:	9301      	str	r3, [sp, #4]
20006b36:	f017 0302 	ands.w	r3, r7, #2
20006b3a:	9307      	str	r3, [sp, #28]
20006b3c:	d002      	beq.n	20006b44 <_svfiprintf_r+0x1f8>
20006b3e:	9b01      	ldr	r3, [sp, #4]
20006b40:	3302      	adds	r3, #2
20006b42:	9301      	str	r3, [sp, #4]
20006b44:	f017 0384 	ands.w	r3, r7, #132	; 0x84
20006b48:	9308      	str	r3, [sp, #32]
20006b4a:	f040 81a5 	bne.w	20006e98 <_svfiprintf_r+0x54c>
20006b4e:	9b04      	ldr	r3, [sp, #16]
20006b50:	9a01      	ldr	r2, [sp, #4]
20006b52:	1a9c      	subs	r4, r3, r2
20006b54:	2c00      	cmp	r4, #0
20006b56:	f340 819f 	ble.w	20006e98 <_svfiprintf_r+0x54c>
20006b5a:	2c10      	cmp	r4, #16
20006b5c:	9911      	ldr	r1, [sp, #68]	; 0x44
20006b5e:	f340 83cf 	ble.w	20007300 <_svfiprintf_r+0x9b4>
20006b62:	f8df 8330 	ldr.w	r8, [pc, #816]	; 20006e94 <_svfiprintf_r+0x548>
20006b66:	2510      	movs	r5, #16
20006b68:	970b      	str	r7, [sp, #44]	; 0x2c
20006b6a:	4627      	mov	r7, r4
20006b6c:	4643      	mov	r3, r8
20006b6e:	9a10      	ldr	r2, [sp, #64]	; 0x40
20006b70:	46b0      	mov	r8, r6
20006b72:	9c03      	ldr	r4, [sp, #12]
20006b74:	461e      	mov	r6, r3
20006b76:	e002      	b.n	20006b7e <_svfiprintf_r+0x232>
20006b78:	3f10      	subs	r7, #16
20006b7a:	2f10      	cmp	r7, #16
20006b7c:	dd19      	ble.n	20006bb2 <_svfiprintf_r+0x266>
20006b7e:	3201      	adds	r2, #1
20006b80:	3110      	adds	r1, #16
20006b82:	f8ca 6000 	str.w	r6, [sl]
20006b86:	f10a 0a08 	add.w	sl, sl, #8
20006b8a:	2a07      	cmp	r2, #7
20006b8c:	f84a 5c04 	str.w	r5, [sl, #-4]
20006b90:	9111      	str	r1, [sp, #68]	; 0x44
20006b92:	9210      	str	r2, [sp, #64]	; 0x40
20006b94:	ddf0      	ble.n	20006b78 <_svfiprintf_r+0x22c>
20006b96:	4620      	mov	r0, r4
20006b98:	9900      	ldr	r1, [sp, #0]
20006b9a:	aa0f      	add	r2, sp, #60	; 0x3c
20006b9c:	46ca      	mov	sl, r9
20006b9e:	f7ff fe4d 	bl	2000683c <__ssprint_r>
20006ba2:	2800      	cmp	r0, #0
20006ba4:	f040 8255 	bne.w	20007052 <_svfiprintf_r+0x706>
20006ba8:	3f10      	subs	r7, #16
20006baa:	9911      	ldr	r1, [sp, #68]	; 0x44
20006bac:	9a10      	ldr	r2, [sp, #64]	; 0x40
20006bae:	2f10      	cmp	r7, #16
20006bb0:	dce5      	bgt.n	20006b7e <_svfiprintf_r+0x232>
20006bb2:	4633      	mov	r3, r6
20006bb4:	463c      	mov	r4, r7
20006bb6:	4646      	mov	r6, r8
20006bb8:	9f0b      	ldr	r7, [sp, #44]	; 0x2c
20006bba:	4698      	mov	r8, r3
20006bbc:	3201      	adds	r2, #1
20006bbe:	4421      	add	r1, r4
20006bc0:	f8ca 8000 	str.w	r8, [sl]
20006bc4:	2a07      	cmp	r2, #7
20006bc6:	9111      	str	r1, [sp, #68]	; 0x44
20006bc8:	9210      	str	r2, [sp, #64]	; 0x40
20006bca:	f8ca 4004 	str.w	r4, [sl, #4]
20006bce:	f300 830c 	bgt.w	200071ea <_svfiprintf_r+0x89e>
20006bd2:	f10a 0a08 	add.w	sl, sl, #8
20006bd6:	e161      	b.n	20006e9c <_svfiprintf_r+0x550>
20006bd8:	f017 0320 	ands.w	r3, r7, #32
20006bdc:	9004      	str	r0, [sp, #16]
20006bde:	d07b      	beq.n	20006cd8 <_svfiprintf_r+0x38c>
20006be0:	9a06      	ldr	r2, [sp, #24]
20006be2:	2300      	movs	r3, #0
20006be4:	3207      	adds	r2, #7
20006be6:	f022 0207 	bic.w	r2, r2, #7
20006bea:	f102 0108 	add.w	r1, r2, #8
20006bee:	e9d2 4500 	ldrd	r4, r5, [r2]
20006bf2:	9106      	str	r1, [sp, #24]
20006bf4:	e772      	b.n	20006adc <_svfiprintf_r+0x190>
20006bf6:	4aa5      	ldr	r2, [pc, #660]	; (20006e8c <_svfiprintf_r+0x540>)
20006bf8:	06bc      	lsls	r4, r7, #26
20006bfa:	9004      	str	r0, [sp, #16]
20006bfc:	920a      	str	r2, [sp, #40]	; 0x28
20006bfe:	f88d 1037 	strb.w	r1, [sp, #55]	; 0x37
20006c02:	d542      	bpl.n	20006c8a <_svfiprintf_r+0x33e>
20006c04:	9a06      	ldr	r2, [sp, #24]
20006c06:	3207      	adds	r2, #7
20006c08:	f022 0207 	bic.w	r2, r2, #7
20006c0c:	f102 0108 	add.w	r1, r2, #8
20006c10:	e9d2 4500 	ldrd	r4, r5, [r2]
20006c14:	9106      	str	r1, [sp, #24]
20006c16:	07fa      	lsls	r2, r7, #31
20006c18:	f140 8124 	bpl.w	20006e64 <_svfiprintf_r+0x518>
20006c1c:	ea54 0205 	orrs.w	r2, r4, r5
20006c20:	f000 8120 	beq.w	20006e64 <_svfiprintf_r+0x518>
20006c24:	2230      	movs	r2, #48	; 0x30
20006c26:	f88d 3039 	strb.w	r3, [sp, #57]	; 0x39
20006c2a:	f047 0702 	orr.w	r7, r7, #2
20006c2e:	2302      	movs	r3, #2
20006c30:	f88d 2038 	strb.w	r2, [sp, #56]	; 0x38
20006c34:	e752      	b.n	20006adc <_svfiprintf_r+0x190>
20006c36:	f89b 3000 	ldrb.w	r3, [fp]
20006c3a:	212b      	movs	r1, #43	; 0x2b
20006c3c:	e6cf      	b.n	200069de <_svfiprintf_r+0x92>
20006c3e:	f047 0701 	orr.w	r7, r7, #1
20006c42:	f89b 3000 	ldrb.w	r3, [fp]
20006c46:	e6ca      	b.n	200069de <_svfiprintf_r+0x92>
20006c48:	f89b 3000 	ldrb.w	r3, [fp]
20006c4c:	2900      	cmp	r1, #0
20006c4e:	f47f aec6 	bne.w	200069de <_svfiprintf_r+0x92>
20006c52:	2120      	movs	r1, #32
20006c54:	e6c3      	b.n	200069de <_svfiprintf_r+0x92>
20006c56:	9906      	ldr	r1, [sp, #24]
20006c58:	2201      	movs	r2, #1
20006c5a:	9004      	str	r0, [sp, #16]
20006c5c:	f04f 0000 	mov.w	r0, #0
20006c60:	680b      	ldr	r3, [r1, #0]
20006c62:	4616      	mov	r6, r2
20006c64:	9201      	str	r2, [sp, #4]
20006c66:	3104      	adds	r1, #4
20006c68:	aa12      	add	r2, sp, #72	; 0x48
20006c6a:	f88d 0037 	strb.w	r0, [sp, #55]	; 0x37
20006c6e:	9106      	str	r1, [sp, #24]
20006c70:	9209      	str	r2, [sp, #36]	; 0x24
20006c72:	f88d 3048 	strb.w	r3, [sp, #72]	; 0x48
20006c76:	2300      	movs	r3, #0
20006c78:	9305      	str	r3, [sp, #20]
20006c7a:	e75c      	b.n	20006b36 <_svfiprintf_r+0x1ea>
20006c7c:	4a84      	ldr	r2, [pc, #528]	; (20006e90 <_svfiprintf_r+0x544>)
20006c7e:	06bc      	lsls	r4, r7, #26
20006c80:	9004      	str	r0, [sp, #16]
20006c82:	f88d 1037 	strb.w	r1, [sp, #55]	; 0x37
20006c86:	920a      	str	r2, [sp, #40]	; 0x28
20006c88:	d4bc      	bmi.n	20006c04 <_svfiprintf_r+0x2b8>
20006c8a:	9906      	ldr	r1, [sp, #24]
20006c8c:	06f8      	lsls	r0, r7, #27
20006c8e:	460a      	mov	r2, r1
20006c90:	f100 82d1 	bmi.w	20007236 <_svfiprintf_r+0x8ea>
20006c94:	0679      	lsls	r1, r7, #25
20006c96:	f140 82ce 	bpl.w	20007236 <_svfiprintf_r+0x8ea>
20006c9a:	8814      	ldrh	r4, [r2, #0]
20006c9c:	3204      	adds	r2, #4
20006c9e:	2500      	movs	r5, #0
20006ca0:	9206      	str	r2, [sp, #24]
20006ca2:	e7b8      	b.n	20006c16 <_svfiprintf_r+0x2ca>
20006ca4:	f047 0710 	orr.w	r7, r7, #16
20006ca8:	9004      	str	r0, [sp, #16]
20006caa:	9a06      	ldr	r2, [sp, #24]
20006cac:	06b8      	lsls	r0, r7, #26
20006cae:	f53f af0c 	bmi.w	20006aca <_svfiprintf_r+0x17e>
20006cb2:	06f9      	lsls	r1, r7, #27
20006cb4:	4613      	mov	r3, r2
20006cb6:	f100 82c3 	bmi.w	20007240 <_svfiprintf_r+0x8f4>
20006cba:	067d      	lsls	r5, r7, #25
20006cbc:	f140 82c0 	bpl.w	20007240 <_svfiprintf_r+0x8f4>
20006cc0:	8814      	ldrh	r4, [r2, #0]
20006cc2:	3204      	adds	r2, #4
20006cc4:	2500      	movs	r5, #0
20006cc6:	2301      	movs	r3, #1
20006cc8:	9206      	str	r2, [sp, #24]
20006cca:	e707      	b.n	20006adc <_svfiprintf_r+0x190>
20006ccc:	f047 0710 	orr.w	r7, r7, #16
20006cd0:	9004      	str	r0, [sp, #16]
20006cd2:	f017 0320 	ands.w	r3, r7, #32
20006cd6:	d183      	bne.n	20006be0 <_svfiprintf_r+0x294>
20006cd8:	f017 0210 	ands.w	r2, r7, #16
20006cdc:	f040 82cc 	bne.w	20007278 <_svfiprintf_r+0x92c>
20006ce0:	f017 0340 	ands.w	r3, r7, #64	; 0x40
20006ce4:	f000 82c8 	beq.w	20007278 <_svfiprintf_r+0x92c>
20006ce8:	9906      	ldr	r1, [sp, #24]
20006cea:	4613      	mov	r3, r2
20006cec:	2500      	movs	r5, #0
20006cee:	460a      	mov	r2, r1
20006cf0:	880c      	ldrh	r4, [r1, #0]
20006cf2:	3204      	adds	r2, #4
20006cf4:	9206      	str	r2, [sp, #24]
20006cf6:	e6f1      	b.n	20006adc <_svfiprintf_r+0x190>
20006cf8:	f047 0710 	orr.w	r7, r7, #16
20006cfc:	9004      	str	r0, [sp, #16]
20006cfe:	f88d 1037 	strb.w	r1, [sp, #55]	; 0x37
20006d02:	06b8      	lsls	r0, r7, #26
20006d04:	d547      	bpl.n	20006d96 <_svfiprintf_r+0x44a>
20006d06:	9906      	ldr	r1, [sp, #24]
20006d08:	3107      	adds	r1, #7
20006d0a:	f021 0107 	bic.w	r1, r1, #7
20006d0e:	e9d1 2300 	ldrd	r2, r3, [r1]
20006d12:	3108      	adds	r1, #8
20006d14:	4614      	mov	r4, r2
20006d16:	461d      	mov	r5, r3
20006d18:	9106      	str	r1, [sp, #24]
20006d1a:	2a00      	cmp	r2, #0
20006d1c:	f173 0300 	sbcs.w	r3, r3, #0
20006d20:	f2c0 829f 	blt.w	20007262 <_svfiprintf_r+0x916>
20006d24:	f89d 8037 	ldrb.w	r8, [sp, #55]	; 0x37
20006d28:	2301      	movs	r3, #1
20006d2a:	9605      	str	r6, [sp, #20]
20006d2c:	e6dd      	b.n	20006aea <_svfiprintf_r+0x19e>
20006d2e:	f1a3 0230 	sub.w	r2, r3, #48	; 0x30
20006d32:	2000      	movs	r0, #0
20006d34:	eb00 0080 	add.w	r0, r0, r0, lsl #2
20006d38:	f81b 3b01 	ldrb.w	r3, [fp], #1
20006d3c:	eb02 0040 	add.w	r0, r2, r0, lsl #1
20006d40:	f1a3 0230 	sub.w	r2, r3, #48	; 0x30
20006d44:	2a09      	cmp	r2, #9
20006d46:	d9f5      	bls.n	20006d34 <_svfiprintf_r+0x3e8>
20006d48:	e64b      	b.n	200069e2 <_svfiprintf_r+0x96>
20006d4a:	06be      	lsls	r6, r7, #26
20006d4c:	f88d 1037 	strb.w	r1, [sp, #55]	; 0x37
20006d50:	f100 8299 	bmi.w	20007286 <_svfiprintf_r+0x93a>
20006d54:	06fd      	lsls	r5, r7, #27
20006d56:	f100 82b3 	bmi.w	200072c0 <_svfiprintf_r+0x974>
20006d5a:	067c      	lsls	r4, r7, #25
20006d5c:	f140 82b0 	bpl.w	200072c0 <_svfiprintf_r+0x974>
20006d60:	9a06      	ldr	r2, [sp, #24]
20006d62:	6813      	ldr	r3, [r2, #0]
20006d64:	3204      	adds	r2, #4
20006d66:	9206      	str	r2, [sp, #24]
20006d68:	f8bd 2008 	ldrh.w	r2, [sp, #8]
20006d6c:	801a      	strh	r2, [r3, #0]
20006d6e:	e605      	b.n	2000697c <_svfiprintf_r+0x30>
20006d70:	f89b 3000 	ldrb.w	r3, [fp]
20006d74:	465a      	mov	r2, fp
20006d76:	2b6c      	cmp	r3, #108	; 0x6c
20006d78:	f000 8291 	beq.w	2000729e <_svfiprintf_r+0x952>
20006d7c:	f047 0710 	orr.w	r7, r7, #16
20006d80:	e62d      	b.n	200069de <_svfiprintf_r+0x92>
20006d82:	f047 0740 	orr.w	r7, r7, #64	; 0x40
20006d86:	f89b 3000 	ldrb.w	r3, [fp]
20006d8a:	e628      	b.n	200069de <_svfiprintf_r+0x92>
20006d8c:	9004      	str	r0, [sp, #16]
20006d8e:	06b8      	lsls	r0, r7, #26
20006d90:	f88d 1037 	strb.w	r1, [sp, #55]	; 0x37
20006d94:	d4b7      	bmi.n	20006d06 <_svfiprintf_r+0x3ba>
20006d96:	9a06      	ldr	r2, [sp, #24]
20006d98:	06f9      	lsls	r1, r7, #27
20006d9a:	4613      	mov	r3, r2
20006d9c:	f100 8256 	bmi.w	2000724c <_svfiprintf_r+0x900>
20006da0:	067a      	lsls	r2, r7, #25
20006da2:	f140 8253 	bpl.w	2000724c <_svfiprintf_r+0x900>
20006da6:	f9b3 4000 	ldrsh.w	r4, [r3]
20006daa:	3304      	adds	r3, #4
20006dac:	17e5      	asrs	r5, r4, #31
20006dae:	9306      	str	r3, [sp, #24]
20006db0:	4622      	mov	r2, r4
20006db2:	462b      	mov	r3, r5
20006db4:	e7b1      	b.n	20006d1a <_svfiprintf_r+0x3ce>
20006db6:	9a06      	ldr	r2, [sp, #24]
20006db8:	9004      	str	r0, [sp, #16]
20006dba:	6813      	ldr	r3, [r2, #0]
20006dbc:	1d15      	adds	r5, r2, #4
20006dbe:	f04f 0200 	mov.w	r2, #0
20006dc2:	9309      	str	r3, [sp, #36]	; 0x24
20006dc4:	f88d 2037 	strb.w	r2, [sp, #55]	; 0x37
20006dc8:	2b00      	cmp	r3, #0
20006dca:	f000 829d 	beq.w	20007308 <_svfiprintf_r+0x9bc>
20006dce:	2e00      	cmp	r6, #0
20006dd0:	f2c0 826c 	blt.w	200072ac <_svfiprintf_r+0x960>
20006dd4:	9c09      	ldr	r4, [sp, #36]	; 0x24
20006dd6:	2100      	movs	r1, #0
20006dd8:	4632      	mov	r2, r6
20006dda:	4620      	mov	r0, r4
20006ddc:	f7fe fe22 	bl	20005a24 <memchr>
20006de0:	2800      	cmp	r0, #0
20006de2:	f000 829e 	beq.w	20007322 <_svfiprintf_r+0x9d6>
20006de6:	2300      	movs	r3, #0
20006de8:	1b06      	subs	r6, r0, r4
20006dea:	9506      	str	r5, [sp, #24]
20006dec:	f89d 8037 	ldrb.w	r8, [sp, #55]	; 0x37
20006df0:	9305      	str	r3, [sp, #20]
20006df2:	e696      	b.n	20006b22 <_svfiprintf_r+0x1d6>
20006df4:	f047 0720 	orr.w	r7, r7, #32
20006df8:	f89b 3000 	ldrb.w	r3, [fp]
20006dfc:	e5ef      	b.n	200069de <_svfiprintf_r+0x92>
20006dfe:	9004      	str	r0, [sp, #16]
20006e00:	2278      	movs	r2, #120	; 0x78
20006e02:	9806      	ldr	r0, [sp, #24]
20006e04:	2330      	movs	r3, #48	; 0x30
20006e06:	f88d 2039 	strb.w	r2, [sp, #57]	; 0x39
20006e0a:	f047 0702 	orr.w	r7, r7, #2
20006e0e:	4601      	mov	r1, r0
20006e10:	4a1e      	ldr	r2, [pc, #120]	; (20006e8c <_svfiprintf_r+0x540>)
20006e12:	f88d 3038 	strb.w	r3, [sp, #56]	; 0x38
20006e16:	2500      	movs	r5, #0
20006e18:	3104      	adds	r1, #4
20006e1a:	6804      	ldr	r4, [r0, #0]
20006e1c:	2302      	movs	r3, #2
20006e1e:	920a      	str	r2, [sp, #40]	; 0x28
20006e20:	9106      	str	r1, [sp, #24]
20006e22:	e65b      	b.n	20006adc <_svfiprintf_r+0x190>
20006e24:	f047 0780 	orr.w	r7, r7, #128	; 0x80
20006e28:	f89b 3000 	ldrb.w	r3, [fp]
20006e2c:	e5d7      	b.n	200069de <_svfiprintf_r+0x92>
20006e2e:	f89b 3000 	ldrb.w	r3, [fp]
20006e32:	f10b 0401 	add.w	r4, fp, #1
20006e36:	2b2a      	cmp	r3, #42	; 0x2a
20006e38:	f000 8279 	beq.w	2000732e <_svfiprintf_r+0x9e2>
20006e3c:	f1a3 0230 	sub.w	r2, r3, #48	; 0x30
20006e40:	46a3      	mov	fp, r4
20006e42:	2600      	movs	r6, #0
20006e44:	2a09      	cmp	r2, #9
20006e46:	f63f adcc 	bhi.w	200069e2 <_svfiprintf_r+0x96>
20006e4a:	eb06 0686 	add.w	r6, r6, r6, lsl #2
20006e4e:	f81b 3b01 	ldrb.w	r3, [fp], #1
20006e52:	eb02 0646 	add.w	r6, r2, r6, lsl #1
20006e56:	f1a3 0230 	sub.w	r2, r3, #48	; 0x30
20006e5a:	2a09      	cmp	r2, #9
20006e5c:	d9f5      	bls.n	20006e4a <_svfiprintf_r+0x4fe>
20006e5e:	ea46 76e6 	orr.w	r6, r6, r6, asr #31
20006e62:	e5be      	b.n	200069e2 <_svfiprintf_r+0x96>
20006e64:	2302      	movs	r3, #2
20006e66:	e639      	b.n	20006adc <_svfiprintf_r+0x190>
20006e68:	9004      	str	r0, [sp, #16]
20006e6a:	f88d 1037 	strb.w	r1, [sp, #55]	; 0x37
20006e6e:	2b00      	cmp	r3, #0
20006e70:	f000 80e8 	beq.w	20007044 <_svfiprintf_r+0x6f8>
20006e74:	f88d 3048 	strb.w	r3, [sp, #72]	; 0x48
20006e78:	f04f 0300 	mov.w	r3, #0
20006e7c:	2201      	movs	r2, #1
20006e7e:	f88d 3037 	strb.w	r3, [sp, #55]	; 0x37
20006e82:	ab12      	add	r3, sp, #72	; 0x48
20006e84:	9201      	str	r2, [sp, #4]
20006e86:	4616      	mov	r6, r2
20006e88:	9309      	str	r3, [sp, #36]	; 0x24
20006e8a:	e6f4      	b.n	20006c76 <_svfiprintf_r+0x32a>
20006e8c:	200090a0 	.word	0x200090a0
20006e90:	2000908c 	.word	0x2000908c
20006e94:	20009210 	.word	0x20009210
20006e98:	9911      	ldr	r1, [sp, #68]	; 0x44
20006e9a:	9a10      	ldr	r2, [sp, #64]	; 0x40
20006e9c:	f89d 3037 	ldrb.w	r3, [sp, #55]	; 0x37
20006ea0:	b16b      	cbz	r3, 20006ebe <_svfiprintf_r+0x572>
20006ea2:	3201      	adds	r2, #1
20006ea4:	3101      	adds	r1, #1
20006ea6:	f10d 0037 	add.w	r0, sp, #55	; 0x37
20006eaa:	2301      	movs	r3, #1
20006eac:	2a07      	cmp	r2, #7
20006eae:	9111      	str	r1, [sp, #68]	; 0x44
20006eb0:	9210      	str	r2, [sp, #64]	; 0x40
20006eb2:	e88a 0009 	stmia.w	sl, {r0, r3}
20006eb6:	f300 814f 	bgt.w	20007158 <_svfiprintf_r+0x80c>
20006eba:	f10a 0a08 	add.w	sl, sl, #8
20006ebe:	9b07      	ldr	r3, [sp, #28]
20006ec0:	b163      	cbz	r3, 20006edc <_svfiprintf_r+0x590>
20006ec2:	3201      	adds	r2, #1
20006ec4:	3102      	adds	r1, #2
20006ec6:	a80e      	add	r0, sp, #56	; 0x38
20006ec8:	2302      	movs	r3, #2
20006eca:	2a07      	cmp	r2, #7
20006ecc:	9111      	str	r1, [sp, #68]	; 0x44
20006ece:	9210      	str	r2, [sp, #64]	; 0x40
20006ed0:	e88a 0009 	stmia.w	sl, {r0, r3}
20006ed4:	f300 8135 	bgt.w	20007142 <_svfiprintf_r+0x7f6>
20006ed8:	f10a 0a08 	add.w	sl, sl, #8
20006edc:	9b08      	ldr	r3, [sp, #32]
20006ede:	2b80      	cmp	r3, #128	; 0x80
20006ee0:	f000 80c7 	beq.w	20007072 <_svfiprintf_r+0x726>
20006ee4:	9b05      	ldr	r3, [sp, #20]
20006ee6:	1b9c      	subs	r4, r3, r6
20006ee8:	2c00      	cmp	r4, #0
20006eea:	dd3a      	ble.n	20006f62 <_svfiprintf_r+0x616>
20006eec:	2c10      	cmp	r4, #16
20006eee:	f8df 8468 	ldr.w	r8, [pc, #1128]	; 20007358 <_svfiprintf_r+0xa0c>
20006ef2:	dd29      	ble.n	20006f48 <_svfiprintf_r+0x5fc>
20006ef4:	4643      	mov	r3, r8
20006ef6:	9705      	str	r7, [sp, #20]
20006ef8:	46b0      	mov	r8, r6
20006efa:	4627      	mov	r7, r4
20006efc:	2510      	movs	r5, #16
20006efe:	9c03      	ldr	r4, [sp, #12]
20006f00:	461e      	mov	r6, r3
20006f02:	e002      	b.n	20006f0a <_svfiprintf_r+0x5be>
20006f04:	3f10      	subs	r7, #16
20006f06:	2f10      	cmp	r7, #16
20006f08:	dd19      	ble.n	20006f3e <_svfiprintf_r+0x5f2>
20006f0a:	3201      	adds	r2, #1
20006f0c:	3110      	adds	r1, #16
20006f0e:	f8ca 6000 	str.w	r6, [sl]
20006f12:	f10a 0a08 	add.w	sl, sl, #8
20006f16:	2a07      	cmp	r2, #7
20006f18:	f84a 5c04 	str.w	r5, [sl, #-4]
20006f1c:	9111      	str	r1, [sp, #68]	; 0x44
20006f1e:	9210      	str	r2, [sp, #64]	; 0x40
20006f20:	ddf0      	ble.n	20006f04 <_svfiprintf_r+0x5b8>
20006f22:	4620      	mov	r0, r4
20006f24:	9900      	ldr	r1, [sp, #0]
20006f26:	aa0f      	add	r2, sp, #60	; 0x3c
20006f28:	46ca      	mov	sl, r9
20006f2a:	f7ff fc87 	bl	2000683c <__ssprint_r>
20006f2e:	2800      	cmp	r0, #0
20006f30:	f040 808f 	bne.w	20007052 <_svfiprintf_r+0x706>
20006f34:	3f10      	subs	r7, #16
20006f36:	9911      	ldr	r1, [sp, #68]	; 0x44
20006f38:	9a10      	ldr	r2, [sp, #64]	; 0x40
20006f3a:	2f10      	cmp	r7, #16
20006f3c:	dce5      	bgt.n	20006f0a <_svfiprintf_r+0x5be>
20006f3e:	4633      	mov	r3, r6
20006f40:	463c      	mov	r4, r7
20006f42:	4646      	mov	r6, r8
20006f44:	9f05      	ldr	r7, [sp, #20]
20006f46:	4698      	mov	r8, r3
20006f48:	3201      	adds	r2, #1
20006f4a:	4421      	add	r1, r4
20006f4c:	f8ca 8000 	str.w	r8, [sl]
20006f50:	2a07      	cmp	r2, #7
20006f52:	9111      	str	r1, [sp, #68]	; 0x44
20006f54:	9210      	str	r2, [sp, #64]	; 0x40
20006f56:	f8ca 4004 	str.w	r4, [sl, #4]
20006f5a:	f300 80e7 	bgt.w	2000712c <_svfiprintf_r+0x7e0>
20006f5e:	f10a 0a08 	add.w	sl, sl, #8
20006f62:	3201      	adds	r2, #1
20006f64:	4431      	add	r1, r6
20006f66:	9b09      	ldr	r3, [sp, #36]	; 0x24
20006f68:	2a07      	cmp	r2, #7
20006f6a:	9111      	str	r1, [sp, #68]	; 0x44
20006f6c:	f8ca 3000 	str.w	r3, [sl]
20006f70:	9210      	str	r2, [sp, #64]	; 0x40
20006f72:	f8ca 6004 	str.w	r6, [sl, #4]
20006f76:	f300 80bd 	bgt.w	200070f4 <_svfiprintf_r+0x7a8>
20006f7a:	f10a 0a08 	add.w	sl, sl, #8
20006f7e:	077b      	lsls	r3, r7, #29
20006f80:	d538      	bpl.n	20006ff4 <_svfiprintf_r+0x6a8>
20006f82:	9b04      	ldr	r3, [sp, #16]
20006f84:	9a01      	ldr	r2, [sp, #4]
20006f86:	1a9c      	subs	r4, r3, r2
20006f88:	2c00      	cmp	r4, #0
20006f8a:	dd33      	ble.n	20006ff4 <_svfiprintf_r+0x6a8>
20006f8c:	2c10      	cmp	r4, #16
20006f8e:	9a10      	ldr	r2, [sp, #64]	; 0x40
20006f90:	f340 81c4 	ble.w	2000731c <_svfiprintf_r+0x9d0>
20006f94:	f8df 83c4 	ldr.w	r8, [pc, #964]	; 2000735c <_svfiprintf_r+0xa10>
20006f98:	2610      	movs	r6, #16
20006f9a:	9d03      	ldr	r5, [sp, #12]
20006f9c:	4647      	mov	r7, r8
20006f9e:	f8dd 8000 	ldr.w	r8, [sp]
20006fa2:	e002      	b.n	20006faa <_svfiprintf_r+0x65e>
20006fa4:	3c10      	subs	r4, #16
20006fa6:	2c10      	cmp	r4, #16
20006fa8:	dd18      	ble.n	20006fdc <_svfiprintf_r+0x690>
20006faa:	3201      	adds	r2, #1
20006fac:	3110      	adds	r1, #16
20006fae:	f8ca 7000 	str.w	r7, [sl]
20006fb2:	f10a 0a08 	add.w	sl, sl, #8
20006fb6:	2a07      	cmp	r2, #7
20006fb8:	f84a 6c04 	str.w	r6, [sl, #-4]
20006fbc:	9111      	str	r1, [sp, #68]	; 0x44
20006fbe:	9210      	str	r2, [sp, #64]	; 0x40
20006fc0:	ddf0      	ble.n	20006fa4 <_svfiprintf_r+0x658>
20006fc2:	4628      	mov	r0, r5
20006fc4:	4641      	mov	r1, r8
20006fc6:	aa0f      	add	r2, sp, #60	; 0x3c
20006fc8:	46ca      	mov	sl, r9
20006fca:	f7ff fc37 	bl	2000683c <__ssprint_r>
20006fce:	2800      	cmp	r0, #0
20006fd0:	d13f      	bne.n	20007052 <_svfiprintf_r+0x706>
20006fd2:	3c10      	subs	r4, #16
20006fd4:	9911      	ldr	r1, [sp, #68]	; 0x44
20006fd6:	9a10      	ldr	r2, [sp, #64]	; 0x40
20006fd8:	2c10      	cmp	r4, #16
20006fda:	dce6      	bgt.n	20006faa <_svfiprintf_r+0x65e>
20006fdc:	46b8      	mov	r8, r7
20006fde:	3201      	adds	r2, #1
20006fe0:	4421      	add	r1, r4
20006fe2:	f8ca 8000 	str.w	r8, [sl]
20006fe6:	2a07      	cmp	r2, #7
20006fe8:	9111      	str	r1, [sp, #68]	; 0x44
20006fea:	9210      	str	r2, [sp, #64]	; 0x40
20006fec:	f8ca 4004 	str.w	r4, [sl, #4]
20006ff0:	f300 810c 	bgt.w	2000720c <_svfiprintf_r+0x8c0>
20006ff4:	9b02      	ldr	r3, [sp, #8]
20006ff6:	9a01      	ldr	r2, [sp, #4]
20006ff8:	9804      	ldr	r0, [sp, #16]
20006ffa:	4282      	cmp	r2, r0
20006ffc:	bfac      	ite	ge
20006ffe:	189b      	addge	r3, r3, r2
20007000:	181b      	addlt	r3, r3, r0
20007002:	9302      	str	r3, [sp, #8]
20007004:	2900      	cmp	r1, #0
20007006:	d17f      	bne.n	20007108 <_svfiprintf_r+0x7bc>
20007008:	2300      	movs	r3, #0
2000700a:	46ca      	mov	sl, r9
2000700c:	9310      	str	r3, [sp, #64]	; 0x40
2000700e:	e4b5      	b.n	2000697c <_svfiprintf_r+0x30>
20007010:	2b01      	cmp	r3, #1
20007012:	f000 80cb 	beq.w	200071ac <_svfiprintf_r+0x860>
20007016:	2b02      	cmp	r3, #2
20007018:	f040 80aa 	bne.w	20007170 <_svfiprintf_r+0x824>
2000701c:	464a      	mov	r2, r9
2000701e:	9e0a      	ldr	r6, [sp, #40]	; 0x28
20007020:	0923      	lsrs	r3, r4, #4
20007022:	f004 000f 	and.w	r0, r4, #15
20007026:	0929      	lsrs	r1, r5, #4
20007028:	ea43 7305 	orr.w	r3, r3, r5, lsl #28
2000702c:	460d      	mov	r5, r1
2000702e:	461c      	mov	r4, r3
20007030:	5c33      	ldrb	r3, [r6, r0]
20007032:	f802 3d01 	strb.w	r3, [r2, #-1]!
20007036:	ea54 0305 	orrs.w	r3, r4, r5
2000703a:	d1f1      	bne.n	20007020 <_svfiprintf_r+0x6d4>
2000703c:	9209      	str	r2, [sp, #36]	; 0x24
2000703e:	ebc2 0609 	rsb	r6, r2, r9
20007042:	e56e      	b.n	20006b22 <_svfiprintf_r+0x1d6>
20007044:	9b11      	ldr	r3, [sp, #68]	; 0x44
20007046:	b123      	cbz	r3, 20007052 <_svfiprintf_r+0x706>
20007048:	9803      	ldr	r0, [sp, #12]
2000704a:	aa0f      	add	r2, sp, #60	; 0x3c
2000704c:	9900      	ldr	r1, [sp, #0]
2000704e:	f7ff fbf5 	bl	2000683c <__ssprint_r>
20007052:	9b00      	ldr	r3, [sp, #0]
20007054:	899b      	ldrh	r3, [r3, #12]
20007056:	f013 0f40 	tst.w	r3, #64	; 0x40
2000705a:	9b02      	ldr	r3, [sp, #8]
2000705c:	bf18      	it	ne
2000705e:	f04f 33ff 	movne.w	r3, #4294967295
20007062:	4618      	mov	r0, r3
20007064:	b02d      	add	sp, #180	; 0xb4
20007066:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
2000706a:	461e      	mov	r6, r3
2000706c:	f8cd 9024 	str.w	r9, [sp, #36]	; 0x24
20007070:	e557      	b.n	20006b22 <_svfiprintf_r+0x1d6>
20007072:	9b04      	ldr	r3, [sp, #16]
20007074:	9801      	ldr	r0, [sp, #4]
20007076:	1a1c      	subs	r4, r3, r0
20007078:	2c00      	cmp	r4, #0
2000707a:	f77f af33 	ble.w	20006ee4 <_svfiprintf_r+0x598>
2000707e:	2c10      	cmp	r4, #16
20007080:	f8df 82d4 	ldr.w	r8, [pc, #724]	; 20007358 <_svfiprintf_r+0xa0c>
20007084:	dd28      	ble.n	200070d8 <_svfiprintf_r+0x78c>
20007086:	4643      	mov	r3, r8
20007088:	9707      	str	r7, [sp, #28]
2000708a:	46b0      	mov	r8, r6
2000708c:	4627      	mov	r7, r4
2000708e:	2510      	movs	r5, #16
20007090:	9c03      	ldr	r4, [sp, #12]
20007092:	461e      	mov	r6, r3
20007094:	e002      	b.n	2000709c <_svfiprintf_r+0x750>
20007096:	3f10      	subs	r7, #16
20007098:	2f10      	cmp	r7, #16
2000709a:	dd18      	ble.n	200070ce <_svfiprintf_r+0x782>
2000709c:	3201      	adds	r2, #1
2000709e:	3110      	adds	r1, #16
200070a0:	f8ca 6000 	str.w	r6, [sl]
200070a4:	f10a 0a08 	add.w	sl, sl, #8
200070a8:	2a07      	cmp	r2, #7
200070aa:	f84a 5c04 	str.w	r5, [sl, #-4]
200070ae:	9111      	str	r1, [sp, #68]	; 0x44
200070b0:	9210      	str	r2, [sp, #64]	; 0x40
200070b2:	ddf0      	ble.n	20007096 <_svfiprintf_r+0x74a>
200070b4:	4620      	mov	r0, r4
200070b6:	9900      	ldr	r1, [sp, #0]
200070b8:	aa0f      	add	r2, sp, #60	; 0x3c
200070ba:	46ca      	mov	sl, r9
200070bc:	f7ff fbbe 	bl	2000683c <__ssprint_r>
200070c0:	2800      	cmp	r0, #0
200070c2:	d1c6      	bne.n	20007052 <_svfiprintf_r+0x706>
200070c4:	3f10      	subs	r7, #16
200070c6:	9911      	ldr	r1, [sp, #68]	; 0x44
200070c8:	9a10      	ldr	r2, [sp, #64]	; 0x40
200070ca:	2f10      	cmp	r7, #16
200070cc:	dce6      	bgt.n	2000709c <_svfiprintf_r+0x750>
200070ce:	4633      	mov	r3, r6
200070d0:	463c      	mov	r4, r7
200070d2:	4646      	mov	r6, r8
200070d4:	9f07      	ldr	r7, [sp, #28]
200070d6:	4698      	mov	r8, r3
200070d8:	3201      	adds	r2, #1
200070da:	4421      	add	r1, r4
200070dc:	f8ca 8000 	str.w	r8, [sl]
200070e0:	2a07      	cmp	r2, #7
200070e2:	9111      	str	r1, [sp, #68]	; 0x44
200070e4:	9210      	str	r2, [sp, #64]	; 0x40
200070e6:	f8ca 4004 	str.w	r4, [sl, #4]
200070ea:	f300 80f1 	bgt.w	200072d0 <_svfiprintf_r+0x984>
200070ee:	f10a 0a08 	add.w	sl, sl, #8
200070f2:	e6f7      	b.n	20006ee4 <_svfiprintf_r+0x598>
200070f4:	9803      	ldr	r0, [sp, #12]
200070f6:	aa0f      	add	r2, sp, #60	; 0x3c
200070f8:	9900      	ldr	r1, [sp, #0]
200070fa:	f7ff fb9f 	bl	2000683c <__ssprint_r>
200070fe:	2800      	cmp	r0, #0
20007100:	d1a7      	bne.n	20007052 <_svfiprintf_r+0x706>
20007102:	9911      	ldr	r1, [sp, #68]	; 0x44
20007104:	46ca      	mov	sl, r9
20007106:	e73a      	b.n	20006f7e <_svfiprintf_r+0x632>
20007108:	9803      	ldr	r0, [sp, #12]
2000710a:	aa0f      	add	r2, sp, #60	; 0x3c
2000710c:	9900      	ldr	r1, [sp, #0]
2000710e:	f7ff fb95 	bl	2000683c <__ssprint_r>
20007112:	2800      	cmp	r0, #0
20007114:	f43f af78 	beq.w	20007008 <_svfiprintf_r+0x6bc>
20007118:	e79b      	b.n	20007052 <_svfiprintf_r+0x706>
2000711a:	9803      	ldr	r0, [sp, #12]
2000711c:	aa0f      	add	r2, sp, #60	; 0x3c
2000711e:	9900      	ldr	r1, [sp, #0]
20007120:	f7ff fb8c 	bl	2000683c <__ssprint_r>
20007124:	2800      	cmp	r0, #0
20007126:	d194      	bne.n	20007052 <_svfiprintf_r+0x706>
20007128:	46ca      	mov	sl, r9
2000712a:	e445      	b.n	200069b8 <_svfiprintf_r+0x6c>
2000712c:	9803      	ldr	r0, [sp, #12]
2000712e:	aa0f      	add	r2, sp, #60	; 0x3c
20007130:	9900      	ldr	r1, [sp, #0]
20007132:	f7ff fb83 	bl	2000683c <__ssprint_r>
20007136:	2800      	cmp	r0, #0
20007138:	d18b      	bne.n	20007052 <_svfiprintf_r+0x706>
2000713a:	9911      	ldr	r1, [sp, #68]	; 0x44
2000713c:	46ca      	mov	sl, r9
2000713e:	9a10      	ldr	r2, [sp, #64]	; 0x40
20007140:	e70f      	b.n	20006f62 <_svfiprintf_r+0x616>
20007142:	9803      	ldr	r0, [sp, #12]
20007144:	aa0f      	add	r2, sp, #60	; 0x3c
20007146:	9900      	ldr	r1, [sp, #0]
20007148:	f7ff fb78 	bl	2000683c <__ssprint_r>
2000714c:	2800      	cmp	r0, #0
2000714e:	d180      	bne.n	20007052 <_svfiprintf_r+0x706>
20007150:	9911      	ldr	r1, [sp, #68]	; 0x44
20007152:	46ca      	mov	sl, r9
20007154:	9a10      	ldr	r2, [sp, #64]	; 0x40
20007156:	e6c1      	b.n	20006edc <_svfiprintf_r+0x590>
20007158:	9803      	ldr	r0, [sp, #12]
2000715a:	aa0f      	add	r2, sp, #60	; 0x3c
2000715c:	9900      	ldr	r1, [sp, #0]
2000715e:	f7ff fb6d 	bl	2000683c <__ssprint_r>
20007162:	2800      	cmp	r0, #0
20007164:	f47f af75 	bne.w	20007052 <_svfiprintf_r+0x706>
20007168:	9911      	ldr	r1, [sp, #68]	; 0x44
2000716a:	46ca      	mov	sl, r9
2000716c:	9a10      	ldr	r2, [sp, #64]	; 0x40
2000716e:	e6a6      	b.n	20006ebe <_svfiprintf_r+0x572>
20007170:	4649      	mov	r1, r9
20007172:	08e2      	lsrs	r2, r4, #3
20007174:	f004 0307 	and.w	r3, r4, #7
20007178:	08e8      	lsrs	r0, r5, #3
2000717a:	ea42 7245 	orr.w	r2, r2, r5, lsl #29
2000717e:	3330      	adds	r3, #48	; 0x30
20007180:	4605      	mov	r5, r0
20007182:	4614      	mov	r4, r2
20007184:	f801 3d01 	strb.w	r3, [r1, #-1]!
20007188:	ea54 0205 	orrs.w	r2, r4, r5
2000718c:	d1f1      	bne.n	20007172 <_svfiprintf_r+0x826>
2000718e:	07f8      	lsls	r0, r7, #31
20007190:	9109      	str	r1, [sp, #36]	; 0x24
20007192:	d54d      	bpl.n	20007230 <_svfiprintf_r+0x8e4>
20007194:	2b30      	cmp	r3, #48	; 0x30
20007196:	9b09      	ldr	r3, [sp, #36]	; 0x24
20007198:	d024      	beq.n	200071e4 <_svfiprintf_r+0x898>
2000719a:	3b01      	subs	r3, #1
2000719c:	461a      	mov	r2, r3
2000719e:	9309      	str	r3, [sp, #36]	; 0x24
200071a0:	2330      	movs	r3, #48	; 0x30
200071a2:	ebc2 0609 	rsb	r6, r2, r9
200071a6:	f801 3c01 	strb.w	r3, [r1, #-1]
200071aa:	e4ba      	b.n	20006b22 <_svfiprintf_r+0x1d6>
200071ac:	2d00      	cmp	r5, #0
200071ae:	bf08      	it	eq
200071b0:	2c0a      	cmpeq	r4, #10
200071b2:	d335      	bcc.n	20007220 <_svfiprintf_r+0x8d4>
200071b4:	464e      	mov	r6, r9
200071b6:	4620      	mov	r0, r4
200071b8:	4629      	mov	r1, r5
200071ba:	220a      	movs	r2, #10
200071bc:	2300      	movs	r3, #0
200071be:	f001 f99b 	bl	200084f8 <__aeabi_uldivmod>
200071c2:	f102 0e30 	add.w	lr, r2, #48	; 0x30
200071c6:	4620      	mov	r0, r4
200071c8:	4629      	mov	r1, r5
200071ca:	2300      	movs	r3, #0
200071cc:	220a      	movs	r2, #10
200071ce:	f806 ed01 	strb.w	lr, [r6, #-1]!
200071d2:	f001 f991 	bl	200084f8 <__aeabi_uldivmod>
200071d6:	4604      	mov	r4, r0
200071d8:	460d      	mov	r5, r1
200071da:	ea54 0305 	orrs.w	r3, r4, r5
200071de:	d1ea      	bne.n	200071b6 <_svfiprintf_r+0x86a>
200071e0:	4633      	mov	r3, r6
200071e2:	9609      	str	r6, [sp, #36]	; 0x24
200071e4:	ebc3 0609 	rsb	r6, r3, r9
200071e8:	e49b      	b.n	20006b22 <_svfiprintf_r+0x1d6>
200071ea:	9803      	ldr	r0, [sp, #12]
200071ec:	aa0f      	add	r2, sp, #60	; 0x3c
200071ee:	9900      	ldr	r1, [sp, #0]
200071f0:	f7ff fb24 	bl	2000683c <__ssprint_r>
200071f4:	2800      	cmp	r0, #0
200071f6:	f47f af2c 	bne.w	20007052 <_svfiprintf_r+0x706>
200071fa:	9911      	ldr	r1, [sp, #68]	; 0x44
200071fc:	46ca      	mov	sl, r9
200071fe:	9a10      	ldr	r2, [sp, #64]	; 0x40
20007200:	e64c      	b.n	20006e9c <_svfiprintf_r+0x550>
20007202:	9306      	str	r3, [sp, #24]
20007204:	f89b 3000 	ldrb.w	r3, [fp]
20007208:	f7ff bbe9 	b.w	200069de <_svfiprintf_r+0x92>
2000720c:	9803      	ldr	r0, [sp, #12]
2000720e:	aa0f      	add	r2, sp, #60	; 0x3c
20007210:	9900      	ldr	r1, [sp, #0]
20007212:	f7ff fb13 	bl	2000683c <__ssprint_r>
20007216:	2800      	cmp	r0, #0
20007218:	f47f af1b 	bne.w	20007052 <_svfiprintf_r+0x706>
2000721c:	9911      	ldr	r1, [sp, #68]	; 0x44
2000721e:	e6e9      	b.n	20006ff4 <_svfiprintf_r+0x6a8>
20007220:	ab2c      	add	r3, sp, #176	; 0xb0
20007222:	3430      	adds	r4, #48	; 0x30
20007224:	f803 4d41 	strb.w	r4, [r3, #-65]!
20007228:	ebc3 0609 	rsb	r6, r3, r9
2000722c:	9309      	str	r3, [sp, #36]	; 0x24
2000722e:	e478      	b.n	20006b22 <_svfiprintf_r+0x1d6>
20007230:	ebc1 0609 	rsb	r6, r1, r9
20007234:	e475      	b.n	20006b22 <_svfiprintf_r+0x1d6>
20007236:	6814      	ldr	r4, [r2, #0]
20007238:	3204      	adds	r2, #4
2000723a:	2500      	movs	r5, #0
2000723c:	9206      	str	r2, [sp, #24]
2000723e:	e4ea      	b.n	20006c16 <_svfiprintf_r+0x2ca>
20007240:	3204      	adds	r2, #4
20007242:	681c      	ldr	r4, [r3, #0]
20007244:	2500      	movs	r5, #0
20007246:	2301      	movs	r3, #1
20007248:	9206      	str	r2, [sp, #24]
2000724a:	e447      	b.n	20006adc <_svfiprintf_r+0x190>
2000724c:	681c      	ldr	r4, [r3, #0]
2000724e:	3304      	adds	r3, #4
20007250:	17e5      	asrs	r5, r4, #31
20007252:	4622      	mov	r2, r4
20007254:	9306      	str	r3, [sp, #24]
20007256:	2a00      	cmp	r2, #0
20007258:	462b      	mov	r3, r5
2000725a:	f173 0300 	sbcs.w	r3, r3, #0
2000725e:	f6bf ad61 	bge.w	20006d24 <_svfiprintf_r+0x3d8>
20007262:	f04f 082d 	mov.w	r8, #45	; 0x2d
20007266:	4264      	negs	r4, r4
20007268:	9605      	str	r6, [sp, #20]
2000726a:	f04f 0301 	mov.w	r3, #1
2000726e:	eb65 0545 	sbc.w	r5, r5, r5, lsl #1
20007272:	f88d 8037 	strb.w	r8, [sp, #55]	; 0x37
20007276:	e438      	b.n	20006aea <_svfiprintf_r+0x19e>
20007278:	9906      	ldr	r1, [sp, #24]
2000727a:	2500      	movs	r5, #0
2000727c:	460a      	mov	r2, r1
2000727e:	680c      	ldr	r4, [r1, #0]
20007280:	3204      	adds	r2, #4
20007282:	9206      	str	r2, [sp, #24]
20007284:	e42a      	b.n	20006adc <_svfiprintf_r+0x190>
20007286:	9902      	ldr	r1, [sp, #8]
20007288:	9a06      	ldr	r2, [sp, #24]
2000728a:	17cd      	asrs	r5, r1, #31
2000728c:	4608      	mov	r0, r1
2000728e:	6813      	ldr	r3, [r2, #0]
20007290:	3204      	adds	r2, #4
20007292:	4629      	mov	r1, r5
20007294:	9206      	str	r2, [sp, #24]
20007296:	e9c3 0100 	strd	r0, r1, [r3]
2000729a:	f7ff bb6f 	b.w	2000697c <_svfiprintf_r+0x30>
2000729e:	f10b 0b01 	add.w	fp, fp, #1
200072a2:	f047 0720 	orr.w	r7, r7, #32
200072a6:	7853      	ldrb	r3, [r2, #1]
200072a8:	f7ff bb99 	b.w	200069de <_svfiprintf_r+0x92>
200072ac:	9809      	ldr	r0, [sp, #36]	; 0x24
200072ae:	9506      	str	r5, [sp, #24]
200072b0:	f7fb fdd8 	bl	20002e64 <strlen>
200072b4:	2300      	movs	r3, #0
200072b6:	4606      	mov	r6, r0
200072b8:	f89d 8037 	ldrb.w	r8, [sp, #55]	; 0x37
200072bc:	9305      	str	r3, [sp, #20]
200072be:	e430      	b.n	20006b22 <_svfiprintf_r+0x1d6>
200072c0:	9a06      	ldr	r2, [sp, #24]
200072c2:	6813      	ldr	r3, [r2, #0]
200072c4:	3204      	adds	r2, #4
200072c6:	9206      	str	r2, [sp, #24]
200072c8:	9a02      	ldr	r2, [sp, #8]
200072ca:	601a      	str	r2, [r3, #0]
200072cc:	f7ff bb56 	b.w	2000697c <_svfiprintf_r+0x30>
200072d0:	9803      	ldr	r0, [sp, #12]
200072d2:	aa0f      	add	r2, sp, #60	; 0x3c
200072d4:	9900      	ldr	r1, [sp, #0]
200072d6:	f7ff fab1 	bl	2000683c <__ssprint_r>
200072da:	2800      	cmp	r0, #0
200072dc:	f47f aeb9 	bne.w	20007052 <_svfiprintf_r+0x706>
200072e0:	9911      	ldr	r1, [sp, #68]	; 0x44
200072e2:	46ca      	mov	sl, r9
200072e4:	9a10      	ldr	r2, [sp, #64]	; 0x40
200072e6:	e5fd      	b.n	20006ee4 <_svfiprintf_r+0x598>
200072e8:	2140      	movs	r1, #64	; 0x40
200072ea:	f7fe f8ef 	bl	200054cc <_malloc_r>
200072ee:	9a00      	ldr	r2, [sp, #0]
200072f0:	6010      	str	r0, [r2, #0]
200072f2:	6110      	str	r0, [r2, #16]
200072f4:	b338      	cbz	r0, 20007346 <_svfiprintf_r+0x9fa>
200072f6:	2340      	movs	r3, #64	; 0x40
200072f8:	9a00      	ldr	r2, [sp, #0]
200072fa:	6153      	str	r3, [r2, #20]
200072fc:	f7ff bb34 	b.w	20006968 <_svfiprintf_r+0x1c>
20007300:	9a10      	ldr	r2, [sp, #64]	; 0x40
20007302:	f8df 8058 	ldr.w	r8, [pc, #88]	; 2000735c <_svfiprintf_r+0xa10>
20007306:	e459      	b.n	20006bbc <_svfiprintf_r+0x270>
20007308:	2e06      	cmp	r6, #6
2000730a:	4b12      	ldr	r3, [pc, #72]	; (20007354 <_svfiprintf_r+0xa08>)
2000730c:	9506      	str	r5, [sp, #24]
2000730e:	bf28      	it	cs
20007310:	2606      	movcs	r6, #6
20007312:	9309      	str	r3, [sp, #36]	; 0x24
20007314:	ea26 73e6 	bic.w	r3, r6, r6, asr #31
20007318:	9301      	str	r3, [sp, #4]
2000731a:	e4ac      	b.n	20006c76 <_svfiprintf_r+0x32a>
2000731c:	f8df 803c 	ldr.w	r8, [pc, #60]	; 2000735c <_svfiprintf_r+0xa10>
20007320:	e65d      	b.n	20006fde <_svfiprintf_r+0x692>
20007322:	9506      	str	r5, [sp, #24]
20007324:	9005      	str	r0, [sp, #20]
20007326:	f89d 8037 	ldrb.w	r8, [sp, #55]	; 0x37
2000732a:	f7ff bbfa 	b.w	20006b22 <_svfiprintf_r+0x1d6>
2000732e:	9a06      	ldr	r2, [sp, #24]
20007330:	465b      	mov	r3, fp
20007332:	46a3      	mov	fp, r4
20007334:	6816      	ldr	r6, [r2, #0]
20007336:	3204      	adds	r2, #4
20007338:	785b      	ldrb	r3, [r3, #1]
2000733a:	2e00      	cmp	r6, #0
2000733c:	9206      	str	r2, [sp, #24]
2000733e:	f6bf ab4e 	bge.w	200069de <_svfiprintf_r+0x92>
20007342:	f7ff bb4a 	b.w	200069da <_svfiprintf_r+0x8e>
20007346:	230c      	movs	r3, #12
20007348:	9a03      	ldr	r2, [sp, #12]
2000734a:	f04f 30ff 	mov.w	r0, #4294967295
2000734e:	6013      	str	r3, [r2, #0]
20007350:	e688      	b.n	20007064 <_svfiprintf_r+0x718>
20007352:	bf00      	nop
20007354:	200090b4 	.word	0x200090b4
20007358:	20009220 	.word	0x20009220
2000735c:	20009210 	.word	0x20009210

20007360 <_calloc_r>:
20007360:	b510      	push	{r4, lr}
20007362:	fb02 f101 	mul.w	r1, r2, r1
20007366:	f7fe f8b1 	bl	200054cc <_malloc_r>
2000736a:	4604      	mov	r4, r0
2000736c:	b168      	cbz	r0, 2000738a <_calloc_r+0x2a>
2000736e:	f850 2c04 	ldr.w	r2, [r0, #-4]
20007372:	f022 0203 	bic.w	r2, r2, #3
20007376:	3a04      	subs	r2, #4
20007378:	2a24      	cmp	r2, #36	; 0x24
2000737a:	d814      	bhi.n	200073a6 <_calloc_r+0x46>
2000737c:	2a13      	cmp	r2, #19
2000737e:	d806      	bhi.n	2000738e <_calloc_r+0x2e>
20007380:	4603      	mov	r3, r0
20007382:	2200      	movs	r2, #0
20007384:	601a      	str	r2, [r3, #0]
20007386:	605a      	str	r2, [r3, #4]
20007388:	609a      	str	r2, [r3, #8]
2000738a:	4620      	mov	r0, r4
2000738c:	bd10      	pop	{r4, pc}
2000738e:	2100      	movs	r1, #0
20007390:	2a1b      	cmp	r2, #27
20007392:	6001      	str	r1, [r0, #0]
20007394:	6041      	str	r1, [r0, #4]
20007396:	d90b      	bls.n	200073b0 <_calloc_r+0x50>
20007398:	2a24      	cmp	r2, #36	; 0x24
2000739a:	6081      	str	r1, [r0, #8]
2000739c:	60c1      	str	r1, [r0, #12]
2000739e:	d00a      	beq.n	200073b6 <_calloc_r+0x56>
200073a0:	f100 0310 	add.w	r3, r0, #16
200073a4:	e7ed      	b.n	20007382 <_calloc_r+0x22>
200073a6:	2100      	movs	r1, #0
200073a8:	f000 f992 	bl	200076d0 <memset>
200073ac:	4620      	mov	r0, r4
200073ae:	bd10      	pop	{r4, pc}
200073b0:	f100 0308 	add.w	r3, r0, #8
200073b4:	e7e5      	b.n	20007382 <_calloc_r+0x22>
200073b6:	6101      	str	r1, [r0, #16]
200073b8:	f100 0318 	add.w	r3, r0, #24
200073bc:	6141      	str	r1, [r0, #20]
200073be:	e7e0      	b.n	20007382 <_calloc_r+0x22>

200073c0 <_malloc_trim_r>:
200073c0:	b5f8      	push	{r3, r4, r5, r6, r7, lr}
200073c2:	4f22      	ldr	r7, [pc, #136]	; (2000744c <_malloc_trim_r+0x8c>)
200073c4:	460c      	mov	r4, r1
200073c6:	4606      	mov	r6, r0
200073c8:	f7fe fc0e 	bl	20005be8 <__malloc_lock>
200073cc:	68ba      	ldr	r2, [r7, #8]
200073ce:	4b20      	ldr	r3, [pc, #128]	; (20007450 <_malloc_trim_r+0x90>)
200073d0:	6855      	ldr	r5, [r2, #4]
200073d2:	f025 0503 	bic.w	r5, r5, #3
200073d6:	1b2c      	subs	r4, r5, r4
200073d8:	f604 74ef 	addw	r4, r4, #4079	; 0xfef
200073dc:	4023      	ands	r3, r4
200073de:	f5a3 5480 	sub.w	r4, r3, #4096	; 0x1000
200073e2:	f5b4 5f80 	cmp.w	r4, #4096	; 0x1000
200073e6:	db07      	blt.n	200073f8 <_malloc_trim_r+0x38>
200073e8:	4630      	mov	r0, r6
200073ea:	2100      	movs	r1, #0
200073ec:	f7ff f89a 	bl	20006524 <_sbrk_r>
200073f0:	68bb      	ldr	r3, [r7, #8]
200073f2:	442b      	add	r3, r5
200073f4:	4298      	cmp	r0, r3
200073f6:	d004      	beq.n	20007402 <_malloc_trim_r+0x42>
200073f8:	4630      	mov	r0, r6
200073fa:	f7fe fbf7 	bl	20005bec <__malloc_unlock>
200073fe:	2000      	movs	r0, #0
20007400:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
20007402:	4630      	mov	r0, r6
20007404:	4261      	negs	r1, r4
20007406:	f7ff f88d 	bl	20006524 <_sbrk_r>
2000740a:	3001      	adds	r0, #1
2000740c:	d00d      	beq.n	2000742a <_malloc_trim_r+0x6a>
2000740e:	4a11      	ldr	r2, [pc, #68]	; (20007454 <_malloc_trim_r+0x94>)
20007410:	1b2d      	subs	r5, r5, r4
20007412:	68b9      	ldr	r1, [r7, #8]
20007414:	4630      	mov	r0, r6
20007416:	6813      	ldr	r3, [r2, #0]
20007418:	f045 0501 	orr.w	r5, r5, #1
2000741c:	1b1b      	subs	r3, r3, r4
2000741e:	604d      	str	r5, [r1, #4]
20007420:	6013      	str	r3, [r2, #0]
20007422:	f7fe fbe3 	bl	20005bec <__malloc_unlock>
20007426:	2001      	movs	r0, #1
20007428:	bdf8      	pop	{r3, r4, r5, r6, r7, pc}
2000742a:	4630      	mov	r0, r6
2000742c:	2100      	movs	r1, #0
2000742e:	f7ff f879 	bl	20006524 <_sbrk_r>
20007432:	68ba      	ldr	r2, [r7, #8]
20007434:	1a83      	subs	r3, r0, r2
20007436:	2b0f      	cmp	r3, #15
20007438:	ddde      	ble.n	200073f8 <_malloc_trim_r+0x38>
2000743a:	4c07      	ldr	r4, [pc, #28]	; (20007458 <_malloc_trim_r+0x98>)
2000743c:	f043 0301 	orr.w	r3, r3, #1
20007440:	4904      	ldr	r1, [pc, #16]	; (20007454 <_malloc_trim_r+0x94>)
20007442:	6824      	ldr	r4, [r4, #0]
20007444:	6053      	str	r3, [r2, #4]
20007446:	1b00      	subs	r0, r0, r4
20007448:	6008      	str	r0, [r1, #0]
2000744a:	e7d5      	b.n	200073f8 <_malloc_trim_r+0x38>
2000744c:	20009770 	.word	0x20009770
20007450:	fffff000 	.word	0xfffff000
20007454:	20009c38 	.word	0x20009c38
20007458:	20009b7c 	.word	0x20009b7c

2000745c <_free_r>:
2000745c:	2900      	cmp	r1, #0
2000745e:	d04d      	beq.n	200074fc <_free_r+0xa0>
20007460:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
20007464:	460c      	mov	r4, r1
20007466:	4680      	mov	r8, r0
20007468:	f7fe fbbe 	bl	20005be8 <__malloc_lock>
2000746c:	4963      	ldr	r1, [pc, #396]	; (200075fc <_free_r+0x1a0>)
2000746e:	f854 7c04 	ldr.w	r7, [r4, #-4]
20007472:	f1a4 0508 	sub.w	r5, r4, #8
20007476:	688e      	ldr	r6, [r1, #8]
20007478:	f027 0301 	bic.w	r3, r7, #1
2000747c:	18ea      	adds	r2, r5, r3
2000747e:	6850      	ldr	r0, [r2, #4]
20007480:	4296      	cmp	r6, r2
20007482:	f020 0003 	bic.w	r0, r0, #3
20007486:	d059      	beq.n	2000753c <_free_r+0xe0>
20007488:	07fe      	lsls	r6, r7, #31
2000748a:	6050      	str	r0, [r2, #4]
2000748c:	d40b      	bmi.n	200074a6 <_free_r+0x4a>
2000748e:	f854 4c08 	ldr.w	r4, [r4, #-8]
20007492:	f101 0708 	add.w	r7, r1, #8
20007496:	1b2d      	subs	r5, r5, r4
20007498:	4423      	add	r3, r4
2000749a:	68ac      	ldr	r4, [r5, #8]
2000749c:	42bc      	cmp	r4, r7
2000749e:	d066      	beq.n	2000756e <_free_r+0x112>
200074a0:	68ef      	ldr	r7, [r5, #12]
200074a2:	60e7      	str	r7, [r4, #12]
200074a4:	60bc      	str	r4, [r7, #8]
200074a6:	1814      	adds	r4, r2, r0
200074a8:	6864      	ldr	r4, [r4, #4]
200074aa:	07e4      	lsls	r4, r4, #31
200074ac:	d40c      	bmi.n	200074c8 <_free_r+0x6c>
200074ae:	4c54      	ldr	r4, [pc, #336]	; (20007600 <_free_r+0x1a4>)
200074b0:	4403      	add	r3, r0
200074b2:	6890      	ldr	r0, [r2, #8]
200074b4:	42a0      	cmp	r0, r4
200074b6:	d07c      	beq.n	200075b2 <_free_r+0x156>
200074b8:	68d4      	ldr	r4, [r2, #12]
200074ba:	f043 0201 	orr.w	r2, r3, #1
200074be:	60c4      	str	r4, [r0, #12]
200074c0:	60a0      	str	r0, [r4, #8]
200074c2:	606a      	str	r2, [r5, #4]
200074c4:	50eb      	str	r3, [r5, r3]
200074c6:	e003      	b.n	200074d0 <_free_r+0x74>
200074c8:	f043 0201 	orr.w	r2, r3, #1
200074cc:	606a      	str	r2, [r5, #4]
200074ce:	50eb      	str	r3, [r5, r3]
200074d0:	f5b3 7f00 	cmp.w	r3, #512	; 0x200
200074d4:	d213      	bcs.n	200074fe <_free_r+0xa2>
200074d6:	08db      	lsrs	r3, r3, #3
200074d8:	2001      	movs	r0, #1
200074da:	684a      	ldr	r2, [r1, #4]
200074dc:	109c      	asrs	r4, r3, #2
200074de:	eb01 03c3 	add.w	r3, r1, r3, lsl #3
200074e2:	40a0      	lsls	r0, r4
200074e4:	689c      	ldr	r4, [r3, #8]
200074e6:	4302      	orrs	r2, r0
200074e8:	60eb      	str	r3, [r5, #12]
200074ea:	60ac      	str	r4, [r5, #8]
200074ec:	604a      	str	r2, [r1, #4]
200074ee:	609d      	str	r5, [r3, #8]
200074f0:	60e5      	str	r5, [r4, #12]
200074f2:	4640      	mov	r0, r8
200074f4:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
200074f8:	f7fe bb78 	b.w	20005bec <__malloc_unlock>
200074fc:	4770      	bx	lr
200074fe:	0a5a      	lsrs	r2, r3, #9
20007500:	2a04      	cmp	r2, #4
20007502:	d847      	bhi.n	20007594 <_free_r+0x138>
20007504:	099a      	lsrs	r2, r3, #6
20007506:	f102 0438 	add.w	r4, r2, #56	; 0x38
2000750a:	0060      	lsls	r0, r4, #1
2000750c:	eb01 0080 	add.w	r0, r1, r0, lsl #2
20007510:	493a      	ldr	r1, [pc, #232]	; (200075fc <_free_r+0x1a0>)
20007512:	6882      	ldr	r2, [r0, #8]
20007514:	4282      	cmp	r2, r0
20007516:	d043      	beq.n	200075a0 <_free_r+0x144>
20007518:	6851      	ldr	r1, [r2, #4]
2000751a:	f021 0103 	bic.w	r1, r1, #3
2000751e:	4299      	cmp	r1, r3
20007520:	d902      	bls.n	20007528 <_free_r+0xcc>
20007522:	6892      	ldr	r2, [r2, #8]
20007524:	4290      	cmp	r0, r2
20007526:	d1f7      	bne.n	20007518 <_free_r+0xbc>
20007528:	68d3      	ldr	r3, [r2, #12]
2000752a:	60eb      	str	r3, [r5, #12]
2000752c:	4640      	mov	r0, r8
2000752e:	60aa      	str	r2, [r5, #8]
20007530:	609d      	str	r5, [r3, #8]
20007532:	60d5      	str	r5, [r2, #12]
20007534:	e8bd 41f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, lr}
20007538:	f7fe bb58 	b.w	20005bec <__malloc_unlock>
2000753c:	07ff      	lsls	r7, r7, #31
2000753e:	4403      	add	r3, r0
20007540:	d407      	bmi.n	20007552 <_free_r+0xf6>
20007542:	f854 2c08 	ldr.w	r2, [r4, #-8]
20007546:	1aad      	subs	r5, r5, r2
20007548:	4413      	add	r3, r2
2000754a:	68a8      	ldr	r0, [r5, #8]
2000754c:	68ea      	ldr	r2, [r5, #12]
2000754e:	60c2      	str	r2, [r0, #12]
20007550:	6090      	str	r0, [r2, #8]
20007552:	4a2c      	ldr	r2, [pc, #176]	; (20007604 <_free_r+0x1a8>)
20007554:	f043 0001 	orr.w	r0, r3, #1
20007558:	6812      	ldr	r2, [r2, #0]
2000755a:	6068      	str	r0, [r5, #4]
2000755c:	4293      	cmp	r3, r2
2000755e:	608d      	str	r5, [r1, #8]
20007560:	d3c7      	bcc.n	200074f2 <_free_r+0x96>
20007562:	4b29      	ldr	r3, [pc, #164]	; (20007608 <_free_r+0x1ac>)
20007564:	4640      	mov	r0, r8
20007566:	6819      	ldr	r1, [r3, #0]
20007568:	f7ff ff2a 	bl	200073c0 <_malloc_trim_r>
2000756c:	e7c1      	b.n	200074f2 <_free_r+0x96>
2000756e:	1811      	adds	r1, r2, r0
20007570:	6849      	ldr	r1, [r1, #4]
20007572:	07c9      	lsls	r1, r1, #31
20007574:	d409      	bmi.n	2000758a <_free_r+0x12e>
20007576:	4403      	add	r3, r0
20007578:	68d1      	ldr	r1, [r2, #12]
2000757a:	6892      	ldr	r2, [r2, #8]
2000757c:	f043 0001 	orr.w	r0, r3, #1
20007580:	60d1      	str	r1, [r2, #12]
20007582:	608a      	str	r2, [r1, #8]
20007584:	6068      	str	r0, [r5, #4]
20007586:	50eb      	str	r3, [r5, r3]
20007588:	e7b3      	b.n	200074f2 <_free_r+0x96>
2000758a:	f043 0201 	orr.w	r2, r3, #1
2000758e:	606a      	str	r2, [r5, #4]
20007590:	50eb      	str	r3, [r5, r3]
20007592:	e7ae      	b.n	200074f2 <_free_r+0x96>
20007594:	2a14      	cmp	r2, #20
20007596:	d815      	bhi.n	200075c4 <_free_r+0x168>
20007598:	f102 045b 	add.w	r4, r2, #91	; 0x5b
2000759c:	0060      	lsls	r0, r4, #1
2000759e:	e7b5      	b.n	2000750c <_free_r+0xb0>
200075a0:	10a4      	asrs	r4, r4, #2
200075a2:	2701      	movs	r7, #1
200075a4:	6848      	ldr	r0, [r1, #4]
200075a6:	4613      	mov	r3, r2
200075a8:	fa07 f404 	lsl.w	r4, r7, r4
200075ac:	4320      	orrs	r0, r4
200075ae:	6048      	str	r0, [r1, #4]
200075b0:	e7bb      	b.n	2000752a <_free_r+0xce>
200075b2:	f043 0201 	orr.w	r2, r3, #1
200075b6:	614d      	str	r5, [r1, #20]
200075b8:	610d      	str	r5, [r1, #16]
200075ba:	60e8      	str	r0, [r5, #12]
200075bc:	60a8      	str	r0, [r5, #8]
200075be:	606a      	str	r2, [r5, #4]
200075c0:	50eb      	str	r3, [r5, r3]
200075c2:	e796      	b.n	200074f2 <_free_r+0x96>
200075c4:	2a54      	cmp	r2, #84	; 0x54
200075c6:	d804      	bhi.n	200075d2 <_free_r+0x176>
200075c8:	0b1a      	lsrs	r2, r3, #12
200075ca:	f102 046e 	add.w	r4, r2, #110	; 0x6e
200075ce:	0060      	lsls	r0, r4, #1
200075d0:	e79c      	b.n	2000750c <_free_r+0xb0>
200075d2:	f5b2 7faa 	cmp.w	r2, #340	; 0x154
200075d6:	d804      	bhi.n	200075e2 <_free_r+0x186>
200075d8:	0bda      	lsrs	r2, r3, #15
200075da:	f102 0477 	add.w	r4, r2, #119	; 0x77
200075de:	0060      	lsls	r0, r4, #1
200075e0:	e794      	b.n	2000750c <_free_r+0xb0>
200075e2:	f240 5054 	movw	r0, #1364	; 0x554
200075e6:	4282      	cmp	r2, r0
200075e8:	d804      	bhi.n	200075f4 <_free_r+0x198>
200075ea:	0c9a      	lsrs	r2, r3, #18
200075ec:	f102 047c 	add.w	r4, r2, #124	; 0x7c
200075f0:	0060      	lsls	r0, r4, #1
200075f2:	e78b      	b.n	2000750c <_free_r+0xb0>
200075f4:	20fc      	movs	r0, #252	; 0xfc
200075f6:	247e      	movs	r4, #126	; 0x7e
200075f8:	e788      	b.n	2000750c <_free_r+0xb0>
200075fa:	bf00      	nop
200075fc:	20009770 	.word	0x20009770
20007600:	20009778 	.word	0x20009778
20007604:	20009b78 	.word	0x20009b78
20007608:	20009c34 	.word	0x20009c34

2000760c <memmove>:
2000760c:	4288      	cmp	r0, r1
2000760e:	b5f0      	push	{r4, r5, r6, r7, lr}
20007610:	d90d      	bls.n	2000762e <memmove+0x22>
20007612:	188b      	adds	r3, r1, r2
20007614:	4298      	cmp	r0, r3
20007616:	d20a      	bcs.n	2000762e <memmove+0x22>
20007618:	1881      	adds	r1, r0, r2
2000761a:	2a00      	cmp	r2, #0
2000761c:	d053      	beq.n	200076c6 <memmove+0xba>
2000761e:	1a9a      	subs	r2, r3, r2
20007620:	f813 4d01 	ldrb.w	r4, [r3, #-1]!
20007624:	4293      	cmp	r3, r2
20007626:	f801 4d01 	strb.w	r4, [r1, #-1]!
2000762a:	d1f9      	bne.n	20007620 <memmove+0x14>
2000762c:	bdf0      	pop	{r4, r5, r6, r7, pc}
2000762e:	2a0f      	cmp	r2, #15
20007630:	d947      	bls.n	200076c2 <memmove+0xb6>
20007632:	ea40 0301 	orr.w	r3, r0, r1
20007636:	079b      	lsls	r3, r3, #30
20007638:	d146      	bne.n	200076c8 <memmove+0xbc>
2000763a:	f100 0410 	add.w	r4, r0, #16
2000763e:	f101 0310 	add.w	r3, r1, #16
20007642:	4615      	mov	r5, r2
20007644:	f853 6c10 	ldr.w	r6, [r3, #-16]
20007648:	3d10      	subs	r5, #16
2000764a:	3310      	adds	r3, #16
2000764c:	3410      	adds	r4, #16
2000764e:	f844 6c20 	str.w	r6, [r4, #-32]
20007652:	2d0f      	cmp	r5, #15
20007654:	f853 6c1c 	ldr.w	r6, [r3, #-28]
20007658:	f844 6c1c 	str.w	r6, [r4, #-28]
2000765c:	f853 6c18 	ldr.w	r6, [r3, #-24]
20007660:	f844 6c18 	str.w	r6, [r4, #-24]
20007664:	f853 6c14 	ldr.w	r6, [r3, #-20]
20007668:	f844 6c14 	str.w	r6, [r4, #-20]
2000766c:	d8ea      	bhi.n	20007644 <memmove+0x38>
2000766e:	f1a2 0310 	sub.w	r3, r2, #16
20007672:	f002 0e0f 	and.w	lr, r2, #15
20007676:	f023 030f 	bic.w	r3, r3, #15
2000767a:	f1be 0f03 	cmp.w	lr, #3
2000767e:	f103 0310 	add.w	r3, r3, #16
20007682:	4419      	add	r1, r3
20007684:	4403      	add	r3, r0
20007686:	d921      	bls.n	200076cc <memmove+0xc0>
20007688:	1f1e      	subs	r6, r3, #4
2000768a:	460d      	mov	r5, r1
2000768c:	4674      	mov	r4, lr
2000768e:	3c04      	subs	r4, #4
20007690:	f855 7b04 	ldr.w	r7, [r5], #4
20007694:	2c03      	cmp	r4, #3
20007696:	f846 7f04 	str.w	r7, [r6, #4]!
2000769a:	d8f8      	bhi.n	2000768e <memmove+0x82>
2000769c:	f1ae 0404 	sub.w	r4, lr, #4
200076a0:	f002 0203 	and.w	r2, r2, #3
200076a4:	f024 0403 	bic.w	r4, r4, #3
200076a8:	3404      	adds	r4, #4
200076aa:	4423      	add	r3, r4
200076ac:	4421      	add	r1, r4
200076ae:	b152      	cbz	r2, 200076c6 <memmove+0xba>
200076b0:	3b01      	subs	r3, #1
200076b2:	440a      	add	r2, r1
200076b4:	f811 4b01 	ldrb.w	r4, [r1], #1
200076b8:	4291      	cmp	r1, r2
200076ba:	f803 4f01 	strb.w	r4, [r3, #1]!
200076be:	d1f9      	bne.n	200076b4 <memmove+0xa8>
200076c0:	bdf0      	pop	{r4, r5, r6, r7, pc}
200076c2:	4603      	mov	r3, r0
200076c4:	e7f3      	b.n	200076ae <memmove+0xa2>
200076c6:	bdf0      	pop	{r4, r5, r6, r7, pc}
200076c8:	4603      	mov	r3, r0
200076ca:	e7f1      	b.n	200076b0 <memmove+0xa4>
200076cc:	4672      	mov	r2, lr
200076ce:	e7ee      	b.n	200076ae <memmove+0xa2>

200076d0 <memset>:
200076d0:	0783      	lsls	r3, r0, #30
200076d2:	b470      	push	{r4, r5, r6}
200076d4:	d049      	beq.n	2000776a <memset+0x9a>
200076d6:	1e54      	subs	r4, r2, #1
200076d8:	2a00      	cmp	r2, #0
200076da:	d044      	beq.n	20007766 <memset+0x96>
200076dc:	b2cd      	uxtb	r5, r1
200076de:	4603      	mov	r3, r0
200076e0:	e002      	b.n	200076e8 <memset+0x18>
200076e2:	2c00      	cmp	r4, #0
200076e4:	d03f      	beq.n	20007766 <memset+0x96>
200076e6:	4614      	mov	r4, r2
200076e8:	f803 5b01 	strb.w	r5, [r3], #1
200076ec:	f013 0f03 	tst.w	r3, #3
200076f0:	f104 32ff 	add.w	r2, r4, #4294967295
200076f4:	d1f5      	bne.n	200076e2 <memset+0x12>
200076f6:	2c03      	cmp	r4, #3
200076f8:	d92e      	bls.n	20007758 <memset+0x88>
200076fa:	b2cd      	uxtb	r5, r1
200076fc:	2c0f      	cmp	r4, #15
200076fe:	ea45 2505 	orr.w	r5, r5, r5, lsl #8
20007702:	ea45 4505 	orr.w	r5, r5, r5, lsl #16
20007706:	d919      	bls.n	2000773c <memset+0x6c>
20007708:	f103 0210 	add.w	r2, r3, #16
2000770c:	4626      	mov	r6, r4
2000770e:	3e10      	subs	r6, #16
20007710:	f842 5c10 	str.w	r5, [r2, #-16]
20007714:	f842 5c0c 	str.w	r5, [r2, #-12]
20007718:	3210      	adds	r2, #16
2000771a:	2e0f      	cmp	r6, #15
2000771c:	f842 5c18 	str.w	r5, [r2, #-24]
20007720:	f842 5c14 	str.w	r5, [r2, #-20]
20007724:	d8f3      	bhi.n	2000770e <memset+0x3e>
20007726:	f1a4 0210 	sub.w	r2, r4, #16
2000772a:	f004 040f 	and.w	r4, r4, #15
2000772e:	f022 020f 	bic.w	r2, r2, #15
20007732:	2c03      	cmp	r4, #3
20007734:	f102 0210 	add.w	r2, r2, #16
20007738:	4413      	add	r3, r2
2000773a:	d90d      	bls.n	20007758 <memset+0x88>
2000773c:	461e      	mov	r6, r3
2000773e:	4622      	mov	r2, r4
20007740:	3a04      	subs	r2, #4
20007742:	f846 5b04 	str.w	r5, [r6], #4
20007746:	2a03      	cmp	r2, #3
20007748:	d8fa      	bhi.n	20007740 <memset+0x70>
2000774a:	1f22      	subs	r2, r4, #4
2000774c:	f004 0403 	and.w	r4, r4, #3
20007750:	f022 0203 	bic.w	r2, r2, #3
20007754:	3204      	adds	r2, #4
20007756:	4413      	add	r3, r2
20007758:	b12c      	cbz	r4, 20007766 <memset+0x96>
2000775a:	b2c9      	uxtb	r1, r1
2000775c:	441c      	add	r4, r3
2000775e:	f803 1b01 	strb.w	r1, [r3], #1
20007762:	42a3      	cmp	r3, r4
20007764:	d1fb      	bne.n	2000775e <memset+0x8e>
20007766:	bc70      	pop	{r4, r5, r6}
20007768:	4770      	bx	lr
2000776a:	4614      	mov	r4, r2
2000776c:	4603      	mov	r3, r0
2000776e:	e7c2      	b.n	200076f6 <memset+0x26>

20007770 <_realloc_r>:
20007770:	e92d 4ff0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
20007774:	4617      	mov	r7, r2
20007776:	b083      	sub	sp, #12
20007778:	460e      	mov	r6, r1
2000777a:	2900      	cmp	r1, #0
2000777c:	f000 80e2 	beq.w	20007944 <_realloc_r+0x1d4>
20007780:	f107 040b 	add.w	r4, r7, #11
20007784:	4681      	mov	r9, r0
20007786:	f7fe fa2f 	bl	20005be8 <__malloc_lock>
2000778a:	f856 3c04 	ldr.w	r3, [r6, #-4]
2000778e:	2c16      	cmp	r4, #22
20007790:	f1a6 0808 	sub.w	r8, r6, #8
20007794:	f023 0503 	bic.w	r5, r3, #3
20007798:	d850      	bhi.n	2000783c <_realloc_r+0xcc>
2000779a:	2210      	movs	r2, #16
2000779c:	2100      	movs	r1, #0
2000779e:	4614      	mov	r4, r2
200077a0:	42bc      	cmp	r4, r7
200077a2:	f0c0 80dc 	bcc.w	2000795e <_realloc_r+0x1ee>
200077a6:	2900      	cmp	r1, #0
200077a8:	f040 80d9 	bne.w	2000795e <_realloc_r+0x1ee>
200077ac:	4295      	cmp	r5, r2
200077ae:	da4a      	bge.n	20007846 <_realloc_r+0xd6>
200077b0:	f8df b3a8 	ldr.w	fp, [pc, #936]	; 20007b5c <_realloc_r+0x3ec>
200077b4:	eb08 0105 	add.w	r1, r8, r5
200077b8:	f8db 0008 	ldr.w	r0, [fp, #8]
200077bc:	4288      	cmp	r0, r1
200077be:	f000 80d3 	beq.w	20007968 <_realloc_r+0x1f8>
200077c2:	6848      	ldr	r0, [r1, #4]
200077c4:	f020 0e01 	bic.w	lr, r0, #1
200077c8:	448e      	add	lr, r1
200077ca:	f8de e004 	ldr.w	lr, [lr, #4]
200077ce:	f01e 0f01 	tst.w	lr, #1
200077d2:	d14e      	bne.n	20007872 <_realloc_r+0x102>
200077d4:	f020 0003 	bic.w	r0, r0, #3
200077d8:	4428      	add	r0, r5
200077da:	4290      	cmp	r0, r2
200077dc:	f280 80b8 	bge.w	20007950 <_realloc_r+0x1e0>
200077e0:	07db      	lsls	r3, r3, #31
200077e2:	f100 808b 	bmi.w	200078fc <_realloc_r+0x18c>
200077e6:	f856 3c08 	ldr.w	r3, [r6, #-8]
200077ea:	ebc3 0a08 	rsb	sl, r3, r8
200077ee:	f8da 3004 	ldr.w	r3, [sl, #4]
200077f2:	f023 0303 	bic.w	r3, r3, #3
200077f6:	eb00 0e03 	add.w	lr, r0, r3
200077fa:	4596      	cmp	lr, r2
200077fc:	db43      	blt.n	20007886 <_realloc_r+0x116>
200077fe:	68cb      	ldr	r3, [r1, #12]
20007800:	4657      	mov	r7, sl
20007802:	6889      	ldr	r1, [r1, #8]
20007804:	1f2a      	subs	r2, r5, #4
20007806:	60cb      	str	r3, [r1, #12]
20007808:	2a24      	cmp	r2, #36	; 0x24
2000780a:	6099      	str	r1, [r3, #8]
2000780c:	f857 1f08 	ldr.w	r1, [r7, #8]!
20007810:	f8da 300c 	ldr.w	r3, [sl, #12]
20007814:	60cb      	str	r3, [r1, #12]
20007816:	6099      	str	r1, [r3, #8]
20007818:	f200 813c 	bhi.w	20007a94 <_realloc_r+0x324>
2000781c:	2a13      	cmp	r2, #19
2000781e:	f240 80fa 	bls.w	20007a16 <_realloc_r+0x2a6>
20007822:	6833      	ldr	r3, [r6, #0]
20007824:	2a1b      	cmp	r2, #27
20007826:	f8ca 3008 	str.w	r3, [sl, #8]
2000782a:	6873      	ldr	r3, [r6, #4]
2000782c:	f8ca 300c 	str.w	r3, [sl, #12]
20007830:	f200 813b 	bhi.w	20007aaa <_realloc_r+0x33a>
20007834:	3608      	adds	r6, #8
20007836:	f10a 0310 	add.w	r3, sl, #16
2000783a:	e0ed      	b.n	20007a18 <_realloc_r+0x2a8>
2000783c:	f024 0407 	bic.w	r4, r4, #7
20007840:	4622      	mov	r2, r4
20007842:	0fe1      	lsrs	r1, r4, #31
20007844:	e7ac      	b.n	200077a0 <_realloc_r+0x30>
20007846:	4637      	mov	r7, r6
20007848:	1b2a      	subs	r2, r5, r4
2000784a:	f003 0301 	and.w	r3, r3, #1
2000784e:	2a0f      	cmp	r2, #15
20007850:	d841      	bhi.n	200078d6 <_realloc_r+0x166>
20007852:	eb08 0205 	add.w	r2, r8, r5
20007856:	431d      	orrs	r5, r3
20007858:	f8c8 5004 	str.w	r5, [r8, #4]
2000785c:	6853      	ldr	r3, [r2, #4]
2000785e:	f043 0301 	orr.w	r3, r3, #1
20007862:	6053      	str	r3, [r2, #4]
20007864:	4648      	mov	r0, r9
20007866:	f7fe f9c1 	bl	20005bec <__malloc_unlock>
2000786a:	4638      	mov	r0, r7
2000786c:	b003      	add	sp, #12
2000786e:	e8bd 8ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, pc}
20007872:	07d9      	lsls	r1, r3, #31
20007874:	d442      	bmi.n	200078fc <_realloc_r+0x18c>
20007876:	f856 3c08 	ldr.w	r3, [r6, #-8]
2000787a:	ebc3 0a08 	rsb	sl, r3, r8
2000787e:	f8da 3004 	ldr.w	r3, [sl, #4]
20007882:	f023 0303 	bic.w	r3, r3, #3
20007886:	442b      	add	r3, r5
20007888:	4293      	cmp	r3, r2
2000788a:	db37      	blt.n	200078fc <_realloc_r+0x18c>
2000788c:	4657      	mov	r7, sl
2000788e:	1f2a      	subs	r2, r5, #4
20007890:	f8da 100c 	ldr.w	r1, [sl, #12]
20007894:	f857 0f08 	ldr.w	r0, [r7, #8]!
20007898:	2a24      	cmp	r2, #36	; 0x24
2000789a:	60c1      	str	r1, [r0, #12]
2000789c:	6088      	str	r0, [r1, #8]
2000789e:	f200 80c6 	bhi.w	20007a2e <_realloc_r+0x2be>
200078a2:	2a13      	cmp	r2, #19
200078a4:	f240 80ff 	bls.w	20007aa6 <_realloc_r+0x336>
200078a8:	6831      	ldr	r1, [r6, #0]
200078aa:	2a1b      	cmp	r2, #27
200078ac:	f8ca 1008 	str.w	r1, [sl, #8]
200078b0:	6871      	ldr	r1, [r6, #4]
200078b2:	f8ca 100c 	str.w	r1, [sl, #12]
200078b6:	f200 810d 	bhi.w	20007ad4 <_realloc_r+0x364>
200078ba:	3608      	adds	r6, #8
200078bc:	f10a 0210 	add.w	r2, sl, #16
200078c0:	6831      	ldr	r1, [r6, #0]
200078c2:	461d      	mov	r5, r3
200078c4:	46d0      	mov	r8, sl
200078c6:	6011      	str	r1, [r2, #0]
200078c8:	6873      	ldr	r3, [r6, #4]
200078ca:	6053      	str	r3, [r2, #4]
200078cc:	68b3      	ldr	r3, [r6, #8]
200078ce:	6093      	str	r3, [r2, #8]
200078d0:	f8da 3004 	ldr.w	r3, [sl, #4]
200078d4:	e7b8      	b.n	20007848 <_realloc_r+0xd8>
200078d6:	eb08 0504 	add.w	r5, r8, r4
200078da:	f042 0601 	orr.w	r6, r2, #1
200078de:	431c      	orrs	r4, r3
200078e0:	4648      	mov	r0, r9
200078e2:	442a      	add	r2, r5
200078e4:	f105 0108 	add.w	r1, r5, #8
200078e8:	f8c8 4004 	str.w	r4, [r8, #4]
200078ec:	606e      	str	r6, [r5, #4]
200078ee:	6853      	ldr	r3, [r2, #4]
200078f0:	f043 0301 	orr.w	r3, r3, #1
200078f4:	6053      	str	r3, [r2, #4]
200078f6:	f7ff fdb1 	bl	2000745c <_free_r>
200078fa:	e7b3      	b.n	20007864 <_realloc_r+0xf4>
200078fc:	4639      	mov	r1, r7
200078fe:	4648      	mov	r0, r9
20007900:	f7fd fde4 	bl	200054cc <_malloc_r>
20007904:	4607      	mov	r7, r0
20007906:	2800      	cmp	r0, #0
20007908:	d0ac      	beq.n	20007864 <_realloc_r+0xf4>
2000790a:	f856 3c04 	ldr.w	r3, [r6, #-4]
2000790e:	f1a0 0108 	sub.w	r1, r0, #8
20007912:	f023 0201 	bic.w	r2, r3, #1
20007916:	4442      	add	r2, r8
20007918:	4291      	cmp	r1, r2
2000791a:	f000 80b4 	beq.w	20007a86 <_realloc_r+0x316>
2000791e:	1f2a      	subs	r2, r5, #4
20007920:	2a24      	cmp	r2, #36	; 0x24
20007922:	f200 80a1 	bhi.w	20007a68 <_realloc_r+0x2f8>
20007926:	2a13      	cmp	r2, #19
20007928:	d86a      	bhi.n	20007a00 <_realloc_r+0x290>
2000792a:	4603      	mov	r3, r0
2000792c:	4632      	mov	r2, r6
2000792e:	6811      	ldr	r1, [r2, #0]
20007930:	6019      	str	r1, [r3, #0]
20007932:	6851      	ldr	r1, [r2, #4]
20007934:	6059      	str	r1, [r3, #4]
20007936:	6892      	ldr	r2, [r2, #8]
20007938:	609a      	str	r2, [r3, #8]
2000793a:	4631      	mov	r1, r6
2000793c:	4648      	mov	r0, r9
2000793e:	f7ff fd8d 	bl	2000745c <_free_r>
20007942:	e78f      	b.n	20007864 <_realloc_r+0xf4>
20007944:	4611      	mov	r1, r2
20007946:	b003      	add	sp, #12
20007948:	e8bd 4ff0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, lr}
2000794c:	f7fd bdbe 	b.w	200054cc <_malloc_r>
20007950:	68ca      	ldr	r2, [r1, #12]
20007952:	4637      	mov	r7, r6
20007954:	6889      	ldr	r1, [r1, #8]
20007956:	4605      	mov	r5, r0
20007958:	60ca      	str	r2, [r1, #12]
2000795a:	6091      	str	r1, [r2, #8]
2000795c:	e774      	b.n	20007848 <_realloc_r+0xd8>
2000795e:	230c      	movs	r3, #12
20007960:	2000      	movs	r0, #0
20007962:	f8c9 3000 	str.w	r3, [r9]
20007966:	e781      	b.n	2000786c <_realloc_r+0xfc>
20007968:	6841      	ldr	r1, [r0, #4]
2000796a:	f104 0010 	add.w	r0, r4, #16
2000796e:	f021 0103 	bic.w	r1, r1, #3
20007972:	4429      	add	r1, r5
20007974:	4281      	cmp	r1, r0
20007976:	da63      	bge.n	20007a40 <_realloc_r+0x2d0>
20007978:	07db      	lsls	r3, r3, #31
2000797a:	d4bf      	bmi.n	200078fc <_realloc_r+0x18c>
2000797c:	f856 3c08 	ldr.w	r3, [r6, #-8]
20007980:	ebc3 0a08 	rsb	sl, r3, r8
20007984:	f8da 3004 	ldr.w	r3, [sl, #4]
20007988:	f023 0303 	bic.w	r3, r3, #3
2000798c:	eb01 0c03 	add.w	ip, r1, r3
20007990:	4560      	cmp	r0, ip
20007992:	f73f af78 	bgt.w	20007886 <_realloc_r+0x116>
20007996:	4657      	mov	r7, sl
20007998:	1f2a      	subs	r2, r5, #4
2000799a:	f8da 300c 	ldr.w	r3, [sl, #12]
2000799e:	f857 1f08 	ldr.w	r1, [r7, #8]!
200079a2:	2a24      	cmp	r2, #36	; 0x24
200079a4:	60cb      	str	r3, [r1, #12]
200079a6:	6099      	str	r1, [r3, #8]
200079a8:	f200 80b8 	bhi.w	20007b1c <_realloc_r+0x3ac>
200079ac:	2a13      	cmp	r2, #19
200079ae:	f240 80a8 	bls.w	20007b02 <_realloc_r+0x392>
200079b2:	6833      	ldr	r3, [r6, #0]
200079b4:	2a1b      	cmp	r2, #27
200079b6:	f8ca 3008 	str.w	r3, [sl, #8]
200079ba:	6873      	ldr	r3, [r6, #4]
200079bc:	f8ca 300c 	str.w	r3, [sl, #12]
200079c0:	f200 80b5 	bhi.w	20007b2e <_realloc_r+0x3be>
200079c4:	3608      	adds	r6, #8
200079c6:	f10a 0310 	add.w	r3, sl, #16
200079ca:	6832      	ldr	r2, [r6, #0]
200079cc:	601a      	str	r2, [r3, #0]
200079ce:	6872      	ldr	r2, [r6, #4]
200079d0:	605a      	str	r2, [r3, #4]
200079d2:	68b2      	ldr	r2, [r6, #8]
200079d4:	609a      	str	r2, [r3, #8]
200079d6:	ebc4 030c 	rsb	r3, r4, ip
200079da:	eb0a 0204 	add.w	r2, sl, r4
200079de:	4648      	mov	r0, r9
200079e0:	f043 0301 	orr.w	r3, r3, #1
200079e4:	f8cb 2008 	str.w	r2, [fp, #8]
200079e8:	6053      	str	r3, [r2, #4]
200079ea:	f8da 3004 	ldr.w	r3, [sl, #4]
200079ee:	f003 0301 	and.w	r3, r3, #1
200079f2:	431c      	orrs	r4, r3
200079f4:	f8ca 4004 	str.w	r4, [sl, #4]
200079f8:	f7fe f8f8 	bl	20005bec <__malloc_unlock>
200079fc:	4638      	mov	r0, r7
200079fe:	e735      	b.n	2000786c <_realloc_r+0xfc>
20007a00:	6833      	ldr	r3, [r6, #0]
20007a02:	2a1b      	cmp	r2, #27
20007a04:	6003      	str	r3, [r0, #0]
20007a06:	6873      	ldr	r3, [r6, #4]
20007a08:	6043      	str	r3, [r0, #4]
20007a0a:	d831      	bhi.n	20007a70 <_realloc_r+0x300>
20007a0c:	f100 0308 	add.w	r3, r0, #8
20007a10:	f106 0208 	add.w	r2, r6, #8
20007a14:	e78b      	b.n	2000792e <_realloc_r+0x1be>
20007a16:	463b      	mov	r3, r7
20007a18:	6832      	ldr	r2, [r6, #0]
20007a1a:	4675      	mov	r5, lr
20007a1c:	46d0      	mov	r8, sl
20007a1e:	601a      	str	r2, [r3, #0]
20007a20:	6872      	ldr	r2, [r6, #4]
20007a22:	605a      	str	r2, [r3, #4]
20007a24:	68b2      	ldr	r2, [r6, #8]
20007a26:	609a      	str	r2, [r3, #8]
20007a28:	f8da 3004 	ldr.w	r3, [sl, #4]
20007a2c:	e70c      	b.n	20007848 <_realloc_r+0xd8>
20007a2e:	4631      	mov	r1, r6
20007a30:	4638      	mov	r0, r7
20007a32:	461d      	mov	r5, r3
20007a34:	46d0      	mov	r8, sl
20007a36:	f7ff fde9 	bl	2000760c <memmove>
20007a3a:	f8da 3004 	ldr.w	r3, [sl, #4]
20007a3e:	e703      	b.n	20007848 <_realloc_r+0xd8>
20007a40:	1b0b      	subs	r3, r1, r4
20007a42:	eb08 0204 	add.w	r2, r8, r4
20007a46:	4648      	mov	r0, r9
20007a48:	f043 0301 	orr.w	r3, r3, #1
20007a4c:	f8cb 2008 	str.w	r2, [fp, #8]
20007a50:	6053      	str	r3, [r2, #4]
20007a52:	f856 3c04 	ldr.w	r3, [r6, #-4]
20007a56:	f003 0301 	and.w	r3, r3, #1
20007a5a:	431c      	orrs	r4, r3
20007a5c:	f846 4c04 	str.w	r4, [r6, #-4]
20007a60:	f7fe f8c4 	bl	20005bec <__malloc_unlock>
20007a64:	4630      	mov	r0, r6
20007a66:	e701      	b.n	2000786c <_realloc_r+0xfc>
20007a68:	4631      	mov	r1, r6
20007a6a:	f7ff fdcf 	bl	2000760c <memmove>
20007a6e:	e764      	b.n	2000793a <_realloc_r+0x1ca>
20007a70:	68b3      	ldr	r3, [r6, #8]
20007a72:	2a24      	cmp	r2, #36	; 0x24
20007a74:	6083      	str	r3, [r0, #8]
20007a76:	68f3      	ldr	r3, [r6, #12]
20007a78:	60c3      	str	r3, [r0, #12]
20007a7a:	d022      	beq.n	20007ac2 <_realloc_r+0x352>
20007a7c:	f100 0310 	add.w	r3, r0, #16
20007a80:	f106 0210 	add.w	r2, r6, #16
20007a84:	e753      	b.n	2000792e <_realloc_r+0x1be>
20007a86:	f850 2c04 	ldr.w	r2, [r0, #-4]
20007a8a:	4637      	mov	r7, r6
20007a8c:	f022 0203 	bic.w	r2, r2, #3
20007a90:	4415      	add	r5, r2
20007a92:	e6d9      	b.n	20007848 <_realloc_r+0xd8>
20007a94:	4631      	mov	r1, r6
20007a96:	4638      	mov	r0, r7
20007a98:	4675      	mov	r5, lr
20007a9a:	46d0      	mov	r8, sl
20007a9c:	f7ff fdb6 	bl	2000760c <memmove>
20007aa0:	f8da 3004 	ldr.w	r3, [sl, #4]
20007aa4:	e6d0      	b.n	20007848 <_realloc_r+0xd8>
20007aa6:	463a      	mov	r2, r7
20007aa8:	e70a      	b.n	200078c0 <_realloc_r+0x150>
20007aaa:	68b3      	ldr	r3, [r6, #8]
20007aac:	2a24      	cmp	r2, #36	; 0x24
20007aae:	f8ca 3010 	str.w	r3, [sl, #16]
20007ab2:	68f3      	ldr	r3, [r6, #12]
20007ab4:	f8ca 3014 	str.w	r3, [sl, #20]
20007ab8:	d018      	beq.n	20007aec <_realloc_r+0x37c>
20007aba:	3610      	adds	r6, #16
20007abc:	f10a 0318 	add.w	r3, sl, #24
20007ac0:	e7aa      	b.n	20007a18 <_realloc_r+0x2a8>
20007ac2:	6931      	ldr	r1, [r6, #16]
20007ac4:	f100 0318 	add.w	r3, r0, #24
20007ac8:	f106 0218 	add.w	r2, r6, #24
20007acc:	6101      	str	r1, [r0, #16]
20007ace:	6971      	ldr	r1, [r6, #20]
20007ad0:	6141      	str	r1, [r0, #20]
20007ad2:	e72c      	b.n	2000792e <_realloc_r+0x1be>
20007ad4:	68b1      	ldr	r1, [r6, #8]
20007ad6:	2a24      	cmp	r2, #36	; 0x24
20007ad8:	f8ca 1010 	str.w	r1, [sl, #16]
20007adc:	68f1      	ldr	r1, [r6, #12]
20007ade:	f8ca 1014 	str.w	r1, [sl, #20]
20007ae2:	d010      	beq.n	20007b06 <_realloc_r+0x396>
20007ae4:	3610      	adds	r6, #16
20007ae6:	f10a 0218 	add.w	r2, sl, #24
20007aea:	e6e9      	b.n	200078c0 <_realloc_r+0x150>
20007aec:	6932      	ldr	r2, [r6, #16]
20007aee:	f10a 0320 	add.w	r3, sl, #32
20007af2:	3618      	adds	r6, #24
20007af4:	f8ca 2018 	str.w	r2, [sl, #24]
20007af8:	f856 2c04 	ldr.w	r2, [r6, #-4]
20007afc:	f8ca 201c 	str.w	r2, [sl, #28]
20007b00:	e78a      	b.n	20007a18 <_realloc_r+0x2a8>
20007b02:	463b      	mov	r3, r7
20007b04:	e761      	b.n	200079ca <_realloc_r+0x25a>
20007b06:	6931      	ldr	r1, [r6, #16]
20007b08:	f10a 0220 	add.w	r2, sl, #32
20007b0c:	3618      	adds	r6, #24
20007b0e:	f8ca 1018 	str.w	r1, [sl, #24]
20007b12:	f856 1c04 	ldr.w	r1, [r6, #-4]
20007b16:	f8ca 101c 	str.w	r1, [sl, #28]
20007b1a:	e6d1      	b.n	200078c0 <_realloc_r+0x150>
20007b1c:	4631      	mov	r1, r6
20007b1e:	4638      	mov	r0, r7
20007b20:	f8cd c004 	str.w	ip, [sp, #4]
20007b24:	f7ff fd72 	bl	2000760c <memmove>
20007b28:	f8dd c004 	ldr.w	ip, [sp, #4]
20007b2c:	e753      	b.n	200079d6 <_realloc_r+0x266>
20007b2e:	68b3      	ldr	r3, [r6, #8]
20007b30:	2a24      	cmp	r2, #36	; 0x24
20007b32:	f8ca 3010 	str.w	r3, [sl, #16]
20007b36:	68f3      	ldr	r3, [r6, #12]
20007b38:	f8ca 3014 	str.w	r3, [sl, #20]
20007b3c:	d003      	beq.n	20007b46 <_realloc_r+0x3d6>
20007b3e:	3610      	adds	r6, #16
20007b40:	f10a 0318 	add.w	r3, sl, #24
20007b44:	e741      	b.n	200079ca <_realloc_r+0x25a>
20007b46:	6932      	ldr	r2, [r6, #16]
20007b48:	f10a 0320 	add.w	r3, sl, #32
20007b4c:	3618      	adds	r6, #24
20007b4e:	f8ca 2018 	str.w	r2, [sl, #24]
20007b52:	f856 2c04 	ldr.w	r2, [r6, #-4]
20007b56:	f8ca 201c 	str.w	r2, [sl, #28]
20007b5a:	e736      	b.n	200079ca <_realloc_r+0x25a>
20007b5c:	20009770 	.word	0x20009770

20007b60 <cleanup_glue>:
20007b60:	b538      	push	{r3, r4, r5, lr}
20007b62:	460c      	mov	r4, r1
20007b64:	6809      	ldr	r1, [r1, #0]
20007b66:	4605      	mov	r5, r0
20007b68:	b109      	cbz	r1, 20007b6e <cleanup_glue+0xe>
20007b6a:	f7ff fff9 	bl	20007b60 <cleanup_glue>
20007b6e:	4628      	mov	r0, r5
20007b70:	4621      	mov	r1, r4
20007b72:	e8bd 4038 	ldmia.w	sp!, {r3, r4, r5, lr}
20007b76:	f7ff bc71 	b.w	2000745c <_free_r>
20007b7a:	bf00      	nop

20007b7c <_reclaim_reent>:
20007b7c:	4b20      	ldr	r3, [pc, #128]	; (20007c00 <_reclaim_reent+0x84>)
20007b7e:	681b      	ldr	r3, [r3, #0]
20007b80:	4298      	cmp	r0, r3
20007b82:	d03c      	beq.n	20007bfe <_reclaim_reent+0x82>
20007b84:	6cc3      	ldr	r3, [r0, #76]	; 0x4c
20007b86:	b570      	push	{r4, r5, r6, lr}
20007b88:	4605      	mov	r5, r0
20007b8a:	b18b      	cbz	r3, 20007bb0 <_reclaim_reent+0x34>
20007b8c:	2600      	movs	r6, #0
20007b8e:	5999      	ldr	r1, [r3, r6]
20007b90:	b139      	cbz	r1, 20007ba2 <_reclaim_reent+0x26>
20007b92:	680c      	ldr	r4, [r1, #0]
20007b94:	4628      	mov	r0, r5
20007b96:	f7ff fc61 	bl	2000745c <_free_r>
20007b9a:	4621      	mov	r1, r4
20007b9c:	2c00      	cmp	r4, #0
20007b9e:	d1f8      	bne.n	20007b92 <_reclaim_reent+0x16>
20007ba0:	6ceb      	ldr	r3, [r5, #76]	; 0x4c
20007ba2:	3604      	adds	r6, #4
20007ba4:	2e80      	cmp	r6, #128	; 0x80
20007ba6:	d1f2      	bne.n	20007b8e <_reclaim_reent+0x12>
20007ba8:	4619      	mov	r1, r3
20007baa:	4628      	mov	r0, r5
20007bac:	f7ff fc56 	bl	2000745c <_free_r>
20007bb0:	6c29      	ldr	r1, [r5, #64]	; 0x40
20007bb2:	b111      	cbz	r1, 20007bba <_reclaim_reent+0x3e>
20007bb4:	4628      	mov	r0, r5
20007bb6:	f7ff fc51 	bl	2000745c <_free_r>
20007bba:	f8d5 1148 	ldr.w	r1, [r5, #328]	; 0x148
20007bbe:	b151      	cbz	r1, 20007bd6 <_reclaim_reent+0x5a>
20007bc0:	f505 76a6 	add.w	r6, r5, #332	; 0x14c
20007bc4:	42b1      	cmp	r1, r6
20007bc6:	d006      	beq.n	20007bd6 <_reclaim_reent+0x5a>
20007bc8:	680c      	ldr	r4, [r1, #0]
20007bca:	4628      	mov	r0, r5
20007bcc:	f7ff fc46 	bl	2000745c <_free_r>
20007bd0:	42a6      	cmp	r6, r4
20007bd2:	4621      	mov	r1, r4
20007bd4:	d1f8      	bne.n	20007bc8 <_reclaim_reent+0x4c>
20007bd6:	6d69      	ldr	r1, [r5, #84]	; 0x54
20007bd8:	b111      	cbz	r1, 20007be0 <_reclaim_reent+0x64>
20007bda:	4628      	mov	r0, r5
20007bdc:	f7ff fc3e 	bl	2000745c <_free_r>
20007be0:	6bab      	ldr	r3, [r5, #56]	; 0x38
20007be2:	b903      	cbnz	r3, 20007be6 <_reclaim_reent+0x6a>
20007be4:	bd70      	pop	{r4, r5, r6, pc}
20007be6:	6beb      	ldr	r3, [r5, #60]	; 0x3c
20007be8:	4628      	mov	r0, r5
20007bea:	4798      	blx	r3
20007bec:	f8d5 12e0 	ldr.w	r1, [r5, #736]	; 0x2e0
20007bf0:	2900      	cmp	r1, #0
20007bf2:	d0f7      	beq.n	20007be4 <_reclaim_reent+0x68>
20007bf4:	4628      	mov	r0, r5
20007bf6:	e8bd 4070 	ldmia.w	sp!, {r4, r5, r6, lr}
20007bfa:	f7ff bfb1 	b.w	20007b60 <cleanup_glue>
20007bfe:	4770      	bx	lr
20007c00:	200096f0 	.word	0x200096f0

20007c04 <__aeabi_drsub>:
20007c04:	f081 4100 	eor.w	r1, r1, #2147483648	; 0x80000000
20007c08:	e002      	b.n	20007c10 <__adddf3>
20007c0a:	bf00      	nop

20007c0c <__aeabi_dsub>:
20007c0c:	f083 4300 	eor.w	r3, r3, #2147483648	; 0x80000000

20007c10 <__adddf3>:
20007c10:	b530      	push	{r4, r5, lr}
20007c12:	ea4f 0441 	mov.w	r4, r1, lsl #1
20007c16:	ea4f 0543 	mov.w	r5, r3, lsl #1
20007c1a:	ea94 0f05 	teq	r4, r5
20007c1e:	bf08      	it	eq
20007c20:	ea90 0f02 	teqeq	r0, r2
20007c24:	bf1f      	itttt	ne
20007c26:	ea54 0c00 	orrsne.w	ip, r4, r0
20007c2a:	ea55 0c02 	orrsne.w	ip, r5, r2
20007c2e:	ea7f 5c64 	mvnsne.w	ip, r4, asr #21
20007c32:	ea7f 5c65 	mvnsne.w	ip, r5, asr #21
20007c36:	f000 80e2 	beq.w	20007dfe <__adddf3+0x1ee>
20007c3a:	ea4f 5454 	mov.w	r4, r4, lsr #21
20007c3e:	ebd4 5555 	rsbs	r5, r4, r5, lsr #21
20007c42:	bfb8      	it	lt
20007c44:	426d      	neglt	r5, r5
20007c46:	dd0c      	ble.n	20007c62 <__adddf3+0x52>
20007c48:	442c      	add	r4, r5
20007c4a:	ea80 0202 	eor.w	r2, r0, r2
20007c4e:	ea81 0303 	eor.w	r3, r1, r3
20007c52:	ea82 0000 	eor.w	r0, r2, r0
20007c56:	ea83 0101 	eor.w	r1, r3, r1
20007c5a:	ea80 0202 	eor.w	r2, r0, r2
20007c5e:	ea81 0303 	eor.w	r3, r1, r3
20007c62:	2d36      	cmp	r5, #54	; 0x36
20007c64:	bf88      	it	hi
20007c66:	bd30      	pophi	{r4, r5, pc}
20007c68:	f011 4f00 	tst.w	r1, #2147483648	; 0x80000000
20007c6c:	ea4f 3101 	mov.w	r1, r1, lsl #12
20007c70:	f44f 1c80 	mov.w	ip, #1048576	; 0x100000
20007c74:	ea4c 3111 	orr.w	r1, ip, r1, lsr #12
20007c78:	d002      	beq.n	20007c80 <__adddf3+0x70>
20007c7a:	4240      	negs	r0, r0
20007c7c:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
20007c80:	f013 4f00 	tst.w	r3, #2147483648	; 0x80000000
20007c84:	ea4f 3303 	mov.w	r3, r3, lsl #12
20007c88:	ea4c 3313 	orr.w	r3, ip, r3, lsr #12
20007c8c:	d002      	beq.n	20007c94 <__adddf3+0x84>
20007c8e:	4252      	negs	r2, r2
20007c90:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
20007c94:	ea94 0f05 	teq	r4, r5
20007c98:	f000 80a7 	beq.w	20007dea <__adddf3+0x1da>
20007c9c:	f1a4 0401 	sub.w	r4, r4, #1
20007ca0:	f1d5 0e20 	rsbs	lr, r5, #32
20007ca4:	db0d      	blt.n	20007cc2 <__adddf3+0xb2>
20007ca6:	fa02 fc0e 	lsl.w	ip, r2, lr
20007caa:	fa22 f205 	lsr.w	r2, r2, r5
20007cae:	1880      	adds	r0, r0, r2
20007cb0:	f141 0100 	adc.w	r1, r1, #0
20007cb4:	fa03 f20e 	lsl.w	r2, r3, lr
20007cb8:	1880      	adds	r0, r0, r2
20007cba:	fa43 f305 	asr.w	r3, r3, r5
20007cbe:	4159      	adcs	r1, r3
20007cc0:	e00e      	b.n	20007ce0 <__adddf3+0xd0>
20007cc2:	f1a5 0520 	sub.w	r5, r5, #32
20007cc6:	f10e 0e20 	add.w	lr, lr, #32
20007cca:	2a01      	cmp	r2, #1
20007ccc:	fa03 fc0e 	lsl.w	ip, r3, lr
20007cd0:	bf28      	it	cs
20007cd2:	f04c 0c02 	orrcs.w	ip, ip, #2
20007cd6:	fa43 f305 	asr.w	r3, r3, r5
20007cda:	18c0      	adds	r0, r0, r3
20007cdc:	eb51 71e3 	adcs.w	r1, r1, r3, asr #31
20007ce0:	f001 4500 	and.w	r5, r1, #2147483648	; 0x80000000
20007ce4:	d507      	bpl.n	20007cf6 <__adddf3+0xe6>
20007ce6:	f04f 0e00 	mov.w	lr, #0
20007cea:	f1dc 0c00 	rsbs	ip, ip, #0
20007cee:	eb7e 0000 	sbcs.w	r0, lr, r0
20007cf2:	eb6e 0101 	sbc.w	r1, lr, r1
20007cf6:	f5b1 1f80 	cmp.w	r1, #1048576	; 0x100000
20007cfa:	d31b      	bcc.n	20007d34 <__adddf3+0x124>
20007cfc:	f5b1 1f00 	cmp.w	r1, #2097152	; 0x200000
20007d00:	d30c      	bcc.n	20007d1c <__adddf3+0x10c>
20007d02:	0849      	lsrs	r1, r1, #1
20007d04:	ea5f 0030 	movs.w	r0, r0, rrx
20007d08:	ea4f 0c3c 	mov.w	ip, ip, rrx
20007d0c:	f104 0401 	add.w	r4, r4, #1
20007d10:	ea4f 5244 	mov.w	r2, r4, lsl #21
20007d14:	f512 0f80 	cmn.w	r2, #4194304	; 0x400000
20007d18:	f080 809a 	bcs.w	20007e50 <__adddf3+0x240>
20007d1c:	f1bc 4f00 	cmp.w	ip, #2147483648	; 0x80000000
20007d20:	bf08      	it	eq
20007d22:	ea5f 0c50 	movseq.w	ip, r0, lsr #1
20007d26:	f150 0000 	adcs.w	r0, r0, #0
20007d2a:	eb41 5104 	adc.w	r1, r1, r4, lsl #20
20007d2e:	ea41 0105 	orr.w	r1, r1, r5
20007d32:	bd30      	pop	{r4, r5, pc}
20007d34:	ea5f 0c4c 	movs.w	ip, ip, lsl #1
20007d38:	4140      	adcs	r0, r0
20007d3a:	eb41 0101 	adc.w	r1, r1, r1
20007d3e:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
20007d42:	f1a4 0401 	sub.w	r4, r4, #1
20007d46:	d1e9      	bne.n	20007d1c <__adddf3+0x10c>
20007d48:	f091 0f00 	teq	r1, #0
20007d4c:	bf04      	itt	eq
20007d4e:	4601      	moveq	r1, r0
20007d50:	2000      	moveq	r0, #0
20007d52:	fab1 f381 	clz	r3, r1
20007d56:	bf08      	it	eq
20007d58:	3320      	addeq	r3, #32
20007d5a:	f1a3 030b 	sub.w	r3, r3, #11
20007d5e:	f1b3 0220 	subs.w	r2, r3, #32
20007d62:	da0c      	bge.n	20007d7e <__adddf3+0x16e>
20007d64:	320c      	adds	r2, #12
20007d66:	dd08      	ble.n	20007d7a <__adddf3+0x16a>
20007d68:	f102 0c14 	add.w	ip, r2, #20
20007d6c:	f1c2 020c 	rsb	r2, r2, #12
20007d70:	fa01 f00c 	lsl.w	r0, r1, ip
20007d74:	fa21 f102 	lsr.w	r1, r1, r2
20007d78:	e00c      	b.n	20007d94 <__adddf3+0x184>
20007d7a:	f102 0214 	add.w	r2, r2, #20
20007d7e:	bfd8      	it	le
20007d80:	f1c2 0c20 	rsble	ip, r2, #32
20007d84:	fa01 f102 	lsl.w	r1, r1, r2
20007d88:	fa20 fc0c 	lsr.w	ip, r0, ip
20007d8c:	bfdc      	itt	le
20007d8e:	ea41 010c 	orrle.w	r1, r1, ip
20007d92:	4090      	lslle	r0, r2
20007d94:	1ae4      	subs	r4, r4, r3
20007d96:	bfa2      	ittt	ge
20007d98:	eb01 5104 	addge.w	r1, r1, r4, lsl #20
20007d9c:	4329      	orrge	r1, r5
20007d9e:	bd30      	popge	{r4, r5, pc}
20007da0:	ea6f 0404 	mvn.w	r4, r4
20007da4:	3c1f      	subs	r4, #31
20007da6:	da1c      	bge.n	20007de2 <__adddf3+0x1d2>
20007da8:	340c      	adds	r4, #12
20007daa:	dc0e      	bgt.n	20007dca <__adddf3+0x1ba>
20007dac:	f104 0414 	add.w	r4, r4, #20
20007db0:	f1c4 0220 	rsb	r2, r4, #32
20007db4:	fa20 f004 	lsr.w	r0, r0, r4
20007db8:	fa01 f302 	lsl.w	r3, r1, r2
20007dbc:	ea40 0003 	orr.w	r0, r0, r3
20007dc0:	fa21 f304 	lsr.w	r3, r1, r4
20007dc4:	ea45 0103 	orr.w	r1, r5, r3
20007dc8:	bd30      	pop	{r4, r5, pc}
20007dca:	f1c4 040c 	rsb	r4, r4, #12
20007dce:	f1c4 0220 	rsb	r2, r4, #32
20007dd2:	fa20 f002 	lsr.w	r0, r0, r2
20007dd6:	fa01 f304 	lsl.w	r3, r1, r4
20007dda:	ea40 0003 	orr.w	r0, r0, r3
20007dde:	4629      	mov	r1, r5
20007de0:	bd30      	pop	{r4, r5, pc}
20007de2:	fa21 f004 	lsr.w	r0, r1, r4
20007de6:	4629      	mov	r1, r5
20007de8:	bd30      	pop	{r4, r5, pc}
20007dea:	f094 0f00 	teq	r4, #0
20007dee:	f483 1380 	eor.w	r3, r3, #1048576	; 0x100000
20007df2:	bf06      	itte	eq
20007df4:	f481 1180 	eoreq.w	r1, r1, #1048576	; 0x100000
20007df8:	3401      	addeq	r4, #1
20007dfa:	3d01      	subne	r5, #1
20007dfc:	e74e      	b.n	20007c9c <__adddf3+0x8c>
20007dfe:	ea7f 5c64 	mvns.w	ip, r4, asr #21
20007e02:	bf18      	it	ne
20007e04:	ea7f 5c65 	mvnsne.w	ip, r5, asr #21
20007e08:	d029      	beq.n	20007e5e <__adddf3+0x24e>
20007e0a:	ea94 0f05 	teq	r4, r5
20007e0e:	bf08      	it	eq
20007e10:	ea90 0f02 	teqeq	r0, r2
20007e14:	d005      	beq.n	20007e22 <__adddf3+0x212>
20007e16:	ea54 0c00 	orrs.w	ip, r4, r0
20007e1a:	bf04      	itt	eq
20007e1c:	4619      	moveq	r1, r3
20007e1e:	4610      	moveq	r0, r2
20007e20:	bd30      	pop	{r4, r5, pc}
20007e22:	ea91 0f03 	teq	r1, r3
20007e26:	bf1e      	ittt	ne
20007e28:	2100      	movne	r1, #0
20007e2a:	2000      	movne	r0, #0
20007e2c:	bd30      	popne	{r4, r5, pc}
20007e2e:	ea5f 5c54 	movs.w	ip, r4, lsr #21
20007e32:	d105      	bne.n	20007e40 <__adddf3+0x230>
20007e34:	0040      	lsls	r0, r0, #1
20007e36:	4149      	adcs	r1, r1
20007e38:	bf28      	it	cs
20007e3a:	f041 4100 	orrcs.w	r1, r1, #2147483648	; 0x80000000
20007e3e:	bd30      	pop	{r4, r5, pc}
20007e40:	f514 0480 	adds.w	r4, r4, #4194304	; 0x400000
20007e44:	bf3c      	itt	cc
20007e46:	f501 1180 	addcc.w	r1, r1, #1048576	; 0x100000
20007e4a:	bd30      	popcc	{r4, r5, pc}
20007e4c:	f001 4500 	and.w	r5, r1, #2147483648	; 0x80000000
20007e50:	f045 41fe 	orr.w	r1, r5, #2130706432	; 0x7f000000
20007e54:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
20007e58:	f04f 0000 	mov.w	r0, #0
20007e5c:	bd30      	pop	{r4, r5, pc}
20007e5e:	ea7f 5c64 	mvns.w	ip, r4, asr #21
20007e62:	bf1a      	itte	ne
20007e64:	4619      	movne	r1, r3
20007e66:	4610      	movne	r0, r2
20007e68:	ea7f 5c65 	mvnseq.w	ip, r5, asr #21
20007e6c:	bf1c      	itt	ne
20007e6e:	460b      	movne	r3, r1
20007e70:	4602      	movne	r2, r0
20007e72:	ea50 3401 	orrs.w	r4, r0, r1, lsl #12
20007e76:	bf06      	itte	eq
20007e78:	ea52 3503 	orrseq.w	r5, r2, r3, lsl #12
20007e7c:	ea91 0f03 	teqeq	r1, r3
20007e80:	f441 2100 	orrne.w	r1, r1, #524288	; 0x80000
20007e84:	bd30      	pop	{r4, r5, pc}
20007e86:	bf00      	nop

20007e88 <__aeabi_ui2d>:
20007e88:	f090 0f00 	teq	r0, #0
20007e8c:	bf04      	itt	eq
20007e8e:	2100      	moveq	r1, #0
20007e90:	4770      	bxeq	lr
20007e92:	b530      	push	{r4, r5, lr}
20007e94:	f44f 6480 	mov.w	r4, #1024	; 0x400
20007e98:	f104 0432 	add.w	r4, r4, #50	; 0x32
20007e9c:	f04f 0500 	mov.w	r5, #0
20007ea0:	f04f 0100 	mov.w	r1, #0
20007ea4:	e750      	b.n	20007d48 <__adddf3+0x138>
20007ea6:	bf00      	nop

20007ea8 <__aeabi_i2d>:
20007ea8:	f090 0f00 	teq	r0, #0
20007eac:	bf04      	itt	eq
20007eae:	2100      	moveq	r1, #0
20007eb0:	4770      	bxeq	lr
20007eb2:	b530      	push	{r4, r5, lr}
20007eb4:	f44f 6480 	mov.w	r4, #1024	; 0x400
20007eb8:	f104 0432 	add.w	r4, r4, #50	; 0x32
20007ebc:	f010 4500 	ands.w	r5, r0, #2147483648	; 0x80000000
20007ec0:	bf48      	it	mi
20007ec2:	4240      	negmi	r0, r0
20007ec4:	f04f 0100 	mov.w	r1, #0
20007ec8:	e73e      	b.n	20007d48 <__adddf3+0x138>
20007eca:	bf00      	nop

20007ecc <__aeabi_f2d>:
20007ecc:	0042      	lsls	r2, r0, #1
20007ece:	ea4f 01e2 	mov.w	r1, r2, asr #3
20007ed2:	ea4f 0131 	mov.w	r1, r1, rrx
20007ed6:	ea4f 7002 	mov.w	r0, r2, lsl #28
20007eda:	bf1f      	itttt	ne
20007edc:	f012 437f 	andsne.w	r3, r2, #4278190080	; 0xff000000
20007ee0:	f093 4f7f 	teqne	r3, #4278190080	; 0xff000000
20007ee4:	f081 5160 	eorne.w	r1, r1, #939524096	; 0x38000000
20007ee8:	4770      	bxne	lr
20007eea:	f092 0f00 	teq	r2, #0
20007eee:	bf14      	ite	ne
20007ef0:	f093 4f7f 	teqne	r3, #4278190080	; 0xff000000
20007ef4:	4770      	bxeq	lr
20007ef6:	b530      	push	{r4, r5, lr}
20007ef8:	f44f 7460 	mov.w	r4, #896	; 0x380
20007efc:	f001 4500 	and.w	r5, r1, #2147483648	; 0x80000000
20007f00:	f021 4100 	bic.w	r1, r1, #2147483648	; 0x80000000
20007f04:	e720      	b.n	20007d48 <__adddf3+0x138>
20007f06:	bf00      	nop

20007f08 <__aeabi_ul2d>:
20007f08:	ea50 0201 	orrs.w	r2, r0, r1
20007f0c:	bf08      	it	eq
20007f0e:	4770      	bxeq	lr
20007f10:	b530      	push	{r4, r5, lr}
20007f12:	f04f 0500 	mov.w	r5, #0
20007f16:	e00a      	b.n	20007f2e <__aeabi_l2d+0x16>

20007f18 <__aeabi_l2d>:
20007f18:	ea50 0201 	orrs.w	r2, r0, r1
20007f1c:	bf08      	it	eq
20007f1e:	4770      	bxeq	lr
20007f20:	b530      	push	{r4, r5, lr}
20007f22:	f011 4500 	ands.w	r5, r1, #2147483648	; 0x80000000
20007f26:	d502      	bpl.n	20007f2e <__aeabi_l2d+0x16>
20007f28:	4240      	negs	r0, r0
20007f2a:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
20007f2e:	f44f 6480 	mov.w	r4, #1024	; 0x400
20007f32:	f104 0432 	add.w	r4, r4, #50	; 0x32
20007f36:	ea5f 5c91 	movs.w	ip, r1, lsr #22
20007f3a:	f43f aedc 	beq.w	20007cf6 <__adddf3+0xe6>
20007f3e:	f04f 0203 	mov.w	r2, #3
20007f42:	ea5f 0cdc 	movs.w	ip, ip, lsr #3
20007f46:	bf18      	it	ne
20007f48:	3203      	addne	r2, #3
20007f4a:	ea5f 0cdc 	movs.w	ip, ip, lsr #3
20007f4e:	bf18      	it	ne
20007f50:	3203      	addne	r2, #3
20007f52:	eb02 02dc 	add.w	r2, r2, ip, lsr #3
20007f56:	f1c2 0320 	rsb	r3, r2, #32
20007f5a:	fa00 fc03 	lsl.w	ip, r0, r3
20007f5e:	fa20 f002 	lsr.w	r0, r0, r2
20007f62:	fa01 fe03 	lsl.w	lr, r1, r3
20007f66:	ea40 000e 	orr.w	r0, r0, lr
20007f6a:	fa21 f102 	lsr.w	r1, r1, r2
20007f6e:	4414      	add	r4, r2
20007f70:	e6c1      	b.n	20007cf6 <__adddf3+0xe6>
20007f72:	bf00      	nop

20007f74 <__aeabi_dmul>:
20007f74:	b570      	push	{r4, r5, r6, lr}
20007f76:	f04f 0cff 	mov.w	ip, #255	; 0xff
20007f7a:	f44c 6ce0 	orr.w	ip, ip, #1792	; 0x700
20007f7e:	ea1c 5411 	ands.w	r4, ip, r1, lsr #20
20007f82:	bf1d      	ittte	ne
20007f84:	ea1c 5513 	andsne.w	r5, ip, r3, lsr #20
20007f88:	ea94 0f0c 	teqne	r4, ip
20007f8c:	ea95 0f0c 	teqne	r5, ip
20007f90:	f000 f8de 	bleq	20008150 <__aeabi_dmul+0x1dc>
20007f94:	442c      	add	r4, r5
20007f96:	ea81 0603 	eor.w	r6, r1, r3
20007f9a:	ea21 514c 	bic.w	r1, r1, ip, lsl #21
20007f9e:	ea23 534c 	bic.w	r3, r3, ip, lsl #21
20007fa2:	ea50 3501 	orrs.w	r5, r0, r1, lsl #12
20007fa6:	bf18      	it	ne
20007fa8:	ea52 3503 	orrsne.w	r5, r2, r3, lsl #12
20007fac:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
20007fb0:	f443 1380 	orr.w	r3, r3, #1048576	; 0x100000
20007fb4:	d038      	beq.n	20008028 <__aeabi_dmul+0xb4>
20007fb6:	fba0 ce02 	umull	ip, lr, r0, r2
20007fba:	f04f 0500 	mov.w	r5, #0
20007fbe:	fbe1 e502 	umlal	lr, r5, r1, r2
20007fc2:	f006 4200 	and.w	r2, r6, #2147483648	; 0x80000000
20007fc6:	fbe0 e503 	umlal	lr, r5, r0, r3
20007fca:	f04f 0600 	mov.w	r6, #0
20007fce:	fbe1 5603 	umlal	r5, r6, r1, r3
20007fd2:	f09c 0f00 	teq	ip, #0
20007fd6:	bf18      	it	ne
20007fd8:	f04e 0e01 	orrne.w	lr, lr, #1
20007fdc:	f1a4 04ff 	sub.w	r4, r4, #255	; 0xff
20007fe0:	f5b6 7f00 	cmp.w	r6, #512	; 0x200
20007fe4:	f564 7440 	sbc.w	r4, r4, #768	; 0x300
20007fe8:	d204      	bcs.n	20007ff4 <__aeabi_dmul+0x80>
20007fea:	ea5f 0e4e 	movs.w	lr, lr, lsl #1
20007fee:	416d      	adcs	r5, r5
20007ff0:	eb46 0606 	adc.w	r6, r6, r6
20007ff4:	ea42 21c6 	orr.w	r1, r2, r6, lsl #11
20007ff8:	ea41 5155 	orr.w	r1, r1, r5, lsr #21
20007ffc:	ea4f 20c5 	mov.w	r0, r5, lsl #11
20008000:	ea40 505e 	orr.w	r0, r0, lr, lsr #21
20008004:	ea4f 2ece 	mov.w	lr, lr, lsl #11
20008008:	f1b4 0cfd 	subs.w	ip, r4, #253	; 0xfd
2000800c:	bf88      	it	hi
2000800e:	f5bc 6fe0 	cmphi.w	ip, #1792	; 0x700
20008012:	d81e      	bhi.n	20008052 <__aeabi_dmul+0xde>
20008014:	f1be 4f00 	cmp.w	lr, #2147483648	; 0x80000000
20008018:	bf08      	it	eq
2000801a:	ea5f 0e50 	movseq.w	lr, r0, lsr #1
2000801e:	f150 0000 	adcs.w	r0, r0, #0
20008022:	eb41 5104 	adc.w	r1, r1, r4, lsl #20
20008026:	bd70      	pop	{r4, r5, r6, pc}
20008028:	f006 4600 	and.w	r6, r6, #2147483648	; 0x80000000
2000802c:	ea46 0101 	orr.w	r1, r6, r1
20008030:	ea40 0002 	orr.w	r0, r0, r2
20008034:	ea81 0103 	eor.w	r1, r1, r3
20008038:	ebb4 045c 	subs.w	r4, r4, ip, lsr #1
2000803c:	bfc2      	ittt	gt
2000803e:	ebd4 050c 	rsbsgt	r5, r4, ip
20008042:	ea41 5104 	orrgt.w	r1, r1, r4, lsl #20
20008046:	bd70      	popgt	{r4, r5, r6, pc}
20008048:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
2000804c:	f04f 0e00 	mov.w	lr, #0
20008050:	3c01      	subs	r4, #1
20008052:	f300 80ab 	bgt.w	200081ac <__aeabi_dmul+0x238>
20008056:	f114 0f36 	cmn.w	r4, #54	; 0x36
2000805a:	bfde      	ittt	le
2000805c:	2000      	movle	r0, #0
2000805e:	f001 4100 	andle.w	r1, r1, #2147483648	; 0x80000000
20008062:	bd70      	pople	{r4, r5, r6, pc}
20008064:	f1c4 0400 	rsb	r4, r4, #0
20008068:	3c20      	subs	r4, #32
2000806a:	da35      	bge.n	200080d8 <__aeabi_dmul+0x164>
2000806c:	340c      	adds	r4, #12
2000806e:	dc1b      	bgt.n	200080a8 <__aeabi_dmul+0x134>
20008070:	f104 0414 	add.w	r4, r4, #20
20008074:	f1c4 0520 	rsb	r5, r4, #32
20008078:	fa00 f305 	lsl.w	r3, r0, r5
2000807c:	fa20 f004 	lsr.w	r0, r0, r4
20008080:	fa01 f205 	lsl.w	r2, r1, r5
20008084:	ea40 0002 	orr.w	r0, r0, r2
20008088:	f001 4200 	and.w	r2, r1, #2147483648	; 0x80000000
2000808c:	f021 4100 	bic.w	r1, r1, #2147483648	; 0x80000000
20008090:	eb10 70d3 	adds.w	r0, r0, r3, lsr #31
20008094:	fa21 f604 	lsr.w	r6, r1, r4
20008098:	eb42 0106 	adc.w	r1, r2, r6
2000809c:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
200080a0:	bf08      	it	eq
200080a2:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
200080a6:	bd70      	pop	{r4, r5, r6, pc}
200080a8:	f1c4 040c 	rsb	r4, r4, #12
200080ac:	f1c4 0520 	rsb	r5, r4, #32
200080b0:	fa00 f304 	lsl.w	r3, r0, r4
200080b4:	fa20 f005 	lsr.w	r0, r0, r5
200080b8:	fa01 f204 	lsl.w	r2, r1, r4
200080bc:	ea40 0002 	orr.w	r0, r0, r2
200080c0:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
200080c4:	eb10 70d3 	adds.w	r0, r0, r3, lsr #31
200080c8:	f141 0100 	adc.w	r1, r1, #0
200080cc:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
200080d0:	bf08      	it	eq
200080d2:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
200080d6:	bd70      	pop	{r4, r5, r6, pc}
200080d8:	f1c4 0520 	rsb	r5, r4, #32
200080dc:	fa00 f205 	lsl.w	r2, r0, r5
200080e0:	ea4e 0e02 	orr.w	lr, lr, r2
200080e4:	fa20 f304 	lsr.w	r3, r0, r4
200080e8:	fa01 f205 	lsl.w	r2, r1, r5
200080ec:	ea43 0302 	orr.w	r3, r3, r2
200080f0:	fa21 f004 	lsr.w	r0, r1, r4
200080f4:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
200080f8:	fa21 f204 	lsr.w	r2, r1, r4
200080fc:	ea20 0002 	bic.w	r0, r0, r2
20008100:	eb00 70d3 	add.w	r0, r0, r3, lsr #31
20008104:	ea5e 0e43 	orrs.w	lr, lr, r3, lsl #1
20008108:	bf08      	it	eq
2000810a:	ea20 70d3 	biceq.w	r0, r0, r3, lsr #31
2000810e:	bd70      	pop	{r4, r5, r6, pc}
20008110:	f094 0f00 	teq	r4, #0
20008114:	d10f      	bne.n	20008136 <__aeabi_dmul+0x1c2>
20008116:	f001 4600 	and.w	r6, r1, #2147483648	; 0x80000000
2000811a:	0040      	lsls	r0, r0, #1
2000811c:	eb41 0101 	adc.w	r1, r1, r1
20008120:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
20008124:	bf08      	it	eq
20008126:	3c01      	subeq	r4, #1
20008128:	d0f7      	beq.n	2000811a <__aeabi_dmul+0x1a6>
2000812a:	ea41 0106 	orr.w	r1, r1, r6
2000812e:	f095 0f00 	teq	r5, #0
20008132:	bf18      	it	ne
20008134:	4770      	bxne	lr
20008136:	f003 4600 	and.w	r6, r3, #2147483648	; 0x80000000
2000813a:	0052      	lsls	r2, r2, #1
2000813c:	eb43 0303 	adc.w	r3, r3, r3
20008140:	f413 1f80 	tst.w	r3, #1048576	; 0x100000
20008144:	bf08      	it	eq
20008146:	3d01      	subeq	r5, #1
20008148:	d0f7      	beq.n	2000813a <__aeabi_dmul+0x1c6>
2000814a:	ea43 0306 	orr.w	r3, r3, r6
2000814e:	4770      	bx	lr
20008150:	ea94 0f0c 	teq	r4, ip
20008154:	ea0c 5513 	and.w	r5, ip, r3, lsr #20
20008158:	bf18      	it	ne
2000815a:	ea95 0f0c 	teqne	r5, ip
2000815e:	d00c      	beq.n	2000817a <__aeabi_dmul+0x206>
20008160:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
20008164:	bf18      	it	ne
20008166:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
2000816a:	d1d1      	bne.n	20008110 <__aeabi_dmul+0x19c>
2000816c:	ea81 0103 	eor.w	r1, r1, r3
20008170:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
20008174:	f04f 0000 	mov.w	r0, #0
20008178:	bd70      	pop	{r4, r5, r6, pc}
2000817a:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
2000817e:	bf06      	itte	eq
20008180:	4610      	moveq	r0, r2
20008182:	4619      	moveq	r1, r3
20008184:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
20008188:	d019      	beq.n	200081be <__aeabi_dmul+0x24a>
2000818a:	ea94 0f0c 	teq	r4, ip
2000818e:	d102      	bne.n	20008196 <__aeabi_dmul+0x222>
20008190:	ea50 3601 	orrs.w	r6, r0, r1, lsl #12
20008194:	d113      	bne.n	200081be <__aeabi_dmul+0x24a>
20008196:	ea95 0f0c 	teq	r5, ip
2000819a:	d105      	bne.n	200081a8 <__aeabi_dmul+0x234>
2000819c:	ea52 3603 	orrs.w	r6, r2, r3, lsl #12
200081a0:	bf1c      	itt	ne
200081a2:	4610      	movne	r0, r2
200081a4:	4619      	movne	r1, r3
200081a6:	d10a      	bne.n	200081be <__aeabi_dmul+0x24a>
200081a8:	ea81 0103 	eor.w	r1, r1, r3
200081ac:	f001 4100 	and.w	r1, r1, #2147483648	; 0x80000000
200081b0:	f041 41fe 	orr.w	r1, r1, #2130706432	; 0x7f000000
200081b4:	f441 0170 	orr.w	r1, r1, #15728640	; 0xf00000
200081b8:	f04f 0000 	mov.w	r0, #0
200081bc:	bd70      	pop	{r4, r5, r6, pc}
200081be:	f041 41fe 	orr.w	r1, r1, #2130706432	; 0x7f000000
200081c2:	f441 0178 	orr.w	r1, r1, #16252928	; 0xf80000
200081c6:	bd70      	pop	{r4, r5, r6, pc}

200081c8 <__aeabi_ddiv>:
200081c8:	b570      	push	{r4, r5, r6, lr}
200081ca:	f04f 0cff 	mov.w	ip, #255	; 0xff
200081ce:	f44c 6ce0 	orr.w	ip, ip, #1792	; 0x700
200081d2:	ea1c 5411 	ands.w	r4, ip, r1, lsr #20
200081d6:	bf1d      	ittte	ne
200081d8:	ea1c 5513 	andsne.w	r5, ip, r3, lsr #20
200081dc:	ea94 0f0c 	teqne	r4, ip
200081e0:	ea95 0f0c 	teqne	r5, ip
200081e4:	f000 f8a7 	bleq	20008336 <__aeabi_ddiv+0x16e>
200081e8:	eba4 0405 	sub.w	r4, r4, r5
200081ec:	ea81 0e03 	eor.w	lr, r1, r3
200081f0:	ea52 3503 	orrs.w	r5, r2, r3, lsl #12
200081f4:	ea4f 3101 	mov.w	r1, r1, lsl #12
200081f8:	f000 8088 	beq.w	2000830c <__aeabi_ddiv+0x144>
200081fc:	ea4f 3303 	mov.w	r3, r3, lsl #12
20008200:	f04f 5580 	mov.w	r5, #268435456	; 0x10000000
20008204:	ea45 1313 	orr.w	r3, r5, r3, lsr #4
20008208:	ea43 6312 	orr.w	r3, r3, r2, lsr #24
2000820c:	ea4f 2202 	mov.w	r2, r2, lsl #8
20008210:	ea45 1511 	orr.w	r5, r5, r1, lsr #4
20008214:	ea45 6510 	orr.w	r5, r5, r0, lsr #24
20008218:	ea4f 2600 	mov.w	r6, r0, lsl #8
2000821c:	f00e 4100 	and.w	r1, lr, #2147483648	; 0x80000000
20008220:	429d      	cmp	r5, r3
20008222:	bf08      	it	eq
20008224:	4296      	cmpeq	r6, r2
20008226:	f144 04fd 	adc.w	r4, r4, #253	; 0xfd
2000822a:	f504 7440 	add.w	r4, r4, #768	; 0x300
2000822e:	d202      	bcs.n	20008236 <__aeabi_ddiv+0x6e>
20008230:	085b      	lsrs	r3, r3, #1
20008232:	ea4f 0232 	mov.w	r2, r2, rrx
20008236:	1ab6      	subs	r6, r6, r2
20008238:	eb65 0503 	sbc.w	r5, r5, r3
2000823c:	085b      	lsrs	r3, r3, #1
2000823e:	ea4f 0232 	mov.w	r2, r2, rrx
20008242:	f44f 1080 	mov.w	r0, #1048576	; 0x100000
20008246:	f44f 2c00 	mov.w	ip, #524288	; 0x80000
2000824a:	ebb6 0e02 	subs.w	lr, r6, r2
2000824e:	eb75 0e03 	sbcs.w	lr, r5, r3
20008252:	bf22      	ittt	cs
20008254:	1ab6      	subcs	r6, r6, r2
20008256:	4675      	movcs	r5, lr
20008258:	ea40 000c 	orrcs.w	r0, r0, ip
2000825c:	085b      	lsrs	r3, r3, #1
2000825e:	ea4f 0232 	mov.w	r2, r2, rrx
20008262:	ebb6 0e02 	subs.w	lr, r6, r2
20008266:	eb75 0e03 	sbcs.w	lr, r5, r3
2000826a:	bf22      	ittt	cs
2000826c:	1ab6      	subcs	r6, r6, r2
2000826e:	4675      	movcs	r5, lr
20008270:	ea40 005c 	orrcs.w	r0, r0, ip, lsr #1
20008274:	085b      	lsrs	r3, r3, #1
20008276:	ea4f 0232 	mov.w	r2, r2, rrx
2000827a:	ebb6 0e02 	subs.w	lr, r6, r2
2000827e:	eb75 0e03 	sbcs.w	lr, r5, r3
20008282:	bf22      	ittt	cs
20008284:	1ab6      	subcs	r6, r6, r2
20008286:	4675      	movcs	r5, lr
20008288:	ea40 009c 	orrcs.w	r0, r0, ip, lsr #2
2000828c:	085b      	lsrs	r3, r3, #1
2000828e:	ea4f 0232 	mov.w	r2, r2, rrx
20008292:	ebb6 0e02 	subs.w	lr, r6, r2
20008296:	eb75 0e03 	sbcs.w	lr, r5, r3
2000829a:	bf22      	ittt	cs
2000829c:	1ab6      	subcs	r6, r6, r2
2000829e:	4675      	movcs	r5, lr
200082a0:	ea40 00dc 	orrcs.w	r0, r0, ip, lsr #3
200082a4:	ea55 0e06 	orrs.w	lr, r5, r6
200082a8:	d018      	beq.n	200082dc <__aeabi_ddiv+0x114>
200082aa:	ea4f 1505 	mov.w	r5, r5, lsl #4
200082ae:	ea45 7516 	orr.w	r5, r5, r6, lsr #28
200082b2:	ea4f 1606 	mov.w	r6, r6, lsl #4
200082b6:	ea4f 03c3 	mov.w	r3, r3, lsl #3
200082ba:	ea43 7352 	orr.w	r3, r3, r2, lsr #29
200082be:	ea4f 02c2 	mov.w	r2, r2, lsl #3
200082c2:	ea5f 1c1c 	movs.w	ip, ip, lsr #4
200082c6:	d1c0      	bne.n	2000824a <__aeabi_ddiv+0x82>
200082c8:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
200082cc:	d10b      	bne.n	200082e6 <__aeabi_ddiv+0x11e>
200082ce:	ea41 0100 	orr.w	r1, r1, r0
200082d2:	f04f 0000 	mov.w	r0, #0
200082d6:	f04f 4c00 	mov.w	ip, #2147483648	; 0x80000000
200082da:	e7b6      	b.n	2000824a <__aeabi_ddiv+0x82>
200082dc:	f411 1f80 	tst.w	r1, #1048576	; 0x100000
200082e0:	bf04      	itt	eq
200082e2:	4301      	orreq	r1, r0
200082e4:	2000      	moveq	r0, #0
200082e6:	f1b4 0cfd 	subs.w	ip, r4, #253	; 0xfd
200082ea:	bf88      	it	hi
200082ec:	f5bc 6fe0 	cmphi.w	ip, #1792	; 0x700
200082f0:	f63f aeaf 	bhi.w	20008052 <__aeabi_dmul+0xde>
200082f4:	ebb5 0c03 	subs.w	ip, r5, r3
200082f8:	bf04      	itt	eq
200082fa:	ebb6 0c02 	subseq.w	ip, r6, r2
200082fe:	ea5f 0c50 	movseq.w	ip, r0, lsr #1
20008302:	f150 0000 	adcs.w	r0, r0, #0
20008306:	eb41 5104 	adc.w	r1, r1, r4, lsl #20
2000830a:	bd70      	pop	{r4, r5, r6, pc}
2000830c:	f00e 4e00 	and.w	lr, lr, #2147483648	; 0x80000000
20008310:	ea4e 3111 	orr.w	r1, lr, r1, lsr #12
20008314:	eb14 045c 	adds.w	r4, r4, ip, lsr #1
20008318:	bfc2      	ittt	gt
2000831a:	ebd4 050c 	rsbsgt	r5, r4, ip
2000831e:	ea41 5104 	orrgt.w	r1, r1, r4, lsl #20
20008322:	bd70      	popgt	{r4, r5, r6, pc}
20008324:	f441 1180 	orr.w	r1, r1, #1048576	; 0x100000
20008328:	f04f 0e00 	mov.w	lr, #0
2000832c:	3c01      	subs	r4, #1
2000832e:	e690      	b.n	20008052 <__aeabi_dmul+0xde>
20008330:	ea45 0e06 	orr.w	lr, r5, r6
20008334:	e68d      	b.n	20008052 <__aeabi_dmul+0xde>
20008336:	ea0c 5513 	and.w	r5, ip, r3, lsr #20
2000833a:	ea94 0f0c 	teq	r4, ip
2000833e:	bf08      	it	eq
20008340:	ea95 0f0c 	teqeq	r5, ip
20008344:	f43f af3b 	beq.w	200081be <__aeabi_dmul+0x24a>
20008348:	ea94 0f0c 	teq	r4, ip
2000834c:	d10a      	bne.n	20008364 <__aeabi_ddiv+0x19c>
2000834e:	ea50 3401 	orrs.w	r4, r0, r1, lsl #12
20008352:	f47f af34 	bne.w	200081be <__aeabi_dmul+0x24a>
20008356:	ea95 0f0c 	teq	r5, ip
2000835a:	f47f af25 	bne.w	200081a8 <__aeabi_dmul+0x234>
2000835e:	4610      	mov	r0, r2
20008360:	4619      	mov	r1, r3
20008362:	e72c      	b.n	200081be <__aeabi_dmul+0x24a>
20008364:	ea95 0f0c 	teq	r5, ip
20008368:	d106      	bne.n	20008378 <__aeabi_ddiv+0x1b0>
2000836a:	ea52 3503 	orrs.w	r5, r2, r3, lsl #12
2000836e:	f43f aefd 	beq.w	2000816c <__aeabi_dmul+0x1f8>
20008372:	4610      	mov	r0, r2
20008374:	4619      	mov	r1, r3
20008376:	e722      	b.n	200081be <__aeabi_dmul+0x24a>
20008378:	ea50 0641 	orrs.w	r6, r0, r1, lsl #1
2000837c:	bf18      	it	ne
2000837e:	ea52 0643 	orrsne.w	r6, r2, r3, lsl #1
20008382:	f47f aec5 	bne.w	20008110 <__aeabi_dmul+0x19c>
20008386:	ea50 0441 	orrs.w	r4, r0, r1, lsl #1
2000838a:	f47f af0d 	bne.w	200081a8 <__aeabi_dmul+0x234>
2000838e:	ea52 0543 	orrs.w	r5, r2, r3, lsl #1
20008392:	f47f aeeb 	bne.w	2000816c <__aeabi_dmul+0x1f8>
20008396:	e712      	b.n	200081be <__aeabi_dmul+0x24a>

20008398 <__gedf2>:
20008398:	f04f 3cff 	mov.w	ip, #4294967295
2000839c:	e006      	b.n	200083ac <__cmpdf2+0x4>
2000839e:	bf00      	nop

200083a0 <__ledf2>:
200083a0:	f04f 0c01 	mov.w	ip, #1
200083a4:	e002      	b.n	200083ac <__cmpdf2+0x4>
200083a6:	bf00      	nop

200083a8 <__cmpdf2>:
200083a8:	f04f 0c01 	mov.w	ip, #1
200083ac:	f84d cd04 	str.w	ip, [sp, #-4]!
200083b0:	ea4f 0c41 	mov.w	ip, r1, lsl #1
200083b4:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
200083b8:	ea4f 0c43 	mov.w	ip, r3, lsl #1
200083bc:	bf18      	it	ne
200083be:	ea7f 5c6c 	mvnsne.w	ip, ip, asr #21
200083c2:	d01b      	beq.n	200083fc <__cmpdf2+0x54>
200083c4:	b001      	add	sp, #4
200083c6:	ea50 0c41 	orrs.w	ip, r0, r1, lsl #1
200083ca:	bf0c      	ite	eq
200083cc:	ea52 0c43 	orrseq.w	ip, r2, r3, lsl #1
200083d0:	ea91 0f03 	teqne	r1, r3
200083d4:	bf02      	ittt	eq
200083d6:	ea90 0f02 	teqeq	r0, r2
200083da:	2000      	moveq	r0, #0
200083dc:	4770      	bxeq	lr
200083de:	f110 0f00 	cmn.w	r0, #0
200083e2:	ea91 0f03 	teq	r1, r3
200083e6:	bf58      	it	pl
200083e8:	4299      	cmppl	r1, r3
200083ea:	bf08      	it	eq
200083ec:	4290      	cmpeq	r0, r2
200083ee:	bf2c      	ite	cs
200083f0:	17d8      	asrcs	r0, r3, #31
200083f2:	ea6f 70e3 	mvncc.w	r0, r3, asr #31
200083f6:	f040 0001 	orr.w	r0, r0, #1
200083fa:	4770      	bx	lr
200083fc:	ea4f 0c41 	mov.w	ip, r1, lsl #1
20008400:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
20008404:	d102      	bne.n	2000840c <__cmpdf2+0x64>
20008406:	ea50 3c01 	orrs.w	ip, r0, r1, lsl #12
2000840a:	d107      	bne.n	2000841c <__cmpdf2+0x74>
2000840c:	ea4f 0c43 	mov.w	ip, r3, lsl #1
20008410:	ea7f 5c6c 	mvns.w	ip, ip, asr #21
20008414:	d1d6      	bne.n	200083c4 <__cmpdf2+0x1c>
20008416:	ea52 3c03 	orrs.w	ip, r2, r3, lsl #12
2000841a:	d0d3      	beq.n	200083c4 <__cmpdf2+0x1c>
2000841c:	f85d 0b04 	ldr.w	r0, [sp], #4
20008420:	4770      	bx	lr
20008422:	bf00      	nop

20008424 <__aeabi_cdrcmple>:
20008424:	4684      	mov	ip, r0
20008426:	4610      	mov	r0, r2
20008428:	4662      	mov	r2, ip
2000842a:	468c      	mov	ip, r1
2000842c:	4619      	mov	r1, r3
2000842e:	4663      	mov	r3, ip
20008430:	e000      	b.n	20008434 <__aeabi_cdcmpeq>
20008432:	bf00      	nop

20008434 <__aeabi_cdcmpeq>:
20008434:	b501      	push	{r0, lr}
20008436:	f7ff ffb7 	bl	200083a8 <__cmpdf2>
2000843a:	2800      	cmp	r0, #0
2000843c:	bf48      	it	mi
2000843e:	f110 0f00 	cmnmi.w	r0, #0
20008442:	bd01      	pop	{r0, pc}

20008444 <__aeabi_dcmpeq>:
20008444:	f84d ed08 	str.w	lr, [sp, #-8]!
20008448:	f7ff fff4 	bl	20008434 <__aeabi_cdcmpeq>
2000844c:	bf0c      	ite	eq
2000844e:	2001      	moveq	r0, #1
20008450:	2000      	movne	r0, #0
20008452:	f85d fb08 	ldr.w	pc, [sp], #8
20008456:	bf00      	nop

20008458 <__aeabi_dcmplt>:
20008458:	f84d ed08 	str.w	lr, [sp, #-8]!
2000845c:	f7ff ffea 	bl	20008434 <__aeabi_cdcmpeq>
20008460:	bf34      	ite	cc
20008462:	2001      	movcc	r0, #1
20008464:	2000      	movcs	r0, #0
20008466:	f85d fb08 	ldr.w	pc, [sp], #8
2000846a:	bf00      	nop

2000846c <__aeabi_dcmple>:
2000846c:	f84d ed08 	str.w	lr, [sp, #-8]!
20008470:	f7ff ffe0 	bl	20008434 <__aeabi_cdcmpeq>
20008474:	bf94      	ite	ls
20008476:	2001      	movls	r0, #1
20008478:	2000      	movhi	r0, #0
2000847a:	f85d fb08 	ldr.w	pc, [sp], #8
2000847e:	bf00      	nop

20008480 <__aeabi_dcmpge>:
20008480:	f84d ed08 	str.w	lr, [sp, #-8]!
20008484:	f7ff ffce 	bl	20008424 <__aeabi_cdrcmple>
20008488:	bf94      	ite	ls
2000848a:	2001      	movls	r0, #1
2000848c:	2000      	movhi	r0, #0
2000848e:	f85d fb08 	ldr.w	pc, [sp], #8
20008492:	bf00      	nop

20008494 <__aeabi_dcmpgt>:
20008494:	f84d ed08 	str.w	lr, [sp, #-8]!
20008498:	f7ff ffc4 	bl	20008424 <__aeabi_cdrcmple>
2000849c:	bf34      	ite	cc
2000849e:	2001      	movcc	r0, #1
200084a0:	2000      	movcs	r0, #0
200084a2:	f85d fb08 	ldr.w	pc, [sp], #8
200084a6:	bf00      	nop

200084a8 <__aeabi_d2iz>:
200084a8:	ea4f 0241 	mov.w	r2, r1, lsl #1
200084ac:	f512 1200 	adds.w	r2, r2, #2097152	; 0x200000
200084b0:	d215      	bcs.n	200084de <__aeabi_d2iz+0x36>
200084b2:	d511      	bpl.n	200084d8 <__aeabi_d2iz+0x30>
200084b4:	f46f 7378 	mvn.w	r3, #992	; 0x3e0
200084b8:	ebb3 5262 	subs.w	r2, r3, r2, asr #21
200084bc:	d912      	bls.n	200084e4 <__aeabi_d2iz+0x3c>
200084be:	ea4f 23c1 	mov.w	r3, r1, lsl #11
200084c2:	f043 4300 	orr.w	r3, r3, #2147483648	; 0x80000000
200084c6:	ea43 5350 	orr.w	r3, r3, r0, lsr #21
200084ca:	f011 4f00 	tst.w	r1, #2147483648	; 0x80000000
200084ce:	fa23 f002 	lsr.w	r0, r3, r2
200084d2:	bf18      	it	ne
200084d4:	4240      	negne	r0, r0
200084d6:	4770      	bx	lr
200084d8:	f04f 0000 	mov.w	r0, #0
200084dc:	4770      	bx	lr
200084de:	ea50 3001 	orrs.w	r0, r0, r1, lsl #12
200084e2:	d105      	bne.n	200084f0 <__aeabi_d2iz+0x48>
200084e4:	f011 4000 	ands.w	r0, r1, #2147483648	; 0x80000000
200084e8:	bf08      	it	eq
200084ea:	f06f 4000 	mvneq.w	r0, #2147483648	; 0x80000000
200084ee:	4770      	bx	lr
200084f0:	f04f 0000 	mov.w	r0, #0
200084f4:	4770      	bx	lr
200084f6:	bf00      	nop

200084f8 <__aeabi_uldivmod>:
200084f8:	b953      	cbnz	r3, 20008510 <__aeabi_uldivmod+0x18>
200084fa:	b94a      	cbnz	r2, 20008510 <__aeabi_uldivmod+0x18>
200084fc:	2900      	cmp	r1, #0
200084fe:	bf08      	it	eq
20008500:	2800      	cmpeq	r0, #0
20008502:	bf1c      	itt	ne
20008504:	f04f 31ff 	movne.w	r1, #4294967295
20008508:	f04f 30ff 	movne.w	r0, #4294967295
2000850c:	f000 b83c 	b.w	20008588 <__aeabi_idiv0>
20008510:	b082      	sub	sp, #8
20008512:	46ec      	mov	ip, sp
20008514:	e92d 5000 	stmdb	sp!, {ip, lr}
20008518:	f000 f81e 	bl	20008558 <__gnu_uldivmod_helper>
2000851c:	f8dd e004 	ldr.w	lr, [sp, #4]
20008520:	b002      	add	sp, #8
20008522:	bc0c      	pop	{r2, r3}
20008524:	4770      	bx	lr
20008526:	bf00      	nop

20008528 <__gnu_ldivmod_helper>:
20008528:	e92d 41f0 	stmdb	sp!, {r4, r5, r6, r7, r8, lr}
2000852c:	4690      	mov	r8, r2
2000852e:	4606      	mov	r6, r0
20008530:	460f      	mov	r7, r1
20008532:	461d      	mov	r5, r3
20008534:	9c06      	ldr	r4, [sp, #24]
20008536:	f000 f829 	bl	2000858c <__divdi3>
2000853a:	fb08 fc01 	mul.w	ip, r8, r1
2000853e:	4686      	mov	lr, r0
20008540:	fba8 2300 	umull	r2, r3, r8, r0
20008544:	fb00 c505 	mla	r5, r0, r5, ip
20008548:	1ab2      	subs	r2, r6, r2
2000854a:	442b      	add	r3, r5
2000854c:	eb67 0303 	sbc.w	r3, r7, r3
20008550:	e9c4 2300 	strd	r2, r3, [r4]
20008554:	e8bd 81f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, pc}

20008558 <__gnu_uldivmod_helper>:
20008558:	e92d 43f8 	stmdb	sp!, {r3, r4, r5, r6, r7, r8, r9, lr}
2000855c:	461d      	mov	r5, r3
2000855e:	4617      	mov	r7, r2
20008560:	4680      	mov	r8, r0
20008562:	4689      	mov	r9, r1
20008564:	9e08      	ldr	r6, [sp, #32]
20008566:	f000 f953 	bl	20008810 <__udivdi3>
2000856a:	fb00 f305 	mul.w	r3, r0, r5
2000856e:	fba0 4507 	umull	r4, r5, r0, r7
20008572:	fb07 3701 	mla	r7, r7, r1, r3
20008576:	ebb8 0404 	subs.w	r4, r8, r4
2000857a:	443d      	add	r5, r7
2000857c:	eb69 0505 	sbc.w	r5, r9, r5
20008580:	e9c6 4500 	strd	r4, r5, [r6]
20008584:	e8bd 83f8 	ldmia.w	sp!, {r3, r4, r5, r6, r7, r8, r9, pc}

20008588 <__aeabi_idiv0>:
20008588:	4770      	bx	lr
2000858a:	bf00      	nop

2000858c <__divdi3>:
2000858c:	2900      	cmp	r1, #0
2000858e:	e92d 47f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, sl, lr}
20008592:	f2c0 80a2 	blt.w	200086da <__divdi3+0x14e>
20008596:	2600      	movs	r6, #0
20008598:	2b00      	cmp	r3, #0
2000859a:	f2c0 8098 	blt.w	200086ce <__divdi3+0x142>
2000859e:	468c      	mov	ip, r1
200085a0:	4690      	mov	r8, r2
200085a2:	469e      	mov	lr, r3
200085a4:	4615      	mov	r5, r2
200085a6:	4604      	mov	r4, r0
200085a8:	460f      	mov	r7, r1
200085aa:	bbcb      	cbnz	r3, 20008620 <__divdi3+0x94>
200085ac:	428a      	cmp	r2, r1
200085ae:	d956      	bls.n	2000865e <__divdi3+0xd2>
200085b0:	fab2 f382 	clz	r3, r2
200085b4:	b13b      	cbz	r3, 200085c6 <__divdi3+0x3a>
200085b6:	f1c3 0220 	rsb	r2, r3, #32
200085ba:	409f      	lsls	r7, r3
200085bc:	409d      	lsls	r5, r3
200085be:	409c      	lsls	r4, r3
200085c0:	fa20 f202 	lsr.w	r2, r0, r2
200085c4:	4317      	orrs	r7, r2
200085c6:	0c28      	lsrs	r0, r5, #16
200085c8:	0c22      	lsrs	r2, r4, #16
200085ca:	fa1f fe85 	uxth.w	lr, r5
200085ce:	fbb7 f1f0 	udiv	r1, r7, r0
200085d2:	fb00 7711 	mls	r7, r0, r1, r7
200085d6:	fb0e f301 	mul.w	r3, lr, r1
200085da:	ea42 4707 	orr.w	r7, r2, r7, lsl #16
200085de:	42bb      	cmp	r3, r7
200085e0:	d907      	bls.n	200085f2 <__divdi3+0x66>
200085e2:	197f      	adds	r7, r7, r5
200085e4:	f080 8101 	bcs.w	200087ea <__divdi3+0x25e>
200085e8:	42bb      	cmp	r3, r7
200085ea:	f240 80fe 	bls.w	200087ea <__divdi3+0x25e>
200085ee:	3902      	subs	r1, #2
200085f0:	442f      	add	r7, r5
200085f2:	1aff      	subs	r7, r7, r3
200085f4:	b2a4      	uxth	r4, r4
200085f6:	fbb7 f3f0 	udiv	r3, r7, r0
200085fa:	fb00 7713 	mls	r7, r0, r3, r7
200085fe:	fb0e fe03 	mul.w	lr, lr, r3
20008602:	ea44 4707 	orr.w	r7, r4, r7, lsl #16
20008606:	45be      	cmp	lr, r7
20008608:	d906      	bls.n	20008618 <__divdi3+0x8c>
2000860a:	197f      	adds	r7, r7, r5
2000860c:	f080 80eb 	bcs.w	200087e6 <__divdi3+0x25a>
20008610:	45be      	cmp	lr, r7
20008612:	f240 80e8 	bls.w	200087e6 <__divdi3+0x25a>
20008616:	3b02      	subs	r3, #2
20008618:	ea43 4301 	orr.w	r3, r3, r1, lsl #16
2000861c:	2200      	movs	r2, #0
2000861e:	e003      	b.n	20008628 <__divdi3+0x9c>
20008620:	428b      	cmp	r3, r1
20008622:	d90f      	bls.n	20008644 <__divdi3+0xb8>
20008624:	2200      	movs	r2, #0
20008626:	4613      	mov	r3, r2
20008628:	1c34      	adds	r4, r6, #0
2000862a:	f04f 0500 	mov.w	r5, #0
2000862e:	bf18      	it	ne
20008630:	2401      	movne	r4, #1
20008632:	4260      	negs	r0, r4
20008634:	eb65 0145 	sbc.w	r1, r5, r5, lsl #1
20008638:	4058      	eors	r0, r3
2000863a:	4051      	eors	r1, r2
2000863c:	1900      	adds	r0, r0, r4
2000863e:	4169      	adcs	r1, r5
20008640:	e8bd 87f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, sl, pc}
20008644:	fab3 f283 	clz	r2, r3
20008648:	2a00      	cmp	r2, #0
2000864a:	f040 8083 	bne.w	20008754 <__divdi3+0x1c8>
2000864e:	4580      	cmp	r8, r0
20008650:	f240 80cd 	bls.w	200087ee <__divdi3+0x262>
20008654:	428b      	cmp	r3, r1
20008656:	f0c0 80ca 	bcc.w	200087ee <__divdi3+0x262>
2000865a:	4613      	mov	r3, r2
2000865c:	e7e4      	b.n	20008628 <__divdi3+0x9c>
2000865e:	b912      	cbnz	r2, 20008666 <__divdi3+0xda>
20008660:	2301      	movs	r3, #1
20008662:	fbb3 f5f2 	udiv	r5, r3, r2
20008666:	fab5 f285 	clz	r2, r5
2000866a:	2a00      	cmp	r2, #0
2000866c:	d13b      	bne.n	200086e6 <__divdi3+0x15a>
2000866e:	1b78      	subs	r0, r7, r5
20008670:	ea4f 4e15 	mov.w	lr, r5, lsr #16
20008674:	fa1f fc85 	uxth.w	ip, r5
20008678:	2201      	movs	r2, #1
2000867a:	0c21      	lsrs	r1, r4, #16
2000867c:	fbb0 f8fe 	udiv	r8, r0, lr
20008680:	fb0e 0018 	mls	r0, lr, r8, r0
20008684:	fb0c f308 	mul.w	r3, ip, r8
20008688:	ea41 4700 	orr.w	r7, r1, r0, lsl #16
2000868c:	42bb      	cmp	r3, r7
2000868e:	d908      	bls.n	200086a2 <__divdi3+0x116>
20008690:	197f      	adds	r7, r7, r5
20008692:	f080 80b0 	bcs.w	200087f6 <__divdi3+0x26a>
20008696:	42bb      	cmp	r3, r7
20008698:	f240 80ad 	bls.w	200087f6 <__divdi3+0x26a>
2000869c:	f1a8 0802 	sub.w	r8, r8, #2
200086a0:	442f      	add	r7, r5
200086a2:	1aff      	subs	r7, r7, r3
200086a4:	b2a4      	uxth	r4, r4
200086a6:	fbb7 f3fe 	udiv	r3, r7, lr
200086aa:	fb0e 7713 	mls	r7, lr, r3, r7
200086ae:	fb0c fc03 	mul.w	ip, ip, r3
200086b2:	ea44 4707 	orr.w	r7, r4, r7, lsl #16
200086b6:	45bc      	cmp	ip, r7
200086b8:	d906      	bls.n	200086c8 <__divdi3+0x13c>
200086ba:	197f      	adds	r7, r7, r5
200086bc:	f080 8099 	bcs.w	200087f2 <__divdi3+0x266>
200086c0:	45bc      	cmp	ip, r7
200086c2:	f240 8096 	bls.w	200087f2 <__divdi3+0x266>
200086c6:	3b02      	subs	r3, #2
200086c8:	ea43 4308 	orr.w	r3, r3, r8, lsl #16
200086cc:	e7ac      	b.n	20008628 <__divdi3+0x9c>
200086ce:	4252      	negs	r2, r2
200086d0:	ea6f 0606 	mvn.w	r6, r6
200086d4:	eb63 0343 	sbc.w	r3, r3, r3, lsl #1
200086d8:	e761      	b.n	2000859e <__divdi3+0x12>
200086da:	4240      	negs	r0, r0
200086dc:	f04f 36ff 	mov.w	r6, #4294967295
200086e0:	eb61 0141 	sbc.w	r1, r1, r1, lsl #1
200086e4:	e758      	b.n	20008598 <__divdi3+0xc>
200086e6:	4095      	lsls	r5, r2
200086e8:	f1c2 0120 	rsb	r1, r2, #32
200086ec:	fa07 f302 	lsl.w	r3, r7, r2
200086f0:	ea4f 4e15 	mov.w	lr, r5, lsr #16
200086f4:	40cf      	lsrs	r7, r1
200086f6:	fa24 f101 	lsr.w	r1, r4, r1
200086fa:	4094      	lsls	r4, r2
200086fc:	fa1f fc85 	uxth.w	ip, r5
20008700:	4319      	orrs	r1, r3
20008702:	fbb7 f2fe 	udiv	r2, r7, lr
20008706:	ea4f 4811 	mov.w	r8, r1, lsr #16
2000870a:	fb0e 7712 	mls	r7, lr, r2, r7
2000870e:	fb0c f302 	mul.w	r3, ip, r2
20008712:	ea48 4707 	orr.w	r7, r8, r7, lsl #16
20008716:	42bb      	cmp	r3, r7
20008718:	d905      	bls.n	20008726 <__divdi3+0x19a>
2000871a:	197f      	adds	r7, r7, r5
2000871c:	d271      	bcs.n	20008802 <__divdi3+0x276>
2000871e:	42bb      	cmp	r3, r7
20008720:	d96f      	bls.n	20008802 <__divdi3+0x276>
20008722:	3a02      	subs	r2, #2
20008724:	442f      	add	r7, r5
20008726:	1aff      	subs	r7, r7, r3
20008728:	b289      	uxth	r1, r1
2000872a:	fbb7 f8fe 	udiv	r8, r7, lr
2000872e:	fb0e 7718 	mls	r7, lr, r8, r7
20008732:	fb0c f008 	mul.w	r0, ip, r8
20008736:	ea41 4307 	orr.w	r3, r1, r7, lsl #16
2000873a:	4298      	cmp	r0, r3
2000873c:	d906      	bls.n	2000874c <__divdi3+0x1c0>
2000873e:	195b      	adds	r3, r3, r5
20008740:	d261      	bcs.n	20008806 <__divdi3+0x27a>
20008742:	4298      	cmp	r0, r3
20008744:	d95f      	bls.n	20008806 <__divdi3+0x27a>
20008746:	f1a8 0802 	sub.w	r8, r8, #2
2000874a:	442b      	add	r3, r5
2000874c:	1a18      	subs	r0, r3, r0
2000874e:	ea48 4202 	orr.w	r2, r8, r2, lsl #16
20008752:	e792      	b.n	2000867a <__divdi3+0xee>
20008754:	f1c2 0720 	rsb	r7, r2, #32
20008758:	fa03 fe02 	lsl.w	lr, r3, r2
2000875c:	fa08 f502 	lsl.w	r5, r8, r2
20008760:	4091      	lsls	r1, r2
20008762:	fa28 f307 	lsr.w	r3, r8, r7
20008766:	fa2c fc07 	lsr.w	ip, ip, r7
2000876a:	40f8      	lsrs	r0, r7
2000876c:	ea43 0e0e 	orr.w	lr, r3, lr
20008770:	4308      	orrs	r0, r1
20008772:	ea4f 481e 	mov.w	r8, lr, lsr #16
20008776:	0c07      	lsrs	r7, r0, #16
20008778:	fa1f fa8e 	uxth.w	sl, lr
2000877c:	fbbc f9f8 	udiv	r9, ip, r8
20008780:	fb08 cc19 	mls	ip, r8, r9, ip
20008784:	fb0a f109 	mul.w	r1, sl, r9
20008788:	ea47 4c0c 	orr.w	ip, r7, ip, lsl #16
2000878c:	4561      	cmp	r1, ip
2000878e:	d907      	bls.n	200087a0 <__divdi3+0x214>
20008790:	eb1c 0c0e 	adds.w	ip, ip, lr
20008794:	d232      	bcs.n	200087fc <__divdi3+0x270>
20008796:	4561      	cmp	r1, ip
20008798:	d930      	bls.n	200087fc <__divdi3+0x270>
2000879a:	f1a9 0902 	sub.w	r9, r9, #2
2000879e:	44f4      	add	ip, lr
200087a0:	ebc1 0c0c 	rsb	ip, r1, ip
200087a4:	b280      	uxth	r0, r0
200087a6:	fbbc f3f8 	udiv	r3, ip, r8
200087aa:	fb08 cc13 	mls	ip, r8, r3, ip
200087ae:	fb0a fa03 	mul.w	sl, sl, r3
200087b2:	ea40 410c 	orr.w	r1, r0, ip, lsl #16
200087b6:	458a      	cmp	sl, r1
200087b8:	d906      	bls.n	200087c8 <__divdi3+0x23c>
200087ba:	eb11 010e 	adds.w	r1, r1, lr
200087be:	d225      	bcs.n	2000880c <__divdi3+0x280>
200087c0:	458a      	cmp	sl, r1
200087c2:	d923      	bls.n	2000880c <__divdi3+0x280>
200087c4:	3b02      	subs	r3, #2
200087c6:	4471      	add	r1, lr
200087c8:	ea43 4309 	orr.w	r3, r3, r9, lsl #16
200087cc:	ebca 0101 	rsb	r1, sl, r1
200087d0:	fba3 8905 	umull	r8, r9, r3, r5
200087d4:	4549      	cmp	r1, r9
200087d6:	d303      	bcc.n	200087e0 <__divdi3+0x254>
200087d8:	d103      	bne.n	200087e2 <__divdi3+0x256>
200087da:	4094      	lsls	r4, r2
200087dc:	4544      	cmp	r4, r8
200087de:	d200      	bcs.n	200087e2 <__divdi3+0x256>
200087e0:	3b01      	subs	r3, #1
200087e2:	2200      	movs	r2, #0
200087e4:	e720      	b.n	20008628 <__divdi3+0x9c>
200087e6:	3b01      	subs	r3, #1
200087e8:	e716      	b.n	20008618 <__divdi3+0x8c>
200087ea:	3901      	subs	r1, #1
200087ec:	e701      	b.n	200085f2 <__divdi3+0x66>
200087ee:	2301      	movs	r3, #1
200087f0:	e71a      	b.n	20008628 <__divdi3+0x9c>
200087f2:	3b01      	subs	r3, #1
200087f4:	e768      	b.n	200086c8 <__divdi3+0x13c>
200087f6:	f108 38ff 	add.w	r8, r8, #4294967295
200087fa:	e752      	b.n	200086a2 <__divdi3+0x116>
200087fc:	f109 39ff 	add.w	r9, r9, #4294967295
20008800:	e7ce      	b.n	200087a0 <__divdi3+0x214>
20008802:	3a01      	subs	r2, #1
20008804:	e78f      	b.n	20008726 <__divdi3+0x19a>
20008806:	f108 38ff 	add.w	r8, r8, #4294967295
2000880a:	e79f      	b.n	2000874c <__divdi3+0x1c0>
2000880c:	3b01      	subs	r3, #1
2000880e:	e7db      	b.n	200087c8 <__divdi3+0x23c>

20008810 <__udivdi3>:
20008810:	e92d 43f0 	stmdb	sp!, {r4, r5, r6, r7, r8, r9, lr}
20008814:	2b00      	cmp	r3, #0
20008816:	d140      	bne.n	2000889a <__udivdi3+0x8a>
20008818:	428a      	cmp	r2, r1
2000881a:	4604      	mov	r4, r0
2000881c:	4615      	mov	r5, r2
2000881e:	d94a      	bls.n	200088b6 <__udivdi3+0xa6>
20008820:	fab2 f382 	clz	r3, r2
20008824:	460f      	mov	r7, r1
20008826:	b14b      	cbz	r3, 2000883c <__udivdi3+0x2c>
20008828:	f1c3 0620 	rsb	r6, r3, #32
2000882c:	4099      	lsls	r1, r3
2000882e:	fa00 f403 	lsl.w	r4, r0, r3
20008832:	fa02 f503 	lsl.w	r5, r2, r3
20008836:	40f0      	lsrs	r0, r6
20008838:	ea40 0701 	orr.w	r7, r0, r1
2000883c:	0c29      	lsrs	r1, r5, #16
2000883e:	0c26      	lsrs	r6, r4, #16
20008840:	fa1f fe85 	uxth.w	lr, r5
20008844:	fbb7 f0f1 	udiv	r0, r7, r1
20008848:	fb01 7710 	mls	r7, r1, r0, r7
2000884c:	fb0e f200 	mul.w	r2, lr, r0
20008850:	ea46 4307 	orr.w	r3, r6, r7, lsl #16
20008854:	429a      	cmp	r2, r3
20008856:	d907      	bls.n	20008868 <__udivdi3+0x58>
20008858:	195b      	adds	r3, r3, r5
2000885a:	f080 80ea 	bcs.w	20008a32 <__udivdi3+0x222>
2000885e:	429a      	cmp	r2, r3
20008860:	f240 80e7 	bls.w	20008a32 <__udivdi3+0x222>
20008864:	3802      	subs	r0, #2
20008866:	442b      	add	r3, r5
20008868:	1a9a      	subs	r2, r3, r2
2000886a:	b2a4      	uxth	r4, r4
2000886c:	fbb2 f3f1 	udiv	r3, r2, r1
20008870:	fb01 2213 	mls	r2, r1, r3, r2
20008874:	fb0e fe03 	mul.w	lr, lr, r3
20008878:	ea44 4202 	orr.w	r2, r4, r2, lsl #16
2000887c:	4596      	cmp	lr, r2
2000887e:	d906      	bls.n	2000888e <__udivdi3+0x7e>
20008880:	1952      	adds	r2, r2, r5
20008882:	f080 80da 	bcs.w	20008a3a <__udivdi3+0x22a>
20008886:	4596      	cmp	lr, r2
20008888:	f240 80d7 	bls.w	20008a3a <__udivdi3+0x22a>
2000888c:	3b02      	subs	r3, #2
2000888e:	ea43 4000 	orr.w	r0, r3, r0, lsl #16
20008892:	2600      	movs	r6, #0
20008894:	4631      	mov	r1, r6
20008896:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
2000889a:	428b      	cmp	r3, r1
2000889c:	d844      	bhi.n	20008928 <__udivdi3+0x118>
2000889e:	fab3 f683 	clz	r6, r3
200088a2:	2e00      	cmp	r6, #0
200088a4:	d145      	bne.n	20008932 <__udivdi3+0x122>
200088a6:	4282      	cmp	r2, r0
200088a8:	f240 80bf 	bls.w	20008a2a <__udivdi3+0x21a>
200088ac:	428b      	cmp	r3, r1
200088ae:	f0c0 80bc 	bcc.w	20008a2a <__udivdi3+0x21a>
200088b2:	4630      	mov	r0, r6
200088b4:	e7ee      	b.n	20008894 <__udivdi3+0x84>
200088b6:	b912      	cbnz	r2, 200088be <__udivdi3+0xae>
200088b8:	2501      	movs	r5, #1
200088ba:	fbb5 f5f2 	udiv	r5, r5, r2
200088be:	fab5 f285 	clz	r2, r5
200088c2:	2a00      	cmp	r2, #0
200088c4:	d17b      	bne.n	200089be <__udivdi3+0x1ae>
200088c6:	1b4a      	subs	r2, r1, r5
200088c8:	0c2f      	lsrs	r7, r5, #16
200088ca:	fa1f fe85 	uxth.w	lr, r5
200088ce:	2601      	movs	r6, #1
200088d0:	0c23      	lsrs	r3, r4, #16
200088d2:	fbb2 f0f7 	udiv	r0, r2, r7
200088d6:	fb07 2210 	mls	r2, r7, r0, r2
200088da:	fb0e fc00 	mul.w	ip, lr, r0
200088de:	ea43 4102 	orr.w	r1, r3, r2, lsl #16
200088e2:	458c      	cmp	ip, r1
200088e4:	d907      	bls.n	200088f6 <__udivdi3+0xe6>
200088e6:	1949      	adds	r1, r1, r5
200088e8:	f080 80a1 	bcs.w	20008a2e <__udivdi3+0x21e>
200088ec:	458c      	cmp	ip, r1
200088ee:	f240 809e 	bls.w	20008a2e <__udivdi3+0x21e>
200088f2:	3802      	subs	r0, #2
200088f4:	4429      	add	r1, r5
200088f6:	ebcc 0101 	rsb	r1, ip, r1
200088fa:	b2a4      	uxth	r4, r4
200088fc:	fbb1 f3f7 	udiv	r3, r1, r7
20008900:	fb07 1113 	mls	r1, r7, r3, r1
20008904:	fb0e fe03 	mul.w	lr, lr, r3
20008908:	ea44 4401 	orr.w	r4, r4, r1, lsl #16
2000890c:	45a6      	cmp	lr, r4
2000890e:	d906      	bls.n	2000891e <__udivdi3+0x10e>
20008910:	1964      	adds	r4, r4, r5
20008912:	f080 8090 	bcs.w	20008a36 <__udivdi3+0x226>
20008916:	45a6      	cmp	lr, r4
20008918:	f240 808d 	bls.w	20008a36 <__udivdi3+0x226>
2000891c:	3b02      	subs	r3, #2
2000891e:	ea43 4000 	orr.w	r0, r3, r0, lsl #16
20008922:	4631      	mov	r1, r6
20008924:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
20008928:	2600      	movs	r6, #0
2000892a:	4630      	mov	r0, r6
2000892c:	4631      	mov	r1, r6
2000892e:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
20008932:	f1c6 0520 	rsb	r5, r6, #32
20008936:	40b3      	lsls	r3, r6
20008938:	fa02 f706 	lsl.w	r7, r2, r6
2000893c:	fa01 f406 	lsl.w	r4, r1, r6
20008940:	40ea      	lsrs	r2, r5
20008942:	40e9      	lsrs	r1, r5
20008944:	fa20 f505 	lsr.w	r5, r0, r5
20008948:	431a      	orrs	r2, r3
2000894a:	4325      	orrs	r5, r4
2000894c:	ea4f 4c12 	mov.w	ip, r2, lsr #16
20008950:	ea4f 4e15 	mov.w	lr, r5, lsr #16
20008954:	b293      	uxth	r3, r2
20008956:	fbb1 f8fc 	udiv	r8, r1, ip
2000895a:	fb0c 1118 	mls	r1, ip, r8, r1
2000895e:	fb03 f408 	mul.w	r4, r3, r8
20008962:	ea4e 4101 	orr.w	r1, lr, r1, lsl #16
20008966:	428c      	cmp	r4, r1
20008968:	d906      	bls.n	20008978 <__udivdi3+0x168>
2000896a:	1889      	adds	r1, r1, r2
2000896c:	d269      	bcs.n	20008a42 <__udivdi3+0x232>
2000896e:	428c      	cmp	r4, r1
20008970:	d967      	bls.n	20008a42 <__udivdi3+0x232>
20008972:	f1a8 0802 	sub.w	r8, r8, #2
20008976:	4411      	add	r1, r2
20008978:	1b09      	subs	r1, r1, r4
2000897a:	b2ad      	uxth	r5, r5
2000897c:	fbb1 f4fc 	udiv	r4, r1, ip
20008980:	fb0c 1114 	mls	r1, ip, r4, r1
20008984:	fb03 fe04 	mul.w	lr, r3, r4
20008988:	ea45 4301 	orr.w	r3, r5, r1, lsl #16
2000898c:	459e      	cmp	lr, r3
2000898e:	d905      	bls.n	2000899c <__udivdi3+0x18c>
20008990:	189b      	adds	r3, r3, r2
20008992:	d254      	bcs.n	20008a3e <__udivdi3+0x22e>
20008994:	459e      	cmp	lr, r3
20008996:	d952      	bls.n	20008a3e <__udivdi3+0x22e>
20008998:	3c02      	subs	r4, #2
2000899a:	4413      	add	r3, r2
2000899c:	ea44 4408 	orr.w	r4, r4, r8, lsl #16
200089a0:	ebce 0303 	rsb	r3, lr, r3
200089a4:	fba4 8907 	umull	r8, r9, r4, r7
200089a8:	454b      	cmp	r3, r9
200089aa:	d303      	bcc.n	200089b4 <__udivdi3+0x1a4>
200089ac:	d151      	bne.n	20008a52 <__udivdi3+0x242>
200089ae:	40b0      	lsls	r0, r6
200089b0:	4540      	cmp	r0, r8
200089b2:	d24e      	bcs.n	20008a52 <__udivdi3+0x242>
200089b4:	2600      	movs	r6, #0
200089b6:	1e60      	subs	r0, r4, #1
200089b8:	4631      	mov	r1, r6
200089ba:	e8bd 83f0 	ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, pc}
200089be:	f1c2 0620 	rsb	r6, r2, #32
200089c2:	4095      	lsls	r5, r2
200089c4:	fa01 f302 	lsl.w	r3, r1, r2
200089c8:	fa00 f402 	lsl.w	r4, r0, r2
200089cc:	0c2f      	lsrs	r7, r5, #16
200089ce:	40f1      	lsrs	r1, r6
200089d0:	40f0      	lsrs	r0, r6
200089d2:	fa1f fe85 	uxth.w	lr, r5
200089d6:	fbb1 f6f7 	udiv	r6, r1, r7
200089da:	4318      	orrs	r0, r3
200089dc:	fb07 1116 	mls	r1, r7, r6, r1
200089e0:	fb0e f206 	mul.w	r2, lr, r6
200089e4:	ea4f 4c10 	mov.w	ip, r0, lsr #16
200089e8:	ea4c 4301 	orr.w	r3, ip, r1, lsl #16
200089ec:	429a      	cmp	r2, r3
200089ee:	d905      	bls.n	200089fc <__udivdi3+0x1ec>
200089f0:	195b      	adds	r3, r3, r5
200089f2:	d229      	bcs.n	20008a48 <__udivdi3+0x238>
200089f4:	429a      	cmp	r2, r3
200089f6:	d927      	bls.n	20008a48 <__udivdi3+0x238>
200089f8:	3e02      	subs	r6, #2
200089fa:	442b      	add	r3, r5
200089fc:	1a9b      	subs	r3, r3, r2
200089fe:	b280      	uxth	r0, r0
20008a00:	fbb3 fcf7 	udiv	ip, r3, r7
20008a04:	fb07 331c 	mls	r3, r7, ip, r3
20008a08:	fb0e f10c 	mul.w	r1, lr, ip
20008a0c:	ea40 4303 	orr.w	r3, r0, r3, lsl #16
20008a10:	4299      	cmp	r1, r3
20008a12:	d906      	bls.n	20008a22 <__udivdi3+0x212>
20008a14:	195b      	adds	r3, r3, r5
20008a16:	d219      	bcs.n	20008a4c <__udivdi3+0x23c>
20008a18:	4299      	cmp	r1, r3
20008a1a:	d917      	bls.n	20008a4c <__udivdi3+0x23c>
20008a1c:	f1ac 0c02 	sub.w	ip, ip, #2
20008a20:	442b      	add	r3, r5
20008a22:	1a5a      	subs	r2, r3, r1
20008a24:	ea4c 4606 	orr.w	r6, ip, r6, lsl #16
20008a28:	e752      	b.n	200088d0 <__udivdi3+0xc0>
20008a2a:	2001      	movs	r0, #1
20008a2c:	e732      	b.n	20008894 <__udivdi3+0x84>
20008a2e:	3801      	subs	r0, #1
20008a30:	e761      	b.n	200088f6 <__udivdi3+0xe6>
20008a32:	3801      	subs	r0, #1
20008a34:	e718      	b.n	20008868 <__udivdi3+0x58>
20008a36:	3b01      	subs	r3, #1
20008a38:	e771      	b.n	2000891e <__udivdi3+0x10e>
20008a3a:	3b01      	subs	r3, #1
20008a3c:	e727      	b.n	2000888e <__udivdi3+0x7e>
20008a3e:	3c01      	subs	r4, #1
20008a40:	e7ac      	b.n	2000899c <__udivdi3+0x18c>
20008a42:	f108 38ff 	add.w	r8, r8, #4294967295
20008a46:	e797      	b.n	20008978 <__udivdi3+0x168>
20008a48:	3e01      	subs	r6, #1
20008a4a:	e7d7      	b.n	200089fc <__udivdi3+0x1ec>
20008a4c:	f10c 3cff 	add.w	ip, ip, #4294967295
20008a50:	e7e7      	b.n	20008a22 <__udivdi3+0x212>
20008a52:	4620      	mov	r0, r4
20008a54:	2600      	movs	r6, #0
20008a56:	e71d      	b.n	20008894 <__udivdi3+0x84>

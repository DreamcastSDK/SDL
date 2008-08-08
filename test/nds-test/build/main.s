	.cpu arm9tdmi
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 18, 4
	.code	16
	.file	"main.c"
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.text
.Ltext0:
	.bss
	.align	2
glGlob:
	.space	4
	.text
	.align	2
	.global	delay
	.code	16
	.thumb_func
	.type	delay, %function
delay:
.LFB108:
	.file 1 "/home/lifning/hack/nds-test/source/main.c"
	.loc 1 5 0
	push	{r7, lr}
.LCFI0:
	sub	sp, sp, #16
.LCFI1:
	add	r7, sp, #0
.LCFI2:
	str	r0, [r7, #4]
	b	.L2
.L5:
.LBB2:
	.loc 1 7 0
	mov	r3, #60
	str	r3, [r7, #12]
	b	.L3
.L4:
	.loc 1 8 0
	bl	swiWaitForVBlank
.L3:
	ldr	r3, [r7, #12]
	sub	r3, r3, #1
	str	r3, [r7, #12]
	ldr	r3, [r7, #12]
	cmp	r3, #0
	bne	.L4
.L2:
.LBE2:
	.loc 1 6 0
	ldr	r2, [r7, #4]
	asr	r3, r2, #31
	sub	r3, r3, r2
	lsr	r3, r3, #31
	lsl	r3, r3, #24
	lsr	r2, r3, #24
	ldr	r3, [r7, #4]
	sub	r3, r3, #1
	str	r3, [r7, #4]
	cmp	r2, #0
	bne	.L5
	.loc 1 10 0
	mov	sp, r7
	add	sp, sp, #16
	@ sp needed for prologue
	pop	{r7}
	pop	{r0}
	bx	r0
.LFE108:
	.size	delay, .-delay
	.section	.rodata
	.align	2
	.type	C.62.7416, %object
	.size	C.62.7416, 16
C.62.7416:
	.word	8
	.word	8
	.word	240
	.word	176
	.align	2
.LC1:
	.ascii	"# error initializing SDL\000"
	.align	2
.LC3:
	.ascii	"* initialized SDL\012\000"
	.align	2
.LC5:
	.ascii	"# error setting video mode\000"
	.align	2
.LC7:
	.ascii	"* set video mode\012\000"
	.align	2
.LC9:
	.ascii	"# error opening joystick\000"
	.align	2
.LC11:
	.ascii	"* opened joystick\000"
	.text
	.align	2
	.global	main
	.code	16
	.thumb_func
	.type	main, %function
main:
.LFB109:
	.loc 1 12 0
	push	{r4, r7, lr}
.LCFI3:
	sub	sp, sp, #76
.LCFI4:
	add	r7, sp, #0
.LCFI5:
	.loc 1 16 0
	mov	r3, r7
	add	r3, r3, #8
	ldr	r1, .L23
	mov	r2, r3
	mov	r3, r1
	ldmia	r3!, {r0, r1, r4}
	stmia	r2!, {r0, r1, r4}
	ldr	r3, [r3]
	str	r3, [r2]
	.loc 1 18 0
	bl	consoleDemoInit
	.loc 1 19 0
	mov	r3, #136
	lsl	r3, r3, #2
	mov	r0, r3
	bl	SDL_Init
	mov	r3, r0
	cmp	r3, #0
	bge	.L8
	.loc 1 20 0
	ldr	r3, .L23+4
	mov	r0, r3
	bl	puts
	.loc 1 21 0
	bl	SDL_GetError
	mov	r3, r0
	mov	r0, r3
	bl	puts
	.loc 1 22 0
	mov	r0, #1
	str	r0, [r7]
	b	.L9
.L8:
	.loc 1 24 0
	ldr	r3, .L23+8
	mov	r0, r3
	bl	puts
	mov	r0, #1
	bl	delay
	.loc 1 26 0
	mov	r3, #128
	lsl	r3, r3, #1
	mov	r0, r3
	mov	r1, #192
	mov	r2, #16
	mov	r3, #0
	bl	SDL_SetVideoMode
	mov	r3, r0
	str	r3, [r7, #64]
	.loc 1 27 0
	ldr	r3, [r7, #64]
	cmp	r3, #0
	bne	.L10
	.loc 1 28 0
	ldr	r3, .L23+12
	mov	r0, r3
	bl	puts
	.loc 1 29 0
	bl	SDL_GetError
	mov	r3, r0
	mov	r0, r3
	bl	puts
	.loc 1 30 0
	mov	r1, #2
	str	r1, [r7]
	b	.L9
.L10:
	.loc 1 32 0
	ldr	r3, .L23+16
	mov	r0, r3
	bl	puts
	mov	r0, #1
	bl	delay
	.loc 1 34 0
	mov	r0, #0
	bl	SDL_JoystickOpen
	mov	r3, r0
	str	r3, [r7, #68]
	.loc 1 35 0
	ldr	r3, [r7, #68]
	cmp	r3, #0
	bne	.L11
	.loc 1 36 0
	ldr	r3, .L23+20
	mov	r0, r3
	bl	puts
	.loc 1 37 0
	bl	SDL_GetError
	mov	r3, r0
	mov	r0, r3
	bl	puts
	.loc 1 38 0
	mov	r3, #3
	str	r3, [r7]
	b	.L9
.L11:
	.loc 1 40 0
	ldr	r3, .L23+24
	mov	r0, r3
	bl	puts
	mov	r0, #1
	bl	delay
	b	.L12
.L20:
	.loc 1 44 0
	mov	r3, r7
	add	r3, r3, #24
	ldrb	r3, [r3]
	cmp	r3, #12
	bne	.L12
.L13:
	.loc 1 46 0
	mov	r3, r7
	add	r3, r3, #24
	ldrb	r3, [r3, #1]
	str	r3, [r7, #4]
	ldr	r4, [r7, #4]
	cmp	r4, #1
	beq	.L16
	ldr	r0, [r7, #4]
	cmp	r0, #1
	bgt	.L19
	ldr	r1, [r7, #4]
	cmp	r1, #0
	beq	.L15
	b	.L14
.L19:
	ldr	r3, [r7, #4]
	cmp	r3, #2
	beq	.L17
	ldr	r4, [r7, #4]
	cmp	r4, #3
	beq	.L18
	b	.L14
.L15:
	.loc 1 48 0
	ldr	r3, [r7, #64]
	mov	r2, r7
	add	r2, r2, #8
	ldr	r4, .L23+28
	mov	r0, r3
	mov	r1, r2
	mov	r2, r4
	bl	SDL_FillRect
	b	.L14
.L16:
	.loc 1 51 0
	ldr	r3, [r7, #64]
	mov	r2, r7
	add	r2, r2, #8
	ldr	r4, .L23+32
	mov	r0, r3
	mov	r1, r2
	mov	r2, r4
	bl	SDL_FillRect
	b	.L14
.L17:
	.loc 1 54 0
	ldr	r3, [r7, #64]
	mov	r1, r7
	add	r1, r1, #8
	mov	r2, #252
	lsl	r2, r2, #8
	mov	r0, r3
	bl	SDL_FillRect
	b	.L14
.L18:
	.loc 1 57 0
	ldr	r3, [r7, #64]
	mov	r1, r7
	add	r1, r1, #8
	mov	r2, #128
	lsl	r2, r2, #8
	mov	r0, r3
	bl	SDL_FillRect
.L14:
	.loc 1 61 0
	ldr	r3, [r7, #64]
	mov	r0, r3
	bl	SDL_Flip
.L12:
	.loc 1 43 0
	mov	r3, r7
	add	r3, r3, #24
	mov	r0, r3
	bl	SDL_PollEvent
	mov	r3, r0
	cmp	r3, #0
	bne	.L20
	b	.L12
.L9:
	.loc 1 64 0
	ldr	r3, [r7]
	.loc 1 67 0
	mov	r0, r3
	mov	sp, r7
	add	sp, sp, #76
	@ sp needed for prologue
	pop	{r4, r7}
	pop	{r1}
	bx	r1
.L24:
	.align	2
.L23:
	.word	C.62.7416
	.word	.LC1
	.word	.LC3
	.word	.LC5
	.word	.LC7
	.word	.LC9
	.word	.LC11
	.word	32799
	.word	33760
.LFE109:
	.size	main, .-main
	.section	.debug_frame,"",%progbits
.Lframe0:
	.4byte	.LECIE0-.LSCIE0
.LSCIE0:
	.4byte	0xffffffff
	.byte	0x1
	.ascii	"\000"
	.uleb128 0x1
	.sleb128 -4
	.byte	0xe
	.byte	0xc
	.uleb128 0xd
	.uleb128 0x0
	.align	2
.LECIE0:
.LSFDE0:
	.4byte	.LEFDE0-.LASFDE0
.LASFDE0:
	.4byte	.Lframe0
	.4byte	.LFB108
	.4byte	.LFE108-.LFB108
	.byte	0x4
	.4byte	.LCFI0-.LFB108
	.byte	0xe
	.uleb128 0x8
	.byte	0x87
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI1-.LCFI0
	.byte	0xe
	.uleb128 0x18
	.byte	0x4
	.4byte	.LCFI2-.LCFI1
	.byte	0xd
	.uleb128 0x7
	.align	2
.LEFDE0:
.LSFDE2:
	.4byte	.LEFDE2-.LASFDE2
.LASFDE2:
	.4byte	.Lframe0
	.4byte	.LFB109
	.4byte	.LFE109-.LFB109
	.byte	0x4
	.4byte	.LCFI3-.LFB109
	.byte	0xe
	.uleb128 0xc
	.byte	0x84
	.uleb128 0x3
	.byte	0x87
	.uleb128 0x2
	.byte	0x8e
	.uleb128 0x1
	.byte	0x4
	.4byte	.LCFI4-.LCFI3
	.byte	0xe
	.uleb128 0x58
	.byte	0x4
	.4byte	.LCFI5-.LCFI4
	.byte	0xd
	.uleb128 0x7
	.align	2
.LEFDE2:
	.text
.Letext0:
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB108-.Ltext0
	.4byte	.LCFI0-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI0-.Ltext0
	.4byte	.LCFI1-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	.LCFI1-.Ltext0
	.4byte	.LCFI2-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 24
	.4byte	.LCFI2-.Ltext0
	.4byte	.LFE108-.Ltext0
	.2byte	0x2
	.byte	0x77
	.sleb128 24
	.4byte	0x0
	.4byte	0x0
.LLST1:
	.4byte	.LFB109-.Ltext0
	.4byte	.LCFI3-.Ltext0
	.2byte	0x1
	.byte	0x5d
	.4byte	.LCFI3-.Ltext0
	.4byte	.LCFI4-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 12
	.4byte	.LCFI4-.Ltext0
	.4byte	.LCFI5-.Ltext0
	.2byte	0x3
	.byte	0x7d
	.sleb128 88
	.4byte	.LCFI5-.Ltext0
	.4byte	.LFE109-.Ltext0
	.2byte	0x3
	.byte	0x77
	.sleb128 88
	.4byte	0x0
	.4byte	0x0
	.file 2 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_config_nintendods.h"
	.file 3 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_stdinc.h"
	.file 4 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_pixels.h"
	.file 5 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_rect.h"
	.file 6 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_surface.h"
	.file 7 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_video.h"
	.file 8 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_scancode.h"
	.file 9 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_keysym.h"
	.file 10 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_keyboard.h"
	.file 11 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_joystick.h"
	.file 12 "/home/lifning/hack/devkitPro/libnds/include/SDL/SDL_events.h"
	.file 13 "/home/lifning/hack/devkitPro/libnds/include/nds/jtypes.h"
	.file 14 "/home/lifning/hack/devkitPro/libnds/include/nds/arm9/videoGL.h"
	.file 15 "/home/lifning/hack/devkitPro/libnds/include/nds/arm9/trig_lut.h"
	.section	.debug_info
	.4byte	0x1325
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF395
	.byte	0x1
	.4byte	.LASF396
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x3
	.4byte	.LASF3
	.byte	0x2
	.byte	0x1f
	.4byte	0x33
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF1
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x3
	.4byte	.LASF4
	.byte	0x2
	.byte	0x21
	.4byte	0x4c
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF5
	.uleb128 0x3
	.4byte	.LASF6
	.byte	0x2
	.byte	0x22
	.4byte	0x5e
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x3
	.4byte	.LASF7
	.byte	0x2
	.byte	0x23
	.4byte	0x70
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF9
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF10
	.uleb128 0x2
	.byte	0x4
	.byte	0x5
	.4byte	.LASF11
	.uleb128 0x5
	.byte	0x4
	.byte	0x7
	.uleb128 0x6
	.byte	0x4
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF12
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF13
	.uleb128 0x3
	.4byte	.LASF14
	.byte	0x3
	.byte	0x63
	.4byte	0x28
	.uleb128 0x3
	.4byte	.LASF15
	.byte	0x3
	.byte	0x6d
	.4byte	0x41
	.uleb128 0x3
	.4byte	.LASF16
	.byte	0x3
	.byte	0x72
	.4byte	0x53
	.uleb128 0x3
	.4byte	.LASF17
	.byte	0x3
	.byte	0x77
	.4byte	0x65
	.uleb128 0x2
	.byte	0x8
	.byte	0x4
	.4byte	.LASF18
	.uleb128 0x7
	.4byte	.LASF20
	.byte	0x4
	.byte	0x4
	.byte	0xcd
	.4byte	0x111
	.uleb128 0x8
	.ascii	"r\000"
	.byte	0x4
	.byte	0xce
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.ascii	"g\000"
	.byte	0x4
	.byte	0xcf
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x8
	.ascii	"b\000"
	.byte	0x4
	.byte	0xd0
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x9
	.4byte	.LASF19
	.byte	0x4
	.byte	0xd1
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x3
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF20
	.byte	0x4
	.byte	0xd2
	.4byte	0xd2
	.uleb128 0x3
	.4byte	.LASF21
	.byte	0x4
	.byte	0xd5
	.4byte	0x127
	.uleb128 0x7
	.4byte	.LASF21
	.byte	0x10
	.byte	0x4
	.byte	0xd5
	.4byte	0x16c
	.uleb128 0x9
	.4byte	.LASF22
	.byte	0x4
	.byte	0xe1
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF23
	.byte	0x4
	.byte	0xe2
	.4byte	0x1e0
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x9
	.4byte	.LASF24
	.byte	0x4
	.byte	0xe4
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.4byte	.LASF25
	.byte	0x4
	.byte	0xe5
	.4byte	0x1e6
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF26
	.byte	0x4
	.byte	0xd6
	.4byte	0x177
	.uleb128 0xa
	.byte	0x4
	.4byte	0x17d
	.uleb128 0xb
	.byte	0x1
	.4byte	0x5e
	.4byte	0x192
	.uleb128 0xc
	.4byte	0x8f
	.uleb128 0xc
	.4byte	0x192
	.byte	0x0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x11c
	.uleb128 0x7
	.4byte	.LASF27
	.byte	0xc
	.byte	0x4
	.byte	0xd9
	.4byte	0x1cf
	.uleb128 0x9
	.4byte	.LASF28
	.byte	0x4
	.byte	0xda
	.4byte	0x16c
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF29
	.byte	0x4
	.byte	0xdb
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x9
	.4byte	.LASF30
	.byte	0x4
	.byte	0xdc
	.4byte	0x1cf
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x198
	.uleb128 0x3
	.4byte	.LASF27
	.byte	0x4
	.byte	0xdd
	.4byte	0x198
	.uleb128 0xa
	.byte	0x4
	.4byte	0x111
	.uleb128 0xa
	.byte	0x4
	.4byte	0x1d5
	.uleb128 0x7
	.4byte	.LASF31
	.byte	0x20
	.byte	0x4
	.byte	0xea
	.4byte	0x2cb
	.uleb128 0x9
	.4byte	.LASF32
	.byte	0x4
	.byte	0xeb
	.4byte	0x192
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF33
	.byte	0x4
	.byte	0xec
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x9
	.4byte	.LASF34
	.byte	0x4
	.byte	0xed
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x5
	.uleb128 0x9
	.4byte	.LASF35
	.byte	0x4
	.byte	0xee
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x6
	.uleb128 0x9
	.4byte	.LASF36
	.byte	0x4
	.byte	0xef
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x7
	.uleb128 0x9
	.4byte	.LASF37
	.byte	0x4
	.byte	0xf0
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.4byte	.LASF38
	.byte	0x4
	.byte	0xf1
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x9
	.uleb128 0x9
	.4byte	.LASF39
	.byte	0x4
	.byte	0xf2
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0xa
	.uleb128 0x9
	.4byte	.LASF40
	.byte	0x4
	.byte	0xf3
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0xb
	.uleb128 0x9
	.4byte	.LASF41
	.byte	0x4
	.byte	0xf4
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x9
	.4byte	.LASF42
	.byte	0x4
	.byte	0xf5
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0xd
	.uleb128 0x9
	.4byte	.LASF43
	.byte	0x4
	.byte	0xf6
	.4byte	0xc0
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x9
	.4byte	.LASF44
	.byte	0x4
	.byte	0xf7
	.4byte	0xc0
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x9
	.4byte	.LASF45
	.byte	0x4
	.byte	0xf8
	.4byte	0xc0
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x9
	.4byte	.LASF46
	.byte	0x4
	.byte	0xf9
	.4byte	0xc0
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF31
	.byte	0x4
	.byte	0xfa
	.4byte	0x1ec
	.uleb128 0x7
	.4byte	.LASF47
	.byte	0x10
	.byte	0x5
	.byte	0x39
	.4byte	0x313
	.uleb128 0x8
	.ascii	"x\000"
	.byte	0x5
	.byte	0x3a
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.ascii	"y\000"
	.byte	0x5
	.byte	0x3a
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.ascii	"w\000"
	.byte	0x5
	.byte	0x3b
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.ascii	"h\000"
	.byte	0x5
	.byte	0x3b
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF47
	.byte	0x5
	.byte	0x3c
	.4byte	0x2d6
	.uleb128 0x7
	.4byte	.LASF48
	.byte	0x40
	.byte	0x6
	.byte	0x3e
	.4byte	0x3dd
	.uleb128 0x9
	.4byte	.LASF49
	.byte	0x6
	.byte	0x3f
	.4byte	0xc0
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF50
	.byte	0x6
	.byte	0x40
	.4byte	0x3dd
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.ascii	"w\000"
	.byte	0x6
	.byte	0x41
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x8
	.ascii	"h\000"
	.byte	0x6
	.byte	0x41
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x9
	.4byte	.LASF51
	.byte	0x6
	.byte	0x42
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x9
	.4byte	.LASF52
	.byte	0x6
	.byte	0x43
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x9
	.4byte	.LASF29
	.byte	0x6
	.byte	0x46
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x9
	.4byte	.LASF53
	.byte	0x6
	.byte	0x49
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x9
	.4byte	.LASF54
	.byte	0x6
	.byte	0x4a
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x9
	.4byte	.LASF55
	.byte	0x6
	.byte	0x4d
	.4byte	0x313
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0x8
	.ascii	"map\000"
	.byte	0x6
	.byte	0x50
	.4byte	0x3e9
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0x9
	.4byte	.LASF56
	.byte	0x6
	.byte	0x53
	.4byte	0x70
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0x9
	.4byte	.LASF24
	.byte	0x6
	.byte	0x56
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.byte	0x0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x2cb
	.uleb128 0xd
	.4byte	.LASF304
	.byte	0x1
	.uleb128 0xa
	.byte	0x4
	.4byte	0x3e3
	.uleb128 0x3
	.4byte	.LASF48
	.byte	0x6
	.byte	0x57
	.4byte	0x31e
	.uleb128 0x3
	.4byte	.LASF57
	.byte	0x7
	.byte	0x5e
	.4byte	0xc0
	.uleb128 0xe
	.byte	0x2
	.byte	0x8
	.byte	0x2d
	.4byte	0xa61
	.uleb128 0xf
	.4byte	.LASF58
	.sleb128 0
	.uleb128 0xf
	.4byte	.LASF59
	.sleb128 4
	.uleb128 0xf
	.4byte	.LASF60
	.sleb128 5
	.uleb128 0xf
	.4byte	.LASF61
	.sleb128 6
	.uleb128 0xf
	.4byte	.LASF62
	.sleb128 7
	.uleb128 0xf
	.4byte	.LASF63
	.sleb128 8
	.uleb128 0xf
	.4byte	.LASF64
	.sleb128 9
	.uleb128 0xf
	.4byte	.LASF65
	.sleb128 10
	.uleb128 0xf
	.4byte	.LASF66
	.sleb128 11
	.uleb128 0xf
	.4byte	.LASF67
	.sleb128 12
	.uleb128 0xf
	.4byte	.LASF68
	.sleb128 13
	.uleb128 0xf
	.4byte	.LASF69
	.sleb128 14
	.uleb128 0xf
	.4byte	.LASF70
	.sleb128 15
	.uleb128 0xf
	.4byte	.LASF71
	.sleb128 16
	.uleb128 0xf
	.4byte	.LASF72
	.sleb128 17
	.uleb128 0xf
	.4byte	.LASF73
	.sleb128 18
	.uleb128 0xf
	.4byte	.LASF74
	.sleb128 19
	.uleb128 0xf
	.4byte	.LASF75
	.sleb128 20
	.uleb128 0xf
	.4byte	.LASF76
	.sleb128 21
	.uleb128 0xf
	.4byte	.LASF77
	.sleb128 22
	.uleb128 0xf
	.4byte	.LASF78
	.sleb128 23
	.uleb128 0xf
	.4byte	.LASF79
	.sleb128 24
	.uleb128 0xf
	.4byte	.LASF80
	.sleb128 25
	.uleb128 0xf
	.4byte	.LASF81
	.sleb128 26
	.uleb128 0xf
	.4byte	.LASF82
	.sleb128 27
	.uleb128 0xf
	.4byte	.LASF83
	.sleb128 28
	.uleb128 0xf
	.4byte	.LASF84
	.sleb128 29
	.uleb128 0xf
	.4byte	.LASF85
	.sleb128 30
	.uleb128 0xf
	.4byte	.LASF86
	.sleb128 31
	.uleb128 0xf
	.4byte	.LASF87
	.sleb128 32
	.uleb128 0xf
	.4byte	.LASF88
	.sleb128 33
	.uleb128 0xf
	.4byte	.LASF89
	.sleb128 34
	.uleb128 0xf
	.4byte	.LASF90
	.sleb128 35
	.uleb128 0xf
	.4byte	.LASF91
	.sleb128 36
	.uleb128 0xf
	.4byte	.LASF92
	.sleb128 37
	.uleb128 0xf
	.4byte	.LASF93
	.sleb128 38
	.uleb128 0xf
	.4byte	.LASF94
	.sleb128 39
	.uleb128 0xf
	.4byte	.LASF95
	.sleb128 40
	.uleb128 0xf
	.4byte	.LASF96
	.sleb128 41
	.uleb128 0xf
	.4byte	.LASF97
	.sleb128 42
	.uleb128 0xf
	.4byte	.LASF98
	.sleb128 43
	.uleb128 0xf
	.4byte	.LASF99
	.sleb128 44
	.uleb128 0xf
	.4byte	.LASF100
	.sleb128 45
	.uleb128 0xf
	.4byte	.LASF101
	.sleb128 46
	.uleb128 0xf
	.4byte	.LASF102
	.sleb128 47
	.uleb128 0xf
	.4byte	.LASF103
	.sleb128 48
	.uleb128 0xf
	.4byte	.LASF104
	.sleb128 49
	.uleb128 0xf
	.4byte	.LASF105
	.sleb128 50
	.uleb128 0xf
	.4byte	.LASF106
	.sleb128 51
	.uleb128 0xf
	.4byte	.LASF107
	.sleb128 52
	.uleb128 0xf
	.4byte	.LASF108
	.sleb128 53
	.uleb128 0xf
	.4byte	.LASF109
	.sleb128 54
	.uleb128 0xf
	.4byte	.LASF110
	.sleb128 55
	.uleb128 0xf
	.4byte	.LASF111
	.sleb128 56
	.uleb128 0xf
	.4byte	.LASF112
	.sleb128 57
	.uleb128 0xf
	.4byte	.LASF113
	.sleb128 58
	.uleb128 0xf
	.4byte	.LASF114
	.sleb128 59
	.uleb128 0xf
	.4byte	.LASF115
	.sleb128 60
	.uleb128 0xf
	.4byte	.LASF116
	.sleb128 61
	.uleb128 0xf
	.4byte	.LASF117
	.sleb128 62
	.uleb128 0xf
	.4byte	.LASF118
	.sleb128 63
	.uleb128 0xf
	.4byte	.LASF119
	.sleb128 64
	.uleb128 0xf
	.4byte	.LASF120
	.sleb128 65
	.uleb128 0xf
	.4byte	.LASF121
	.sleb128 66
	.uleb128 0xf
	.4byte	.LASF122
	.sleb128 67
	.uleb128 0xf
	.4byte	.LASF123
	.sleb128 68
	.uleb128 0xf
	.4byte	.LASF124
	.sleb128 69
	.uleb128 0xf
	.4byte	.LASF125
	.sleb128 70
	.uleb128 0xf
	.4byte	.LASF126
	.sleb128 71
	.uleb128 0xf
	.4byte	.LASF127
	.sleb128 72
	.uleb128 0xf
	.4byte	.LASF128
	.sleb128 73
	.uleb128 0xf
	.4byte	.LASF129
	.sleb128 74
	.uleb128 0xf
	.4byte	.LASF130
	.sleb128 75
	.uleb128 0xf
	.4byte	.LASF131
	.sleb128 76
	.uleb128 0xf
	.4byte	.LASF132
	.sleb128 77
	.uleb128 0xf
	.4byte	.LASF133
	.sleb128 78
	.uleb128 0xf
	.4byte	.LASF134
	.sleb128 79
	.uleb128 0xf
	.4byte	.LASF135
	.sleb128 80
	.uleb128 0xf
	.4byte	.LASF136
	.sleb128 81
	.uleb128 0xf
	.4byte	.LASF137
	.sleb128 82
	.uleb128 0xf
	.4byte	.LASF138
	.sleb128 83
	.uleb128 0xf
	.4byte	.LASF139
	.sleb128 84
	.uleb128 0xf
	.4byte	.LASF140
	.sleb128 85
	.uleb128 0xf
	.4byte	.LASF141
	.sleb128 86
	.uleb128 0xf
	.4byte	.LASF142
	.sleb128 87
	.uleb128 0xf
	.4byte	.LASF143
	.sleb128 88
	.uleb128 0xf
	.4byte	.LASF144
	.sleb128 89
	.uleb128 0xf
	.4byte	.LASF145
	.sleb128 90
	.uleb128 0xf
	.4byte	.LASF146
	.sleb128 91
	.uleb128 0xf
	.4byte	.LASF147
	.sleb128 92
	.uleb128 0xf
	.4byte	.LASF148
	.sleb128 93
	.uleb128 0xf
	.4byte	.LASF149
	.sleb128 94
	.uleb128 0xf
	.4byte	.LASF150
	.sleb128 95
	.uleb128 0xf
	.4byte	.LASF151
	.sleb128 96
	.uleb128 0xf
	.4byte	.LASF152
	.sleb128 97
	.uleb128 0xf
	.4byte	.LASF153
	.sleb128 98
	.uleb128 0xf
	.4byte	.LASF154
	.sleb128 99
	.uleb128 0xf
	.4byte	.LASF155
	.sleb128 100
	.uleb128 0xf
	.4byte	.LASF156
	.sleb128 101
	.uleb128 0xf
	.4byte	.LASF157
	.sleb128 102
	.uleb128 0xf
	.4byte	.LASF158
	.sleb128 103
	.uleb128 0xf
	.4byte	.LASF159
	.sleb128 104
	.uleb128 0xf
	.4byte	.LASF160
	.sleb128 105
	.uleb128 0xf
	.4byte	.LASF161
	.sleb128 106
	.uleb128 0xf
	.4byte	.LASF162
	.sleb128 107
	.uleb128 0xf
	.4byte	.LASF163
	.sleb128 108
	.uleb128 0xf
	.4byte	.LASF164
	.sleb128 109
	.uleb128 0xf
	.4byte	.LASF165
	.sleb128 110
	.uleb128 0xf
	.4byte	.LASF166
	.sleb128 111
	.uleb128 0xf
	.4byte	.LASF167
	.sleb128 112
	.uleb128 0xf
	.4byte	.LASF168
	.sleb128 113
	.uleb128 0xf
	.4byte	.LASF169
	.sleb128 114
	.uleb128 0xf
	.4byte	.LASF170
	.sleb128 115
	.uleb128 0xf
	.4byte	.LASF171
	.sleb128 116
	.uleb128 0xf
	.4byte	.LASF172
	.sleb128 117
	.uleb128 0xf
	.4byte	.LASF173
	.sleb128 118
	.uleb128 0xf
	.4byte	.LASF174
	.sleb128 119
	.uleb128 0xf
	.4byte	.LASF175
	.sleb128 120
	.uleb128 0xf
	.4byte	.LASF176
	.sleb128 121
	.uleb128 0xf
	.4byte	.LASF177
	.sleb128 122
	.uleb128 0xf
	.4byte	.LASF178
	.sleb128 123
	.uleb128 0xf
	.4byte	.LASF179
	.sleb128 124
	.uleb128 0xf
	.4byte	.LASF180
	.sleb128 125
	.uleb128 0xf
	.4byte	.LASF181
	.sleb128 126
	.uleb128 0xf
	.4byte	.LASF182
	.sleb128 127
	.uleb128 0xf
	.4byte	.LASF183
	.sleb128 128
	.uleb128 0xf
	.4byte	.LASF184
	.sleb128 129
	.uleb128 0xf
	.4byte	.LASF185
	.sleb128 133
	.uleb128 0xf
	.4byte	.LASF186
	.sleb128 134
	.uleb128 0xf
	.4byte	.LASF187
	.sleb128 135
	.uleb128 0xf
	.4byte	.LASF188
	.sleb128 136
	.uleb128 0xf
	.4byte	.LASF189
	.sleb128 137
	.uleb128 0xf
	.4byte	.LASF190
	.sleb128 138
	.uleb128 0xf
	.4byte	.LASF191
	.sleb128 139
	.uleb128 0xf
	.4byte	.LASF192
	.sleb128 140
	.uleb128 0xf
	.4byte	.LASF193
	.sleb128 141
	.uleb128 0xf
	.4byte	.LASF194
	.sleb128 142
	.uleb128 0xf
	.4byte	.LASF195
	.sleb128 143
	.uleb128 0xf
	.4byte	.LASF196
	.sleb128 144
	.uleb128 0xf
	.4byte	.LASF197
	.sleb128 145
	.uleb128 0xf
	.4byte	.LASF198
	.sleb128 146
	.uleb128 0xf
	.4byte	.LASF199
	.sleb128 147
	.uleb128 0xf
	.4byte	.LASF200
	.sleb128 148
	.uleb128 0xf
	.4byte	.LASF201
	.sleb128 149
	.uleb128 0xf
	.4byte	.LASF202
	.sleb128 150
	.uleb128 0xf
	.4byte	.LASF203
	.sleb128 151
	.uleb128 0xf
	.4byte	.LASF204
	.sleb128 152
	.uleb128 0xf
	.4byte	.LASF205
	.sleb128 153
	.uleb128 0xf
	.4byte	.LASF206
	.sleb128 154
	.uleb128 0xf
	.4byte	.LASF207
	.sleb128 155
	.uleb128 0xf
	.4byte	.LASF208
	.sleb128 156
	.uleb128 0xf
	.4byte	.LASF209
	.sleb128 157
	.uleb128 0xf
	.4byte	.LASF210
	.sleb128 158
	.uleb128 0xf
	.4byte	.LASF211
	.sleb128 159
	.uleb128 0xf
	.4byte	.LASF212
	.sleb128 160
	.uleb128 0xf
	.4byte	.LASF213
	.sleb128 161
	.uleb128 0xf
	.4byte	.LASF214
	.sleb128 162
	.uleb128 0xf
	.4byte	.LASF215
	.sleb128 163
	.uleb128 0xf
	.4byte	.LASF216
	.sleb128 164
	.uleb128 0xf
	.4byte	.LASF217
	.sleb128 176
	.uleb128 0xf
	.4byte	.LASF218
	.sleb128 177
	.uleb128 0xf
	.4byte	.LASF219
	.sleb128 178
	.uleb128 0xf
	.4byte	.LASF220
	.sleb128 179
	.uleb128 0xf
	.4byte	.LASF221
	.sleb128 180
	.uleb128 0xf
	.4byte	.LASF222
	.sleb128 181
	.uleb128 0xf
	.4byte	.LASF223
	.sleb128 182
	.uleb128 0xf
	.4byte	.LASF224
	.sleb128 183
	.uleb128 0xf
	.4byte	.LASF225
	.sleb128 184
	.uleb128 0xf
	.4byte	.LASF226
	.sleb128 185
	.uleb128 0xf
	.4byte	.LASF227
	.sleb128 186
	.uleb128 0xf
	.4byte	.LASF228
	.sleb128 187
	.uleb128 0xf
	.4byte	.LASF229
	.sleb128 188
	.uleb128 0xf
	.4byte	.LASF230
	.sleb128 189
	.uleb128 0xf
	.4byte	.LASF231
	.sleb128 190
	.uleb128 0xf
	.4byte	.LASF232
	.sleb128 191
	.uleb128 0xf
	.4byte	.LASF233
	.sleb128 192
	.uleb128 0xf
	.4byte	.LASF234
	.sleb128 193
	.uleb128 0xf
	.4byte	.LASF235
	.sleb128 194
	.uleb128 0xf
	.4byte	.LASF236
	.sleb128 195
	.uleb128 0xf
	.4byte	.LASF237
	.sleb128 196
	.uleb128 0xf
	.4byte	.LASF238
	.sleb128 197
	.uleb128 0xf
	.4byte	.LASF239
	.sleb128 198
	.uleb128 0xf
	.4byte	.LASF240
	.sleb128 199
	.uleb128 0xf
	.4byte	.LASF241
	.sleb128 200
	.uleb128 0xf
	.4byte	.LASF242
	.sleb128 201
	.uleb128 0xf
	.4byte	.LASF243
	.sleb128 202
	.uleb128 0xf
	.4byte	.LASF244
	.sleb128 203
	.uleb128 0xf
	.4byte	.LASF245
	.sleb128 204
	.uleb128 0xf
	.4byte	.LASF246
	.sleb128 205
	.uleb128 0xf
	.4byte	.LASF247
	.sleb128 206
	.uleb128 0xf
	.4byte	.LASF248
	.sleb128 207
	.uleb128 0xf
	.4byte	.LASF249
	.sleb128 208
	.uleb128 0xf
	.4byte	.LASF250
	.sleb128 209
	.uleb128 0xf
	.4byte	.LASF251
	.sleb128 210
	.uleb128 0xf
	.4byte	.LASF252
	.sleb128 211
	.uleb128 0xf
	.4byte	.LASF253
	.sleb128 212
	.uleb128 0xf
	.4byte	.LASF254
	.sleb128 213
	.uleb128 0xf
	.4byte	.LASF255
	.sleb128 214
	.uleb128 0xf
	.4byte	.LASF256
	.sleb128 215
	.uleb128 0xf
	.4byte	.LASF257
	.sleb128 216
	.uleb128 0xf
	.4byte	.LASF258
	.sleb128 217
	.uleb128 0xf
	.4byte	.LASF259
	.sleb128 218
	.uleb128 0xf
	.4byte	.LASF260
	.sleb128 219
	.uleb128 0xf
	.4byte	.LASF261
	.sleb128 220
	.uleb128 0xf
	.4byte	.LASF262
	.sleb128 221
	.uleb128 0xf
	.4byte	.LASF263
	.sleb128 224
	.uleb128 0xf
	.4byte	.LASF264
	.sleb128 225
	.uleb128 0xf
	.4byte	.LASF265
	.sleb128 226
	.uleb128 0xf
	.4byte	.LASF266
	.sleb128 227
	.uleb128 0xf
	.4byte	.LASF267
	.sleb128 228
	.uleb128 0xf
	.4byte	.LASF268
	.sleb128 229
	.uleb128 0xf
	.4byte	.LASF269
	.sleb128 230
	.uleb128 0xf
	.4byte	.LASF270
	.sleb128 231
	.uleb128 0xf
	.4byte	.LASF271
	.sleb128 257
	.uleb128 0xf
	.4byte	.LASF272
	.sleb128 258
	.uleb128 0xf
	.4byte	.LASF273
	.sleb128 259
	.uleb128 0xf
	.4byte	.LASF274
	.sleb128 260
	.uleb128 0xf
	.4byte	.LASF275
	.sleb128 261
	.uleb128 0xf
	.4byte	.LASF276
	.sleb128 262
	.uleb128 0xf
	.4byte	.LASF277
	.sleb128 263
	.uleb128 0xf
	.4byte	.LASF278
	.sleb128 264
	.uleb128 0xf
	.4byte	.LASF279
	.sleb128 265
	.uleb128 0xf
	.4byte	.LASF280
	.sleb128 266
	.uleb128 0xf
	.4byte	.LASF281
	.sleb128 267
	.uleb128 0xf
	.4byte	.LASF282
	.sleb128 268
	.uleb128 0xf
	.4byte	.LASF283
	.sleb128 269
	.uleb128 0xf
	.4byte	.LASF284
	.sleb128 270
	.uleb128 0xf
	.4byte	.LASF285
	.sleb128 271
	.uleb128 0xf
	.4byte	.LASF286
	.sleb128 272
	.uleb128 0xf
	.4byte	.LASF287
	.sleb128 273
	.uleb128 0xf
	.4byte	.LASF288
	.sleb128 274
	.uleb128 0xf
	.4byte	.LASF289
	.sleb128 275
	.uleb128 0xf
	.4byte	.LASF290
	.sleb128 276
	.uleb128 0xf
	.4byte	.LASF291
	.sleb128 277
	.uleb128 0xf
	.4byte	.LASF292
	.sleb128 278
	.uleb128 0xf
	.4byte	.LASF293
	.sleb128 279
	.uleb128 0xf
	.4byte	.LASF294
	.sleb128 280
	.uleb128 0xf
	.4byte	.LASF295
	.sleb128 281
	.uleb128 0xf
	.4byte	.LASF296
	.sleb128 282
	.uleb128 0xf
	.4byte	.LASF297
	.sleb128 512
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF298
	.byte	0x8
	.2byte	0x13b
	.4byte	0x405
	.uleb128 0x3
	.4byte	.LASF299
	.byte	0x9
	.byte	0x2b
	.4byte	0xb5
	.uleb128 0x7
	.4byte	.LASF300
	.byte	0x10
	.byte	0xa
	.byte	0x32
	.4byte	0xabd
	.uleb128 0x9
	.4byte	.LASF301
	.byte	0xa
	.byte	0x33
	.4byte	0xa61
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x8
	.ascii	"sym\000"
	.byte	0xa
	.byte	0x34
	.4byte	0xa6d
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.ascii	"mod\000"
	.byte	0xa
	.byte	0x35
	.4byte	0xaa
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.4byte	.LASF302
	.byte	0xa
	.byte	0x36
	.4byte	0xc0
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF300
	.byte	0xa
	.byte	0x37
	.4byte	0xa78
	.uleb128 0x3
	.4byte	.LASF303
	.byte	0xb
	.byte	0x32
	.4byte	0xad3
	.uleb128 0xd
	.4byte	.LASF305
	.byte	0x1
	.uleb128 0xe
	.byte	0x1
	.byte	0xc
	.byte	0x3a
	.4byte	0xb60
	.uleb128 0xf
	.4byte	.LASF306
	.sleb128 0
	.uleb128 0xf
	.4byte	.LASF307
	.sleb128 1
	.uleb128 0xf
	.4byte	.LASF308
	.sleb128 2
	.uleb128 0xf
	.4byte	.LASF309
	.sleb128 3
	.uleb128 0xf
	.4byte	.LASF310
	.sleb128 4
	.uleb128 0xf
	.4byte	.LASF311
	.sleb128 5
	.uleb128 0xf
	.4byte	.LASF312
	.sleb128 6
	.uleb128 0xf
	.4byte	.LASF313
	.sleb128 7
	.uleb128 0xf
	.4byte	.LASF314
	.sleb128 8
	.uleb128 0xf
	.4byte	.LASF315
	.sleb128 9
	.uleb128 0xf
	.4byte	.LASF316
	.sleb128 10
	.uleb128 0xf
	.4byte	.LASF317
	.sleb128 11
	.uleb128 0xf
	.4byte	.LASF318
	.sleb128 12
	.uleb128 0xf
	.4byte	.LASF319
	.sleb128 13
	.uleb128 0xf
	.4byte	.LASF320
	.sleb128 14
	.uleb128 0xf
	.4byte	.LASF321
	.sleb128 15
	.uleb128 0xf
	.4byte	.LASF322
	.sleb128 16
	.uleb128 0xf
	.4byte	.LASF323
	.sleb128 17
	.uleb128 0xf
	.4byte	.LASF324
	.sleb128 18
	.uleb128 0xf
	.4byte	.LASF325
	.sleb128 24
	.uleb128 0xf
	.4byte	.LASF326
	.sleb128 32
	.byte	0x0
	.uleb128 0x7
	.4byte	.LASF327
	.byte	0x10
	.byte	0xc
	.byte	0x7d
	.4byte	0xbb3
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0x7e
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF329
	.byte	0xc
	.byte	0x7f
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x9
	.4byte	.LASF330
	.byte	0xc
	.byte	0x80
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x9
	.4byte	.LASF331
	.byte	0xc
	.byte	0x81
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.4byte	.LASF332
	.byte	0xc
	.byte	0x82
	.4byte	0x3fa
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF327
	.byte	0xc
	.byte	0x83
	.4byte	0xb60
	.uleb128 0x7
	.4byte	.LASF333
	.byte	0x18
	.byte	0xc
	.byte	0x8b
	.4byte	0xc11
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0x8c
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0x8d
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x9
	.4byte	.LASF335
	.byte	0xc
	.byte	0x8e
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x9
	.4byte	.LASF336
	.byte	0xc
	.byte	0x8f
	.4byte	0xabd
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x9
	.4byte	.LASF332
	.byte	0xc
	.byte	0x90
	.4byte	0x3fa
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF333
	.byte	0xc
	.byte	0x91
	.4byte	0xbbe
	.uleb128 0x7
	.4byte	.LASF337
	.byte	0x28
	.byte	0xc
	.byte	0x9a
	.4byte	0xc61
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0x9b
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0x9c
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x9
	.4byte	.LASF338
	.byte	0xc
	.byte	0x9d
	.4byte	0xc61
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x9
	.4byte	.LASF332
	.byte	0xc
	.byte	0x9e
	.4byte	0x3fa
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.byte	0x0
	.uleb128 0x11
	.4byte	0x98
	.4byte	0xc71
	.uleb128 0x12
	.4byte	0x8c
	.byte	0x1f
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF337
	.byte	0xc
	.byte	0x9f
	.4byte	0xc1c
	.uleb128 0x7
	.4byte	.LASF339
	.byte	0x18
	.byte	0xc
	.byte	0xa7
	.4byte	0xcf5
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0xa8
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0xa9
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x9
	.4byte	.LASF335
	.byte	0xc
	.byte	0xaa
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x8
	.ascii	"x\000"
	.byte	0xc
	.byte	0xab
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.ascii	"y\000"
	.byte	0xc
	.byte	0xac
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.4byte	.LASF340
	.byte	0xc
	.byte	0xad
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x9
	.4byte	.LASF341
	.byte	0xc
	.byte	0xae
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x9
	.4byte	.LASF332
	.byte	0xc
	.byte	0xaf
	.4byte	0x3fa
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF339
	.byte	0xc
	.byte	0xb0
	.4byte	0xc7c
	.uleb128 0x7
	.4byte	.LASF342
	.byte	0x10
	.byte	0xc
	.byte	0xb8
	.4byte	0xd6b
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0xb9
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0xba
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x9
	.4byte	.LASF343
	.byte	0xc
	.byte	0xbb
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x9
	.4byte	.LASF335
	.byte	0xc
	.byte	0xbc
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x3
	.uleb128 0x8
	.ascii	"x\000"
	.byte	0xc
	.byte	0xbd
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.ascii	"y\000"
	.byte	0xc
	.byte	0xbe
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.4byte	.LASF332
	.byte	0xc
	.byte	0xbf
	.4byte	0x3fa
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF342
	.byte	0xc
	.byte	0xc0
	.4byte	0xd00
	.uleb128 0x7
	.4byte	.LASF344
	.byte	0x10
	.byte	0xc
	.byte	0xc8
	.4byte	0xdc5
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0xc9
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0xca
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x8
	.ascii	"x\000"
	.byte	0xc
	.byte	0xcb
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.ascii	"y\000"
	.byte	0xc
	.byte	0xcc
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x9
	.4byte	.LASF332
	.byte	0xc
	.byte	0xcd
	.4byte	0x3fa
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF344
	.byte	0xc
	.byte	0xce
	.4byte	0xd76
	.uleb128 0x7
	.4byte	.LASF345
	.byte	0x8
	.byte	0xc
	.byte	0xd6
	.4byte	0xe15
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0xd7
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0xd8
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x9
	.4byte	.LASF346
	.byte	0xc
	.byte	0xd9
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x9
	.4byte	.LASF347
	.byte	0xc
	.byte	0xda
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF345
	.byte	0xc
	.byte	0xdb
	.4byte	0xdd0
	.uleb128 0x7
	.4byte	.LASF348
	.byte	0xc
	.byte	0xc
	.byte	0xe3
	.4byte	0xe73
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0xe4
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0xe5
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x9
	.4byte	.LASF349
	.byte	0xc
	.byte	0xe6
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x9
	.4byte	.LASF340
	.byte	0xc
	.byte	0xe7
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x9
	.4byte	.LASF341
	.byte	0xc
	.byte	0xe8
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF348
	.byte	0xc
	.byte	0xe9
	.4byte	0xe20
	.uleb128 0x7
	.4byte	.LASF350
	.byte	0x4
	.byte	0xc
	.byte	0xf1
	.4byte	0xec3
	.uleb128 0x9
	.4byte	.LASF328
	.byte	0xc
	.byte	0xf2
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x9
	.4byte	.LASF334
	.byte	0xc
	.byte	0xf3
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x8
	.ascii	"hat\000"
	.byte	0xc
	.byte	0xf4
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x9
	.4byte	.LASF347
	.byte	0xc
	.byte	0xf5
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x3
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF350
	.byte	0xc
	.byte	0xfb
	.4byte	0xe7e
	.uleb128 0x13
	.4byte	.LASF351
	.byte	0x4
	.byte	0xc
	.2byte	0x103
	.4byte	0xf18
	.uleb128 0x14
	.4byte	.LASF328
	.byte	0xc
	.2byte	0x104
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x14
	.4byte	.LASF334
	.byte	0xc
	.2byte	0x105
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x14
	.4byte	.LASF343
	.byte	0xc
	.2byte	0x106
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.uleb128 0x14
	.4byte	.LASF335
	.byte	0xc
	.2byte	0x107
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x3
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF351
	.byte	0xc
	.2byte	0x108
	.4byte	0xece
	.uleb128 0x13
	.4byte	.LASF352
	.byte	0x1
	.byte	0xc
	.2byte	0x110
	.4byte	0xf41
	.uleb128 0x14
	.4byte	.LASF328
	.byte	0xc
	.2byte	0x111
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF352
	.byte	0xc
	.2byte	0x112
	.4byte	0xf24
	.uleb128 0x13
	.4byte	.LASF353
	.byte	0x14
	.byte	0xc
	.2byte	0x11a
	.4byte	0xfa6
	.uleb128 0x14
	.4byte	.LASF328
	.byte	0xc
	.2byte	0x11b
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x14
	.4byte	.LASF354
	.byte	0xc
	.2byte	0x11c
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x14
	.4byte	.LASF330
	.byte	0xc
	.2byte	0x11d
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x14
	.4byte	.LASF331
	.byte	0xc
	.2byte	0x11e
	.4byte	0x8f
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x14
	.4byte	.LASF332
	.byte	0xc
	.2byte	0x11f
	.4byte	0x3fa
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF353
	.byte	0xc
	.2byte	0x120
	.4byte	0xf4d
	.uleb128 0x10
	.4byte	.LASF355
	.byte	0xc
	.2byte	0x12a
	.4byte	0xfbe
	.uleb128 0xd
	.4byte	.LASF355
	.byte	0x1
	.uleb128 0x13
	.4byte	.LASF356
	.byte	0x8
	.byte	0xc
	.2byte	0x12c
	.4byte	0xff0
	.uleb128 0x14
	.4byte	.LASF328
	.byte	0xc
	.2byte	0x12d
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x15
	.ascii	"msg\000"
	.byte	0xc
	.2byte	0x12e
	.4byte	0xff0
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0x0
	.uleb128 0xa
	.byte	0x4
	.4byte	0xfb2
	.uleb128 0x10
	.4byte	.LASF356
	.byte	0xc
	.2byte	0x12f
	.4byte	0xfc4
	.uleb128 0x13
	.4byte	.LASF357
	.byte	0x3
	.byte	0xc
	.2byte	0x133
	.4byte	0x103d
	.uleb128 0x14
	.4byte	.LASF328
	.byte	0xc
	.2byte	0x134
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x14
	.4byte	.LASF358
	.byte	0xc
	.2byte	0x135
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x1
	.uleb128 0x14
	.4byte	.LASF335
	.byte	0xc
	.2byte	0x136
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x2
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF357
	.byte	0xc
	.2byte	0x137
	.4byte	0x1002
	.uleb128 0x13
	.4byte	.LASF359
	.byte	0xc
	.byte	0xc
	.2byte	0x139
	.4byte	0x1080
	.uleb128 0x14
	.4byte	.LASF328
	.byte	0xc
	.2byte	0x13a
	.4byte	0x9f
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x15
	.ascii	"w\000"
	.byte	0xc
	.2byte	0x13b
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x15
	.ascii	"h\000"
	.byte	0xc
	.2byte	0x13c
	.4byte	0x5e
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF359
	.byte	0xc
	.2byte	0x13d
	.4byte	0x1049
	.uleb128 0x16
	.4byte	.LASF372
	.byte	0x28
	.byte	0xc
	.2byte	0x145
	.4byte	0x115a
	.uleb128 0x17
	.4byte	.LASF328
	.byte	0xc
	.2byte	0x146
	.4byte	0x9f
	.uleb128 0x17
	.4byte	.LASF360
	.byte	0xc
	.2byte	0x147
	.4byte	0xbb3
	.uleb128 0x18
	.ascii	"key\000"
	.byte	0xc
	.2byte	0x148
	.4byte	0xc11
	.uleb128 0x17
	.4byte	.LASF338
	.byte	0xc
	.2byte	0x149
	.4byte	0xc71
	.uleb128 0x17
	.4byte	.LASF361
	.byte	0xc
	.2byte	0x14a
	.4byte	0xcf5
	.uleb128 0x17
	.4byte	.LASF343
	.byte	0xc
	.2byte	0x14b
	.4byte	0xd6b
	.uleb128 0x17
	.4byte	.LASF362
	.byte	0xc
	.2byte	0x14c
	.4byte	0xdc5
	.uleb128 0x17
	.4byte	.LASF363
	.byte	0xc
	.2byte	0x14d
	.4byte	0xe15
	.uleb128 0x17
	.4byte	.LASF364
	.byte	0xc
	.2byte	0x14e
	.4byte	0xe73
	.uleb128 0x17
	.4byte	.LASF365
	.byte	0xc
	.2byte	0x14f
	.4byte	0xec3
	.uleb128 0x17
	.4byte	.LASF366
	.byte	0xc
	.2byte	0x150
	.4byte	0xf18
	.uleb128 0x17
	.4byte	.LASF367
	.byte	0xc
	.2byte	0x151
	.4byte	0xf41
	.uleb128 0x17
	.4byte	.LASF368
	.byte	0xc
	.2byte	0x152
	.4byte	0xfa6
	.uleb128 0x17
	.4byte	.LASF369
	.byte	0xc
	.2byte	0x153
	.4byte	0xff6
	.uleb128 0x17
	.4byte	.LASF370
	.byte	0xc
	.2byte	0x156
	.4byte	0x103d
	.uleb128 0x17
	.4byte	.LASF371
	.byte	0xc
	.2byte	0x157
	.4byte	0x1080
	.byte	0x0
	.uleb128 0x10
	.4byte	.LASF372
	.byte	0xc
	.2byte	0x158
	.4byte	0x108c
	.uleb128 0x3
	.4byte	.LASF373
	.byte	0xd
	.byte	0x45
	.4byte	0x70
	.uleb128 0x2
	.byte	0x4
	.byte	0x4
	.4byte	.LASF374
	.uleb128 0xe
	.byte	0x1
	.byte	0xe
	.byte	0x9b
	.4byte	0x1199
	.uleb128 0xf
	.4byte	.LASF375
	.sleb128 0
	.uleb128 0xf
	.4byte	.LASF376
	.sleb128 1
	.uleb128 0xf
	.4byte	.LASF377
	.sleb128 2
	.uleb128 0xf
	.4byte	.LASF378
	.sleb128 3
	.byte	0x0
	.uleb128 0x3
	.4byte	.LASF379
	.byte	0xe
	.byte	0xa0
	.4byte	0x1178
	.uleb128 0x19
	.2byte	0x2018
	.byte	0xe
	.2byte	0x119
	.4byte	0x121c
	.uleb128 0x14
	.4byte	.LASF380
	.byte	0xe
	.2byte	0x11a
	.4byte	0x1199
	.byte	0x2
	.byte	0x23
	.uleb128 0x0
	.uleb128 0x14
	.4byte	.LASF381
	.byte	0xe
	.2byte	0x11d
	.4byte	0x1166
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x14
	.4byte	.LASF382
	.byte	0xe
	.2byte	0x120
	.4byte	0x121c
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x14
	.4byte	.LASF383
	.byte	0xe
	.2byte	0x121
	.4byte	0x1166
	.byte	0x3
	.byte	0x23
	.uleb128 0x2008
	.uleb128 0x14
	.4byte	.LASF384
	.byte	0xe
	.2byte	0x122
	.4byte	0x122d
	.byte	0x3
	.byte	0x23
	.uleb128 0x200c
	.uleb128 0x14
	.4byte	.LASF385
	.byte	0xe
	.2byte	0x123
	.4byte	0x1166
	.byte	0x3
	.byte	0x23
	.uleb128 0x2010
	.uleb128 0x14
	.4byte	.LASF386
	.byte	0xe
	.2byte	0x124
	.4byte	0x5e
	.byte	0x3
	.byte	0x23
	.uleb128 0x2014
	.byte	0x0
	.uleb128 0x11
	.4byte	0x1166
	.4byte	0x122d
	.uleb128 0x1a
	.4byte	0x8c
	.2byte	0x7ff
	.byte	0x0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x1166
	.uleb128 0x10
	.4byte	.LASF387
	.byte	0xe
	.2byte	0x126
	.4byte	0x11a4
	.uleb128 0x1b
	.byte	0x1
	.4byte	.LASF397
	.byte	0x1
	.byte	0x5
	.byte	0x1
	.4byte	.LFB108
	.4byte	.LFE108
	.4byte	.LLST0
	.4byte	0x127b
	.uleb128 0x1c
	.ascii	"s\000"
	.byte	0x1
	.byte	0x5
	.4byte	0x5e
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x1d
	.4byte	.LBB2
	.4byte	.LBE2
	.uleb128 0x1e
	.ascii	"i\000"
	.byte	0x1
	.byte	0x7
	.4byte	0x5e
	.byte	0x2
	.byte	0x91
	.sleb128 -12
	.byte	0x0
	.byte	0x0
	.uleb128 0x1f
	.byte	0x1
	.4byte	.LASF398
	.byte	0x1
	.byte	0xc
	.byte	0x1
	.4byte	0x5e
	.4byte	.LFB109
	.4byte	.LFE109
	.4byte	.LLST1
	.4byte	0x12d2
	.uleb128 0x20
	.4byte	.LASF388
	.byte	0x1
	.byte	0xd
	.4byte	0x12d2
	.byte	0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x20
	.4byte	.LASF389
	.byte	0x1
	.byte	0xe
	.4byte	0x12d8
	.byte	0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x20
	.4byte	.LASF329
	.byte	0x1
	.byte	0xf
	.4byte	0x115a
	.byte	0x2
	.byte	0x91
	.sleb128 -64
	.uleb128 0x20
	.4byte	.LASF390
	.byte	0x1
	.byte	0x10
	.4byte	0x313
	.byte	0x3
	.byte	0x91
	.sleb128 -80
	.byte	0x0
	.uleb128 0xa
	.byte	0x4
	.4byte	0x3ef
	.uleb128 0xa
	.byte	0x4
	.4byte	0xac8
	.uleb128 0x21
	.4byte	.LASF391
	.byte	0xe
	.2byte	0x129
	.4byte	0x12f0
	.byte	0x5
	.byte	0x3
	.4byte	glGlob
	.uleb128 0xa
	.byte	0x4
	.4byte	0x1233
	.uleb128 0x11
	.4byte	0x3a
	.4byte	0x1301
	.uleb128 0x22
	.byte	0x0
	.uleb128 0x23
	.4byte	.LASF392
	.byte	0xf
	.byte	0x2c
	.4byte	0x12f6
	.byte	0x1
	.byte	0x1
	.uleb128 0x23
	.4byte	.LASF393
	.byte	0xf
	.byte	0x2d
	.4byte	0x12f6
	.byte	0x1
	.byte	0x1
	.uleb128 0x23
	.4byte	.LASF394
	.byte	0xf
	.byte	0x2e
	.4byte	0x12f6
	.byte	0x1
	.byte	0x1
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x6
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x7
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x9
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0xa
	.uleb128 0xf
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xb
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xc
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xd
	.uleb128 0x13
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.uleb128 0xe
	.uleb128 0x4
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0xf
	.uleb128 0x28
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0x0
	.byte	0x0
	.uleb128 0x10
	.uleb128 0x16
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x11
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x12
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0x0
	.byte	0x0
	.uleb128 0x13
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x14
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x15
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x16
	.uleb128 0x17
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x17
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x18
	.uleb128 0xd
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x19
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0x5
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1a
	.uleb128 0x21
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0x0
	.byte	0x0
	.uleb128 0x1b
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x1d
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0x0
	.byte	0x0
	.uleb128 0x1e
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x1f
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x20
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x21
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x22
	.uleb128 0x21
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.uleb128 0x23
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",%progbits
	.4byte	0x21
	.2byte	0x2
	.4byte	.Ldebug_info0
	.4byte	0x1329
	.4byte	0x123f
	.ascii	"delay\000"
	.4byte	0x127b
	.ascii	"main\000"
	.4byte	0x0
	.section	.debug_aranges,"",%progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.2byte	0x0
	.2byte	0x0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0x0
	.4byte	0x0
	.section	.debug_str,"MS",%progbits,1
.LASF11:
	.ascii	"long int\000"
.LASF112:
	.ascii	"SDL_SCANCODE_CAPSLOCK\000"
.LASF298:
	.ascii	"SDL_scancode\000"
.LASF166:
	.ascii	"SDL_SCANCODE_F20\000"
.LASF167:
	.ascii	"SDL_SCANCODE_F21\000"
.LASF243:
	.ascii	"SDL_SCANCODE_KP_DBLVERTICALBAR\000"
.LASF169:
	.ascii	"SDL_SCANCODE_F23\000"
.LASF16:
	.ascii	"Sint32\000"
.LASF133:
	.ascii	"SDL_SCANCODE_PAGEDOWN\000"
.LASF168:
	.ascii	"SDL_SCANCODE_F22\000"
.LASF241:
	.ascii	"SDL_SCANCODE_KP_DBLAMPERSAND\000"
.LASF170:
	.ascii	"SDL_SCANCODE_F24\000"
.LASF22:
	.ascii	"ncolors\000"
.LASF50:
	.ascii	"format\000"
.LASF300:
	.ascii	"SDL_keysym\000"
.LASF32:
	.ascii	"palette\000"
.LASF312:
	.ascii	"SDL_MOUSEBUTTONDOWN\000"
.LASF286:
	.ascii	"SDL_SCANCODE_AC_STOP\000"
.LASF49:
	.ascii	"flags\000"
.LASF246:
	.ascii	"SDL_SCANCODE_KP_SPACE\000"
.LASF313:
	.ascii	"SDL_MOUSEBUTTONUP\000"
.LASF15:
	.ascii	"Uint16\000"
.LASF367:
	.ascii	"quit\000"
.LASF38:
	.ascii	"Aloss\000"
.LASF33:
	.ascii	"BitsPerPixel\000"
.LASF317:
	.ascii	"SDL_JOYHATMOTION\000"
.LASF30:
	.ascii	"next\000"
.LASF385:
	.ascii	"nextPBlock\000"
.LASF275:
	.ascii	"SDL_SCANCODE_AUDIOPLAY\000"
.LASF223:
	.ascii	"SDL_SCANCODE_KP_LEFTPAREN\000"
.LASF309:
	.ascii	"SDL_KEYUP\000"
.LASF369:
	.ascii	"syswm\000"
.LASF51:
	.ascii	"pitch\000"
.LASF289:
	.ascii	"SDL_SCANCODE_BRIGHTNESSDOWN\000"
.LASF355:
	.ascii	"SDL_SysWMmsg\000"
.LASF157:
	.ascii	"SDL_SCANCODE_POWER\000"
.LASF54:
	.ascii	"lock_data\000"
.LASF208:
	.ascii	"SDL_SCANCODE_CLEAR\000"
.LASF2:
	.ascii	"short int\000"
.LASF96:
	.ascii	"SDL_SCANCODE_ESCAPE\000"
.LASF332:
	.ascii	"windowID\000"
.LASF346:
	.ascii	"axis\000"
.LASF305:
	.ascii	"_SDL_Joystick\000"
.LASF226:
	.ascii	"SDL_SCANCODE_KP_RIGHTBRACE\000"
.LASF291:
	.ascii	"SDL_SCANCODE_DISPLAYSWITCH\000"
.LASF251:
	.ascii	"SDL_SCANCODE_KP_MEMCLEAR\000"
.LASF177:
	.ascii	"SDL_SCANCODE_UNDO\000"
.LASF142:
	.ascii	"SDL_SCANCODE_KP_PLUS\000"
.LASF127:
	.ascii	"SDL_SCANCODE_PAUSE\000"
.LASF267:
	.ascii	"SDL_SCANCODE_RCTRL\000"
.LASF181:
	.ascii	"SDL_SCANCODE_FIND\000"
.LASF362:
	.ascii	"wheel\000"
.LASF287:
	.ascii	"SDL_SCANCODE_AC_REFRESH\000"
.LASF234:
	.ascii	"SDL_SCANCODE_KP_F\000"
.LASF100:
	.ascii	"SDL_SCANCODE_MINUS\000"
.LASF359:
	.ascii	"SDL_ResizeEvent\000"
.LASF388:
	.ascii	"screen\000"
.LASF293:
	.ascii	"SDL_SCANCODE_KBDILLUMDOWN\000"
.LASF245:
	.ascii	"SDL_SCANCODE_KP_HASH\000"
.LASF218:
	.ascii	"SDL_SCANCODE_KP_000\000"
.LASF17:
	.ascii	"Uint32\000"
.LASF109:
	.ascii	"SDL_SCANCODE_COMMA\000"
.LASF7:
	.ascii	"uint32_t\000"
.LASF397:
	.ascii	"delay\000"
.LASF296:
	.ascii	"SDL_SCANCODE_SLEEP\000"
.LASF217:
	.ascii	"SDL_SCANCODE_KP_00\000"
.LASF14:
	.ascii	"Uint8\000"
.LASF36:
	.ascii	"Gloss\000"
.LASF52:
	.ascii	"pixels\000"
.LASF235:
	.ascii	"SDL_SCANCODE_KP_XOR\000"
.LASF45:
	.ascii	"Bmask\000"
.LASF295:
	.ascii	"SDL_SCANCODE_EJECT\000"
.LASF364:
	.ascii	"jball\000"
.LASF327:
	.ascii	"SDL_WindowEvent\000"
.LASF213:
	.ascii	"SDL_SCANCODE_OPER\000"
.LASF186:
	.ascii	"SDL_SCANCODE_KP_EQUALSAS400\000"
.LASF259:
	.ascii	"SDL_SCANCODE_KP_BINARY\000"
.LASF329:
	.ascii	"event\000"
.LASF244:
	.ascii	"SDL_SCANCODE_KP_COLON\000"
.LASF91:
	.ascii	"SDL_SCANCODE_7\000"
.LASF278:
	.ascii	"SDL_SCANCODE_WWW\000"
.LASF261:
	.ascii	"SDL_SCANCODE_KP_DECIMAL\000"
.LASF268:
	.ascii	"SDL_SCANCODE_RSHIFT\000"
.LASF24:
	.ascii	"refcount\000"
.LASF283:
	.ascii	"SDL_SCANCODE_AC_HOME\000"
.LASF394:
	.ascii	"TAN_bin\000"
.LASF386:
	.ascii	"nameCount\000"
.LASF272:
	.ascii	"SDL_SCANCODE_AUDIONEXT\000"
.LASF10:
	.ascii	"long long unsigned int\000"
.LASF342:
	.ascii	"SDL_MouseButtonEvent\000"
.LASF27:
	.ascii	"SDL_PaletteWatch\000"
.LASF263:
	.ascii	"SDL_SCANCODE_LCTRL\000"
.LASF31:
	.ascii	"SDL_PixelFormat\000"
.LASF249:
	.ascii	"SDL_SCANCODE_KP_MEMSTORE\000"
.LASF221:
	.ascii	"SDL_SCANCODE_CURRENCYUNIT\000"
.LASF98:
	.ascii	"SDL_SCANCODE_TAB\000"
.LASF42:
	.ascii	"Ashift\000"
.LASF280:
	.ascii	"SDL_SCANCODE_CALCULATOR\000"
.LASF176:
	.ascii	"SDL_SCANCODE_AGAIN\000"
.LASF26:
	.ascii	"SDL_PaletteChangedFunc\000"
.LASF155:
	.ascii	"SDL_SCANCODE_NONUSBACKSLASH\000"
.LASF128:
	.ascii	"SDL_SCANCODE_INSERT\000"
.LASF178:
	.ascii	"SDL_SCANCODE_CUT\000"
.LASF266:
	.ascii	"SDL_SCANCODE_LGUI\000"
.LASF102:
	.ascii	"SDL_SCANCODE_LEFTBRACKET\000"
.LASF347:
	.ascii	"value\000"
.LASF188:
	.ascii	"SDL_SCANCODE_INTERNATIONAL2\000"
.LASF189:
	.ascii	"SDL_SCANCODE_INTERNATIONAL3\000"
.LASF190:
	.ascii	"SDL_SCANCODE_INTERNATIONAL4\000"
.LASF191:
	.ascii	"SDL_SCANCODE_INTERNATIONAL5\000"
.LASF192:
	.ascii	"SDL_SCANCODE_INTERNATIONAL6\000"
.LASF193:
	.ascii	"SDL_SCANCODE_INTERNATIONAL7\000"
.LASF194:
	.ascii	"SDL_SCANCODE_INTERNATIONAL8\000"
.LASF195:
	.ascii	"SDL_SCANCODE_INTERNATIONAL9\000"
.LASF156:
	.ascii	"SDL_SCANCODE_APPLICATION\000"
.LASF273:
	.ascii	"SDL_SCANCODE_AUDIOPREV\000"
.LASF104:
	.ascii	"SDL_SCANCODE_BACKSLASH\000"
.LASF23:
	.ascii	"colors\000"
.LASF46:
	.ascii	"Amask\000"
.LASF375:
	.ascii	"GL_PROJECTION\000"
.LASF55:
	.ascii	"clip_rect\000"
.LASF228:
	.ascii	"SDL_SCANCODE_KP_BACKSPACE\000"
.LASF214:
	.ascii	"SDL_SCANCODE_CLEARAGAIN\000"
.LASF353:
	.ascii	"SDL_UserEvent\000"
.LASF265:
	.ascii	"SDL_SCANCODE_LALT\000"
.LASF105:
	.ascii	"SDL_SCANCODE_NONUSHASH\000"
.LASF319:
	.ascii	"SDL_JOYBUTTONUP\000"
.LASF343:
	.ascii	"button\000"
.LASF220:
	.ascii	"SDL_SCANCODE_DECIMALSEPARATOR\000"
.LASF238:
	.ascii	"SDL_SCANCODE_KP_LESS\000"
.LASF139:
	.ascii	"SDL_SCANCODE_KP_DIVIDE\000"
.LASF316:
	.ascii	"SDL_JOYBALLMOTION\000"
.LASF247:
	.ascii	"SDL_SCANCODE_KP_AT\000"
.LASF281:
	.ascii	"SDL_SCANCODE_COMPUTER\000"
.LASF297:
	.ascii	"SDL_NUM_SCANCODES\000"
.LASF320:
	.ascii	"SDL_QUIT\000"
.LASF108:
	.ascii	"SDL_SCANCODE_GRAVE\000"
.LASF260:
	.ascii	"SDL_SCANCODE_KP_OCTAL\000"
.LASF183:
	.ascii	"SDL_SCANCODE_VOLUMEUP\000"
.LASF35:
	.ascii	"Rloss\000"
.LASF354:
	.ascii	"code\000"
.LASF345:
	.ascii	"SDL_JoyAxisEvent\000"
.LASF205:
	.ascii	"SDL_SCANCODE_ALTERASE\000"
.LASF149:
	.ascii	"SDL_SCANCODE_KP_6\000"
.LASF371:
	.ascii	"resize\000"
.LASF43:
	.ascii	"Rmask\000"
.LASF262:
	.ascii	"SDL_SCANCODE_KP_HEXADECIMAL\000"
.LASF299:
	.ascii	"SDLKey\000"
.LASF384:
	.ascii	"nextBlock\000"
.LASF20:
	.ascii	"SDL_Color\000"
.LASF292:
	.ascii	"SDL_SCANCODE_KBDILLUMTOGGLE\000"
.LASF236:
	.ascii	"SDL_SCANCODE_KP_POWER\000"
.LASF13:
	.ascii	"char\000"
.LASF44:
	.ascii	"Gmask\000"
.LASF392:
	.ascii	"COS_bin\000"
.LASF395:
	.ascii	"GNU C 4.3.0\000"
.LASF391:
	.ascii	"glGlob\000"
.LASF380:
	.ascii	"matrixMode\000"
.LASF180:
	.ascii	"SDL_SCANCODE_PASTE\000"
.LASF338:
	.ascii	"text\000"
.LASF89:
	.ascii	"SDL_SCANCODE_5\000"
.LASF321:
	.ascii	"SDL_SYSWMEVENT\000"
.LASF255:
	.ascii	"SDL_SCANCODE_KP_MEMDIVIDE\000"
.LASF9:
	.ascii	"long long int\000"
.LASF254:
	.ascii	"SDL_SCANCODE_KP_MEMMULTIPLY\000"
.LASF365:
	.ascii	"jhat\000"
.LASF3:
	.ascii	"uint8_t\000"
.LASF47:
	.ascii	"SDL_Rect\000"
.LASF53:
	.ascii	"locked\000"
.LASF393:
	.ascii	"SIN_bin\000"
.LASF60:
	.ascii	"SDL_SCANCODE_B\000"
.LASF358:
	.ascii	"gain\000"
.LASF325:
	.ascii	"SDL_USEREVENT\000"
.LASF141:
	.ascii	"SDL_SCANCODE_KP_MINUS\000"
.LASF383:
	.ascii	"activeTexture\000"
.LASF143:
	.ascii	"SDL_SCANCODE_KP_ENTER\000"
.LASF65:
	.ascii	"SDL_SCANCODE_G\000"
.LASF185:
	.ascii	"SDL_SCANCODE_KP_COMMA\000"
.LASF132:
	.ascii	"SDL_SCANCODE_END\000"
.LASF110:
	.ascii	"SDL_SCANCODE_PERIOD\000"
.LASF106:
	.ascii	"SDL_SCANCODE_SEMICOLON\000"
.LASF381:
	.ascii	"clearColor\000"
.LASF39:
	.ascii	"Rshift\000"
.LASF274:
	.ascii	"SDL_SCANCODE_AUDIOSTOP\000"
.LASF242:
	.ascii	"SDL_SCANCODE_KP_VERTICALBAR\000"
.LASF103:
	.ascii	"SDL_SCANCODE_RIGHTBRACKET\000"
.LASF330:
	.ascii	"data1\000"
.LASF331:
	.ascii	"data2\000"
.LASF356:
	.ascii	"SDL_SysWMEvent\000"
.LASF304:
	.ascii	"SDL_BlitMap\000"
.LASF76:
	.ascii	"SDL_SCANCODE_R\000"
.LASF336:
	.ascii	"keysym\000"
.LASF366:
	.ascii	"jbutton\000"
.LASF302:
	.ascii	"unicode\000"
.LASF360:
	.ascii	"window\000"
.LASF285:
	.ascii	"SDL_SCANCODE_AC_FORWARD\000"
.LASF131:
	.ascii	"SDL_SCANCODE_DELETE\000"
.LASF135:
	.ascii	"SDL_SCANCODE_LEFT\000"
.LASF111:
	.ascii	"SDL_SCANCODE_SLASH\000"
.LASF240:
	.ascii	"SDL_SCANCODE_KP_AMPERSAND\000"
.LASF239:
	.ascii	"SDL_SCANCODE_KP_GREATER\000"
.LASF282:
	.ascii	"SDL_SCANCODE_AC_SEARCH\000"
.LASF315:
	.ascii	"SDL_JOYAXISMOTION\000"
.LASF125:
	.ascii	"SDL_SCANCODE_PRINTSCREEN\000"
.LASF216:
	.ascii	"SDL_SCANCODE_EXSEL\000"
.LASF219:
	.ascii	"SDL_SCANCODE_THOUSANDSSEPARATOR\000"
.LASF373:
	.ascii	"uint32\000"
.LASF363:
	.ascii	"jaxis\000"
.LASF29:
	.ascii	"userdata\000"
.LASF171:
	.ascii	"SDL_SCANCODE_EXECUTE\000"
.LASF294:
	.ascii	"SDL_SCANCODE_KBDILLUMUP\000"
.LASF352:
	.ascii	"SDL_QuitEvent\000"
.LASF301:
	.ascii	"scancode\000"
.LASF12:
	.ascii	"long unsigned int\000"
.LASF21:
	.ascii	"SDL_Palette\000"
.LASF382:
	.ascii	"textures\000"
.LASF351:
	.ascii	"SDL_JoyButtonEvent\000"
.LASF368:
	.ascii	"user\000"
.LASF40:
	.ascii	"Gshift\000"
.LASF227:
	.ascii	"SDL_SCANCODE_KP_TAB\000"
.LASF256:
	.ascii	"SDL_SCANCODE_KP_PLUSMINUS\000"
.LASF288:
	.ascii	"SDL_SCANCODE_AC_BOOKMARKS\000"
.LASF58:
	.ascii	"SDL_SCANCODE_UNKNOWN\000"
.LASF334:
	.ascii	"which\000"
.LASF370:
	.ascii	"active\000"
.LASF5:
	.ascii	"short unsigned int\000"
.LASF341:
	.ascii	"yrel\000"
.LASF173:
	.ascii	"SDL_SCANCODE_MENU\000"
.LASF129:
	.ascii	"SDL_SCANCODE_HOME\000"
.LASF126:
	.ascii	"SDL_SCANCODE_SCROLLLOCK\000"
.LASF4:
	.ascii	"uint16_t\000"
.LASF250:
	.ascii	"SDL_SCANCODE_KP_MEMRECALL\000"
.LASF396:
	.ascii	"/home/lifning/hack/nds-test/source/main.c\000"
.LASF145:
	.ascii	"SDL_SCANCODE_KP_2\000"
.LASF340:
	.ascii	"xrel\000"
.LASF146:
	.ascii	"SDL_SCANCODE_KP_3\000"
.LASF130:
	.ascii	"SDL_SCANCODE_PAGEUP\000"
.LASF94:
	.ascii	"SDL_SCANCODE_0\000"
.LASF85:
	.ascii	"SDL_SCANCODE_1\000"
.LASF86:
	.ascii	"SDL_SCANCODE_2\000"
.LASF87:
	.ascii	"SDL_SCANCODE_3\000"
.LASF88:
	.ascii	"SDL_SCANCODE_4\000"
.LASF1:
	.ascii	"unsigned char\000"
.LASF90:
	.ascii	"SDL_SCANCODE_6\000"
.LASF207:
	.ascii	"SDL_SCANCODE_CANCEL\000"
.LASF92:
	.ascii	"SDL_SCANCODE_8\000"
.LASF93:
	.ascii	"SDL_SCANCODE_9\000"
.LASF175:
	.ascii	"SDL_SCANCODE_STOP\000"
.LASF59:
	.ascii	"SDL_SCANCODE_A\000"
.LASF184:
	.ascii	"SDL_SCANCODE_VOLUMEDOWN\000"
.LASF61:
	.ascii	"SDL_SCANCODE_C\000"
.LASF62:
	.ascii	"SDL_SCANCODE_D\000"
.LASF63:
	.ascii	"SDL_SCANCODE_E\000"
.LASF64:
	.ascii	"SDL_SCANCODE_F\000"
.LASF174:
	.ascii	"SDL_SCANCODE_SELECT\000"
.LASF66:
	.ascii	"SDL_SCANCODE_H\000"
.LASF67:
	.ascii	"SDL_SCANCODE_I\000"
.LASF68:
	.ascii	"SDL_SCANCODE_J\000"
.LASF69:
	.ascii	"SDL_SCANCODE_K\000"
.LASF70:
	.ascii	"SDL_SCANCODE_L\000"
.LASF71:
	.ascii	"SDL_SCANCODE_M\000"
.LASF72:
	.ascii	"SDL_SCANCODE_N\000"
.LASF73:
	.ascii	"SDL_SCANCODE_O\000"
.LASF74:
	.ascii	"SDL_SCANCODE_P\000"
.LASF75:
	.ascii	"SDL_SCANCODE_Q\000"
.LASF222:
	.ascii	"SDL_SCANCODE_CURRENCYSUBUNIT\000"
.LASF77:
	.ascii	"SDL_SCANCODE_S\000"
.LASF78:
	.ascii	"SDL_SCANCODE_T\000"
.LASF79:
	.ascii	"SDL_SCANCODE_U\000"
.LASF80:
	.ascii	"SDL_SCANCODE_V\000"
.LASF81:
	.ascii	"SDL_SCANCODE_W\000"
.LASF82:
	.ascii	"SDL_SCANCODE_X\000"
.LASF83:
	.ascii	"SDL_SCANCODE_Y\000"
.LASF84:
	.ascii	"SDL_SCANCODE_Z\000"
.LASF318:
	.ascii	"SDL_JOYBUTTONDOWN\000"
.LASF279:
	.ascii	"SDL_SCANCODE_MAIL\000"
.LASF378:
	.ascii	"GL_TEXTURE\000"
.LASF182:
	.ascii	"SDL_SCANCODE_MUTE\000"
.LASF377:
	.ascii	"GL_MODELVIEW\000"
.LASF99:
	.ascii	"SDL_SCANCODE_SPACE\000"
.LASF37:
	.ascii	"Bloss\000"
.LASF357:
	.ascii	"SDL_ActiveEvent\000"
.LASF97:
	.ascii	"SDL_SCANCODE_BACKSPACE\000"
.LASF306:
	.ascii	"SDL_NOEVENT\000"
.LASF276:
	.ascii	"SDL_SCANCODE_AUDIOMUTE\000"
.LASF310:
	.ascii	"SDL_TEXTINPUT\000"
.LASF34:
	.ascii	"BytesPerPixel\000"
.LASF253:
	.ascii	"SDL_SCANCODE_KP_MEMSUBTRACT\000"
.LASF314:
	.ascii	"SDL_MOUSEWHEEL\000"
.LASF270:
	.ascii	"SDL_SCANCODE_RGUI\000"
.LASF8:
	.ascii	"unsigned int\000"
.LASF187:
	.ascii	"SDL_SCANCODE_INTERNATIONAL1\000"
.LASF95:
	.ascii	"SDL_SCANCODE_RETURN\000"
.LASF172:
	.ascii	"SDL_SCANCODE_HELP\000"
.LASF349:
	.ascii	"ball\000"
.LASF333:
	.ascii	"SDL_KeyboardEvent\000"
.LASF361:
	.ascii	"motion\000"
.LASF387:
	.ascii	"gl_hidden_globals\000"
.LASF348:
	.ascii	"SDL_JoyBallEvent\000"
.LASF215:
	.ascii	"SDL_SCANCODE_CRSEL\000"
.LASF269:
	.ascii	"SDL_SCANCODE_RALT\000"
.LASF41:
	.ascii	"Bshift\000"
.LASF19:
	.ascii	"unused\000"
.LASF374:
	.ascii	"float\000"
.LASF257:
	.ascii	"SDL_SCANCODE_KP_CLEAR\000"
.LASF6:
	.ascii	"int32_t\000"
.LASF264:
	.ascii	"SDL_SCANCODE_LSHIFT\000"
.LASF252:
	.ascii	"SDL_SCANCODE_KP_MEMADD\000"
.LASF48:
	.ascii	"SDL_Surface\000"
.LASF134:
	.ascii	"SDL_SCANCODE_RIGHT\000"
.LASF206:
	.ascii	"SDL_SCANCODE_SYSREQ\000"
.LASF308:
	.ascii	"SDL_KEYDOWN\000"
.LASF322:
	.ascii	"SDL_EVENT_RESERVED1\000"
.LASF323:
	.ascii	"SDL_EVENT_RESERVED2\000"
.LASF324:
	.ascii	"SDL_EVENT_RESERVED3\000"
.LASF350:
	.ascii	"SDL_JoyHatEvent\000"
.LASF328:
	.ascii	"type\000"
.LASF113:
	.ascii	"SDL_SCANCODE_F1\000"
.LASF114:
	.ascii	"SDL_SCANCODE_F2\000"
.LASF115:
	.ascii	"SDL_SCANCODE_F3\000"
.LASF116:
	.ascii	"SDL_SCANCODE_F4\000"
.LASF117:
	.ascii	"SDL_SCANCODE_F5\000"
.LASF118:
	.ascii	"SDL_SCANCODE_F6\000"
.LASF119:
	.ascii	"SDL_SCANCODE_F7\000"
.LASF120:
	.ascii	"SDL_SCANCODE_F8\000"
.LASF121:
	.ascii	"SDL_SCANCODE_F9\000"
.LASF379:
	.ascii	"GL_MATRIX_MODE_ENUM\000"
.LASF124:
	.ascii	"SDL_SCANCODE_F12\000"
.LASF137:
	.ascii	"SDL_SCANCODE_UP\000"
.LASF337:
	.ascii	"SDL_TextInputEvent\000"
.LASF290:
	.ascii	"SDL_SCANCODE_BRIGHTNESSUP\000"
.LASF210:
	.ascii	"SDL_SCANCODE_RETURN2\000"
.LASF165:
	.ascii	"SDL_SCANCODE_F19\000"
.LASF57:
	.ascii	"SDL_WindowID\000"
.LASF107:
	.ascii	"SDL_SCANCODE_APOSTROPHE\000"
.LASF335:
	.ascii	"state\000"
.LASF28:
	.ascii	"callback\000"
.LASF158:
	.ascii	"SDL_SCANCODE_KP_EQUALS\000"
.LASF339:
	.ascii	"SDL_MouseMotionEvent\000"
.LASF122:
	.ascii	"SDL_SCANCODE_F10\000"
.LASF225:
	.ascii	"SDL_SCANCODE_KP_LEFTBRACE\000"
.LASF372:
	.ascii	"SDL_Event\000"
.LASF344:
	.ascii	"SDL_MouseWheelEvent\000"
.LASF311:
	.ascii	"SDL_MOUSEMOTION\000"
.LASF56:
	.ascii	"format_version\000"
.LASF390:
	.ascii	"rect\000"
.LASF0:
	.ascii	"signed char\000"
.LASF211:
	.ascii	"SDL_SCANCODE_SEPARATOR\000"
.LASF248:
	.ascii	"SDL_SCANCODE_KP_EXCLAM\000"
.LASF153:
	.ascii	"SDL_SCANCODE_KP_0\000"
.LASF144:
	.ascii	"SDL_SCANCODE_KP_1\000"
.LASF277:
	.ascii	"SDL_SCANCODE_MEDIASELECT\000"
.LASF212:
	.ascii	"SDL_SCANCODE_OUT\000"
.LASF147:
	.ascii	"SDL_SCANCODE_KP_4\000"
.LASF148:
	.ascii	"SDL_SCANCODE_KP_5\000"
.LASF271:
	.ascii	"SDL_SCANCODE_MODE\000"
.LASF150:
	.ascii	"SDL_SCANCODE_KP_7\000"
.LASF151:
	.ascii	"SDL_SCANCODE_KP_8\000"
.LASF152:
	.ascii	"SDL_SCANCODE_KP_9\000"
.LASF398:
	.ascii	"main\000"
.LASF258:
	.ascii	"SDL_SCANCODE_KP_CLEARENTRY\000"
.LASF303:
	.ascii	"SDL_Joystick\000"
.LASF229:
	.ascii	"SDL_SCANCODE_KP_A\000"
.LASF230:
	.ascii	"SDL_SCANCODE_KP_B\000"
.LASF231:
	.ascii	"SDL_SCANCODE_KP_C\000"
.LASF232:
	.ascii	"SDL_SCANCODE_KP_D\000"
.LASF233:
	.ascii	"SDL_SCANCODE_KP_E\000"
.LASF389:
	.ascii	"stick\000"
.LASF25:
	.ascii	"watch\000"
.LASF18:
	.ascii	"double\000"
.LASF138:
	.ascii	"SDL_SCANCODE_NUMLOCKCLEAR\000"
.LASF123:
	.ascii	"SDL_SCANCODE_F11\000"
.LASF209:
	.ascii	"SDL_SCANCODE_PRIOR\000"
.LASF159:
	.ascii	"SDL_SCANCODE_F13\000"
.LASF160:
	.ascii	"SDL_SCANCODE_F14\000"
.LASF161:
	.ascii	"SDL_SCANCODE_F15\000"
.LASF162:
	.ascii	"SDL_SCANCODE_F16\000"
.LASF163:
	.ascii	"SDL_SCANCODE_F17\000"
.LASF164:
	.ascii	"SDL_SCANCODE_F18\000"
.LASF179:
	.ascii	"SDL_SCANCODE_COPY\000"
.LASF101:
	.ascii	"SDL_SCANCODE_EQUALS\000"
.LASF326:
	.ascii	"SDL_NUMEVENTS\000"
.LASF284:
	.ascii	"SDL_SCANCODE_AC_BACK\000"
.LASF196:
	.ascii	"SDL_SCANCODE_LANG1\000"
.LASF197:
	.ascii	"SDL_SCANCODE_LANG2\000"
.LASF198:
	.ascii	"SDL_SCANCODE_LANG3\000"
.LASF199:
	.ascii	"SDL_SCANCODE_LANG4\000"
.LASF200:
	.ascii	"SDL_SCANCODE_LANG5\000"
.LASF201:
	.ascii	"SDL_SCANCODE_LANG6\000"
.LASF202:
	.ascii	"SDL_SCANCODE_LANG7\000"
.LASF203:
	.ascii	"SDL_SCANCODE_LANG8\000"
.LASF204:
	.ascii	"SDL_SCANCODE_LANG9\000"
.LASF140:
	.ascii	"SDL_SCANCODE_KP_MULTIPLY\000"
.LASF376:
	.ascii	"GL_POSITION\000"
.LASF307:
	.ascii	"SDL_WINDOWEVENT\000"
.LASF136:
	.ascii	"SDL_SCANCODE_DOWN\000"
.LASF154:
	.ascii	"SDL_SCANCODE_KP_PERIOD\000"
.LASF237:
	.ascii	"SDL_SCANCODE_KP_PERCENT\000"
.LASF224:
	.ascii	"SDL_SCANCODE_KP_RIGHTPAREN\000"
	.ident	"GCC: (devkitARM release 23b) 4.3.0"

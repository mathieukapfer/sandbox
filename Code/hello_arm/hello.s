	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.thumb
	.file	"hello.cpp"
	.section	.text._ZN4TotoC2Ei,"axG",%progbits,_ZN4TotoC5Ei,comdat
	.align	2
	.weak	_ZN4TotoC2Ei
	.thumb
	.thumb_func
	.type	_ZN4TotoC2Ei, %function
_ZN4TotoC2Ei:
	.fnstart
.LFB1:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	sub	sp, sp, #12
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7]
	ldr	r3, [r7, #4]
	ldr	r2, [r7]
	str	r2, [r3]
	ldr	r3, [r7, #4]
	mov	r0, r3
	adds	r7, r7, #12
	mov	sp, r7
	@ sp needed
	ldr	r7, [sp], #4
	bx	lr
	.cantunwind
	.fnend
	.size	_ZN4TotoC2Ei, .-_ZN4TotoC2Ei
	.weak	_ZN4TotoC1Ei
	.thumb_set _ZN4TotoC1Ei,_ZN4TotoC2Ei
	.section	.text._ZN4TotoD2Ev,"axG",%progbits,_ZN4TotoD5Ev,comdat
	.align	2
	.weak	_ZN4TotoD2Ev
	.thumb
	.thumb_func
	.type	_ZN4TotoD2Ev, %function
_ZN4TotoD2Ev:
	.fnstart
.LFB4:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	push	{r7}
	sub	sp, sp, #12
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r3, [r7, #4]
	mov	r0, r3
	adds	r7, r7, #12
	mov	sp, r7
	@ sp needed
	ldr	r7, [sp], #4
	bx	lr
	.cantunwind
	.fnend
	.size	_ZN4TotoD2Ev, .-_ZN4TotoD2Ev
	.weak	_ZN4TotoD1Ev
	.thumb_set _ZN4TotoD1Ev,_ZN4TotoD2Ev
	.bss
	.align	2
_ZL17global_static_obj:
	.space	4100
	.align	2
_ZL18global_static_obj2:
	.space	4100
	.global	global_obj
	.align	2
	.type	global_obj, %object
	.size	global_obj, 4100
global_obj:
	.space	4100
	.data
	.align	2
	.type	_ZL10static_int, %object
	.size	_ZL10static_int, 4
_ZL10static_int:
	.word	43981
	.bss
	.align	2
_ZL24static_int_uninitialized:
	.space	4
	.global	global_int_uninitialized
	.align	2
	.type	global_int_uninitialized, %object
	.size	global_int_uninitialized, 4
global_int_uninitialized:
	.space	4
	.global	global_int
	.data
	.align	2
	.type	global_int, %object
	.size	global_int, 4
global_int:
	.word	4660
	.bss
	.align	2
_ZGVZ4mainE17local_static_obj2:
	.space	4
	.text
	.align	2
	.global	main
	.thumb
	.thumb_func
	.type	main, %function
main:
	.fnstart
.LFB6:
	@ args = 0, pretend = 0, frame = 4112
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r7, lr}
	sub	sp, sp, #4096
	sub	sp, sp, #20
	add	r7, sp, #0
	add	r3, r7, #16
	subs	r3, r3, #12
	str	r0, [r3]
	add	r3, r7, #16
	subs	r3, r3, #16
	str	r1, [r3]
	ldr	r3, .L10
	ldr	r3, [r3]
	and	r3, r3, #1
	cmp	r3, #0
	bne	.L8
	ldr	r0, .L10
	bl	__cxa_guard_acquire
	mov	r3, r0
	cmp	r3, #0
	ite	eq
	moveq	r3, #0
	movne	r3, #1
	uxtb	r3, r3
	cmp	r3, #0
	beq	.L8
	ldr	r0, .L10+4
	movw	r1, #65226
	bl	_ZN4TotoC1Ei
	ldr	r0, .L10
	bl	__cxa_guard_release
	ldr	r0, .L10+4
	ldr	r1, .L10+8
	ldr	r2, .L10+12
	bl	__aeabi_atexit
.L8:
	add	r3, r7, #16
	subs	r3, r3, #4
	mov	r0, r3
	movs	r1, #0
	bl	_ZN4TotoC1Ei
	movs	r4, #0
	add	r3, r7, #16
	subs	r3, r3, #4
	mov	r0, r3
	bl	_ZN4TotoD1Ev
	mov	r3, r4
	mov	r0, r3
	add	r7, r7, #4096
	adds	r7, r7, #20
	mov	sp, r7
	@ sp needed
	pop	{r4, r7, pc}
.L11:
	.align	2
.L10:
	.word	_ZGVZ4mainE17local_static_obj2
	.word	_ZZ4mainE17local_static_obj2
	.word	_ZN4TotoD1Ev
	.word	__dso_handle
	.cantunwind
	.fnend
	.size	main, .-main
	.align	2
	.thumb
	.thumb_func
	.type	_Z41__static_initialization_and_destruction_0ii, %function
_Z41__static_initialization_and_destruction_0ii:
	.fnstart
.LFB7:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7]
	ldr	r3, [r7, #4]
	cmp	r3, #1
	bne	.L12
	ldr	r2, [r7]
	movw	r3, #65535
	cmp	r2, r3
	bne	.L12
	ldr	r0, .L14
	movw	r1, #51966
	bl	_ZN4TotoC1Ei
	ldr	r0, .L14
	ldr	r1, .L14+4
	ldr	r2, .L14+8
	bl	__aeabi_atexit
	ldr	r0, .L14+12
	movw	r1, #65226
	bl	_ZN4TotoC1Ei
	ldr	r0, .L14+12
	ldr	r1, .L14+4
	ldr	r2, .L14+8
	bl	__aeabi_atexit
	ldr	r0, .L14+16
	movw	r1, #4660
	bl	_ZN4TotoC1Ei
	ldr	r0, .L14+16
	ldr	r1, .L14+4
	ldr	r2, .L14+8
	bl	__aeabi_atexit
.L12:
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L15:
	.align	2
.L14:
	.word	_ZL17global_static_obj
	.word	_ZN4TotoD1Ev
	.word	__dso_handle
	.word	_ZL18global_static_obj2
	.word	global_obj
	.cantunwind
	.fnend
	.size	_Z41__static_initialization_and_destruction_0ii, .-_Z41__static_initialization_and_destruction_0ii
	.section	.rodata
	.align	2
	.type	_ZL3msg, %object
	.size	_ZL3msg, 25
_ZL3msg:
	.ascii	"this is a rodata section\000"
	.text
	.align	2
	.thumb
	.thumb_func
	.type	_GLOBAL__sub_I_global_obj, %function
_GLOBAL__sub_I_global_obj:
	.fnstart
.LFB8:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	add	r7, sp, #0
	movs	r0, #1
	movw	r1, #65535
	bl	_Z41__static_initialization_and_destruction_0ii
	pop	{r7, pc}
	.cantunwind
	.fnend
	.size	_GLOBAL__sub_I_global_obj, .-_GLOBAL__sub_I_global_obj
	.section	.init_array,"aw",%init_array
	.align	2
	.word	_GLOBAL__sub_I_global_obj(target1)
	.bss
	.align	2
_ZZ4mainE17local_static_obj2:
	.space	4100
	.hidden	__dso_handle
	.ident	"GCC: (4.8.4-1+11-1) 4.8.4 20141219 (release)"

#
#   Copyright (C) 2023 Albin Ahlbäck
#
#   This file is part of FLINT.
#
#   FLINT is free software: you can redistribute it and/or modify it under
#   the terms of the GNU Lesser General Public License (LGPL) as published
#   by the Free Software Foundation; either version 2.1 of the License, or
#   (at your option) any later version.  See <https://www.gnu.org/licenses/>.
#

include(`config.m4')dnl
dnl
.text

.macro	m12 res=%rdi, res_offset=0, ap=%rsi, ap_offset=0, r0, r1, r2, r3, r4, r5
	mulx	(0+\ap_offset)*8(\ap), \r0, \r3
	mulx	(1+\ap_offset)*8(\ap), \r1, \r5
	mulx	(2+\ap_offset)*8(\ap), \r2, \r4
	add	\r3, \r1
	adc	\r5, \r2
	mov	\r0, (0+\res_offset)*8(\res)
	mov	\r1, (1+\res_offset)*8(\res)
	mov	\r2, (2+\res_offset)*8(\res)
	mulx	(3+\ap_offset)*8(\ap), \r3, \r5
	mulx	(4+\ap_offset)*8(\ap), \r0, \r1
	adc	\r3, \r4
	adc	\r5, \r0
	mov	\r4, (3+\res_offset)*8(\res)
	mov	\r0, (4+\res_offset)*8(\res)
	mulx	(5+\ap_offset)*8(\ap), \r3, \r5
	mulx	(6+\ap_offset)*8(\ap), \r2, \r4
	adc	\r3, \r1
	adc	\r5, \r2
	mov	\r1, (5+\res_offset)*8(\res)
	mov	\r2, (6+\res_offset)*8(\res)
	mulx	(7+\ap_offset)*8(\ap), \r3, \r5
	mulx	(8+\ap_offset)*8(\ap), \r0, \r1
	adc	\r3, \r4
	adc	\r5, \r0
	mov	\r4, (7+\res_offset)*8(\res)
	mov	\r0, (8+\res_offset)*8(\res)
	mulx	(9+\ap_offset)*8(\ap), \r3, \r5
	mulx	(10+\ap_offset)*8(\ap), \r2, \r4
	adc	\r3, \r1
	adc	\r5, \r2
	mov	\r1, (9+\res_offset)*8(\res)
	mov	\r2, (10+\res_offset)*8(\res)
	mulx	(11+\ap_offset)*8(\ap), \r3, \r5
	adc	\r3, \r4
	adc	$0, \r5
	mov	\r4, (11+\res_offset)*8(\res)
.endm
.global	FUNC(flint_mpn_mul_12_1)
.p2align	4, 0x90
TYPE(flint_mpn_mul_12_1)

FUNC(flint_mpn_mul_12_1):
	.cfi_startproc
	mov	0*8(%rdx), %rdx
	m12	%rdi, 0, %rsi, 0, %rcx, %r8, %r9, %r10, %r11, %rax
	mov	%rax, 12*8(%rdi)

	ret
.flint_mpn_mul_12_1_end:
SIZE(flint_mpn_mul_12_1, .flint_mpn_mul_12_1_end)
.cfi_endproc
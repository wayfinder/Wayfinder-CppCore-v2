/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Fast math functions.
 * Some of these come from the header file ArchFastMath.h
 */

/** Used for inlining magic in ArchFastMath.h. */ 
#define SHARED_ArchFastMath_C

#include "arch.h"

namespace isab {


/*+ Define if the input parameter to make sqrt_int_fast() take
 *  an uint32 instead of a int32. This changes some scaling etc
 *  code in the function itself. Undefining this symbol makes
 *  sqrt_int_fast() a bit faster. +*/
#define INPUT_IS_UINT




asm("\n"
"	.section .text									\n"
"	.align 1									\n"
"	.global _sqrt_int_fast								\n"
"_sqrt_int_fast:									\n"
"	push.l	er6		; Standard function entry - only needed for debugging   \n"
"	mov.l	er7,er6		; (Since we do not use the stack )			\n"
"											\n"
"	; Our input argument, R, is in er0						\n"
"											\n"
#ifdef INPUT_IS_UINT
"	; We need a spare register to remember if we have scaled - push r3		\n"
"	push.w	r3									\n"
"	sub.b	r3l,r3l									\n"
"											\n"
"	; Check if R>=0x80000000							\n"
"	mov.l	er0,er0									\n"
"	bpl	sqrt_int_nonneg:8							\n"
"	; Scale down by 1/4								\n"
"	shlr.l	er0									\n"
"	shlr.l	er0									\n"
"	mov.b	#1,r3l		; Remeber the scaling					\n"
"	; Fall through									\n"
#else
"	; Check if R<0									\n"
"	mov.l	er0,er0									\n"
"	bpl	sqrt_int_nonneg:8							\n"
"	sub.w	r0,r0									\n"
"	pop.l	er6									\n"
"	rts										\n"
#endif
"											\n"
"sqrt_int_nonneg:									\n"
"	; Check if R>0x00010000								\n"
"	mov.w	e0,e0									\n"
"	bne	sqrt_int_high:8								\n"
"											\n"
"	; R is small, start at 0xff							\n"
"	mov.w	#0x00ff,r1								\n"
"	bra	sqrt_got_init_guess:8							\n"
"											\n"
"sqrt_int_high:										\n"
"	mov.w	e0,e2									\n"
"	mov.w	#0xffff,r1								\n"
"	bra	sqrt_int_high_loop_test:8						\n"
"											\n"
"sqrt_int_high_loop:									\n"
"	shll.w	e2									\n"
"	shll.w	e2									\n"
"	shlr.w	r1									\n"
"sqrt_int_high_loop_test:								\n"
"	mov.w	e2,r2		; See if any of the two MSB are set			\n"
"	and.b	#0xc0,r2h								\n"
"	beq	sqrt_int_high_loop:8							\n"
"											\n"
"sqrt_got_init_guess:									\n"
"	extu.l	er1		; Allow 32-bit add later on				\n"
"sqrt_loop:										\n"
"	; When we get here r1 holds the guess and er0 the input.			\n"
"	; e1 is always zero and used only to allow fast 32 bit				\n"
"	; add instead of 16 bit add+add with carry.					\n"
"	; Safety check that the divide wont overflow. Im 				\n"
"        ; convinced this cannot happen due to the following:				\n"
"	;   * A too large approximation never changes to a too				\n"
"        ;     small approximation except due to truncation and 			\n"
"	;     is then at most one too small, The worst case,				\n"
"	;     (2^31-1)/0xfffe=32769 is still within 65535.				\n"
"	;   * A too large approximation is suceeded by a more				\n"
"	;     correct approximation, i.e. a smaller one.				\n"
"	; Still, the test was in the original intel paper so				\n"
"        ; let us leave it in for the moment.						\n"
"											\n"
"	cmp.w	r1,e0									\n"
"	bhs	sqrt_done:8								\n"
"											\n"
"	mov.l	er0,er2									\n"
"	divxu	r1,er2		; r2=R/Xold						\n"
"											\n"
"	cmp.w	r1,r2		; If R/Xold>=Xold					\n"
"	bhs	sqrt_done:8	;   done						\n"
"											\n"
"	extu.l	er2									\n"
"	add.l	er2,er1		; er1=R/Xold+Xold					\n"
"	shlr	er1		; er1=(R/Xold+Xold)/2					\n"
"											\n"
"	bra	sqrt_loop:8								\n"
"											\n"
"sqrt_done:										\n"
#ifdef INPUT_IS_UINT
"	mov.b	r3l,r3l		; See if we prescaled					\n"
"	beq	sqrt_no_postscale							\n"
"	shll.w	r1									\n"
"sqrt_no_postscale:									\n"
"	pop.w	r3									\n"
#endif
"	mov.w	r1,r0									\n"
"	pop.l	er6									\n"
"	rts										\n"
);


};



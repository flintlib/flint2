/*
    Copyright (C) 2009 William Hart
    Copyright (C) 2022 Albin Ahlbäck

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#ifndef FLINT_H
#define FLINT_H

#ifdef FLINT_INLINES_C
#define FLINT_INLINE FLINT_DLL
#else
#define FLINT_INLINE static __inline__
#endif

/* include GMP ****************************************************************/

#undef ulong
#define ulong ulongxx /* ensure vendor doesn't typedef ulong */
#include "gmp.h"
#undef ulong

#if __GNU_MP_VERSION < 5
#error GMP 5.0.0 or MPIR 2.6.0 or later are required
#endif

/* handle number of bits ******************************************************/

#if GMP_LIMB_BITS == 64
#define FLINT_BITS 64
#define FLINT_D_BITS 53
#define FLINT64 1
#else
#define FLINT_BITS 32
#define FLINT_D_BITS 31
#endif

/* keywords *******************************************************************/

#ifndef __GNUC__
#define __asm__     asm
#define __inline__  inline
#endif

#if FLINT_USES_TLS
#if defined(__GNUC__) && __STDC_VERSION__ >= 201112L && __GNUC__ == 4 && __GNUC_MINOR__ < 9
/* GCC 4.7, 4.8 with -std=gnu11 purport to support C11 via __STDC_VERSION__ but lack _Thread_local */
#define FLINT_TLS_PREFIX __thread
#elif __STDC_VERSION__ >= 201112L
#define FLINT_TLS_PREFIX _Thread_local
#elif defined(_MSC_VER)
#define FLINT_TLS_PREFIX __declspec(thread)
#elif defined(__GNUC__)
#define FLINT_TLS_PREFIX __thread
#else
#error "thread local prefix defined in C11 or later"
#endif
#else
#define FLINT_TLS_PREFIX
#endif

#if FLINT_WANT_ASSERT
#define FLINT_ASSERT(param) assert(param)
#else
#define FLINT_ASSERT(param)
#endif

#if defined(__GNUC__)
#define FLINT_UNUSED(x) UNUSED_ ## x __attribute__((unused))
#define FLINT_SET_BUT_UNUSED(x) x __attribute__((unused))
#if __GNUC__ >= 4
#define FLINT_WARN_UNUSED __attribute__((warn_unused_result))
#else
#define FLINT_WARN_UNUSED
#endif
#else
#define __attribute__(x)
#define FLINT_UNUSED(x) x
#define FLINT_SET_BUT_UNUSED(x) x
#define FLINT_WARN_UNUSED
#endif

/* include FLINT headers ******************************************************/

#include "flint-config.h"
#include "flint-types.h"
#include "longlong.h"
#include "exception.h"

#if FLINT_USES_GC
#include "gc.h"
#endif

#if FLINT_WANT_ASSERT
#include <assert.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* version numbering **********************************************************/

#define __FLINT_VERSION 2
#define __FLINT_VERSION_MINOR 9
#define __FLINT_VERSION_PATCHLEVEL 0
#define FLINT_VERSION "2.9.0"
#define __FLINT_RELEASE (__FLINT_VERSION * 10000 + \
                         __FLINT_VERSION_MINOR * 100 + \
                         __FLINT_VERSION_PATCHLEVEL)

extern char flint_version[];

/* word constants *************************************************************/

#if defined(_WIN64) || defined(__mips64)
#define WORD(xx) (xx##LL)
#define UWORD(xx) (xx##ULL)
#ifndef FLINT_NO_WORDMAC
#define UWORD_MAX ULLONG_MAX
#define UWORD_MIN ULLONG_MIN
#define WORD_MAX LLONG_MAX
#define WORD_MIN LLONG_MIN
#endif
#else
#define WORD(xx) (xx##L)
#define UWORD(xx) (xx##UL)
#ifndef FLINT_NO_WORDMAC
#define UWORD_MAX ULONG_MAX
#define UWORD_MIN ULONG_MIN
#define WORD_MAX LONG_MAX
#define WORD_MIN LONG_MIN
#endif
#endif

/* fmpz constants *************************************************************/

/* The largest bit count for an fmpz to be small */
#define SMALL_FMPZ_BITCOUNT_MAX (FLINT_BITS - 2)

/* maximum positive value a small coefficient can have */
#define COEFF_MAX ((WORD(1) << SMALL_FMPZ_BITCOUNT_MAX) - WORD(1))

/* minimum negative value a small coefficient can have */
#define COEFF_MIN (-((WORD(1) << SMALL_FMPZ_BITCOUNT_MAX) - WORD(1)))

/* other constants ************************************************************/

#ifdef NEED_CLZ_TAB
FLINT_DLL extern const unsigned char __flint_clz_tab[128];
#endif

/* macros *********************************************************************/

#define FLINT_MAX(x, y) ((x) > (y) ? (x) : (y))
#define FLINT_MIN(x, y) ((x) > (y) ? (y) : (x))
#define FLINT_ABS(x) ((slong)(x) < 0 ? -(x) : (x))
#define FLINT_SIGN_EXT(x) (-(ulong)((slong)(x) < 0))
#define FLINT_SGN(x) ((0 < (slong)(x)) - ((slong)(x) < 0))

#define FLINT_FLOG2(k)  (FLINT_BIT_COUNT(k) - 1)
#define FLINT_CLOG2(k)  FLINT_BIT_COUNT((k) - 1)

/* memory handling ************************************************************/

FLINT_DLL void * flint_malloc(size_t size);
FLINT_DLL void * flint_realloc(void * ptr, size_t size);
FLINT_DLL void * flint_calloc(size_t num, size_t size);
FLINT_DLL void flint_free(void * ptr);

typedef void (*flint_cleanup_function_t)(void);
FLINT_DLL void flint_register_cleanup_function(flint_cleanup_function_t cleanup_function);
FLINT_DLL void flint_cleanup(void);
FLINT_DLL void flint_cleanup_master(void);

FLINT_DLL void __flint_set_memory_functions(void *(*alloc_func) (size_t),
     void *(*calloc_func) (size_t, size_t), void *(*realloc_func) (void *, size_t),
                                                              void (*free_func) (void *));

FLINT_DLL FLINT_NORETURN void flint_abort(void);
FLINT_DLL void flint_set_abort(FLINT_NORETURN void (*func)(void));

/* The following function is only used when allocating space for matrices */
FLINT_INLINE slong flint_mul_sizes(slong x, slong y)
{
    ulong hi, lo;

    umul_ppmm(hi, lo, (ulong) x, (ulong) y);
    if (hi != 0 || lo > WORD_MAX)
    {
        flint_throw(FLINT_ALLOC,
            "Overflow when allocating space for an %wd x %wd object.\n", x, y);
    }

    return lo;
}

/* generic functions **********************************************************/

/* Beware when using the unsigned return value in signed arithmetic */
static __inline__
mp_limb_t FLINT_BIT_COUNT(mp_limb_t x)
{
   mp_limb_t zeros = FLINT_BITS;
   if (x) count_leading_zeros(zeros, x);
   return FLINT_BITS - zeros;
}

/* thread helper functions ****************************************************/

FLINT_DLL int flint_get_num_threads(void);
FLINT_DLL void flint_set_num_threads(int num_threads);
FLINT_DLL void _flint_set_num_workers(int num_workers);
FLINT_DLL int flint_set_num_workers(int num_workers);
FLINT_DLL void flint_reset_num_workers(int max_workers);
FLINT_DLL int flint_set_thread_affinity(int * cpus, slong length);
FLINT_DLL int flint_restore_thread_affinity();

int flint_test_multiplier(void);

/* pseudo-random numbers ******************************************************/

FLINT_INLINE
void flint_randinit(flint_rand_t state)
{
   state->gmp_init = 0;
#if FLINT64
    state->__randval = UWORD(13845646450878251009);
    state->__randval2 = UWORD(13142370077570254774);
#else
    state->__randval = UWORD(4187301858);
    state->__randval2 = UWORD(3721271368);
#endif
}

FLINT_INLINE
void flint_randseed(flint_rand_t state, ulong seed1, ulong seed2)
{
   state->__randval = seed1;
   state->__randval2 = seed2;
}

FLINT_INLINE
void flint_get_randseed(ulong * seed1, ulong * seed2, flint_rand_t state)
{
   *seed1 = state->__randval;
   *seed2 = state->__randval2;
}


FLINT_INLINE
void _flint_rand_init_gmp(flint_rand_t state)
{
    if (!state->gmp_init)
    {
        gmp_randinit_default(state->gmp_state);
        state->gmp_init = 1;
    }
}

FLINT_INLINE
void flint_randclear(flint_rand_t state)
{
    if (state->gmp_init)
        gmp_randclear(state->gmp_state);
}

FLINT_INLINE
flint_rand_s * flint_rand_alloc(void)
{
    return (flint_rand_s *) flint_malloc(sizeof(flint_rand_s));
}

FLINT_INLINE
void flint_rand_free(flint_rand_s * state)
{
    flint_free(state);
}

#ifdef __cplusplus
}
#endif

#endif

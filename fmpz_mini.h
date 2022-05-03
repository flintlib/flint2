/*
    Copyright (C) 2022 Albin Ahlbäck

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#ifndef FMPZ_MINI_H
#define FMPZ_MINI_H

#ifdef FMPZ_MINI_INLINES_C
#define FMPZ_MINI_INLINE FLINT_DLL
#else
#define FMPZ_MINI_INLINE static __inline__
#endif

#include "flint.h"
#include "fmpz-conversions.h"
#ifdef LONGSLONG
#define flint_mpz_set_si mpz_set_si
#define flint_mpz_set_ui mpz_set_ui
#else
#include "gmpcompat.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

FMPZ_MINI_INLINE
void fmpz_init(fmpz_t f)
{
    (*f) = WORD(0);
}

FLINT_DLL void _fmpz_clear_mpz(fmpz f);

FLINT_DLL __mpz_struct * _fmpz_new_mpz(void);

FMPZ_MINI_INLINE
void fmpz_clear(fmpz_t f)
{
    if (COEFF_IS_MPZ(*f))
        _fmpz_clear_mpz(*f);
}

FMPZ_MINI_INLINE
void _fmpz_demote(fmpz_t f)
{
    /* 
       warning, if fmpz_demote changes, fmpz_zero must
       also be changed to match
    */
    if (COEFF_IS_MPZ(*f)) 
    {
        _fmpz_clear_mpz(*f);
        (*f) = WORD(0);
    }
}

FLINT_DLL __mpz_struct * _fmpz_promote_val(fmpz_t f);

FLINT_DLL void _fmpz_demote_val(fmpz_t f);

FLINT_DLL __mpz_struct * _fmpz_promote(fmpz_t f);

FMPZ_MINI_INLINE
void fmpz_zero(fmpz_t f)
{
    if (COEFF_IS_MPZ(*f))
        _fmpz_clear_mpz(*f);
    *f = WORD(0);
}

FMPZ_MINI_INLINE
void fmpz_one(fmpz_t f)
{
    if (COEFF_IS_MPZ(*f)) 
        _fmpz_clear_mpz(*f);
    *f = WORD(1);
}

FMPZ_MINI_INLINE
int fmpz_is_zero(const fmpz_t f)
{
    return (*f == 0);
}

FMPZ_MINI_INLINE
int fmpz_is_one(const fmpz_t f)
{
    return (*f == 1);
}

FLINT_DLL int fmpz_equal(const fmpz_t f, const fmpz_t g);

FLINT_DLL void fmpz_set(fmpz_t f, const fmpz_t g);

FMPZ_MINI_INLINE void
fmpz_set_si(fmpz_t f, slong val)
{
    if (val < COEFF_MIN || val > COEFF_MAX) /* val is large */
    {
        __mpz_struct * mf = _fmpz_promote(f);
        flint_mpz_set_si(mf, val);
    }
    else
    {
        _fmpz_demote(f);
        *f = val;               /* val is small */
    }
}

FMPZ_MINI_INLINE void
fmpz_set_ui(fmpz_t f, ulong val)
{
    if (val > COEFF_MAX)        /* val is large */
    {
        __mpz_struct * mf = _fmpz_promote(f);
        flint_mpz_set_ui(mf, val);
    }
    else
    {
        _fmpz_demote(f);
        *f = val;               /* val is small */
    }
}

FMPZ_MINI_INLINE void
fmpz_neg_ui(fmpz_t f, ulong val)
{
    if (val > COEFF_MAX)
    {
        __mpz_struct * mf = _fmpz_promote(f);
        flint_mpz_set_ui(mf, val);
        mpz_neg(mf, mf);
    }
    else
    {
        _fmpz_demote(f);
        *f = -(slong) val;
    }
}

FMPZ_MINI_INLINE
void fmpz_init_set_ui(fmpz_t f, ulong g)
{
    if (g <= COEFF_MAX)
    {
        *f = g;
    }
    else
    {
        __mpz_struct * ptr;

        ptr = _fmpz_new_mpz();
        *f = PTR_TO_COEFF(ptr);
        flint_mpz_set_ui(ptr, g);
    }
}

FMPZ_MINI_INLINE
void fmpz_init_set_si(fmpz_t f, slong g)
{
    if (COEFF_MIN <= g && g <= COEFF_MAX)
    {
        *f = g;
    }
    else
    {
        __mpz_struct * ptr;
        ptr = _fmpz_new_mpz();
        *f = PTR_TO_COEFF(ptr);
        flint_mpz_set_si(ptr, g);
    }
}

FMPZ_MINI_INLINE
void fmpz_swap(fmpz_t f, fmpz_t g)
{
    if (f != g)  /* swapping required */
    {
        fmpz t = *f;
        *f = *g;
        *g = t;
    }
}

FLINT_DLL slong fmpz_get_si(const fmpz_t f);
FLINT_DLL ulong fmpz_get_ui(const fmpz_t f);

FLINT_DLL int fmpz_fits_si(const fmpz_t f);

#ifdef __cplusplus
}
#endif

#endif

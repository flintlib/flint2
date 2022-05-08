/*
    Copyright (C) 2010 Sebastian Pancratz
    Copyright (C) 2011 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#ifndef alloca
# ifdef __GNUC__
#  define alloca __builtin_alloca
# else
#  if HAVE_ALLOCA_H
#   include <alloca.h>
#  else
#   if _MSC_VER
#    include <malloc.h>
#    define alloca _alloca
#   else
#    ifdef __DECC
#     define alloca(x) __ALLOCA(x)
#    else
#     ifdef BSD
#      include <stdlib.h>
#     else
#      error Could not find alloca
#     endif
#    endif
#   endif
#  endif
# endif
#endif

#include "nmod_poly.h"
#include "flint-impl.h"

void _nmod_poly_rem(ulong_ptr R, ulong_srcptr A, slong lenA, 
                              ulong_srcptr B, slong lenB, nmod_t mod)
{
    TMP_INIT;

    if (lenA - lenB == 1)
    {
        _nmod_poly_rem_q1(R, A, lenA, B, lenB, mod);
    }
    else if (lenA < NMOD_DIVREM_DIVCONQUER_CUTOFF)
    {
        ulong_ptr W;
        
        TMP_START;
        W = TMP_ALLOC(NMOD_DIVREM_BC_ITCH(lenA, lenB, mod)*sizeof(ulong));

        _nmod_poly_rem_basecase(R, W, A, lenA, B, lenB, mod);
        TMP_END;
    }
    else
    {
        ulong_ptr Q = _nmod_vec_init(lenA - lenB + 1);

        _nmod_poly_divrem(Q, R, A, lenA, B, lenB, mod);
        _nmod_vec_clear(Q);
    }
}

void nmod_poly_rem(nmod_poly_t R, const nmod_poly_t A, const nmod_poly_t B)
{
    const slong lenA = A->length, lenB = B->length;
    nmod_poly_t tR;
    ulong_ptr r;

    if (lenB == 0)
        flint_throw(FLINT_DIVZERO, "nmod_poly_rem\n");

    if (lenA < lenB)
    {
        nmod_poly_set(R, A);
        return;
    }

    if (R == A || R == B)
    {
        nmod_poly_init2_preinv(tR, B->mod.n, B->mod.ninv, lenB - 1);
        r = tR->coeffs;
    }
    else
    {
        nmod_poly_fit_length(R, lenB - 1);
        r = R->coeffs;
    }

    _nmod_poly_rem(r, A->coeffs, lenA, B->coeffs, lenB, A->mod);

    if (R == A || R == B)
    {
        nmod_poly_swap(R, tR);
        nmod_poly_clear(tR);
    }
        
    R->length = lenB - 1;
    _nmod_poly_normalise(R);
}

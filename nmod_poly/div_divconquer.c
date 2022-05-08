/*
    Copyright (C) 2008, 2009, 2011 William Hart
    Copyright (C) 2012 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "ulong_extras.h"
#include "nmod_poly.h"
#include "flint-impl.h"

static void
__nmod_poly_div_divconquer(ulong_ptr Q, ulong_srcptr A, slong lenA, 
                             ulong_srcptr B, slong lenB, nmod_t mod)
{
    if (lenA < 2 * lenB - 1)
    {
        /*
           Convert unbalanced division into a 2 n1 - 1 by n1 division
         */

        const slong n1 = lenA - lenB + 1;
        const slong n2 = lenB - n1;

        ulong_srcptr p1 = A + n2;
        ulong_srcptr d1 = B + n2;
        
        ulong_ptr V = _nmod_vec_init(n1 - 1 + NMOD_DIVREM_DC_ITCH(n1, mod));
        ulong_ptr W = V + NMOD_DIVREM_DC_ITCH(n1, mod);

        _nmod_poly_div_divconquer_recursive(Q, W, V, p1, d1, n1, mod);

        _nmod_vec_clear(V);
    }
    else  /* lenA = 2 * lenB - 1 */
    {
        ulong_ptr V = _nmod_vec_init(lenB - 1 + NMOD_DIVREM_DC_ITCH(lenB, mod));
        ulong_ptr W = V + NMOD_DIVREM_DC_ITCH(lenB, mod);
 
        _nmod_poly_div_divconquer_recursive(Q, W, V, A, B, lenB, mod);
        
        _nmod_vec_clear(V);
    }
}


/* needed due to partial overlap */
static void
_nmod_vec_sub_dec(ulong_ptr a, ulong_srcptr b, ulong_srcptr c, slong n, nmod_t mod)
{
    slong i;

    for (i = n - 1; i >= 0; i--)
        a[i] = n_submod(b[i], c[i], mod.n);
}

void
_nmod_poly_div_divconquer(ulong_ptr Q, ulong_srcptr A, slong lenA,
                             ulong_srcptr B, slong lenB, nmod_t mod)
{
    if (lenA <= 2 * lenB - 1)
    {
        __nmod_poly_div_divconquer(Q, A, lenA, B, lenB, mod);
    }
    else  /* lenA > 2 * lenB - 1 */
    {
        ulong_ptr S, T, V, R;
        slong shift, next, n = 2 * lenB - 1;

        S = _nmod_vec_init(2 * n + (lenB - 1) + NMOD_DIVREM_DC_ITCH(lenB, mod));
        T = S + n;
        R = T + n;
        V = R + (lenB - 1);

        shift = lenA - n;
        _NMOD_VEC_SET(S, A + shift, n);

        while (lenA >= n)
        {
            shift = lenA - n;
            _nmod_poly_divrem_divconquer_recursive(Q + shift, T, R, V, S, B, lenB, mod);
            next = FLINT_MIN(lenB, shift);
            _nmod_vec_sub_dec(S + next, S, T, lenB - 1, mod);
            _NMOD_VEC_SET(S, A + shift - next, next);
            lenA -= lenB;
        }

        if (lenA >= lenB)
            __nmod_poly_div_divconquer(Q, S, lenA, B, lenB, mod);

        _nmod_vec_clear(S);
    }
}


void
nmod_poly_div_divconquer(nmod_poly_t Q,
                            const nmod_poly_t A, const nmod_poly_t B)
{
    nmod_poly_t tQ;
    ulong_ptr q;
    slong Alen, Blen;

    Blen = B->length;

    if (Blen == 0)
    {
        if (nmod_poly_modulus(B) == 1)
        {
            nmod_poly_set(Q, A);
            return;
        }
        else
            flint_throw(FLINT_DIVZERO, "nmod_poly_div_divconquer\n");
    }

    Alen = A->length;

    if (Alen < Blen)
    {
        nmod_poly_zero(Q);
        return;
    }

    if (Q == A || Q == B)
    {
        nmod_poly_init2(tQ, A->mod.n, Alen - Blen + 1);
        q = tQ->coeffs;
    }
    else
    {
        nmod_poly_fit_length(Q, Alen - Blen + 1);
        q = Q->coeffs;
    }

    _nmod_poly_div_divconquer(q, A->coeffs, Alen,
                                       B->coeffs, Blen, A->mod);

    if (Q == A || Q == B)
    {
        nmod_poly_swap(tQ, Q);
        nmod_poly_clear(tQ);
    }
    
    Q->length = Alen - Blen + 1;
}

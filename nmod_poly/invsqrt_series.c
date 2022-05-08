/*
    Copyright (C) 2011 William Hart
    Copyright (C) 2011 Fredrik Johansson

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
__nmod_poly_invsqrt_series_prealloc(ulong_ptr g, 
                                    ulong_srcptr h, ulong_ptr t, ulong_ptr u,
                                    slong n, nmod_t mod)
{
    const int alloc = (t == NULL);
    const slong m    = (n + 1) / 2;
    ulong c;

    if (n == 1)
    {
        g[0] = UWORD(1);
        return;
    }

    if (alloc)
    {
        t = _nmod_vec_init(n);
        u = _nmod_vec_init(n);
    }

    __nmod_poly_invsqrt_series_prealloc(g, h, t, u, m, mod);

    _NMOD_VEC_ZERO(g + m, n - m);

    _nmod_poly_mul(t, g, m, g, m, mod);
    if (2*m - 1 < n)
        t[n-1] = UWORD(0);

    _nmod_poly_mullow(u, t, n, g, n, n, mod);
    _nmod_poly_mullow(t, u, n, h, n, n, mod);

    c = n_invmod(mod.n - 2, mod.n);
    _nmod_vec_scalar_mul_nmod(g + m, t + m, n - m, c, mod);

    if (alloc)
    {
        _nmod_vec_clear(t);
        _nmod_vec_clear(u);
    }
}

void _nmod_poly_invsqrt_series(ulong_ptr g, ulong_srcptr h, slong n, nmod_t mod)
{
    __nmod_poly_invsqrt_series_prealloc(g, h, NULL, NULL, n, mod);
}

void nmod_poly_invsqrt_series(nmod_poly_t g, const nmod_poly_t h, slong n)
{
    const slong hlen = h->length;
    ulong_ptr g_coeffs, h_coeffs;
    nmod_poly_t t1;

    if (n == 0 || h->length == 0 || h->coeffs[0] == 0)
        flint_throw(FLINT_DIVZERO, "nmod_poly_invsqrt\n");

    if (h->coeffs[0] != UWORD(1))
        flint_throw(FLINT_ERROR, "Constant term != 1 in nmod_poly_invsqrt_series\n");

    if (hlen < n)
    {
        h_coeffs = _nmod_vec_init(n);
        FLINT_MPN_COPYI(h_coeffs, h->coeffs, hlen);
        FLINT_MPN_ZERO(h_coeffs + hlen, n - hlen);
    }
    else
        h_coeffs = h->coeffs;

    if (h == g && hlen >= n)
    {
        nmod_poly_init2(t1, h->mod.n, n);
        g_coeffs = t1->coeffs;
    }
    else
    {
        nmod_poly_fit_length(g, n);
        g_coeffs = g->coeffs;
    }

    _nmod_poly_invsqrt_series(g_coeffs, h_coeffs, n, h->mod);

    if (h == g && hlen >= n)
    {
        nmod_poly_swap(g, t1);
        nmod_poly_clear(t1);
    }
    
    g->length = n;

    if (hlen < n)
        _nmod_vec_clear(h_coeffs);

    _nmod_poly_normalise(g);
}

/*
    Copyright (C) 2010 Sebastian Pancratz
    Copyright (C) 2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "ulong_extras.h"
#include "nmod.h"
#include "nmod_vec.h"
#include "nmod_poly.h"

void
_nmod_poly_revert_series(mp_ptr Qinv, mp_srcptr Q, slong n, nmod_t mod)
{
    _nmod_poly_revert_series_lagrange_fast(Qinv, Q, n, mod);
}

void
nmod_poly_revert_series(nmod_poly_t Qinv,
                                 const nmod_poly_t Q, slong n)
{
    mp_ptr Qinv_coeffs, Q_coeffs;
    nmod_poly_t t1;
    slong Qlen;

    Qlen = Q->length;

    if (Qlen < 2 || Q->coeffs[0] != 0 || Q->coeffs[1] == 0)
    {
        flint_printf("Exception (nmod_poly_revert_series). Input must have \n"
               "zero constant and an invertible coefficient of x^1.\n");
        flint_abort();
    }

    if (Qlen < n)
    {
        Q_coeffs = _nmod_vec_init(n);
        flint_mpn_copyi(Q_coeffs, Q->coeffs, Qlen);
        flint_mpn_zero(Q_coeffs + Qlen, n - Qlen);
    }
    else
        Q_coeffs = Q->coeffs;

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_init2(t1, Q->mod.n, n);
        Qinv_coeffs = t1->coeffs;
    }
    else
    {
        nmod_poly_fit_length(Qinv, n);
        Qinv_coeffs = Qinv->coeffs;
    }

    _nmod_poly_revert_series(Qinv_coeffs, Q_coeffs, n, Q->mod);

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_swap(Qinv, t1);
        nmod_poly_clear(t1);
    }

    Qinv->length = n;

    if (Qlen < n)
        _nmod_vec_clear(Q_coeffs);

    _nmod_poly_normalise(Qinv);
}

void
_nmod_poly_revert_series_lagrange(mp_ptr Qinv, mp_srcptr Q, slong n, nmod_t mod)
{
    slong i;
    mp_ptr R, S, T, tmp;

    if (n >= 1) Qinv[0] = UWORD(0);
    if (n >= 2) Qinv[1] = n_invmod(Q[1], mod.n);
    if (n <= 2)
        return;

    R = _nmod_vec_init(n - 1);
    S = _nmod_vec_init(n - 1);
    T = _nmod_vec_init(n - 1);

    _nmod_poly_inv_series(R, Q + 1, n - 1, n - 1, mod);
    _nmod_vec_set(S, R, n - 1);

    for (i = 2; i < n; i++)
    {
        _nmod_poly_mullow(T, S, n - 1, R, n - 1, n - 1, mod);
        Qinv[i] = nmod_div(T[i - 1], i, mod);
        tmp = S; S = T; T = tmp;
    }

    _nmod_vec_clear(R);
    _nmod_vec_clear(S);
    _nmod_vec_clear(T);
}

void
nmod_poly_revert_series_lagrange(nmod_poly_t Qinv,
                                 const nmod_poly_t Q, slong n)
{
    mp_ptr Qinv_coeffs, Q_coeffs;
    nmod_poly_t t1;
    slong Qlen;

    Qlen = Q->length;

    if (Qlen < 2 || Q->coeffs[0] != 0 || Q->coeffs[1] == 0)
    {
        flint_printf("Exception (nmod_poly_revert_series_lagrange). Input must \n"
               "have zero constant and an invertible coefficient of x^1.\n");
        flint_abort();
    }

    if (Qlen < n)
    {
        Q_coeffs = _nmod_vec_init(n);
        flint_mpn_copyi(Q_coeffs, Q->coeffs, Qlen);
        flint_mpn_zero(Q_coeffs + Qlen, n - Qlen);
    }
    else
        Q_coeffs = Q->coeffs;

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_init2(t1, Q->mod.n, n);
        Qinv_coeffs = t1->coeffs;
    }
    else
    {
        nmod_poly_fit_length(Qinv, n);
        Qinv_coeffs = Qinv->coeffs;
    }

    _nmod_poly_revert_series_lagrange(Qinv_coeffs, Q_coeffs, n, Q->mod);

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_swap(Qinv, t1);
        nmod_poly_clear(t1);
    }

    Qinv->length = n;

    if (Qlen < n)
        _nmod_vec_clear(Q_coeffs);

    _nmod_poly_normalise(Qinv);
}

/* pointer to (x/Q)^i */
#define Ri(ii) (R + (n-1)*((ii)-1))

void
_nmod_poly_revert_series_lagrange_fast(mp_ptr Qinv,
                                            mp_srcptr Q, slong n, nmod_t mod)
{
    slong i, j, k, m;
    mp_ptr R, S, T, tmp;

    if (n >= 1) Qinv[0] = UWORD(0);
    if (n >= 2) Qinv[1] = n_invmod(Q[1], mod.n);
    if (n <= 2)
        return;

    m = n_sqrt(n);

    R = _nmod_vec_init((n - 1) * m);
    S = _nmod_vec_init(n - 1);
    T = _nmod_vec_init(n - 1);

    _nmod_poly_inv_series(Ri(1), Q + 1, n - 1, n - 1, mod);
    for (i = 2; i <= m; i++)
        _nmod_poly_mullow(Ri(i), Ri(i-1), n - 1, Ri(1), n - 1, n - 1, mod);
    for (i = 2; i < m; i++)
        Qinv[i] = nmod_div(Ri(i)[i-1], i, mod);

    _nmod_vec_set(S, Ri(m), n - 1);

    for (i = m; i < n; i += m)
    {
        Qinv[i] = nmod_div(S[i-1], i, mod);
        for (j = 1; j < m && i + j < n; j++)
        {
            mp_limb_t s;
            int nlimbs = _nmod_vec_dot_bound_limbs(i + j, mod);
            NMOD_VEC_DOT(s, k, i + j, S[k], Ri(j)[i+j-1-k], mod, nlimbs);
            Qinv[i+j] = nmod_div(s, i+j, mod);
        }

        if (i + 1 < n)
        {
            _nmod_poly_mullow(T, S, n - 1, Ri(m), n - 1, n - 1, mod);
            tmp = S; S = T; T = tmp;
        }
    }

    _nmod_vec_clear(R);
    _nmod_vec_clear(S);
    _nmod_vec_clear(T);
}

void
nmod_poly_revert_series_lagrange_fast(nmod_poly_t Qinv,
                                 const nmod_poly_t Q, slong n)
{
    mp_ptr Qinv_coeffs, Q_coeffs;
    nmod_poly_t t1;
    slong Qlen;

    Qlen = Q->length;

    if (Qlen < 2 || Q->coeffs[0] != 0 || Q->coeffs[1] == 0)
    {
        flint_printf("Exception (nmod_poly_revert_series_lagrange_fast). Input must \n"
               "have zero constant and an invertible coefficient of x^1.\n");
        flint_abort();
    }

    if (Qlen < n)
    {
        Q_coeffs = _nmod_vec_init(n);
        flint_mpn_copyi(Q_coeffs, Q->coeffs, Qlen);
        flint_mpn_zero(Q_coeffs + Qlen, n - Qlen);
    }
    else
        Q_coeffs = Q->coeffs;

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_init2(t1, Q->mod.n, n);
        Qinv_coeffs = t1->coeffs;
    }
    else
    {
        nmod_poly_fit_length(Qinv, n);
        Qinv_coeffs = Qinv->coeffs;
    }

    _nmod_poly_revert_series_lagrange_fast(Qinv_coeffs, Q_coeffs, n, Q->mod);

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_swap(Qinv, t1);
        nmod_poly_clear(t1);
    }

    Qinv->length = n;

    if (Qlen < n)
        _nmod_vec_clear(Q_coeffs);

    _nmod_poly_normalise(Qinv);
}

#define FLINT_REVERSE_NEWTON_CUTOFF 15

void
_nmod_poly_revert_series_newton(mp_ptr Qinv, mp_srcptr Q, slong n, nmod_t mod)
{
    slong *a, i, k;
    mp_ptr T, U, V;

    if (n >= 1) Qinv[0] = UWORD(0);
    if (n >= 2) Qinv[1] = n_invmod(Q[1], mod.n);
    if (n <= 2)
        return;

    T = _nmod_vec_init(n);
    U = _nmod_vec_init(n);
    V = _nmod_vec_init(n);

    k = n;
    for (i = 1; (WORD(1) << i) < k; i++);
    a = (slong *) flint_malloc(i * sizeof(slong));
    a[i = 0] = k;
    while (k >= FLINT_REVERSE_NEWTON_CUTOFF)
        a[++i] = (k = (k + 1) / 2);

    _nmod_poly_revert_series_lagrange(Qinv, Q, k, mod);
    _nmod_vec_zero(Qinv + k, n - k);

    for (i--; i >= 0; i--)
    {
        k = a[i];
        _nmod_poly_compose_series(T, Q, k, Qinv, k, k, mod);
        _nmod_poly_derivative(U, T, k, mod); U[k - 1] = UWORD(0);
        T[1] = UWORD(0);
        _nmod_poly_div_series(V, T, k, U, k, k, mod);
        _nmod_poly_derivative(T, Qinv, k, mod);
        _nmod_poly_mullow(U, V, k, T, k, k, mod);
        _nmod_vec_sub(Qinv, Qinv, U, k, mod);
    }

    flint_free(a);
    _nmod_vec_clear(T);
    _nmod_vec_clear(U);
    _nmod_vec_clear(V);
}

void
nmod_poly_revert_series_newton(nmod_poly_t Qinv,
                                 const nmod_poly_t Q, slong n)
{
    mp_ptr Qinv_coeffs, Q_coeffs;
    nmod_poly_t t1;
    slong Qlen;

    Qlen = Q->length;

    if (Qlen < 2 || Q->coeffs[0] != 0 || Q->coeffs[1] == 0)
    {
        flint_printf("Exception (nmod_poly_revert_series_newton). Input must have \n"
               "zero constant and an invertible coefficient of x^1.\n");
        flint_abort();
    }

    if (Qlen < n)
    {
        Q_coeffs = _nmod_vec_init(n);
        flint_mpn_copyi(Q_coeffs, Q->coeffs, Qlen);
        flint_mpn_zero(Q_coeffs + Qlen, n - Qlen);
    }
    else
        Q_coeffs = Q->coeffs;

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_init2(t1, Q->mod.n, n);
        Qinv_coeffs = t1->coeffs;
    }
    else
    {
        nmod_poly_fit_length(Qinv, n);
        Qinv_coeffs = Qinv->coeffs;
    }

    _nmod_poly_revert_series_newton(Qinv_coeffs, Q_coeffs, n, Q->mod);

    if (Q == Qinv && Qlen >= n)
    {
        nmod_poly_swap(Qinv, t1);
        nmod_poly_clear(t1);
    }

    Qinv->length = n;

    if (Qlen < n)
        _nmod_vec_clear(Q_coeffs);

    _nmod_poly_normalise(Qinv);
}

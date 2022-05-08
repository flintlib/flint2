/*
    Copyright (C) 2021 Fredrik Johansson

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

#include "flint-impl.h"
#include "nmod.h"
#include "nmod_poly.h"
#include "nmod_mat.h"

void
_nmod_mat_charpoly_berkowitz(ulong_ptr cp, const nmod_mat_t mat, nmod_t mod)
{
    const slong n = mat->r;

    if (mod.n == 1)
    {
        _NMOD_VEC_ZERO(cp, n + 1);
    }
    else if (n == 0)
    {
        cp[0] = 1;
    }
    else if (n == 1)
    {
        cp[0] = nmod_neg(nmod_mat_entry(mat, 0, 0), mod);
        cp[1] = 1;
    }
    else if (n == 2)
    {
        cp[0] = nmod_sub(nmod_mul(nmod_mat_entry(mat, 0, 0), nmod_mat_entry(mat, 1, 1), mod),
                         nmod_mul(nmod_mat_entry(mat, 0, 1), nmod_mat_entry(mat, 1, 0), mod), mod);
        cp[1] = nmod_add(nmod_mat_entry(mat, 0, 0), nmod_mat_entry(mat, 1, 1), mod);
        cp[1] = nmod_neg(cp[1], mod);
        cp[2] = 1;
    }
    else
    {
        slong i, k, t;
        ulong_ptr a, A, s;
        int nlimbs;
        TMP_INIT;

        TMP_START;
        a = TMP_ALLOC(sizeof(ulong) * (n * n));
        A = a + (n - 1) * n;

        nlimbs = _nmod_vec_dot_bound_limbs(n, mod);

        _NMOD_VEC_ZERO(cp, n + 1);
        cp[0] = nmod_neg(nmod_mat_entry(mat, 0, 0), mod);

        for (t = 1; t < n; t++)
        {
            for (i = 0; i <= t; i++)
            {
                a[0 * n + i] = nmod_mat_entry(mat, i, t);
            }

            A[0] = nmod_mat_entry(mat, t, t);

            for (k = 1; k < t; k++)
            {
                for (i = 0; i <= t; i++)
                {
                    s = a + k * n + i;
                    s[0] = _nmod_vec_dot(mat->rows[i], a + (k - 1) * n, t + 1, mod, nlimbs);
                }

                A[k] = a[k * n + t];
            }

            A[t] = _nmod_vec_dot(mat->rows[t], a + (t - 1) * n, t + 1, mod, nlimbs);

            for (k = 0; k <= t; k++)
            {
                cp[k] = nmod_sub(cp[k], _nmod_vec_dot_rev(A, cp, k, mod, nlimbs), mod);
                cp[k] = nmod_sub(cp[k], A[k], mod);
            }
        }

        /* Shift all coefficients up by one */
        for (i = n; i > 0; i--)
            cp[i] = cp[i - 1];

        cp[0] = 1;
        _nmod_poly_reverse(cp, cp, n + 1, n + 1);

        TMP_END;
    }
}

void nmod_mat_charpoly_berkowitz(nmod_poly_t cp, const nmod_mat_t mat)
{
    if (mat->r != mat->c)
        flint_throw(FLINT_ERROR, "Non-square matrix in nmod_mat_charpoly_berkowitz\n");

    nmod_poly_fit_length(cp, mat->r + 1);
    _nmod_poly_set_length(cp, mat->r + 1);
    _nmod_mat_charpoly_berkowitz(cp->coeffs, mat, mat->mod);
}

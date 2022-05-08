/*
    Copyright (C) 2010,2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "nmod.h"
#include "nmod_mat.h"

void
nmod_mat_solve_triu_classical(nmod_mat_t X, const nmod_mat_t U,
                                                const nmod_mat_t B, int unit)
{
    int nlimbs;
    slong i, j, n, m;
    nmod_t mod;
    ulong_ptr inv, tmp;

    n = U->r;
    m = B->c;
    mod = U->mod;

    if (!unit)
    {
        inv = _nmod_vec_init(n);
        for (i = 0; i < n; i++)
            inv[i] = n_invmod(nmod_mat_entry(U, i, i), mod.n);
    }
    else
        inv = NULL;

    nlimbs = _nmod_vec_dot_bound_limbs(n, mod);
    tmp = _nmod_vec_init(n);

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
            tmp[j] = nmod_mat_entry(X, j, i);

        for (j = n - 1; j >= 0; j--)
        {
            ulong s;
            s = _nmod_vec_dot(U->rows[j] + j + 1,
                              tmp + j + 1, n - j - 1, mod, nlimbs);
            s = nmod_sub(nmod_mat_entry(B, j, i), s, mod);
            if (!unit)
                s = n_mulmod2_preinv(s, inv[j], mod.n, mod.ninv);
            tmp[j] = s;
        }

        for (j = 0; j < n; j++)
            nmod_mat_entry(X, j, i) = tmp[j];
    }

    _nmod_vec_clear(tmp);
    if (!unit)
        _nmod_vec_clear(inv);
}

/*
    Copyright (C) 2010,2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "perm.h"
#include "fmpz_mini.h"
#include "fmpz_mat.h"

int
fmpz_mat_randpermdiag(fmpz_mat_t mat, flint_rand_t state,
                      const fmpz * diag, slong n)
{
    int parity;
    slong i;
    slong * rows;
    slong * cols;

    rows = _perm_init(mat->r);
    cols = _perm_init(mat->c);

    parity = _perm_randtest(rows, mat->r, state);
    parity ^= _perm_randtest(cols, mat->c, state);

    fmpz_mat_zero(mat);
    for (i = 0; i < n; i++)
        fmpz_set(fmpz_mat_entry(mat, rows[i], cols[i]), diag + i);

    _perm_clear(rows);
    _perm_clear(cols);

    return parity;
}

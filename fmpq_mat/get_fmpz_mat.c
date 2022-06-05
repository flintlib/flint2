/*
    Copyright (C) 2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpq_mat-impl.h"


int
fmpq_mat_get_fmpz_mat(fmpz_mat_t dest, const fmpq_mat_t mat)
{
    slong i, j;
    
    for (i = 0; i < mat->r; i++)
    {
        for (j = 0; j < mat->c; j++)
        {
            if (fmpz_is_one(fmpq_mat_entry_den(mat, i, j)))
                fmpz_set(fmpz_mat_entry(dest, i, j),
                         fmpq_mat_entry_num(mat, i, j));
            else
                return 0;
        }
    }
    return 1;
}

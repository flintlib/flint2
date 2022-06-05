/*
    Copyright (C) 2011 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "padic_mat-impl.h"


void padic_mat_init(padic_mat_t A, slong r, slong c)
{
    fmpz_mat_init(padic_mat(A), r, c);
    A->val = 0;
    A->N   = PADIC_DEFAULT_PREC;
}

void padic_mat_init2(padic_mat_t A, slong r, slong c, slong prec)
{
    fmpz_mat_init(padic_mat(A), r, c);
    A->val = 0;
    A->N   = prec;
}


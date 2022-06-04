/*
    Copyright (C) 2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpz_mat-impl.h"


void
fmpz_mat_det_modular(fmpz_t det, const fmpz_mat_t A, int proved)
{
    fmpz_t d;
    fmpz_init(d);
    fmpz_one(d);
    fmpz_mat_det_modular_given_divisor(det, A, d, proved);
    fmpz_clear(d);
}

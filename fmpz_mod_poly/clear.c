/*
    Copyright (C) 2011 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpz_mini.h"

void fmpz_mod_poly_clear(fmpz_mod_poly_t poly, const fmpz_mod_ctx_t ctx)
{
    slong i;

    for (i = 0; i < poly->alloc; i++)  /* Clean up any mpz_t's */
        _fmpz_demote(poly->coeffs + i);
    if (poly->coeffs) 
        flint_free(poly->coeffs);  /* clean up ordinary coeffs */
}


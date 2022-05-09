/*
    Copyright (C) 2010 Sebastian Pancratz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "gmp.h"
#include "ulong_extras.h"
#include "fmpz_mini.h"

void fmpz_sqrtrem(fmpz_t f, fmpz_t r, const fmpz_t g)
{
    if (fmpz_sgn(g) < 0)
        flint_throw(FLINT_ERROR, "g is negative in fmpz_sqrtrem\n");
    
    if (!COEFF_IS_MPZ(*g))
    {
        if (COEFF_IS_MPZ(*r))
            _fmpz_clear_mpz(*r);
        fmpz_set_ui(f, n_sqrtrem((ulong *) r, *g));
    }
    else
    {
        mpz_mock_ptr mr, mf;
        _fmpz_promote(f); /* must not hang on to pointer whilst promoting */
        mr = _fmpz_promote(r);
		mf = COEFF_TO_PTR(*f);

        mpz_sqrtrem((mpz_ptr) mf, (mpz_ptr) mr, (mpz_ptr) COEFF_TO_PTR(*g));
        _fmpz_demote_val(f);
        _fmpz_demote_val(r);
    }
}

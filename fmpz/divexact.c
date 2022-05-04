/*
    Copyright (C) 2009 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpz_mini.h"
#ifdef LONGSLONG
# define flint_mpz_divexact_ui mpz_divexact_ui
#else
# include "gmpcompat.h"
#endif

void
fmpz_divexact(fmpz_t f, const fmpz_t g, const fmpz_t h)
{
    fmpz c1 = *g;
    fmpz c2 = *h;

    if (fmpz_is_zero(h))
        flint_throw(FLINT_DIVZERO, "fmpz_divexact\n");

    if (!COEFF_IS_MPZ(c1))  /* g is small, h must be also or division isn't exact */
    {
        fmpz_set_si(f, c1 / c2);
    }
    else  /* g is large */
    {
        __mpz_struct * mf = _fmpz_promote(f);

        if (!COEFF_IS_MPZ(c2))  /* h is small */
        {
            if (c2 > 0)  /* h > 0 */
            {
                flint_mpz_divexact_ui(mf, COEFF_TO_PTR(c1), c2);
                _fmpz_demote_val(f);  /* division by h may result in small value */
            }
            else
            {
                flint_mpz_divexact_ui(mf, COEFF_TO_PTR(c1), -c2);
                _fmpz_demote_val(f);  /* division by h may result in small value */

                fmpz_neg(f, f);
            }
        }
        else  /* both are large */
        {
            mpz_divexact(mf, COEFF_TO_PTR(c1), COEFF_TO_PTR(c2));
            _fmpz_demote_val(f);  /* division by h may result in small value */
        }
    }
}

/*
    Copyright (C) 2009 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "gmp.h"
#include "fmpz_mini.h"
#ifdef LONGSLONG
# define flint_mpz_set_ui mpz_set_ui
# define flint_mpz_get_ui mpz_get_ui
#else
# include "gmpcompat.h"
#endif

void
fmpz_set_mpz(fmpz_t f, const mpz_t x)
{
    int size = (slong) x->_mp_size;

    if (size == 0)             /* x is zero */
    {
        fmpz_zero(f);
    }
    else if (size == 1)        /* x is positive and 1 limb */
    {
        fmpz_set_ui(f, flint_mpz_get_ui(x));
    }
    else if (size == -1)       /* x is negative and 1 limb */
    {
        ulong uval = flint_mpz_get_ui(x);
        if (uval <= COEFF_MAX)  /* x is small */
        {
            _fmpz_demote(f);
            *f = -uval;
        }
        else                    /* x is large but one limb */
        {
            mpz_mock_ptr mf = _fmpz_promote(f);
            flint_mpz_set_ui((mpz_ptr) mf, uval);
            mpz_neg((mpz_ptr) mf, (mpz_ptr) mf);
        }
    }
    else                        /* x is more than one limb */
    {
        mpz_mock_ptr mf = _fmpz_promote(f);
        mpz_set((mpz_ptr) mf, x);
    }
}

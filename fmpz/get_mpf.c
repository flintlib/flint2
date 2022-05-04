/*
    Copyright (C) 2009 William Hart
    Copyright (C) 2014 Abhinav Baid

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "flint.h"
#include "fmpz-conversions.h"
#ifdef LONGSLONG
# define flint_mpf_set_si mpf_set_si
#else
# include "gmpcompat.h"
#endif

void
fmpz_get_mpf(mpf_t x, const fmpz_t f)
{
    if (!COEFF_IS_MPZ(*f))
        flint_mpf_set_si(x, *f);      /* set x to small value */
    else
        mpf_set_z(x, COEFF_TO_PTR(*f)); /* set x to large value */
}

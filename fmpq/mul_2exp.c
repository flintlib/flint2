/*
    Copyright (C) 2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fmpz.h"
#include "fmpq.h"

void fmpq_mul_2exp(fmpq_t res, const fmpq_t x, flint_bitcnt_t exp)
{
    if (fmpq_is_zero(x) || exp == 0)
    {
        fmpq_set(res, x);
    }
    else
    {
        flint_bitcnt_t v = fmpz_val2(fmpq_denref(x));

        if (exp <= v)
        {
            fmpz_set(fmpq_numref(res), fmpq_numref(x));
            fmpz_fdiv_q_2exp(fmpq_denref(res), fmpq_denref(x), exp);
        }
        else
        {
            fmpz_mul_2exp(fmpq_numref(res), fmpq_numref(x), exp - v);
            fmpz_fdiv_q_2exp(fmpq_denref(res), fmpq_denref(x), v);
        }
    }
}

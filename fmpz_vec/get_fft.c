/*
    Copyright (C) 2008-2011 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "gmp.h"
#include "fmpz.h"
#include "flint-impl.h"

slong _fmpz_vec_get_fft(ulong_ptr * coeffs_f, 
                       const fmpz * coeffs_m, slong l, slong length)
{
    slong size_f = l + 1;
    ulong_ptr coeff;

    ulong mask = WORD(-1);
    slong bits = 0, limbs = 0, size_j, i, c;
    int sign = 1, signed_c;

    for (i = 0; i < length; i++, coeffs_m++)
    {
        c = *coeffs_m;
        signed_c = 0;

        if (!COEFF_IS_MPZ(c)) /* coeff is small */
        {
            size_j = 1;

            if (c < 0) 
            {
                signed_c = 1;
                c = -c;
                coeff = (ulong_ptr) &c;
            }
            else
                coeff = (ulong_ptr) coeffs_m;
        }
        else /* coeff is an mpz_t */
        {
            mpz_mock_ptr mc = COEFF_TO_PTR(c);
            size_j = mc->_mp_size;
            if (size_j < 0) 
            {
                signed_c = 1;
                size_j = -size_j;
            }
            coeff = mc->_mp_d;
        }

        if (signed_c)
            sign = -1;

        if (size_j > limbs + 1) /* coeff is at least 1 limb bigger */
        {
            limbs = size_j - 1;
            bits = FLINT_BIT_COUNT(coeff[size_j - 1]); 
            if (bits == FLINT_BITS)
                mask = WORD(0);
            else
                mask = WORD(-1) - (slong) ((UWORD(1) << bits) - 1);
        }
        else if (size_j == limbs + 1) /* coeff is same size as prev biggest */
        {
            if (coeff[size_j - 1] & mask) /* see if we have more bits than before */
            {
                bits = FLINT_BIT_COUNT(coeff[size_j - 1]);   
                if (bits == FLINT_BITS)
                    mask = WORD(0);
                else
                    mask = WORD(-1) - (slong) ((UWORD(1) << bits) - 1);
            }
        }

        if (signed_c) /* write out FFT coefficient, ensuring sign is correct */
        {
            mpn_neg(coeffs_f[i], coeff, size_j); 
            FLINT_MPN_STORE(coeffs_f[i] + size_j, size_f - size_j, WORD(-1)); 
        }
        else
        {
            FLINT_MPN_COPYI(coeffs_f[i], coeff, size_j); 
            FLINT_MPN_ZERO(coeffs_f[i] + size_j, size_f - size_j); 
        }
    }

    return sign*(FLINT_BITS*limbs + bits);  
}

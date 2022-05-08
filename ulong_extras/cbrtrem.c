/*
    Copyright (C) 2015 William Hart
    Copyright (C) 2015 Fredrik Johansson
    Copyright (C) 2015 Kushagra Singh

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "ulong_extras.h"

ulong
n_cbrtrem(ulong* remainder, ulong n)
{
    ulong base;
    
    if (!n)
    {
        *remainder = 0;
        return 0;
    }

    base = n_cbrt(n);
    *remainder = n - base * base * base;
    return base;
}

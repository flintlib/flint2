/*
    Copyright (C) 2010 William Hart
    Copyright (C) 2014 Abhinav Baid

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "flint.h"

void
_mpf_vec_sub(mpf_ptr res, mpf_srcptr vec1, mpf_srcptr vec2, slong len2)
{
    slong i;
    for (i = 0; i < len2; i++)
        mpf_sub(res + i, vec1 + i, vec2 + i);
}

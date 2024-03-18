/*
    Copyright (C) 2020 Daniel Schultz

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "fq_nmod_poly.h"
#include "n_poly.h"

void n_fq_poly_mulmod(
    n_fq_poly_t A,
    const n_fq_poly_t B,
    const n_fq_poly_t C,
    const n_fq_poly_t M,
    const fq_nmod_ctx_t ctx)
{
    fq_nmod_poly_t a, b, c, m;
    fq_nmod_poly_init(a, ctx);
    fq_nmod_poly_init(b, ctx);
    fq_nmod_poly_init(c, ctx);
    fq_nmod_poly_init(m, ctx);
    n_fq_poly_get_fq_nmod_poly(a, A, ctx);
    n_fq_poly_get_fq_nmod_poly(b, B, ctx);
    n_fq_poly_get_fq_nmod_poly(c, C, ctx);
    n_fq_poly_get_fq_nmod_poly(m, M, ctx);
    fq_nmod_poly_mulmod(a, b, c, m, ctx);
    n_fq_poly_set_fq_nmod_poly(A, a, ctx);
    fq_nmod_poly_clear(a, ctx);
    fq_nmod_poly_clear(b, ctx);
    fq_nmod_poly_clear(c, ctx);
    fq_nmod_poly_clear(m, ctx);
}

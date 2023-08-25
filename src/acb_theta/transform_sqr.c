/*
    Copyright (C) 2023 Jean Kieffer

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "acb_theta.h"

static void
acb_theta_transform_scal(acb_t scal, const fmpz_mat_t mat, acb_srcptr z,
    const acb_mat_t tau, slong k2, slong prec)
{
    slong g = sp2gz_dim(mat);
    fmpz_mat_t c;
    acb_mat_t w;
    acb_ptr Nz, v;
    acb_t mu, x;

    fmpz_mat_init(c, g, g);
    acb_mat_init(w, g, g);
    v = _acb_vec_init(g);
    Nz = _acb_vec_init(g);
    acb_init(mu);
    acb_init(x);

    acb_onei(mu);
    acb_pow_si(mu, mu, k2, prec);
    acb_siegel_cocycle_det(x, mat, tau, prec);
    acb_mul(scal, x, mu, prec);

    acb_siegel_transform_ext(Nz, w, mat, z, tau, prec);
    sp2gz_get_c(c, mat);
    acb_mat_set_fmpz_mat(w, c);
    acb_mat_vector_mul_col(v, w, z, prec);

    acb_dot(x, NULL, 0, v, 1, Nz, 1, g, prec);
    acb_mul_2exp_si(x, x, 1);
    acb_exp_pi_i(x, x, prec);
    acb_mul(scal, scal, x, prec);

    fmpz_mat_clear(c);
    acb_mat_clear(w);
    _acb_vec_clear(v, g);
    _acb_vec_clear(Nz, g);
    acb_clear(mu);
    acb_clear(x);
}

void
acb_theta_transform_sqr(acb_ptr res, const fmpz_mat_t mat, acb_srcptr th2,
    acb_srcptr z, const acb_mat_t tau, slong k2, slong prec)
{
    slong g = acb_mat_nrows(tau);
    slong n = 1 << g;
    acb_t scal;

    acb_init(scal);

    acb_theta_transform_scal(scal, mat, z, tau, k2, prec);
    acb_theta_transform_proj_sqr(res, mat, th2, prec);
    _acb_vec_scalar_mul(res, res, n * n, scal, prec);

    acb_clear(scal);
}

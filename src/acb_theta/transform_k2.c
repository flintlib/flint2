/*
    Copyright (C) 2023 Jean Kieffer

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "acb_theta.h"

static slong
get_power_of_i(const acb_t x)
{
    if (arb_is_positive(acb_realref(x)))
    {
        return 0;
    }
    else if (arb_is_positive(acb_imagref(x)))
    {
        return 1;
    }
    else if (arb_is_negative(acb_realref(x)))
    {
        return 2;
    }
    else if (arb_is_negative(acb_imagref(x)))
    {
        return 3;
    }
    else
    {
        return -1;
    }
}

slong
acb_theta_transform_k2(const fmpz_mat_t mat)
{
    slong g = sp2gz_dim(mat);
    fmpz_mat_t inv;
    acb_mat_t tau;
    acb_ptr z;
    acb_t scal1, scal2, t;
    fmpz_t eps;
    ulong ab;
    slong k2 = -1;
    slong prec = ACB_THETA_LOW_PREC;

    fmpz_mat_init(inv, 2 * g, 2 * g);
    acb_mat_init(tau, g, g);
    z = _acb_vec_init(g);
    fmpz_init(eps);
    acb_init(scal1);
    acb_init(scal2);
    acb_init(t);

    sp2gz_inv(inv, mat);
    ab = acb_theta_transform_char(eps, inv, 0);
    acb_theta_transform_char(eps, mat, ab);

    while (k2 == -1)
    {
        acb_mat_onei(tau);
        acb_theta_naive_00(scal1, z, 1, tau, prec);
        acb_sqr(scal1, scal1, prec);

        acb_siegel_cocycle_det(t, mat, tau, prec);
        acb_siegel_transform(tau, mat, tau, prec);
        acb_theta_naive_ind(scal2, ab, z, 1, tau, prec);
        acb_sqr(scal2, scal2, prec);

        acb_mul(scal1, scal1, t, prec);
        acb_onei(t);
        acb_pow_fmpz(t, t, eps, prec);
        acb_mul(scal1, scal1, t, prec);
        acb_div(scal1, scal2, scal1, prec);

        k2 = get_power_of_i(scal1);
        prec *= 2;
    }

    fmpz_mat_clear(inv);
    acb_mat_clear(tau);
    _acb_vec_clear(z, g);
    fmpz_clear(eps);
    acb_clear(scal1);
    acb_clear(scal2);
    acb_clear(t);
    return k2;
}

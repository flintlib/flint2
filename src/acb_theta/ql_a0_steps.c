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
acb_theta_ql_split(const arb_mat_t cho)
{
    slong g = arb_mat_nrows(cho);
    arb_t cmp;
    slong k;

    arb_init(cmp);

    for (k = g - 1; k >= 1; k--)
    {
        arb_mul_2exp_si(cmp, arb_mat_entry(cho, k - 1, k - 1),
            ACB_THETA_QL_SPLIT);
        if (arb_lt(cmp, arb_mat_entry(cho, k, k)))
        {
            break;
        }
    }

    arb_clear(cmp);
    return k;
}

static int
acb_theta_ql_a0_start(acb_ptr r, acb_srcptr t, acb_srcptr z, arb_srcptr dist0,
    arb_srcptr dist, slong sp, const acb_t f, slong nb_steps, const acb_mat_t tau,
    slong guard, slong prec, acb_theta_ql_worker_t worker)
{
    slong g = acb_mat_nrows(tau);
    slong n = 1 << g;
    int has_t = !_acb_vec_is_zero(t, g);
    int has_z = !_acb_vec_is_zero(z, g);
    slong nb_t = (has_t ? 3 : 1);
    acb_mat_t w;
    acb_ptr x, u, zero;
    arb_ptr d0, d;
    acb_t c;
    int res;

    acb_mat_init(w, g, g);
    x = _acb_vec_init(g);
    u = _acb_vec_init(g);
    zero = _acb_vec_init(g);
    d0 = _arb_vec_init(n);
    d = _arb_vec_init(n);
    acb_init(c);

    acb_mat_scalar_mul_2exp_si(w, tau, nb_steps);
    _acb_vec_scalar_mul_2exp_si(u, t, g, nb_steps);
    _acb_vec_scalar_mul_2exp_si(x, z, g, nb_steps);
    _arb_vec_scalar_mul_2exp_si(d0, dist0, n, nb_steps);
    _arb_vec_scalar_mul_2exp_si(d, dist, n, nb_steps);
    acb_mul_2exp_si(c, f, nb_steps);
    acb_exp_pi_i(c, c, prec);

    if (sp > 0)
    {
        res = acb_theta_ql_a0_split(r, u, zero, d0, w, sp, guard, prec, worker);
        if (res && has_z)
        {
            res = acb_theta_ql_a0_split(r + nb_t * n, u, x, d, w, sp, guard, prec, worker);
        }
    }
    else
    {
        res = acb_theta_ql_a0_naive(r, u, x, d0, d, w, guard, prec);
    }

    if (has_z)
    {
        _acb_vec_scalar_mul(r + nb_t * n, r + nb_t * n, n * nb_t, c, prec);
    }

    acb_mat_clear(w);
    _acb_vec_clear(x, g);
    _acb_vec_clear(u, g);
    _acb_vec_clear(zero, g);
    _arb_vec_clear(d0, n);
    _arb_vec_clear(d, n);
    acb_clear(c);
    return res;
}

static void
acb_theta_ql_a0_step(acb_ptr r, acb_srcptr roots, arb_srcptr dist0, arb_srcptr dist,
    slong k, slong nb_steps, int has_t, int has_z, slong g, slong prec)
{
    slong n = 1 << g;
    arb_ptr d, d0;
    acb_ptr next;
    acb_ptr rts;
    slong nb_t = (has_t ? 3 : 1);
    slong nb_r = (has_t ? 2 : 1);
    slong nb_z = (has_z ? 2 : 1);
    slong j;

    d = _arb_vec_init(n);
    d0 = _arb_vec_init(n);
    next = _acb_vec_init(nb_z * nb_t * n);
    rts = _acb_vec_init(nb_r * nb_z * n);

    _arb_vec_scalar_mul_2exp_si(d, dist, n, k + 1);
    _arb_vec_scalar_mul_2exp_si(d0, dist0, n, k + 1);
    for (j = 0; j < nb_z * nb_r; j++)
    {
        _acb_vec_set(rts + j * n, roots + j * nb_steps * n + k * n, n);
    }

    if (has_t)
    {
        acb_theta_ql_step_3(next, r, r, rts, d0, d0, g, prec);
        if (has_z)
        {
            acb_theta_ql_step_3(next + nb_t * n, r, r + nb_t * n,
                rts + nb_r * n, d0, d, g, prec);
        }
    }
    else
    {
        acb_theta_ql_step_1(next, r, r, rts, d0, d0, g, prec);
        if (has_z)
        {
            acb_theta_ql_step_1(next + nb_t * n, r, r + nb_t * n,
                rts + nb_r * n, d0, d, g, prec);
        }
    }
    _acb_vec_set(r, next, nb_z * nb_t * n);

    _arb_vec_clear(d, n);
    _arb_vec_clear(d0, n);
    _acb_vec_clear(next, nb_z * nb_t * n);
    _acb_vec_clear(rts, nb_r * nb_z * n);
}

int
acb_theta_ql_a0_steps(acb_ptr r, acb_srcptr t, acb_srcptr z, arb_srcptr dist,
    arb_srcptr dist0, const acb_mat_t tau, slong guard, slong prec,
    acb_theta_ql_worker_t worker)
{
    slong g = acb_mat_nrows(tau);
    slong n = 1 << g;
    int has_t = !_acb_vec_is_zero(t, g);
    int has_z = !_acb_vec_is_zero(z, g);
    slong nb_t = (has_t ? 3 : 1);
    slong nb_r = (has_t ? 2 : 1);
    slong nb_z = (has_z ? 2 : 1);
    arb_mat_t cho;
    acb_ptr x, roots;
    acb_t f, c;
    slong sp, nb_steps;
    slong k;
    int res = 1;

    arb_mat_init(cho, g, g);
    x = _acb_vec_init(g);
    acb_init(f);
    acb_init(c);

    acb_theta_eld_cho(cho, tau, ACB_THETA_LOW_PREC);
    sp = acb_theta_ql_split(cho);
    nb_steps = acb_theta_ql_nb_steps(cho, sp, prec);
    roots = _acb_vec_init(nb_z * nb_r * n * nb_steps);

    /* flint_printf("(ql_a0_steps) sp =  %wd, has_z = %wd, has_t = %wd, cho:\n", */
    /*     sp, has_z, has_t); */
    /* arb_mat_printd(cho, 5); */
    /* flint_printf("(ql_a0_steps) Using nb_steps = %wd\n", nb_steps); */

    acb_theta_ql_log_rescale(f, z, tau, prec);
    res = acb_theta_ql_roots(roots, t, z, dist0, dist, tau, nb_steps, guard, prec);

    if (res)
    {
        res = acb_theta_ql_a0_start(r, t, z, dist0, dist, sp, f, nb_steps, tau,
            guard, prec, worker);
        /* flint_printf("(ql_a0_steps) start:\n"); */
        /* _acb_vec_printd(r, nb_z * nb_t * n, 5); */
        /* flint_printf("\n"); */
    }

    if (res)
    {
        for (k = nb_steps - 1; k >= 0; k--)
        {
            acb_theta_ql_a0_step(r, roots, dist0, dist, k, nb_steps, has_t, has_z, g, prec);
            /* flint_printf("after step %wd\n", k); */
            /* _acb_vec_printd(r, nb_z * nb_t * n, 5); */
            /* flint_printf("\n"); */
        }
    }

    if (res && has_z)
    {
        acb_neg(c, f);
        acb_exp_pi_i(c, c, prec);
        _acb_vec_scalar_mul(r + nb_t * n, r + nb_t * n, n * nb_t, c, prec);
    }

    arb_mat_clear(cho);
    _acb_vec_clear(x, g);
    _acb_vec_clear(roots, nb_z * nb_r * n * nb_steps);
    acb_clear(f);
    acb_clear(c);
    return res;
}
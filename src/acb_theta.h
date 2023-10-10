/*
    Copyright (C) 2023 Jean Kieffer

    This file is part of Arb.

    Arb is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#ifndef ACB_THETA_H
#define ACB_THETA_H

#include <stdio.h>
#include <math.h>
#include "ulong_extras.h"
#include "fmpz.h"
#include "fmpz_mat.h"
#include "fmpz_lll.h"
#include "arb.h"
#include "acb.h"
#include "acb_poly.h"
#include "arb_mat.h"
#include "acb_mat.h"
#include "acb_modular.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ACB_THETA_LOW_PREC 32

/* The Siegel modular group */

static __inline__ slong
sp2gz_dim(const fmpz_mat_t mat)
{
    return fmpz_mat_nrows(mat) / 2;
}

void sp2gz_get_a(fmpz_mat_t res, const fmpz_mat_t mat);
void sp2gz_get_b(fmpz_mat_t res, const fmpz_mat_t mat);
void sp2gz_get_c(fmpz_mat_t res, const fmpz_mat_t mat);
void sp2gz_get_d(fmpz_mat_t res, const fmpz_mat_t mat);
void sp2gz_set_abcd(fmpz_mat_t mat, const fmpz_mat_t a, const fmpz_mat_t b,
    const fmpz_mat_t c, const fmpz_mat_t d);

void sp2gz_j(fmpz_mat_t mat);
void sp2gz_block_diag(fmpz_mat_t mat, const fmpz_mat_t U);
void sp2gz_trig(fmpz_mat_t mat, const fmpz_mat_t S);
slong sp2gz_nb_fundamental(slong g);
void sp2gz_fundamental(fmpz_mat_t mat, slong j);

void sp2gz_inv(fmpz_mat_t inv, const fmpz_mat_t mat);
int sp2gz_is_correct(const fmpz_mat_t mat);
void sp2gz_randtest(fmpz_mat_t mat, flint_rand_t state, slong bits);

/* The Siegel half space */

void acb_siegel_cocycle(acb_mat_t c, const fmpz_mat_t mat, const acb_mat_t tau, slong prec);
void acb_siegel_transform_cocycle_inv(acb_mat_t w, acb_mat_t c, acb_mat_t cinv,
    const fmpz_mat_t mat, const acb_mat_t tau, slong prec);
void acb_siegel_transform(acb_mat_t w, const fmpz_mat_t mat, const acb_mat_t tau, slong prec);
void acb_siegel_transform_z(acb_ptr r, acb_mat_t w, const fmpz_mat_t mat,
    acb_srcptr z, const acb_mat_t tau, slong prec);

void acb_siegel_reduce(fmpz_mat_t mat, const acb_mat_t tau, slong prec);

void acb_siegel_randtest(acb_mat_t tau, flint_rand_t state, slong prec, slong mag_bits);
void acb_siegel_randtest_reduced(acb_mat_t tau, flint_rand_t state, slong prec, slong mag_bits);
void acb_siegel_randtest_nice(acb_mat_t tau, flint_rand_t state, slong prec);

/* Theta characteristics */

void acb_theta_char_get_slong(slong* n, ulong a, slong g);
ulong acb_theta_char_get_a(const slong* n, slong g);
void acb_theta_char_get_arb(arb_ptr v, ulong a, slong g);
void acb_theta_char_get_acb(acb_ptr v, ulong a, slong g);

slong acb_theta_char_dot(ulong a, ulong b, slong g);
slong acb_theta_char_dot_slong(ulong a, const slong* n, slong g);
void acb_theta_char_dot_acb(acb_t x, ulong a, acb_srcptr z, slong g, slong prec);

int acb_theta_char_is_even(ulong ab, slong g);
int acb_theta_char_is_goepel(ulong ch1, ulong ch2, ulong ch3, ulong ch4, slong g);
int acb_theta_char_is_syzygous(ulong ch1, ulong ch2, ulong ch3, slong g);

/* Ellipsoids in naive algorithms */

struct acb_theta_eld_struct
{
    slong dim, ambient_dim;
    slong* last_coords;
    slong min, mid, max, nr, nl;
    struct acb_theta_eld_struct* rchildren;
    struct acb_theta_eld_struct* lchildren;
    slong nb_pts, nb_border;
    slong* box;
};

typedef struct acb_theta_eld_struct acb_theta_eld_t[1];

#define acb_theta_eld_dim(E) ((E)->dim)
#define acb_theta_eld_ambient_dim(E) ((E)->ambient_dim)
#define acb_theta_eld_coord(E, k) ((E)->last_coords[(k) - acb_theta_eld_dim(E)])
#define acb_theta_eld_min(E) ((E)->min)
#define acb_theta_eld_mid(E) ((E)->mid)
#define acb_theta_eld_max(E) ((E)->max)
#define acb_theta_eld_nr(E) ((E)->nr)
#define acb_theta_eld_nl(E) ((E)->nl)
#define acb_theta_eld_rchild(E, k) (&(E)->rchildren[(k)])
#define acb_theta_eld_lchild(E, k) (&(E)->lchildren[(k)])
#define acb_theta_eld_nb_pts(E) ((E)->nb_pts)
#define acb_theta_eld_nb_border(E) ((E)->nb_border)
#define acb_theta_eld_box(E, k) ((E)->box[(k)])

void acb_theta_eld_init(acb_theta_eld_t E, slong d, slong g);
void acb_theta_eld_clear(acb_theta_eld_t E);

void acb_theta_eld_interval(slong* min, slong* mid, slong* max, const arb_t ctr, const arf_t rad);
void acb_theta_eld_cho(arb_mat_t C, const acb_mat_t tau, slong prec);
void acb_theta_eld_fill(acb_theta_eld_t E, const arb_mat_t C, const arf_t R2, arb_srcptr v);
void acb_theta_eld_points(slong* pts, const acb_theta_eld_t E);
void acb_theta_eld_border(slong* pts, const acb_theta_eld_t E);
int acb_theta_eld_contains(const acb_theta_eld_t E, slong* pt);
void acb_theta_eld_print(const acb_theta_eld_t E);

/* Precomputations in naive algorithms */

typedef struct
{
    slong dim, nb;
    acb_mat_struct exp_mat;
    acb_ptr sqr_powers;
    slong* indices;
    acb_ptr exp_z;
} acb_theta_precomp_struct;

typedef acb_theta_precomp_struct acb_theta_precomp_t[1];

#define acb_theta_precomp_dim(D) ((D)->dim)
#define acb_theta_precomp_nb(D) ((D)->nb)
#define acb_theta_precomp_exp_mat(D) (&(D)->exp_mat)
#define acb_theta_precomp_sqr_pow(D, k, j) (&(D)->sqr_powers[(j) + (D)->indices[(k)]])
#define acb_theta_precomp_exp_z(D, k, j) (&(D)->exp_z[(k) * (D)->dim + (j)])

void acb_theta_precomp_init(acb_theta_precomp_t D, slong nb, slong g);
void acb_theta_precomp_clear(acb_theta_precomp_t D);
void acb_theta_precomp_set(acb_theta_precomp_t D, acb_srcptr zs,
    const acb_mat_t tau, const acb_theta_eld_t E, slong prec);

/* Naive algorithms */

void acb_theta_naive_radius(arf_t R2, arf_t eps, const arb_mat_t C, slong ord, slong prec);
void acb_theta_naive_reduce(arb_ptr v, acb_ptr new_zs, acb_ptr cs, arb_ptr us,
    acb_srcptr zs, slong nb, const acb_mat_t tau, const arb_mat_t C, slong prec);
void acb_theta_naive_ellipsoid(acb_theta_eld_t E, acb_ptr new_zs, acb_ptr cs,
    arb_ptr us, acb_srcptr zs, slong nb, const acb_mat_t tau, slong prec);
slong acb_theta_naive_fullprec(const acb_theta_eld_t E, slong prec);
void acb_theta_naive_term(acb_t res, acb_srcptr z, const acb_mat_t tau, slong* tup,
    slong* n, slong prec);

typedef void (*acb_theta_naive_worker_t)(acb_ptr, acb_srcptr, acb_srcptr, const slong*,
    slong, const acb_t, const slong*, slong, slong, slong, slong);

void acb_theta_naive_worker(acb_ptr th, slong len, const acb_t c, const arb_t u,
    const acb_theta_eld_t E, const acb_theta_precomp_t D, slong k, slong ord,
    slong prec, acb_theta_naive_worker_t worker);

void acb_theta_naive_00(acb_ptr th, acb_srcptr zs, slong nb, const acb_mat_t tau, slong prec);
void acb_theta_naive_0b(acb_ptr th, acb_srcptr zs, slong nb, const acb_mat_t tau, slong prec);

void acb_theta_naive_fixed_ab(acb_ptr th, ulong ab, acb_srcptr zs, slong nb,
    const acb_mat_t tau, slong prec);
void acb_theta_naive_fixed_a(acb_ptr th, ulong a, acb_srcptr zs, slong nb,
    const acb_mat_t tau, slong prec);
void acb_theta_naive_all(acb_ptr th, acb_srcptr zs, slong nb, const acb_mat_t tau, slong prec);

/* Naive algorithms for derivatives */

slong acb_theta_jet_nb(slong ord, slong g);
slong acb_theta_jet_total_order(const slong* tup, slong g);
void acb_theta_jet_tuples(slong* tups, slong ord, slong g);
slong acb_theta_jet_index(const slong* tup, slong g);

void acb_theta_jet_naive_radius(arf_t R2, arf_t eps, const arb_mat_t C, arb_srcptr v,
    slong ord, slong prec);
void acb_theta_jet_ellipsoid(acb_theta_eld_t E, arb_t u, acb_srcptr z,
    const acb_mat_t tau, slong ord, slong prec);

void acb_theta_jet_naive_00(acb_ptr dth, acb_srcptr z, const acb_mat_t tau,
    slong ord, slong prec);
void acb_theta_jet_naive_fixed_ab(acb_ptr dth, ulong ab, acb_srcptr z, const acb_mat_t tau,
    slong ord, slong prec);
void acb_theta_jet_naive_all(acb_ptr dth, acb_srcptr z, const acb_mat_t tau,
    slong ord, slong prec);

void acb_theta_jet_error_bounds(arb_ptr err, acb_srcptr z, const acb_mat_t tau,
    acb_srcptr dth, slong ord, slong prec);

/* Quasi-linear algorithms on the reduced domain */

void acb_theta_dist_pt(arb_t d, arb_srcptr v, const arb_mat_t C, slong* n, slong prec);
void acb_theta_dist_lat(arb_t d, arb_srcptr v, const arb_mat_t C, slong prec);
void acb_theta_dist_a0(arb_ptr d, acb_srcptr z, const acb_mat_t tau, slong prec);
slong acb_theta_dist_addprec(const arb_t d);

void acb_theta_agm_hadamard(acb_ptr res, acb_srcptr a, slong g, slong prec);
void acb_theta_agm_sqrt(acb_ptr res, acb_srcptr a, acb_srcptr roots, slong nb, slong prec);
void acb_theta_agm_mul(acb_ptr res, acb_srcptr a1, acb_srcptr a2, slong g, slong prec);
void acb_theta_agm_rel_mag_err(arf_t m, arf_t eps, acb_srcptr a, arb_srcptr d,
    slong nb, slong prec);
void acb_theta_agm_mul_tight(acb_ptr res, acb_srcptr a0, acb_srcptr a,
    arb_srcptr d0, arb_srcptr d, slong g, slong prec);

slong acb_theta_ql_nb_steps(const arb_mat_t C, slong s, slong prec);
void acb_theta_ql_log_rescale(acb_t res, acb_srcptr z, const acb_mat_t tau, slong prec);
int acb_theta_ql_roots(acb_ptr rts, acb_srcptr t, acb_srcptr z, arb_srcptr d0,
    arb_srcptr d, const acb_mat_t tau, slong nb_steps, slong guard, slong prec);
void acb_theta_ql_step_1(acb_ptr res, acb_srcptr th0, acb_srcptr th,
    acb_srcptr roots, arb_srcptr d0, arb_srcptr d, slong g, slong prec);
void acb_theta_ql_step_3(acb_ptr res, acb_srcptr th0, acb_srcptr th,
    acb_srcptr roots, arb_srcptr d0, arb_srcptr d, slong g, slong prec);
void acb_theta_ql_step_2(acb_ptr res, acb_srcptr th0, acb_srcptr th,
    acb_srcptr roots, arb_srcptr d0, arb_srcptr d, slong g, slong prec);
void acb_theta_ql_dupl(acb_ptr th2, acb_srcptr th0, acb_srcptr th,
    arb_srcptr d0, arb_srcptr d, slong g, slong prec);

typedef int (*acb_theta_ql_worker_t)(acb_ptr, acb_srcptr, acb_srcptr,
    arb_srcptr, arb_srcptr, const acb_mat_t, slong, slong);

int acb_theta_ql_a0_naive(acb_ptr th, acb_srcptr t, acb_srcptr z, arb_srcptr d0,
    arb_srcptr d, const acb_mat_t tau, slong guard, slong prec);
int acb_theta_ql_a0_split(acb_ptr th, acb_srcptr t, acb_srcptr z, arb_srcptr d,
    const acb_mat_t tau, slong s, slong guard, slong prec, acb_theta_ql_worker_t worker);
int acb_theta_ql_a0_steps(acb_ptr th, acb_srcptr t, acb_srcptr z, arb_srcptr d0,
    arb_srcptr d, const acb_mat_t tau, slong nb_steps, slong s, slong guard,
    slong prec, acb_theta_ql_worker_t worker);
int acb_theta_ql_a0(acb_ptr th, acb_srcptr t, acb_srcptr z, arb_srcptr d0,
    arb_srcptr d, const acb_mat_t tau, slong guard, slong prec);

#define ACB_THETA_QL_TRY 100

slong acb_theta_ql_reduce(acb_ptr x, acb_t c, arb_t u, ulong* a1, acb_srcptr z,
    const acb_mat_t tau, slong prec);
void acb_theta_ql_all(acb_ptr th, acb_srcptr z, const acb_mat_t tau, slong prec);
void acb_theta_ql_all_sqr(acb_ptr th2, acb_srcptr z, const acb_mat_t tau, slong prec);

/* Transformation formulas */

ulong acb_theta_transform_char(slong* e, const fmpz_mat_t mat, ulong ab);
slong acb_theta_transform_kappa(const fmpz_mat_t mat);
void acb_theta_transform_sqrtdet(acb_t res, const fmpz_mat_t mat, const acb_mat_t tau, slong prec);
void acb_theta_transform_proj(acb_ptr res, const fmpz_mat_t mat, acb_srcptr th,
    int sqr, slong prec);
void acb_theta_transform(acb_ptr res, const fmpz_mat_t mat, acb_srcptr th,
    acb_srcptr z, const acb_mat_t tau, slong kappa, int sqr, slong prec);

void acb_theta_all(acb_ptr th, acb_srcptr z, const acb_mat_t tau, int sqr, slong prec);

/* Quasi-linear algorithms for derivatives */

void acb_theta_jet_bounds(arb_t c, arb_t rho, acb_srcptr z, const acb_mat_t tau, slong ord);
void acb_theta_jet_fd_radius(arf_t eps, arf_t err, const arb_t c, const arb_t rho,
    slong ord, slong g, slong prec);
void acb_theta_jet_fourier(acb_ptr res, acb_srcptr val, slong ord, slong g, slong prec);
void acb_theta_jet_fd(acb_ptr dth, const arf_t eps, const arf_t err, acb_srcptr val,
    slong ord, slong g, slong prec);

void acb_theta_jet_all(acb_ptr dth, acb_srcptr z, const acb_mat_t tau, slong ord, slong prec);

/* Genus 2 specifics */

#define ACB_THETA_G2_COV_NB 26

void acb_theta_g2_jet_naive_1(acb_ptr dth, const acb_mat_t tau, slong prec);
void acb_theta_g2_detk_symj(acb_poly_t res, const acb_mat_t m, const acb_poly_t f,
    slong k, slong j, slong prec);
void acb_theta_g2_transvectant(acb_poly_t res, const acb_poly_t g, const acb_poly_t h,
    slong m, slong n, slong k, slong prec);
void acb_theta_g2_transvectant_lead(acb_t r, const acb_poly_t g, const acb_poly_t h,
    slong m, slong n, slong k, slong prec);

void acb_theta_g2_psi4(acb_t res, acb_srcptr th2, slong prec);
void acb_theta_g2_psi6(acb_t res, acb_srcptr th2, slong prec);
void acb_theta_g2_chi10(acb_t res, acb_srcptr th2, slong prec);
void acb_theta_g2_chi12(acb_t res, acb_srcptr th2, slong prec);
void acb_theta_g2_chi5(acb_t res, acb_srcptr th, slong prec);
void acb_theta_g2_chi35(acb_t res, acb_srcptr th, slong prec);
void acb_theta_g2_chi3_6(acb_poly_t res, acb_srcptr dth, slong prec);

void acb_theta_g2_sextic(acb_poly_t res, const acb_mat_t tau, slong prec);
void acb_theta_g2_covariants(acb_poly_struct* res, const acb_poly_t f, slong prec);
void acb_theta_g2_covariants_lead(acb_ptr res, const acb_poly_t f, slong prec);

#ifdef __cplusplus
}
#endif

#endif
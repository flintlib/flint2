/*
    Copyright (C) 2020 Daniel Schultz
    Copyright (C) 2022 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "test_helpers.h"
#include "fmpz_vec.h"
#include "gr_mpoly.h"

TEST_FUNCTION_START(gr_mpoly_get_set_coeff, state)
{
    slong i, j;

    for (i = 0; i < 100; i++)
    {
        gr_ctx_t cctx;
        gr_mpoly_ctx_t ctx;
        gr_mpoly_t f;
        gr_ptr c, d;
        slong len, k;
        flint_bitcnt_t exp_bits;
        int status;
        slong nvars;

        gr_ctx_init_random(cctx, state);
        gr_mpoly_ctx_init_rand(ctx, state, cctx, 20);
        nvars = GR_MPOLY_NVARS(ctx);

        NEW_gr_mpoly_init(f, ctx);
        c = gr_heap_init(cctx);
        d = gr_heap_init(cctx);

        len = n_randint(state, 100);
        exp_bits = n_randint(state, 200) + 2;

        GR_MUST_SUCCEED(NEW_gr_mpoly_randtest_bits(f, state, len, exp_bits, ctx));

        for (j = 0; j < 10; j++)
        {
            ulong * exp = FLINT_ARRAY_ALLOC(nvars, ulong);

            status = GR_SUCCESS;

            GR_MUST_SUCCEED(gr_randtest(c, state, cctx));
            for (k = 0; k < nvars; k++)
                exp[k] = n_randtest(state);

            status |= gr_mpoly_set_coeff_scalar_ui(f, c, exp, ctx);
            NEW_gr_mpoly_assert_canonical(f, ctx);
            status |= gr_mpoly_get_coeff_scalar_ui(d, f, exp, ctx);

            if (status == GR_SUCCESS && gr_equal(c, d, cctx) == T_FALSE)
            {
                flint_printf("FAIL: scalar_ui\n");
                flint_printf("i = %wd, j = %wd\n", i ,j);
                gr_ctx_println(ctx);
                flint_printf("f = "); NEW_gr_mpoly_print_pretty(f, NULL, ctx); flint_printf("\n");
                flint_printf("c = "); gr_print(c, cctx); flint_printf("\n");
                flint_printf("d = "); gr_print(d, cctx); flint_printf("\n");
                fflush(stdout);
                flint_abort();
            }

            flint_free(exp);
        }

        GR_MUST_SUCCEED(NEW_gr_mpoly_randtest_bits(f, state, len, exp_bits, ctx));

        for (j = 0; j < 10; j++)
        {
            fmpz * exp = _fmpz_vec_init(nvars);

            status = GR_SUCCESS;

            GR_MUST_SUCCEED(gr_randtest(c, state, cctx));
            _fmpz_vec_randtest(exp, state, nvars, exp_bits);

            status |= gr_mpoly_set_coeff_scalar_fmpz(f, c, exp, ctx);
            NEW_gr_mpoly_assert_canonical(f, ctx);
            status |= gr_mpoly_get_coeff_scalar_fmpz(d, f, exp, ctx);

            if (status == GR_SUCCESS && gr_equal(c, d, cctx) == T_FALSE)
            {
                flint_printf("FAIL: scalar_fmpz\n");
                flint_printf("i = %wd, j = %wd\n", i ,j);
                gr_ctx_println(ctx);
                flint_printf("f = "); NEW_gr_mpoly_print_pretty(f, NULL, ctx); flint_printf("\n");
                flint_printf("c = "); gr_print(c, cctx); flint_printf("\n");
                flint_printf("d = "); gr_print(d, cctx); flint_printf("\n");
                fflush(stdout);
                flint_abort();
            }

            _fmpz_vec_clear(exp, nvars);
        }

        NEW_gr_mpoly_clear(f, ctx);
        gr_heap_clear(c, cctx);
        gr_heap_clear(d, cctx);

        gr_mpoly_ctx_clear(ctx);
        gr_ctx_clear(cctx);
    }

    TEST_FUNCTION_END(state);
}

/* 
    Copyright (C) 2009, 2011 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <time.h>
#include "flint.h"
#include "ulong_extras.h"
#include "fft.h"

int
main(void)
{
    flint_bitcnt_t depth, w, depth1, w1;
    clock_t start, end;
    double elapsed;
    double best = 0.0;
    mp_size_t best_off, off, best_d, best_w;

    FLINT_TEST_INIT(state);

    flint_printf("/* fft_tuning.h -- autogenerated by tune-fft */\n\n");
    flint_printf("#ifndef FFT_TUNING_H\n");
    flint_printf("#define FFT_TUNING_H\n\n");
    flint_printf("#include \"gmp.h\"\n\n");
    flint_printf("#define FFT_TAB \\\n");
    fflush(stdout);

    
    _flint_rand_init_gmp(state);

    flint_printf("   { "); fflush(stdout);
    for (depth = 6; depth <= 10; depth++)
    {
        flint_printf("{ "); fflush(stdout);
        for (w = 1; w <= 2; w++)
        {
            int iters = 100*((mp_size_t) 1 << (3*(10 - depth)/2)), i;
            
            mp_size_t n = (UWORD(1)<<depth);
            flint_bitcnt_t bits1 = (n*w - (depth + 1))/2; 
            mp_size_t len1 = 2*n;
            mp_size_t len2 = 2*n;

            flint_bitcnt_t b1 = len1*bits1, b2 = len2*bits1;
            mp_size_t n1, n2;
            mp_limb_t * i1, *i2, *r1;
   
            n1 = (b1 - 1)/FLINT_BITS + 1;
            n2 = (b2 - 1)/FLINT_BITS + 1;
                    
            i1 = flint_malloc(2*(n1 + n2)*sizeof(mp_limb_t));
            i2 = i1 + n1;
            r1 = i2 + n2;
   
            flint_mpn_urandomb(i1, state->gmp_state, b1);
            flint_mpn_urandomb(i2, state->gmp_state, b2);
  
            best_off = -1;
            
            for (off = 0; off <= 4; off++)
            {
               start = clock();
               for (i = 0; i < iters; i++)
                  mul_truncate_sqrt2(r1, i1, n1, i2, n2, depth - off, w*((mp_size_t)1 << (off*2)));
               end = clock();
               
               elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
               
               if (elapsed < best || best_off == -1)
               {
                  best_off = off;
                  best = elapsed;
               }
            }
           
            flint_printf("%wd", best_off); 
            if (w != 2) flint_printf(",");
            flint_printf(" "); fflush(stdout);

            flint_free(i1);
        }
        flint_printf("}");
        if (depth != 10) flint_printf(",");
        flint_printf(" "); fflush(stdout);
    }

    flint_printf("}\n\n");
    
    best_d = 12;
    best_w = 1;
    best_off = -1;

    flint_printf("#define MULMOD_TAB \\\n");
    fflush(stdout);
    flint_printf("   { "); fflush(stdout);
    for (depth = 12; best_off != 1 ; depth++)
    {
        for (w = 1; w <= 2; w++)
        {
            int iters, i;
            mp_size_t n = (UWORD(1)<<depth);
            flint_bitcnt_t bits = n*w;
            mp_size_t int_limbs = (bits - 1)/FLINT_BITS + 1;
            mp_limb_t * i1, * i2, * r1, * tt;
        
            if (depth <= 21) iters = 32*((mp_size_t) 1 << (21 - depth));
            else iters = FLINT_MAX(32/((mp_size_t) 1 << (depth - 21)), 1);

            i1 = flint_malloc(6*(int_limbs+1)*sizeof(mp_limb_t));
            i2 = i1 + int_limbs + 1;
            r1 = i2 + int_limbs + 1;
            tt = r1 + 2*(int_limbs + 1);
                
            flint_mpn_urandomb(i1, state->gmp_state, int_limbs*FLINT_BITS);
            flint_mpn_urandomb(i2, state->gmp_state, int_limbs*FLINT_BITS);
            i1[int_limbs] = 0;
            i2[int_limbs] = 0;

            depth1 = FLINT_CLOG2(bits);
            depth1 = depth1/2;

            w1 = bits/(UWORD(1)<<(2*depth1));

            best_off = -1;
            
            for (off = 0; off <= 4; off++)
            {
               start = clock();
               for (i = 0; i < iters; i++)
                  _fft_mulmod_2expp1(r1, i1, i2, int_limbs, depth1 - off, w1*((mp_size_t)1 << (off*2)));
               end = clock();
               
               elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
               
               if (best_off == -1 || elapsed < best)
               {
                  best_off = off;
                  best = elapsed;
               }
            }
           
            start = clock();
            for (i = 0; i < iters; i++)
                flint_mpn_mulmod_2expp1_basecase(r1, i1, i2, 0, bits, tt);
            end = clock();
               
            elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
            if (elapsed < best)
            {
                best_d = depth + (w == 2);
                best_w = w + 1 - 2*(w == 2);
            }

            flint_printf("%wd", best_off); 
            if (w != 2) flint_printf(", ");
            fflush(stdout);

            flint_free(i1);
        }
        flint_printf(", "); fflush(stdout);
    }
    flint_printf("1 }\n\n");
    
    flint_printf("#define FFT_N_NUM %wd\n\n", 2*(depth - 12) + 1);
    
    flint_printf("#define FFT_MULMOD_2EXPP1_CUTOFF %wd\n\n", ((mp_limb_t) 1 << best_d)*best_w/(2*FLINT_BITS));
    
    flint_randclear(state);
    
    flint_printf("#endif\n");
    return 0;
}

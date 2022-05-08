/* 
    Copyright (C) 2009, 2011 William Hart

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include "gmp.h"
#include "flint.h"
#include "fft.h"
      
void fft_negacyclic(ulong ** ii, mp_size_t n, flint_bitcnt_t w, 
                    ulong ** t1, ulong ** t2, ulong ** temp)
{
   mp_size_t i;
   mp_size_t limbs = (w*n)/FLINT_BITS;
   
   /* first apply twiddle factors corresponding to shifts of w*i/2 bits */
   if (w & 1)
   {
      for (i = 0; i < n; i++) 
      {   
          fft_adjust(*t1, ii[i], i/2, limbs, w);
          SWAP_PTRS(ii[i], *t1);
            
          fft_adjust(*t2, ii[n+i], (n+i)/2, limbs, w);
          SWAP_PTRS(ii[n+i], *t2);

          fft_butterfly(*t1, *t2, ii[i], ii[n+i], i, limbs, w);
          SWAP_PTRS(ii[i],   *t1);
          SWAP_PTRS(ii[n+i], *t2);

          i++;
          
          fft_adjust_sqrt2(*t1, ii[i], i, limbs, w, *temp);
          SWAP_PTRS(ii[i], *t1);
          
          fft_adjust_sqrt2(*t2, ii[n+i], n+i, limbs, w, *temp);
          SWAP_PTRS(ii[n+i], *t2);
          
          fft_butterfly(*t1, *t2, ii[i], ii[n+i], i, limbs, w);
          SWAP_PTRS(ii[i],   *t1);
          SWAP_PTRS(ii[n+i], *t2);
       }
   } else
   {
       for (i = 0; i < n; i++) 
       {   
          fft_adjust(*t1, ii[i], i, limbs, w/2);
          SWAP_PTRS(ii[i], *t1);
            
          fft_adjust(*t2, ii[n+i], n+i, limbs, w/2);
          SWAP_PTRS(ii[n+i], *t2);
      
          fft_butterfly(*t1, *t2, ii[i], ii[n+i], i, limbs, w);
          SWAP_PTRS(ii[i],   *t1);
          SWAP_PTRS(ii[n+i], *t2);
       }
   }

   fft_radix2(ii, n/2, 2*w, t1, t2);
   fft_radix2(ii+n, n/2, 2*w, t1, t2);
}

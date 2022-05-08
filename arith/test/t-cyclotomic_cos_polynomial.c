/*
    Copyright (C) 2011 Fredrik Johansson

    This file is part of FLINT.

    FLINT is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <mpfr.h>
#include "flint.h"
#include "arith.h"
#include "fmpz.h"
#include "fmpz_poly.h"


/*
Generated with Mathematica:
Table[Mod[MinimalPolynomial[Cos[2 Pi/n]][1337], 31337], {n,1,500}]
*/

static const short testdata[] = {
    1,
    1336, 1338, 2675, 1337, 8113, 2673, 6283, 2719, 29508, 2765, 6949,
    5437, 2788, 26742, 25554, 26194, 29376, 29506, 30945, 15614, 8957,
    16643, 9263, 21050, 30556, 10533, 1570, 11562, 3988, 16546, 26642, 4041,
    3581, 109, 9839, 27175, 11691, 1460, 28287, 18369, 16503, 3184, 13336,
    23083, 12495, 3246, 14160, 8081, 5301, 8652, 28989, 24149, 17733, 1568,
    4800, 28863, 29280, 13741, 30919, 29819, 28584, 8913, 550, 6207, 13930,
    23373, 12644, 15265, 27975, 30386, 1603, 15894, 22276, 3138, 11610,
    2208, 515, 30817, 23050, 4333, 25031, 13615, 5116, 18609, 25490, 14555,
    22663, 8425, 21751, 19293, 3, 10688, 26829, 14467, 1426, 12413, 5305,
    25377, 27164, 3711,
    9613, 22340, 7457, 3704, 1795, 22877, 31060, 17472, 11317, 22274,
    11036, 7796, 27242, 22174, 3663, 10507, 16599, 18192, 15208, 7257, 7022,
    10810, 27891, 18495, 7032, 11383, 20768, 27351, 31089, 27723, 10486,
    2075, 25298, 20531, 28548, 25342, 6510, 20657, 15608, 5534, 22145,
    30150, 25222, 12128, 389, 21860, 9631, 4536, 4704, 3677, 27282, 26668,
    20784, 15684, 12847, 1307, 10586, 24355, 27553, 10952, 8886, 25029,
    29278, 29964, 17943, 1006, 5895, 11466, 16679, 17500, 5414, 3420, 17644,
    5165, 6255, 2807, 30577, 26277, 14032, 2425, 13945, 27988, 17437, 28204,
    11853, 12265, 8097, 24919, 10703, 18081, 19121, 23364, 14035, 2382,
    1722, 21617, 11863, 27682, 8538, 26401,
    1487, 14570, 14213, 18315, 30244, 14611, 25421, 13954, 29802,
    29118, 5788, 7547, 9710, 21645, 17858, 20672, 2295, 21286, 7217, 30405,
    5090, 22674, 5747, 5809, 13789, 16385, 23732, 12258, 10944, 14669, 2043,
    1453, 13510, 12422, 24073, 3025, 28094, 2770, 9198, 27411, 24736, 28958,
    23508, 27897, 17838, 10690, 5375, 29469, 22458, 9466, 28541, 16308,
    20491, 10320, 9836, 673, 26630, 20819, 25687, 19263, 16620, 28683,
    30268, 1113, 26632, 18450, 17555, 20121, 18083, 12796, 26659, 9788,
    10448, 2828, 29753, 26653, 13636, 6270, 10398, 16224, 1481, 1153, 26387,
    17835, 19289, 2683, 1937, 16760, 14372, 12632, 15716, 12423, 24202,
    14543, 10763, 27059, 437, 18647, 17133, 27774,
    2039, 3931, 7737, 20470, 11068, 26238, 28463, 22610, 28349, 23819,
    22780, 4101, 13218, 12878, 25048, 25163, 11032, 10129, 2571, 9319,
    11708, 6704, 19105, 11593, 24863, 26090, 15235, 18038, 22056, 19624,
    12066, 9798, 16508, 22376, 15776, 10595, 28391, 18898, 11645, 16655,
    19391, 11364, 28198, 4348, 6653, 11962, 22652, 18750, 22125, 21504,
    23718, 25662, 6768, 24234, 29605, 8280, 5246, 23064, 1360, 21538, 4374,
    8186, 7540, 24091, 3017, 23007, 12000, 11289, 8698, 22118, 5505, 18535,
    29647, 15878, 4416, 8598, 13062, 8878, 9674, 5066, 17770, 24888, 20643,
    1345, 22570, 1363, 3710, 18429, 11731, 14885, 12983, 18600, 26334,
    27101, 17858, 22221, 2471, 911, 12033, 2824,
    6354, 984, 28507, 3521, 17963, 6558, 11166, 24004, 24367, 8572,
    19198, 6937, 15220, 13122, 3540, 589, 17503, 14073, 14954, 26020, 12974,
    20684, 19844, 17852, 1097, 10831, 23848, 7013, 15683, 15954, 22290,
    30257, 15807, 22775, 13607, 9428, 30055, 11607, 30426, 2579, 340, 29747,
    25213, 28551, 5705, 15704, 10625, 16932, 3215, 16716, 6698, 21470,
    29839, 511, 23506, 4338, 30506, 18038, 20430, 20586, 18225, 7721, 15812,
    3140, 22149, 4949, 8125, 9897, 6323, 20612, 2012, 23744, 9414, 16497,
    5557, 5225, 8518, 30549, 21805, 5692, 25222, 16326, 22995, 27432, 16385,
    23506, 9911, 23131, 3880, 30647, 13222, 10416, 5619, 2078, 9411, 12398,
    22772, 7328, 17932, 19965,
    -1
};


int main()
{
    fmpz_poly_t p;
    slong n;

    FLINT_TEST_INIT(state);

    flint_printf("cyclotomic_cos_polynomial....");
    fflush(stdout);

    fmpz_poly_init(p);

    for (n = 0; testdata[n] != -1; n++)
    {
        ulong y;
        arith_cos_minpoly(p, n);
        y = fmpz_poly_evaluate_mod(p, 1337, 31337);

        if (y != testdata[n])
        {
            flint_printf("FAIL: n = %wd\n", n);
            flint_printf("y = %wu\n", y);
            flint_printf("\n");
            fflush(stdout);
            flint_abort();
        }
    }

    fmpz_poly_clear(p);

    FLINT_TEST_CLEANUP(state);
    flint_printf("PASS\n");
    return 0;
}

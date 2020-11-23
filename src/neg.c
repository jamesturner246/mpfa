/*
 * neg.c -- Negate an Arpra range.
 *
 * Copyright 2016-2020 James Paul Turner.
 *
 * This file is part of the Arpra library.
 *
 * The Arpra library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Arpra library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the Arpra library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "arpra-impl.h"

void arpra_neg (arpra_range *y, const arpra_range *x1)
{
    mpfi_t ia_range;
    mpfi_t alpha, gamma;
    mpfr_t delta;

    // Domain violations:
    // -(NaN) = (NaN)
    // -(Inf) = (Inf)

    // Handle domain violations.
    if (arpra_nan_p(x1)) {
        arpra_set_nan(y);
        return;
    }
    if (arpra_inf_p(x1)) {
        arpra_set_inf(y);
        return;
    }

    // Initialise vars.
    mpfi_init2(ia_range, y->precision);
    mpfi_init2(alpha, 2);
    mpfi_init2(gamma, 2);
    mpfr_init2(delta, 2);
    mpfi_set_si(alpha, -1);
    mpfi_set_si(gamma, 0);
    mpfr_set_zero(delta, 1);

    // MPFI negation
    mpfi_neg(ia_range, &(x1->true_range));

    // y = - x1
    arpra_helper_affine_1(y, x1, alpha, gamma, delta);

    // Compute true_range.
    arpra_helper_compute_range(y);

    // Mix with IA range, and trim error term.
    arpra_helper_mix_trim(y, ia_range);

    // Check for NaN and Inf.
    arpra_helper_check_result(y);

    // Clear vars.
    mpfi_clear(ia_range);
    mpfi_clear(alpha);
    mpfi_clear(gamma);
    mpfr_clear(delta);
}

/*
 * sub.c -- Subtract one Arpra range from another.
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

void arpra_sub (arpra_range *y, const arpra_range *x1, const arpra_range *x2)
{
    mpfi_t ia_range;
    mpfr_t alpha, beta, gamma, delta;
    mpfr_t temp1, temp2;
    arpra_prec prec_internal;

    // Domain violations:
    // (NaN) - (NaN) = (NaN)
    // (NaN) - (R)   = (NaN)
    // (R)   - (NaN) = (NaN)
    // (Inf) - (Inf) = (NaN)
    // (Inf) - (R)   = (Inf)
    // (R)   - (Inf) = (Inf)

    // Handle domain violations.
    if (arpra_nan_p(x1) || arpra_nan_p(x2)) {
        arpra_set_nan(y);
        return;
    }
    if (arpra_inf_p(x1)) {
        if (arpra_inf_p(x2)) {
            arpra_set_nan(y);
        }
        else {
            arpra_set_inf(y);
        }
        return;
    }
    if (arpra_inf_p(x2)) {
        if (arpra_inf_p(x1)) {
            arpra_set_nan(y);
        }
        else {
            arpra_set_inf(y);
        }
        return;
    }

    // Initialise vars.
    prec_internal = arpra_get_internal_precision();
    mpfi_init2(ia_range, y->precision);
    mpfr_init2(alpha, 2);
    mpfr_init2(beta, 2);
    mpfr_init2(gamma, 2);
    mpfr_init2(delta, 2);
    mpfr_init2(temp1, prec_internal);
    mpfr_init2(temp2, prec_internal);
    mpfr_set_si(alpha, 1, MPFR_RNDN);
    mpfr_set_si(beta, -1, MPFR_RNDN);
    mpfr_set_zero(gamma, 1);
    mpfr_set_zero(delta, 1);

    // MPFI subtraction
    mpfi_sub(ia_range, &(x1->true_range), &(x2->true_range));

    // y = x1 - x2
    arpra_affine_2(y, x1, x2, alpha, beta, gamma, delta);

    // Compute true_range.
    arpra_helper_range(y);

#ifdef ARPRA_MIXED_IAAA
    // Intersect AA and IA ranges.
    mpfi_intersect(&(y->true_range), &(y->true_range), ia_range);

#ifdef ARPRA_MIXED_TRIMMED_IAAA
    // Trim error term if AA range fully encloses mixed IA/AA range.
    mpfr_sub(temp1, &(y->centre), &(y->radius), MPFR_RNDD);
    mpfr_add(temp2, &(y->centre), &(y->radius), MPFR_RNDU);
    if (mpfr_less_p(temp1, &(y->true_range.left))
        && mpfr_greater_p(temp2, &(y->true_range.right))) {
        mpfr_sub(temp1, &(y->true_range.left), temp1, MPFR_RNDD);
        mpfr_sub(temp2, temp2, &(y->true_range.right), MPFR_RNDD);
        mpfr_min(temp1, temp1, temp2, MPFR_RNDD);
        if (mpfr_greater_p(temp1, &(y->deviations[y->nTerms - 1]))) {
            mpfr_sub(&(y->radius), &(y->radius), &(y->deviations[y->nTerms - 1]), MPFR_RNDU);
            mpfr_set_zero(&(y->deviations[y->nTerms - 1]), 1);
        }
        else {
            mpfr_sub(&(y->radius), &(y->radius), temp1, MPFR_RNDU);
            mpfr_sub(&(y->deviations[y->nTerms - 1]), &(y->deviations[y->nTerms - 1]), temp1, MPFR_RNDU);
        }
    }
#endif // ARPRA_MIXED_TRIMMED_IAAA
#endif // ARPRA_MIXED_IAAA

    // Check for NaN and Inf.
    arpra_helper_check_result(y);

    // Clear vars.
    mpfi_clear(ia_range);
    mpfr_clear(alpha);
    mpfr_clear(beta);
    mpfr_clear(gamma);
    mpfr_clear(delta);
    mpfr_clear(temp1);
    mpfr_clear(temp2);
}

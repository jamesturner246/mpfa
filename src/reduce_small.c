/*
 * reduce_small.c -- Reduce terms less than some fraction of the radius.
 *
 * Copyright 2017-2018 James Paul Turner.
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

void arpra_reduce_small (arpra_ptr z, double fraction)
{
    arpra_uint_t zTerm, zNext;
    mpfr_ptr *summands;
    mpfr_t temp, threshold;
    arpra_prec_t prec_internal;

    // Handle trivial cases.
    if ((z->nTerms < 2) || (fraction >= 1)) return;

    // Handle domain violations.
    if (arpra_nan_p(z)) return;
    if (arpra_inf_p(z)) return;

    // Initialise vars.
    prec_internal = arpra_get_internal_prec();
    mpfr_init2(temp, prec_internal);
    mpfr_init2(threshold, prec_internal);
    mpfr_mul_d(threshold, &(z->radius), fraction, MPFR_RNDN);
    mpfr_set_prec(&(z->radius), prec_internal);
    mpfr_set_ui(&(z->radius), 0, MPFR_RNDU);
    zTerm = 0;

    // Shift small deviation terms to back.
    for (zNext = 0; zNext < z->nTerms; zNext++) {
        if (mpfr_cmpabs(&(z->deviations[zNext]), threshold) > 0) {
            if (zTerm < zNext) {
                z->symbols[zTerm] = z->symbols[zNext];
                mpfr_swap(&(z->deviations[zTerm]), &(z->deviations[zNext]));
            }
            zTerm++;
        }
    }
    summands = malloc((zNext - zTerm) * sizeof(mpfr_ptr));

    // Merge the small deviation terms.
    for (zNext = zTerm; zNext < z->nTerms; zNext++) {
        mpfr_abs(&(z->deviations[zNext]), &(z->deviations[zNext]), MPFR_RNDN);
        summands[zNext - zTerm] = &(z->deviations[zNext]);
    }
    mpfr_sum(&(z->deviations[zTerm]), summands, (zNext - zTerm), MPFR_RNDU);

    // Add the remaining deviation terms to radius.
    for (zNext = 0; zNext < zTerm; zNext++) {
        mpfr_abs(temp, &(z->deviations[zTerm]), MPFR_RNDU);
        mpfr_add(&(z->radius), &(z->radius), temp, MPFR_RNDU);
    }

    // Store nonzero merged deviation term.
    if (!mpfr_zero_p(&(z->deviations[zTerm]))) {
        z->symbols[zTerm] = arpra_next_sym();
        mpfr_add(&(z->radius), &(z->radius), &(z->deviations[zTerm]), MPFR_RNDU);
        zTerm++;
    }

    // Clear unused deviation terms.
    for (zNext = zTerm; zNext < z->nTerms; zNext++) {
        mpfr_clear(&(z->deviations[zNext]));
    }

    // Handle domain violations, and resize memory.
    z->nTerms = zTerm;
    if (mpfr_nan_p(&(z->radius))) {
        arpra_set_nan(z);
    }
    else if (mpfr_inf_p(&(z->radius))) {
        arpra_set_inf(z);
    }
    else {
        if (z->nTerms == 0) {
            free(z->symbols);
            free(z->deviations);
        }
        else {
            z->symbols = realloc(z->symbols, z->nTerms * sizeof(arpra_uint_t));
            z->deviations = realloc(z->deviations, z->nTerms * sizeof(mpfr_t));
        }
    }

    // Clear vars.
    mpfr_clear(temp);
    mpfr_clear(threshold);
    free(summands);
}

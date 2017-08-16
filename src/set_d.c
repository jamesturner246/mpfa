/*
 * set_d.c -- Set centre using a double-precision float.
 *
 * Copyright 2016-2017 James Paul Turner.
 *
 * This file is part of the MPFA library.
 *
 * The MPFA library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The MPFA library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the MPFA library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mpfa.h"
#include <stdlib.h>

void mpfa_set_d (mpfa_ptr z, const double centre) {
    mpfa_uint_t zTerm;
    mpfa_prec_t prec_internal;

    prec_internal = mpfa_get_internal_prec();

    // If centre has rounding error:
    if (mpfr_set_d(&(z->centre), centre, MPFR_RNDN)) {
        // Set radius with centre rounding error.
        mpfa_error(&(z->radius), &(z->centre));

        // If z has no noise term memory:
        if (z->nTerms == 0) {
            // Allocate noise term memory.
            z->symbols = malloc(sizeof(mpfa_uint_t));
            z->deviations = malloc(sizeof(mpfa_t));
            mpfr_init2(&(z->deviations[0]), prec_internal);
        }

        // Else if z has too much noise term memory:
        else if (z->nTerms >= 2) {
            // Clear unused noise terms.
            mpfr_prec_round(&(z->deviations[0]), prec_internal, MPFR_RNDN);
            for (zTerm = 1; zTerm < z->nTerms; zTerm++) {
                mpfr_clear(&(z->deviations[zTerm]));
            }
            z->symbols = realloc(z->symbols, sizeof(mpfa_uint_t));
            z->deviations = realloc(z->deviations, sizeof(mpfa_t));
        }
        z->nTerms = 1;

        // Set noise term.
        z->symbols[0] = mpfa_next_sym();
        mpfr_set(&(z->deviations[0]), &(z->radius), MPFR_RNDN);
    }

    // Else centre has no rounding error:
    else {
        // Radius is set, and noise terms are cleared.
        mpfr_set_si(&(z->radius), 0, MPFR_RNDU);
        if (z->nTerms > 0) {
            for (zTerm = 0; zTerm < z->nTerms; zTerm++) {
                mpfr_clear(&(z->deviations[zTerm]));
            }
            z->nTerms = 0;
            free(z->symbols);
            free(z->deviations);
        }
    }
}

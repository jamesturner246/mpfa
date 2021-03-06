/*
 * arpra_ode.h -- Arpra public header for ordinary differential equations.
 *
 * Copyright 2018-2020 James Paul Turner.
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

#ifndef ARPRA_ODE_H
#define ARPRA_ODE_H

#include <arpra.h>

// Arpra ODE typedefs.
typedef struct arpra_ode_system_struct arpra_ode_system;
typedef struct arpra_ode_stepper_struct arpra_ode_stepper;
typedef struct arpra_ode_method_struct arpra_ode_method;
typedef void (*arpra_ode_f) (arpra_range *dxdt, const void *params,
                             const arpra_range *t, const arpra_range **x,
                             const arpra_uint x_grp, const arpra_uint x_dim);

// System definition.
struct arpra_ode_system_struct
{
    arpra_ode_f *f;
    void **params;
    arpra_range *t;
    arpra_range **x;
    arpra_uint grps;
    arpra_uint *dims;
};

// Stepper definition.
struct arpra_ode_stepper_struct
{
    const arpra_ode_method *method;
    arpra_ode_system *system;
    arpra_range *error;
    void *scratch;
};

// Step method definition.
struct arpra_ode_method_struct
{
    void (* const init) (arpra_ode_stepper *stepper, arpra_ode_system *system);
    void (* const clear) (arpra_ode_stepper *stepper);
    void (* const step) (arpra_ode_stepper *stepper, const arpra_range *h);
    const unsigned char stages;
};

#ifdef __cplusplus
extern "C" {
#endif

// Stepper functions.
void arpra_ode_stepper_init (arpra_ode_stepper *stepper, arpra_ode_system *system,
                             const arpra_ode_method *method);
void arpra_ode_stepper_clear (arpra_ode_stepper *stepper);
void arpra_ode_stepper_step (arpra_ode_stepper *stepper, const arpra_range *h);

// Arpra built-in step methods.
extern const arpra_ode_method *arpra_ode_euler;
extern const arpra_ode_method *arpra_ode_trapezoidal;
extern const arpra_ode_method *arpra_ode_bogsham32;
extern const arpra_ode_method *arpra_ode_dopri54;
extern const arpra_ode_method *arpra_ode_dopri87;

#ifdef __cplusplus
}
#endif

#endif // ARPRA_ODE_H

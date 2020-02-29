/**
 * @file SFMT-params.h
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * VIRG-Nexus Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VIRG-Nexus Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 * @file SFMT-params.h
 *
 * @brief SIMD oriented Fast Mersenne Twister(SFMT) pseudorandom
 * number generator using C structure.
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University.
 * Copyright (C) 2012 Mutsuo Saito, Makoto Matsumoto, Hiroshima
 * University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */
#pragma once
#ifndef SFMT_PARAMS_H
#define SFMT_PARAMS_H

#define SFMT_MEXP 19937
/*-----------------
  BASIC DEFINITIONS
  -----------------*/
/** Mersenne Exponent. The period of the sequence
 *  is a multiple of 2^MEXP-1.
 * #define SFMT_MEXP 19937 */
/** SFMT generator has an internal state array of 128-bit integers,
 * and N is its size. */
#define SFMT_N (SFMT_MEXP / 128 + 1)
/** N32 is the size of internal state array when regarded as an array
 * of 32-bit integers.*/
#define SFMT_N32 (SFMT_N * 4)
/** N64 is the size of internal state array when regarded as an array
 * of 64-bit integers.*/
#define SFMT_N64 (SFMT_N * 2)

/*----------------------
  the parameters of SFMT
  following definitions are in paramsXXXX.h file.
  ----------------------*/
/** the pick up position of the array.
#define SFMT_POS1 122
*/

/** the parameter of shift left as four 32-bit registers.
#define SFMT_SL1 18
 */

/** the parameter of shift left as one 128-bit register.
 * The 128-bit integer is shifted by (SFMT_SL2 * 8) bits.
#define SFMT_SL2 1
*/

/** the parameter of shift right as four 32-bit registers.
#define SFMT_SR1 11
*/

/** the parameter of shift right as one 128-bit register.
 * The 128-bit integer is shifted by (SFMT_SR2 * 8) bits.
#define SFMT_SR2 1
*/

/** A bitmask, used in the recursion.  These parameters are introduced
 * to break symmetry of SIMD.
#define SFMT_MSK1 0xdfffffefU
#define SFMT_MSK2 0xddfecb7fU
#define SFMT_MSK3 0xbffaffffU
#define SFMT_MSK4 0xbffffff6U
*/

/** These definitions are part of a 128-bit period certification vector.
#define SFMT_PARITY1    0x00000001U
#define SFMT_PARITY2    0x00000000U
#define SFMT_PARITY3    0x00000000U
#define SFMT_PARITY4    0xc98e126aU
*/

#if SFMT_MEXP == 607
  #include "SFMT-params607.h"
#elif SFMT_MEXP == 1279
  #include "SFMT-params1279.h"
#elif SFMT_MEXP == 2281
  #include "SFMT-params2281.h"
#elif SFMT_MEXP == 4253
  #include "SFMT-params4253.h"
#elif SFMT_MEXP == 11213
  #include "SFMT-params11213.h"
#elif SFMT_MEXP == 19937
  #include "SFMT-params19937.h"
#elif SFMT_MEXP == 44497
  #include "SFMT-params44497.h"
#elif SFMT_MEXP == 86243
  #include "SFMT-params86243.h"
#elif SFMT_MEXP == 132049
  #include "SFMT-params132049.h"
#elif SFMT_MEXP == 216091
  #include "SFMT-params216091.h"
#else
#if defined(__GNUC__) && !defined(__ICC)
  #error "SFMT_MEXP is not valid."
  #undef SFMT_MEXP
#else
  #undef SFMT_MEXP
#endif

#endif

#endif /* SFMT_PARAMS_H */

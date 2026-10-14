/*
 * Copyright (c) 2003, 2007-14 Matteo Frigo
 * Copyright (c) 2003, 2007-14 Massachusetts Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#include "kernel/ifftw.h"

#if HAVE_SVE
extern int sve_getwidth(void);

#if defined(__linux__)

#include <sys/auxv.h>
#include <asm/hwcap.h>

static int have_sve(void) {
  unsigned long hwcap = getauxval(AT_HWCAP);
  return (hwcap & HWCAP_SVE) != 0;
}

int X(have_simd_sve)(int minwidth) {
  return have_sve() && (sve_getwidth() >= minwidth);
}

#elif defined(__APPLE__)
// no Apple hardware supports SVE as of Apr 2026.  Revisit
// if this changes
int X(have_simd_sve)(int minwidth){
  return 0;
}
#endif

#endif // HAVE_SVE

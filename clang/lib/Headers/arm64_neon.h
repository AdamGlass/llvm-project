/*===---- arm64_neon.h - ARM Windows intrinsics ----------------------------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

/* Only include this if we're compiling for the windows platform. */
#ifndef _MSC_VER
#include_next <arm64_neon.h>
#else

#ifndef __ARM64_NEON_H
#define __ARM64_NEON_H

#include <arm_neon.h>

#if defined (__cplusplus)
extern "C" {
#endif

uint8x16_t __iso_volatile_neon_load128(const volatile uint8x16_t *);
uint8x16x2_t __iso_volatile_neon_load128_p(const volatile uint8x16x2_t *);
uint8x8x2_t __iso_volatile_neon_load64_np(const volatile uint8x8x2_t *);
uint8x16x2_t __iso_volatile_neon_load128_np(const volatile uint8x16x2_t *);
void __iso_volatile_neon_store128(const volatile uint8x16_t *, uint8x16_t);
void __iso_volatile_neon_store128_p(const volatile uint8x16x2_t *, uint8x16x2_t);
void __iso_volatile_neon_store64_np(const volatile uint8x8x2_t *, uint8x8x2_t);

#if 0
static inline void __iso_volatile_neon_store64_np(const volatile uint8x8x2_t *__A, uint8x8x2_t __V) {
    __builtin_nontemporal_store(__V, __A);
}
#endif

void __iso_volatile_neon_store128_np(const volatile uint8x16x2_t *, uint8x16x2_t);

#ifdef notyet
float32x1x2_t __iso_volatile_neon_load32_np(const volatile float32x1x2_t *);
void __iso_volatile_neon_store32_np(const volatile float32x1x2_t *, float32x1x2_t);
#endif

#if defined (__cplusplus)
}
#endif

#endif /* __ARM64_NEON_H */
#endif /* _MSC_VER */

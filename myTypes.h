/**
 * \author argawaen
 * \date 03/05/2020
 *
 */
#pragma once
#include <Arduino.h>

namespace ob {

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef float f32;
    typedef double f64;

    struct vec3D {
        f32 x, y, z;
        [[nodiscard]] String toStr() const {
            return " (" + String(x) + ";" + String(y) + ";" + String(z) + ")";
        }
    };
}
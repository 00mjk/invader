// SPDX-License-Identifier: GPL-3.0-only

#ifndef INVADER__BITMAP__SWIZZLE_HPP
#define INVADER__BITMAP__SWIZZLE_HPP

#include <cstddef>
#include <vector>

namespace Invader::Swizzle {
    /**
     * Swizzle the pixel data
     * @param data           raw pixel data
     * @param bits_per_pixel number of bits per pixel (can be 8, 16, 32, 64)
     * @param width          width in pixels
     * @param height         height in pixels
     * @param depth          depth in bitmaps
     * @param deswizzle      deswizzle instead of swizzle
     * @output               (de)swizzled data
     */
    std::vector<std::byte> swizzle(const std::byte *data, std::size_t bits_per_pixel, std::size_t width, std::size_t height, std::size_t depth, bool deswizzle);
}

#endif

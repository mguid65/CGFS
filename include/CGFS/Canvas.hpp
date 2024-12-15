/**
 * @author Matthew Guidry (github: mguid65)
 * @date 2024-10-11
 *
 * @cond IGNORE_LICENSE
 *
 * MIT License
 *
 * Copyright (c) 2024 Matthew Guidry
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @endcond
 */

#ifndef MGUID_CGFS_CANVAS_HPP
#define MGUID_CGFS_CANVAS_HPP

#include <CGFS/Common.hpp>

#include "CGFS/Color.hpp"

#include <iostream>
#include <string>
#include <vector>

#include <fmt/format.h>

namespace cgfs {

inline std::string get_ascii_esc_color_block(uint8_t r, uint8_t g, uint8_t b) {
  return fmt::format("\033[48;2;{};{};{}m   \033[0m", +r, +g, +b);
}

using StringMatrix = std::vector<std::vector<std::string>>;

using Buffer = mguid::NamedTuple<mguid::NamedType<"buffer", StringMatrix>>;

inline StringMatrix from_dimensions(const DimensionsU32& dims) {
  StringMatrix result{dims.get<"height">()};
  for (auto& row : result) { row = std::vector<std::string>{dims.get<"width">()}; }
  return result;
}

struct Canvas : DimensionsU32, BBoxi32, Buffer {
  using BBoxi32::get;
  using Buffer::get;
  using DimensionsU32::get;

  Canvas(DimensionsU32 dim)
      : DimensionsU32{std::move(dim)},
        BBoxi32{
            -static_cast<int32_t>(dim.get<"width">()) / 2,
            dim.get<"width">() / 2,
            -static_cast<int32_t>(dim.get<"height">()) / 2,
            dim.get<"height">() / 2,
        },
        Buffer{from_dimensions(static_cast<const DimensionsU32&>(*this))} {}

  void put_pixel(int32_t x, [[maybe_unused]] int32_t y, Color3 color) {
    const uint8_t r = color.get<"r">();
    const uint8_t g = color.get<"g">();
    const uint8_t b = color.get<"b">();

    const auto width = get<"width">();
    const auto height = get<"height">();

    x = (static_cast<int32_t>(width) / 2) + x;
    y = (static_cast<int32_t>(height) / 2) - y;

    if (x < 0 || x >= static_cast<int32_t>(width) || y < 0 || y >= static_cast<int32_t>(height)) {
      return;
    }

    get<"buffer">()[static_cast<uint32_t>(y)][static_cast<uint32_t>(x)] = get_ascii_esc_color_block(r, g, b);
  }

  void paint() {
    for (const auto& row : get<"buffer">()) {
      for (const auto& pixel : row) {
        std::cout << pixel;
      }
      std::cout << '\n';
    }
  }
};

}  // namespace cgfs

#endif  // MGUID_CGFS_CANVAS_HPP

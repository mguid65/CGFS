/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/8/24
 */

#ifndef CGFS_VIEWPORT_HPP
#define CGFS_VIEWPORT_HPP

#include <CGFS/Common.hpp>

namespace cgfs {
struct Viewport : DimensionsF64 {
  using DimensionsF64::DimensionsF64;

  constexpr Viewport(DimensionsF64 dim) : DimensionsF64(std::move(dim)) {}

  using DimensionsF64::get;
};
}

#endif  // CGFS_VIEWPORT_HPP

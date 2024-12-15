/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/8/24
 */

#ifndef CPPTEMPLATE_COMMON_HPP
#define CPPTEMPLATE_COMMON_HPP

#include "CGFS/ThirdParty/Named/NamedTuple.hpp"
#include "CGFS/ThirdParty/Named/TaggedArray.hpp"

namespace cgfs {

template <typename... Callables>
struct Overload : Callables... {
  using Callables::operator()...;
};

template <typename... Callables>
Overload(Callables...) -> Overload<Callables...>;

using DimensionsU32 =
    mguid::NamedTuple<mguid::NamedType<"width", uint32_t>, mguid::NamedType<"height", uint32_t>>;

using DimensionsF64 =
    mguid::NamedTuple<mguid::NamedType<"width", double>, mguid::NamedType<"height", double>>;

using BBoxi32 =
    mguid::NamedTuple<mguid::NamedType<"left", int32_t>, mguid::NamedType<"right", int32_t>,
                      mguid::NamedType<"bottom", int32_t>, mguid::NamedType<"top", int32_t>>;

using BBoxu32 =
    mguid::NamedTuple<mguid::NamedType<"left", uint32_t>, mguid::NamedType<"right", uint32_t>,
                      mguid::NamedType<"bottom", uint32_t>, mguid::NamedType<"top", uint32_t>>;

template <typename Type>
using Vec2 = mguid::NamedTuple<mguid::NamedType<"x", Type>, mguid::NamedType<"y", Type>>;

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i8 = Vec2<int8_t>;
using Vec2u8 = Vec2<uint8_t>;
using Vec2i16 = Vec2<int16_t>;
using Vec2u16 = Vec2<uint16_t>;
using Vec2i32 = Vec2<int32_t>;
using Vec2u32 = Vec2<uint32_t>;
using Vec2i64 = Vec2<int64_t>;
using Vec2u64 = Vec2<uint64_t>;

template <typename Type>
using Vec3 = mguid::NamedTuple<mguid::NamedType<"x", Type>, mguid::NamedType<"y", Type>,
                               mguid::NamedType<"z", Type>>;

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i8 = Vec3<int8_t>;
using Vec3u8 = Vec3<uint8_t>;
using Vec3i16 = Vec3<int16_t>;
using Vec3u16 = Vec3<uint16_t>;
using Vec3i32 = Vec3<int32_t>;
using Vec3u32 = Vec3<uint32_t>;
using Vec3i64 = Vec3<int64_t>;
using Vec3u64 = Vec3<uint64_t>;

using Mat3f = std::array<std::array<float, 3>, 3>;
using Mat3d = std::array<std::array<double, 3>, 3>;
using Mat3i8 = std::array<std::array<int8_t, 3>, 3>;
using Mat3u8 = std::array<std::array<uint8_t, 3>, 3>;
using Mat3i16 = std::array<std::array<int16_t, 3>, 3>;
using Mat3u16 = std::array<std::array<uint16_t, 3>, 3>;
using Mat3i32 = std::array<std::array<int32_t, 3>, 3>;
using Mat3u32 = std::array<std::array<uint32_t, 3>, 3>;
using Mat3i64 = std::array<std::array<int64_t, 3>, 3>;
using Mat3u64 = std::array<std::array<uint64_t, 3>, 3>;

using Mat4f = std::array<std::array<float, 4>, 4>;
using Mat4d = std::array<std::array<double, 4>, 4>;
using Mat4i8 = std::array<std::array<int8_t, 4>, 4>;
using Mat4u8 = std::array<std::array<uint8_t, 4>, 4>;
using Mat4i16 = std::array<std::array<int16_t, 4>, 4>;
using Mat4u16 = std::array<std::array<uint16_t, 4>, 4>;
using Mat4i32 = std::array<std::array<int32_t, 4>, 4>;
using Mat4u32 = std::array<std::array<uint32_t, 4>, 4>;
using Mat4i64 = std::array<std::array<int64_t, 4>, 4>;
using Mat4u64 = std::array<std::array<uint64_t, 4>, 4>;

constexpr double basically_infinity = 1'000'000'000'000;

}  // namespace cgfs

constexpr inline cgfs::Vec3d operator-(const cgfs::Vec3d& lhs, const cgfs::Vec3d& rhs) {
  return cgfs::Vec3d{lhs.get<"x">() - rhs.get<"x">(), lhs.get<"y">() - rhs.get<"y">(),
                     lhs.get<"z">() - rhs.get<"z">()};
}

constexpr inline cgfs::Vec3d operator-(const cgfs::Vec3d& val) {
  return cgfs::Vec3d{-val.get<"x">(), -val.get<"y">(), -val.get<"z">()};
}

constexpr inline cgfs::Vec3d operator+(const cgfs::Vec3d& lhs, const cgfs::Vec3d& rhs) {
  return cgfs::Vec3d{lhs.get<"x">() + rhs.get<"x">(), lhs.get<"y">() + rhs.get<"y">(),
                     lhs.get<"z">() + rhs.get<"z">()};
}

constexpr inline cgfs::Vec3d operator*(double val, const cgfs::Vec3d& rhs) {
  return cgfs::Vec3d{rhs.get<"x">() * val, rhs.get<"y">() * val, rhs.get<"z">() * val};
}

constexpr inline cgfs::Vec3d operator*(const cgfs::Vec3d& rhs, double val) {
  return cgfs::Vec3d{rhs.get<"x">() * val, rhs.get<"y">() * val, rhs.get<"z">() * val};
}

constexpr inline cgfs::Vec3d operator/(const cgfs::Vec3d& lhs, double val) {
  return cgfs::Vec3d{lhs.get<"x">() / val, lhs.get<"y">() / val, lhs.get<"z">() / val};
}

constexpr inline cgfs::Vec3d operator*(const cgfs::Mat3d& mat, const cgfs::Vec3d& vec3) {
  std::array result{0.0, 0.0, 0.0};
  std::array<double, 3> vec{vec3.get<0>(), vec3.get<1>(), vec3.get<2>()};

  for (size_t i = 0; i < 3; i++) {
    for (size_t j = 0; j < 3; j++) {
      result[i] += vec[j]*mat[i][j];
    }
  }

  return cgfs::Vec3d(result[0], result[1], result[2]);
}

#endif  // CPPTEMPLATE_COMMON_HPP

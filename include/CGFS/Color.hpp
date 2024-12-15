/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/7/24
 */

#ifndef CGFS_COLOR_HPP
#define CGFS_COLOR_HPP

#include "CGFS/ThirdParty/Named/NamedTuple.hpp"

#include <algorithm>
#include <cstdint>

namespace cgfs {

template <typename Type>
using RGB = mguid::NamedTuple<mguid::NamedType<"r", Type>, mguid::NamedType<"g", Type>,
                              mguid::NamedType<"b", Type>>;
template <typename Type>
using RGBA = mguid::NamedTuple<mguid::NamedType<"r", Type>, mguid::NamedType<"g", Type>,
                               mguid::NamedType<"b", Type>, mguid::NamedType<"a", Type>>;

using RGB8Bit = RGB<uint8_t>;
using RGB16Bit = RGB<uint16_t>;
using RGB32Bit = RGB<uint32_t>;
using RGB32F = RGB<float>;
using RGB64F = RGB<double>;
using RGBA8Bit = RGBA<uint8_t>;
using RGBA16Bit = RGBA<uint16_t>;
using RGBA32Bit = RGBA<uint32_t>;
using RGBA32F = RGBA<float>;
using RGBA64F = RGBA<double>;

/**
 * @brief A class that represents 3 component 8 bit color RGB
 */
struct Color3 : RGB8Bit {
  constexpr Color3() {}
  constexpr Color3(uint8_t red, uint8_t green, uint8_t blue) : RGB8Bit(red, green, blue) {}

  constexpr Color3 operator*(uint8_t val) const noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() * std::declval<uint8_t>()), int>);

    return Color3{static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"r">()) * static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"g">()) * static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"b">()) * static_cast<int>(val), 0, 255))};
  }

  constexpr Color3& operator*=(uint8_t val) noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() * std::declval<uint8_t>()), int>);

    get<"r">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"r">()) * static_cast<int>(val), 0, 255));
    get<"g">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"g">()) * static_cast<int>(val), 0, 255));
    get<"b">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"b">()) * static_cast<int>(val), 0, 255));

    return *this;
  }

  constexpr Color3 operator/(uint8_t val) const noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() / std::declval<uint8_t>()), int>);

    return Color3{static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"r">()) / static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"g">()) / static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"b">()) / static_cast<int>(val), 0, 255))};
  }

  constexpr Color3& operator/=(uint8_t val) noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() / std::declval<uint8_t>()), int>);

    get<"r">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"r">()) / static_cast<int>(val), 0, 255));
    get<"g">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"g">()) / static_cast<int>(val), 0, 255));
    get<"b">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"b">()) / static_cast<int>(val), 0, 255));

    return *this;
  }

  constexpr Color3 operator+(uint8_t val) const noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() + std::declval<uint8_t>()), int>);

    return Color3{static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"r">()) + static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"g">()) + static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"b">()) + static_cast<int>(val), 0, 255))};
  }

  constexpr Color3& operator+=(uint8_t val) noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() + std::declval<uint8_t>()), int>);

    get<"r">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"r">()) + static_cast<int>(val), 0, 255));
    get<"g">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"g">()) + static_cast<int>(val), 0, 255));
    get<"b">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"b">()) + static_cast<int>(val), 0, 255));

    return *this;
  }

  constexpr Color3 operator+(const Color3& other) const noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() + std::declval<uint8_t>()), int>);

    return Color3{static_cast<uint8_t>(std::clamp(
                      static_cast<int>(get<"r">()) + static_cast<int>(other.get<"r">()), 0, 255)),
                  static_cast<uint8_t>(std::clamp(
                      static_cast<int>(get<"g">()) + static_cast<int>(other.get<"g">()), 0, 255)),
                  static_cast<uint8_t>(std::clamp(
                      static_cast<int>(get<"b">()) + static_cast<int>(other.get<"b">()), 0, 255))};
  }

  constexpr Color3& operator+=(const Color3& other) noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() + std::declval<uint8_t>()), int>);

    get<"r">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"r">()) + static_cast<int>(other.get<"r">()), 0, 255));
    get<"g">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"g">()) + static_cast<int>(other.get<"g">()), 0, 255));
    get<"b">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"b">()) + static_cast<int>(other.get<"b">()), 0, 255));

    return *this;
  }

  constexpr Color3 operator-(uint8_t val) const noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() - std::declval<uint8_t>()), int>);

    return Color3{static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"r">()) - static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"g">()) - static_cast<int>(val), 0, 255)),
                  static_cast<uint8_t>(
                      std::clamp(static_cast<int>(get<"b">()) - static_cast<int>(val), 0, 255))};
  }

  constexpr Color3& operator-=(uint8_t val) noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() - std::declval<uint8_t>()), int>);

    get<"r">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"r">()) - static_cast<int>(val), 0, 255));
    get<"g">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"g">()) - static_cast<int>(val), 0, 255));
    get<"b">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"b">()) - static_cast<int>(val), 0, 255));

    return *this;
  }

  constexpr Color3 operator-(const Color3& other) const noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() - std::declval<uint8_t>()), int>);

    return Color3{static_cast<uint8_t>(std::clamp(
                      static_cast<int>(get<"r">()) - static_cast<int>(other.get<"r">()), 0, 255)),
                  static_cast<uint8_t>(std::clamp(
                      static_cast<int>(get<"g">()) - static_cast<int>(other.get<"g">()), 0, 255)),
                  static_cast<uint8_t>(std::clamp(
                      static_cast<int>(get<"b">()) - static_cast<int>(other.get<"b">()), 0, 255))};
  }

  constexpr Color3& operator-=(const Color3& other) noexcept {
    static_assert(std::is_same_v<decltype(std::declval<uint8_t>() - std::declval<uint8_t>()), int>);

    get<"r">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"r">()) - static_cast<int>(other.get<"r">()), 0, 255));
    get<"g">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"g">()) - static_cast<int>(other.get<"g">()), 0, 255));
    get<"b">() = static_cast<uint8_t>(
        std::clamp(static_cast<int>(get<"b">()) - static_cast<int>(other.get<"b">()), 0, 255));

    return *this;
  }
};

}  // namespace cgfs

#endif  // CGFS_COLOR_HPP

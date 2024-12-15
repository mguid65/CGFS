/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/9/24
 */

#ifndef CGFS_LIGHT_HPP
#define CGFS_LIGHT_HPP

#include "CGFS/ThirdParty/Named/NamedTuple.hpp"

#include "CGFS/Color.hpp"
#include "CGFS/Common.hpp"

#include <variant>

namespace cgfs {

using NamedIntensity = mguid::NamedType<"intensity", double>;

using PointLightProperties = mguid::NamedTuple<NamedIntensity, mguid::NamedType<"position", Vec3d>>;
using DirectionalLightProperties =
    mguid::NamedTuple<NamedIntensity, mguid::NamedType<"direction", Vec3d>>;
using AmbientLightProperties = mguid::NamedTuple<NamedIntensity>;

using LightProperties = std::variant<AmbientLightProperties, PointLightProperties, DirectionalLightProperties>;

struct Light : LightProperties {
  PointLightProperties& point_light() {
    return get<PointLightProperties>();
  }

  AmbientLightProperties& ambient_light() {
    return get<AmbientLightProperties>();
  }

  DirectionalLightProperties& directional_light() {
    return get<DirectionalLightProperties>();
  }

  [[nodiscard]] const PointLightProperties& point_light() const {
    return get<PointLightProperties>();
  }

  [[nodiscard]] const AmbientLightProperties& ambient_light() const {
    return get<AmbientLightProperties>();
  }

  [[nodiscard]] const DirectionalLightProperties& directional_light() const {
    return get<DirectionalLightProperties>();
  }

  template <typename... Callables>
  constexpr decltype(auto) visit(Callables&&... callables) {
    auto overload_set = Overload{callables...};
    return std::visit(overload_set, *this);
  }

  template <typename... Callables>
  constexpr decltype(auto) visit(Callables&&... callables) const {
    auto overload_set = Overload{callables...};
    return std::visit(overload_set, *this);
  }
private:
  template <typename Alternative>
  Alternative& get() {
    if (auto* ptr = std::get_if<Alternative>(this); ptr != nullptr) {
      return *ptr;
    }
    static_cast<LightProperties&>(*this) = Alternative{};
    return std::get<Alternative>(*this);
  }

  template <typename Alternative>
  const Alternative& get() const {
    return std::get<Alternative>(*this);
  }
};

}  // namespace cgfs

#endif  // CGFS_LIGHT_HPP

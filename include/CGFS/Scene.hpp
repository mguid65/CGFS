/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/8/24
 */

#ifndef CPPTEMPLATE_SCENE_HPP
#define CPPTEMPLATE_SCENE_HPP

#include "CGFS/ThirdParty/Named/NamedTuple.hpp"

#include "CGFS/Color.hpp"
#include "CGFS/Common.hpp"

#include "CGFS/Lighting/Light.hpp"
#include "CGFS/Objects/Sphere.hpp"

#include <array>

namespace cgfs {

template <size_t NumObjects, size_t NumLights>
using SceneProperties =
    mguid::NamedTuple<mguid::NamedType<"objects", std::array<Sphere, NumObjects>>,
                      mguid::NamedType<"lights", std::array<Light, NumLights>>,
                      mguid::NamedType<"background_color", Color3>>;

template <size_t NumObjects, size_t NumLights>
struct Scene : SceneProperties<NumObjects, NumLights> {
  constexpr Scene(const std::array<Sphere, NumObjects>& objects,
                  const std::array<Light, NumLights>& lights,
                  const Color3& background_color)
      : SceneProperties<NumObjects, NumLights>{objects, lights, background_color} {}

  using SceneProperties<NumObjects, NumLights>::get;
  using SceneProperties<NumObjects, NumLights>::SceneProperties;
};

}  // namespace cgfs

#endif  // CPPTEMPLATE_SCENE_HPP

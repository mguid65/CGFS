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

#include "CGFS/Objects/Sphere.hpp"
#include "CGFS/Lighting/Light.hpp"

#include <vector>

namespace cgfs {

using SceneProperties = mguid::NamedTuple<mguid::NamedType<"objects", std::vector<Sphere>>,
                                          mguid::NamedType<"lights", std::vector<Light>>,
                                          mguid::NamedType<"background_color", Color3>>;
struct Scene : SceneProperties {
  using SceneProperties::get;
  using SceneProperties::SceneProperties;
};

}  // namespace cgfs

#endif  // CPPTEMPLATE_SCENE_HPP

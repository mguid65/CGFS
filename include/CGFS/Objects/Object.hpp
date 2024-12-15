/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/9/24
 */

#ifndef CGFS_OBJECT_HPP
#define CGFS_OBJECT_HPP

#include "CGFS/Lighting/Light.hpp"
#include <CGFS/Objects/Sphere.hpp>

namespace cgfs {
struct SceneObject : std::variant<Sphere> {};
}

#endif  // CGFS_OBJECT_HPP

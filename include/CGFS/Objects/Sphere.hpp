/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/9/24
 */

#ifndef CGFS_SPHERE_HPP
#define CGFS_SPHERE_HPP

#include "CGFS/ThirdParty/Named/NamedTuple.hpp"

#include "CGFS/Color.hpp"
#include "CGFS/Common.hpp"
#include "CGFS/Material/Material.hpp"

#include <variant>

namespace cgfs {

// Create generic geometry type
using Sphere =
    mguid::NamedTuple<mguid::NamedType<"center", Vec3d>,
                      mguid::NamedType<"radius", double>,
                      mguid::NamedType<"material", MaterialProperties>>;
}

#endif  // CGFS_SPHERE_HPP

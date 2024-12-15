/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/14/24
 */

#ifndef CGFS_MATERIAL_HPP
#define CGFS_MATERIAL_HPP

#include "CGFS/ThirdParty/Named/NamedTuple.hpp"

#include "CGFS/Color.hpp"
#include "CGFS/Common.hpp"

namespace cgfs {
  using MaterialProperties = mguid::NamedTuple<mguid::NamedType<"color", Color3>,
                                               mguid::NamedType<"specular", double>,
                                               mguid::NamedType<"reflective", double>>;
}

#endif // CGFS_MATERIAL_HPP

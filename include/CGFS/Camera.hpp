/**
 * @brief TODO
 * @author Matthew Guidry (github: mguid65)
 * @date 12/8/24
 */

#ifndef CGFS_CAMERA_HPP
#define CGFS_CAMERA_HPP

#include <CGFS/Common.hpp>

namespace cgfs {

using Origin = Vec3d;
using ProjectionPlane = mguid::NamedTuple<mguid::NamedType<"distance", double>>;

using CameraProperties = mguid::NamedTuple<mguid::NamedType<"origin", Origin>,
                                           mguid::NamedType<"rotation", Mat3d>,
                                           mguid::NamedType<"projection_plane", ProjectionPlane>>;

struct Camera : CameraProperties {
  using CameraProperties::CameraProperties;
  using CameraProperties::get;
};

}  // namespace cgfs
#endif  // CGFS_CAMERA_HPP

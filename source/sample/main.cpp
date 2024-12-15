#include <CGFS/Camera.hpp>
#include <CGFS/Canvas.hpp>
#include <CGFS/Color.hpp>
#include <CGFS/Logger.hpp>
#include <CGFS/Scene.hpp>
#include <CGFS/Viewport.hpp>

#include <cmath>

auto logger = get_logger();

template <typename Type>
double dot(const cgfs::Vec3<Type>& a, const cgfs::Vec3<Type>& b) {
  return (a.template get<"x">() * b.template get<"x">()) +
         (a.template get<"y">() * b.template get<"y">()) +
         (a.template get<"z">() * b.template get<"z">());
}

cgfs::Vec3d reflect_ray(const cgfs::Vec3d& ray, const cgfs::Vec3d normal) {
  return 2.0 * normal * dot(normal, ray) - ray;
}

cgfs::Vec3d canvas_to_viewport(const cgfs::Vec2i32& point, const cgfs::Viewport& viewport,
                               const cgfs::Canvas& canvas, const cgfs::Camera& camera) {
  const double distance_camera_to_proj_plane = camera.get<"projection_plane">().get<"distance">();

  const auto c_w = static_cast<double>(canvas.get<"width">());
  const auto c_h = static_cast<double>(canvas.get<"height">());

  return cgfs::Vec3d{static_cast<double>(point.get<"x">()) * viewport.get<"width">() / c_w,
                     static_cast<double>(point.get<"y">()) * viewport.get<"height">() / c_h,
                     distance_camera_to_proj_plane};
}

using RaySphereIntersectResult =
    mguid::NamedTuple<mguid::NamedType<"t1", double>, mguid::NamedType<"t2", double>>;

RaySphereIntersectResult intersect_ray_sphere(const cgfs::Origin& origin,
                                              const cgfs::Vec3d& direction,
                                              const cgfs::Sphere& sphere) {
  const double r = sphere.get<"radius">();

  const cgfs::Vec3d c_o = origin - sphere.get<"center">();

  const double a = dot(direction, direction);
  const double b = 2.0 * dot(c_o, direction);
  const double c = dot(c_o, c_o) - (r * r);

  const double discriminant = (b * b) - (4.0 * a * c);
  if (discriminant < 0.0) {
    return RaySphereIntersectResult{cgfs::basically_infinity, cgfs::basically_infinity};
  }

  const double t1 = (-b + std::sqrt(discriminant)) / (2.0 * a);
  const double t2 = (-b - std::sqrt(discriminant)) / (2.0 * a);

  return RaySphereIntersectResult{t1, t2};
}

double length(const cgfs::Vec3d& vec) {
  return std::sqrt(vec.get<"x">() * vec.get<"x">() + vec.get<"y">() * vec.get<"y">() +
                   vec.get<"z">() * vec.get<"z">());
}

using ClosestIntersectionResult =
    mguid::NamedTuple<mguid::NamedType<"closest_sphere", cgfs::Sphere const*>,
                      mguid::NamedType<"closest_t", double>>;

ClosestIntersectionResult closest_intersection(const cgfs::Origin& origin,
                                               const cgfs::Vec3d& direction, double t_min,
                                               double t_max, const cgfs::Scene& scene) {
  double closest_t_value = cgfs::basically_infinity;  // closest ray object intersection

  cgfs::Sphere const* closest_sphere = nullptr;

  constexpr auto in_range = [](double val, double rng_min, double rng_max) {
    return val > rng_min && val < rng_max;
  };

  for (const auto& sphere : scene.get<"objects">()) {
    const auto [t1, t2] = intersect_ray_sphere(origin, direction, sphere);

    if (in_range(t1, t_min, t_max) && t1 < closest_t_value) {
      closest_t_value = t1;
      closest_sphere = &sphere;
    }
    if (in_range(t2, t_min, t_max) && t2 < closest_t_value) {
      closest_t_value = t2;
      closest_sphere = &sphere;
    }
  }

  return ClosestIntersectionResult{closest_sphere, closest_t_value};
}

double compute_lighting(const cgfs::Vec3d& point, const cgfs::Vec3d& normal,
                        const cgfs::Vec3d& direction_to_cam, double specular,
                        const cgfs::Scene& scene) {
  double cumulative_intensity = 0.0;

  auto compute_diffuse_specular = [&point, &specular, &normal, &direction_to_cam, &scene](
                                      double light_intensity, const auto& direction, double t_max) {
    double intensity = 0.0;

    const auto n_dot_light = dot(normal, direction);

    const auto [shadow_sphere, shadow_t] =
        closest_intersection(point, direction, 0.001, t_max, scene);
    if (shadow_sphere != nullptr) { return 0.0; }

    // Diffuse
    if (n_dot_light > 0.0) {
      intensity += light_intensity * n_dot_light / (length(normal) * length(direction));
    }

    // Specular
    if (specular != -1.0) {
      const auto reflection = normal * (2.0 * n_dot_light) - direction;
      reflect_ray(normal, direction);
      const auto r_dot_v = dot(reflection, direction_to_cam);

      if (r_dot_v > 0.0) {
        intensity += light_intensity *
                     std::pow(r_dot_v / (length(reflection) * length(direction_to_cam)), specular);
      }
    }

    return intensity;
  };

  for (const auto& light : scene.get<"lights">()) {
    cumulative_intensity += light.visit(
        [](const cgfs::AmbientLightProperties& ambient_light) -> double {
          return ambient_light.get<"intensity">();
        },
        [&point,
         &compute_diffuse_specular](const cgfs::PointLightProperties& point_light) -> double {
          const auto direction = point_light.get<"position">() - point;
          return compute_diffuse_specular(point_light.get<"intensity">(), direction, 1.0);
        },
        [&compute_diffuse_specular](
            const cgfs::DirectionalLightProperties& directional_light) -> double {
          const auto direction = directional_light.get<"direction">();

          return compute_diffuse_specular(directional_light.get<"intensity">(), direction,
                                          cgfs::basically_infinity);
        });
  }

  return cumulative_intensity;
}

cgfs::Color3 trace_ray(const cgfs::Origin& origin, const cgfs::Vec3d& direction, double t_min,
                       double t_max, int recursion_depth, const cgfs::Scene& scene) {
  const auto [closest_sphere, closest_t_value] =
      closest_intersection(origin, direction, t_min, t_max, scene);

  if (closest_sphere == nullptr) { return scene.get<"background_color">(); }

  const auto point = origin + (closest_t_value * direction);
  auto normal = point - closest_sphere->get<"center">();
  normal = normal / length(normal);

  constexpr auto scale_by_intensity = [](const cgfs::Color3& color, double intensity) {
    return cgfs::Color3{static_cast<uint8_t>(std::clamp(
                            static_cast<double>(color.get<"r">()) * intensity, 0.0, 255.0)),
                        static_cast<uint8_t>(std::clamp(
                            static_cast<double>(color.get<"g">()) * intensity, 0.0, 255.0)),
                        static_cast<uint8_t>(std::clamp(
                            static_cast<double>(color.get<"b">()) * intensity, 0.0, 255.0))};
  };

  const cgfs::Color3 local_color = scale_by_intensity(
      closest_sphere->get<"material">().get<"color">(),
      compute_lighting(point, normal, -direction,
                       closest_sphere->get<"material">().get<"specular">(), scene));

  const auto reflectiveness = closest_sphere->get<"material">().get<"reflective">();
  if (recursion_depth <= 0 or reflectiveness <= 0.0) { return local_color; }

  const auto reflection = reflect_ray(-direction, normal);
  const auto reflected_color =
      trace_ray(point, reflection, 0.001, cgfs::basically_infinity, recursion_depth - 1, scene);

  const auto scaled_local_color = scale_by_intensity(local_color, 1.0 - reflectiveness);
  const auto scaled_reflected_color = scale_by_intensity(reflected_color, reflectiveness);

  return scaled_local_color + scaled_reflected_color;
}

int main() {
  cgfs::Scene scene{
      std::vector{cgfs::Sphere{cgfs::Vec3d{0.0, -1.0, 3.0}, 1.0,
                               cgfs::MaterialProperties{cgfs::Color3{255, 0, 0}, 500.0, 0.2}},
                  cgfs::Sphere{cgfs::Vec3d{-2.0, 0.0, 4.0}, 1.0,
                               cgfs::MaterialProperties{cgfs::Color3{0, 255, 0}, 10.0, 0.4}},
                  cgfs::Sphere{cgfs::Vec3d{2.0, 0.0, 4.0}, 1.0,
                               cgfs::MaterialProperties{cgfs::Color3{0, 0, 255}, 500.0, 0.3}},
                  cgfs::Sphere{cgfs::Vec3d{0.0, -5001.0, 0.0}, 5000.0,
                               cgfs::MaterialProperties{cgfs::Color3{255, 255, 0}, 1000.0, 0.5}}},
      std::vector{
          cgfs::Light{cgfs::AmbientLightProperties{0.2}},
          cgfs::Light{cgfs::PointLightProperties{0.6, cgfs::Vec3d{2.0, 1.0, 0.0}}},
          cgfs::Light{cgfs::DirectionalLightProperties{0.2, cgfs::Vec3d{1.0, 4.0, 4.0}}},
      },
      cgfs::Color3{0, 0, 0}};

  cgfs::Canvas canvas{cgfs::DimensionsU32{128, 128}};
  cgfs::Viewport viewport{cgfs::DimensionsF64{1.0, 1.0}};
  cgfs::Camera camera{cgfs::Origin{3.0, 0.0, 1.0},
                      cgfs::Mat3d{0.7071, 0.0, -0.7071,
                                     0.0, 1.0,     0.0,
                                  0.7071, 0.0,  0.7071},
                      cgfs::ProjectionPlane{1.0}};

  logger.info("Left: {}, Right: {}, Bottom: {}, Top: {}", canvas.get<"left">(),
              canvas.get<"right">(), canvas.get<"bottom">(), canvas.get<"top">());

  constexpr auto recursion_depth = 2;

  for (auto x{canvas.get<"left">()}; x < canvas.get<"right">(); ++x) {
    for (auto y{canvas.get<"bottom">()}; y < canvas.get<"top">(); ++y) {
      const auto direction = camera.get<"rotation">() *
                             canvas_to_viewport(cgfs::Vec2i32{x, y}, viewport, canvas, camera);
      const auto color = trace_ray(camera.get<"origin">(), direction, 1.0, cgfs::basically_infinity,
                                   recursion_depth, scene);
      canvas.put_pixel(x, y, color);
    }
  }

  canvas.paint();
}

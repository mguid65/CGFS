#include <CGFS/Camera.hpp>
#include <CGFS/Canvas.hpp>
#include <CGFS/Color.hpp>
#include <CGFS/Logger.hpp>
#include <CGFS/Scene.hpp>
#include <CGFS/Viewport.hpp>

#include <mutex>

auto logger = get_logger();

template <typename T>
constexpr T sqrtNewtonRaphson(T x, T current, T prev) {
  return (current == prev) ? current : sqrtNewtonRaphson(x, 0.5 * (current + x / current), current);
}

template <typename T>
constexpr T sqrt(T x) {
  static_assert(std::is_arithmetic_v<T>, "sqrt only supports arithmetic types.");
  return (x >= 0) ? sqrtNewtonRaphson(x, x, T{}) : throw std::runtime_error("Square root of a negative number!");
}

template <typename Type>
constexpr Type dot(const cgfs::Vec3<Type>& a, const cgfs::Vec3<Type>& b) {
  return (a.template get<"x">() * b.template get<"x">()) +
         (a.template get<"y">() * b.template get<"y">()) +
         (a.template get<"z">() * b.template get<"z">());
}

template <typename Base, typename Exponent>
constexpr Base constexprPow(Base base, Exponent exp)
  requires std::is_arithmetic_v<Base> && std::is_arithmetic_v<Exponent>
{
  return (exp == 0)  ? static_cast<Base>(1)
         : (exp > 0) ? base * constexprPow(base, exp - 1)
                     : static_cast<Base>(1) / constexprPow(base, -exp);
}

constexpr cgfs::Vec3d reflect_ray(const cgfs::Vec3d& ray, const cgfs::Vec3d normal) {
  return 2.0 * normal * dot(normal, ray) - ray;
}

template <size_t Height, size_t Width>
constexpr cgfs::Vec3d canvas_to_viewport(const cgfs::Vec2i32& point, const cgfs::Viewport& viewport,
                                         const cgfs::StaticCanvas<Height, Width>& canvas,
                                         const cgfs::Camera& camera) {
  const double distance_camera_to_proj_plane = camera.get<"projection_plane">().get<"distance">();

  const auto c_w = static_cast<double>(canvas.template get<"width">());
  const auto c_h = static_cast<double>(canvas.template get<"height">());

  return cgfs::Vec3d{static_cast<double>(point.get<"x">()) * viewport.get<"width">() / c_w,
                     static_cast<double>(point.get<"y">()) * viewport.get<"height">() / c_h,
                     distance_camera_to_proj_plane};
}

using RaySphereIntersectResult =
    mguid::NamedTuple<mguid::NamedType<"t1", double>, mguid::NamedType<"t2", double>>;

constexpr RaySphereIntersectResult intersect_ray_sphere(const cgfs::Origin& origin,
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

  const double t1 = (-b + sqrt(discriminant)) / (2.0 * a);
  const double t2 = (-b - sqrt(discriminant)) / (2.0 * a);

  return RaySphereIntersectResult{t1, t2};
}

constexpr double length(const cgfs::Vec3d& vec) {
  return sqrt(vec.get<"x">() * vec.get<"x">() + vec.get<"y">() * vec.get<"y">() +
              vec.get<"z">() * vec.get<"z">());
}

using ClosestIntersectionResult =
    mguid::NamedTuple<mguid::NamedType<"closest_sphere", cgfs::Sphere const*>,
                      mguid::NamedType<"closest_t", double>>;

template <size_t NumObjects, size_t NumLights>
constexpr ClosestIntersectionResult closest_intersection(
    const cgfs::Origin& origin, const cgfs::Vec3d& direction, double t_min, double t_max,
    const cgfs::Scene<NumObjects, NumLights>& scene) {
  double closest_t_value = cgfs::basically_infinity;  // closest ray object intersection

  cgfs::Sphere const* closest_sphere = nullptr;

  constexpr auto in_range = [](double val, double rng_min, double rng_max) {
    return val > rng_min && val < rng_max;
  };

  for (const auto& sphere : scene.template get<"objects">()) {
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

template <size_t NumObjects, size_t NumLights>
constexpr double compute_lighting(const cgfs::Vec3d& point, const cgfs::Vec3d& normal,
                                  const cgfs::Vec3d& direction_to_cam, double specular,
                                  const cgfs::Scene<NumObjects, NumLights>& scene) {
  double cumulative_intensity = 0.0;

  constexpr auto compute_diffuse_specular =
      [](const cgfs::Vec3d& inner_point, const cgfs::Vec3d& inner_normal,
         const cgfs::Vec3d& inner_direction_to_cam, double inner_specular,
         const cgfs::Scene<NumObjects, NumLights>& inner_scene, double light_intensity,
         const auto& direction, double t_max) {
        double intensity = 0.0;

        const auto n_dot_light = dot(inner_normal, direction);

        const auto [shadow_sphere, shadow_t] =
            closest_intersection(inner_point, direction, 0.001, t_max, inner_scene);
        if (shadow_sphere != nullptr) { return 0.0; }

        // Diffuse
        if (n_dot_light > 0.0) {
          intensity += light_intensity * n_dot_light / (length(inner_normal) * length(direction));
        }

        // Specular
        if (inner_specular != -1.0) {
          const auto reflection = inner_normal * (2.0 * n_dot_light) - direction;
          reflect_ray(inner_normal, direction);
          const auto r_dot_v = dot(reflection, inner_direction_to_cam);

          if (r_dot_v > 0.0) {
            intensity += light_intensity *
                         constexprPow(r_dot_v / (length(reflection) * length(inner_direction_to_cam)),
                             inner_specular);
          }
        }

        return intensity;
      };

  for (const auto& light : scene.template get<"lights">()) {
    cumulative_intensity += light.visit(
        [](const cgfs::AmbientLightProperties& ambient_light) -> double {
          return ambient_light.get<"intensity">();
        },
        [&compute_diffuse_specular, &point, &specular, &normal, &direction_to_cam,
         &scene](const cgfs::PointLightProperties& point_light) -> double {
          const auto direction = point_light.get<"position">() - point;
          return compute_diffuse_specular(point, normal, direction_to_cam, specular, scene,
                                          point_light.get<"intensity">(), direction, 1.0);
        },
        [&compute_diffuse_specular, &point, &specular, &normal, &direction_to_cam,
         &scene](const cgfs::DirectionalLightProperties& directional_light) -> double {
          const auto direction = directional_light.get<"direction">();

          return compute_diffuse_specular(point, normal, direction_to_cam, specular, scene,
                                          directional_light.get<"intensity">(), direction,
                                          cgfs::basically_infinity);
        });
  }

  return cumulative_intensity;
}

template <size_t NumObjects, size_t NumLights>
constexpr cgfs::Color3 trace_ray(const cgfs::Origin& origin, const cgfs::Vec3d& direction,
                                 double t_min, double t_max, int recursion_depth,
                                 const cgfs::Scene<NumObjects, NumLights>& scene) {
  const auto [closest_sphere, closest_t_value] =
      closest_intersection(origin, direction, t_min, t_max, scene);

  if (closest_sphere == nullptr) { return scene.template get<"background_color">(); }

  const auto point = origin + (closest_t_value * direction);
  auto normal = point - closest_sphere->template get<"center">();
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
      closest_sphere->template get<"material">().template get<"color">(),
      compute_lighting(point, normal, -direction,
                       closest_sphere->template get<"material">().template get<"specular">(), scene));

  const auto reflectiveness = closest_sphere->template get<"material">().template get<"reflective">();
  if (recursion_depth <= 0 or reflectiveness <= 0.0) { return local_color; }

  const auto reflection = reflect_ray(-direction, normal);
  const auto reflected_color =
      trace_ray(point, reflection, 0.001, cgfs::basically_infinity, recursion_depth - 1, scene);

  const auto scaled_local_color = scale_by_intensity(local_color, 1.0 - reflectiveness);
  const auto scaled_reflected_color = scale_by_intensity(reflected_color, reflectiveness);

  return scaled_local_color + scaled_reflected_color;
}

template <size_t Width, size_t Height>
int run() {
  cgfs::Scene scene{
      std::array{cgfs::Sphere{cgfs::Vec3d{0.0, -1.0, 3.0}, 1.0,
                               cgfs::MaterialProperties{cgfs::Color3{255, 0, 0}, 10.0, 0.3}},
                  cgfs::Sphere{cgfs::Vec3d{-2.0, 0.0, 4.0}, 1.0,
                               cgfs::MaterialProperties{cgfs::Color3{255, 255, 0}, 10.0, 0.3}},
                  cgfs::Sphere{cgfs::Vec3d{2.0, 0.0, 4.0}, 1.0,
                               cgfs::MaterialProperties{cgfs::Color3{0, 0, 255}, 10.0, 0.3}},
                  cgfs::Sphere{cgfs::Vec3d{0.0, -5001.0, 0.0}, 5000.0,
                               cgfs::MaterialProperties{cgfs::Color3{100, 100, 100}, 1.0, 0.1}}},
      std::array{
          cgfs::Light{cgfs::AmbientLightProperties{0.2}},
          cgfs::Light{cgfs::PointLightProperties{0.6, cgfs::Vec3d{2.0, 1.0, 0.0}}},
          cgfs::Light{cgfs::DirectionalLightProperties{0.2, cgfs::Vec3d{1.0, 4.0, 4.0}}},
      },
      cgfs::Color3{150, 175, 255}};

  cgfs::StaticCanvas<Height, Width> canvas;
  cgfs::Viewport viewport{cgfs::DimensionsF64{1.0, 1.0}};
  cgfs::Camera camera{cgfs::Origin{0.0, 0.0, 0.0},
                      cgfs::Mat3d{1.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0,
                                  0.0, 0.0, 1.0},
                      cgfs::ProjectionPlane{1.0}};

  const auto top = canvas.template get<"top">();
  const auto right = canvas.template get<"right">();
  const auto bottom = canvas.template get<"bottom">();
  const auto left = canvas.template get<"left">();

  // logger.info("Left: {}, Right: {}, Bottom: {}, Top: {}", top, right, bottom, left);

  constexpr auto recursion_depth = 2;

  const auto cam_rotation = camera.get<"rotation">();
  const auto cam_origin = camera.get<"origin">();

  std::once_flag flag;

  try {
    bool running = true;
    SDL_Event event;

    while (running) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
          running = false;
        }
      }

      std::call_once(flag, [&]() {
        for (auto x{left}; x < right; ++x) {
          for (auto y{bottom}; y < top; ++y) {
            const auto direction =
                cam_rotation * canvas_to_viewport(cgfs::Vec2i32{x, y}, viewport, canvas, camera);
            const auto color =
                trace_ray(cam_origin, direction, 1.0, cgfs::basically_infinity, recursion_depth, scene);
            canvas.put_pixel(x, y, color);
          }
        }
      });

      canvas.render();
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}

int main() {
  return run<720, 720>();
}

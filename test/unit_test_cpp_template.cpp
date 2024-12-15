#include "CGFS/Color.hpp"

#include <catch2/catch_all.hpp>

TEST_CASE("Color3") {
  SECTION("Constructor") {
    constexpr cgfs::Color3 test_color{128, 128, 128};

    STATIC_REQUIRE(test_color.get<"r">() == 128);
    STATIC_REQUIRE(test_color.get<"g">() == 128);
    STATIC_REQUIRE(test_color.get<"b">() == 128);

    STATIC_REQUIRE(test_color.get<0>() == 128);
    STATIC_REQUIRE(test_color.get<1>() == 128);
    STATIC_REQUIRE(test_color.get<2>() == 128);

    STATIC_REQUIRE(std::get<0>(test_color) == 128);
    STATIC_REQUIRE(std::get<1>(test_color) == 128);
    STATIC_REQUIRE(std::get<2>(test_color) == 128);

    STATIC_REQUIRE(mguid::get<"r">(test_color) == 128);
    STATIC_REQUIRE(mguid::get<"g">(test_color) == 128);
    STATIC_REQUIRE(mguid::get<"b">(test_color) == 128);
  }

  SECTION("Maths Operations") {
    // Scalar Multiplication *=
    cgfs::Color3 test_color{128, 128, 128};

    test_color *= 2;
    REQUIRE(mguid::get<"r">(test_color) == 255);
    REQUIRE(mguid::get<"g">(test_color) == 255);
    REQUIRE(mguid::get<"b">(test_color) == 255);

    // Scalar Division /=
    test_color = cgfs::Color3{128, 128, 128};
    test_color /= 2;
    REQUIRE(mguid::get<"r">(test_color) == 64);
    REQUIRE(mguid::get<"g">(test_color) == 64);
    REQUIRE(mguid::get<"b">(test_color) == 64);

    // Scalar Addition +=
    test_color = cgfs::Color3{100, 100, 100};
    test_color += 50;
    REQUIRE(mguid::get<"r">(test_color) == 150);
    REQUIRE(mguid::get<"g">(test_color) == 150);
    REQUIRE(mguid::get<"b">(test_color) == 150);

    // Scalar Subtraction -=
    test_color = cgfs::Color3{100, 100, 100};
    test_color -= 50;
    REQUIRE(mguid::get<"r">(test_color) == 50);
    REQUIRE(mguid::get<"g">(test_color) == 50);
    REQUIRE(mguid::get<"b">(test_color) == 50);

    // Color3 Addition +=
    cgfs::Color3 color1{100, 50, 200};
    cgfs::Color3 color2{100, 100, 100};
    color1 += color2;
    REQUIRE(mguid::get<"r">(color1) == 200);
    REQUIRE(mguid::get<"g">(color1) == 150);
    REQUIRE(mguid::get<"b">(color1) == 255);

    // Color3 Subtraction -=
    color1 = cgfs::Color3{150, 150, 150};
    color2 = cgfs::Color3{50, 100, 200};
    color1 -= color2;
    REQUIRE(mguid::get<"r">(color1) == 100);
    REQUIRE(mguid::get<"g">(color1) == 50);
    REQUIRE(mguid::get<"b">(color1) == 0);

    // Color3 Addition +
    cgfs::Color3 color_add = color1 + color2;
    REQUIRE(mguid::get<"r">(color_add) == 150);
    REQUIRE(mguid::get<"g">(color_add) == 150);
    REQUIRE(mguid::get<"b">(color_add) == 200);

    // Color3 Subtraction -
    cgfs::Color3 color_sub = color1 - color2;
    REQUIRE(mguid::get<"r">(color_sub) == 50);
    REQUIRE(mguid::get<"g">(color_sub) == 0);
    REQUIRE(mguid::get<"b">(color_sub) == 0);

    // Static Tests for Compile-Time Verification
    constexpr cgfs::Color3 static_test_color = []() {
      cgfs::Color3 c{128, 128, 128};
      c *= 2;
      return c;
    }();
    STATIC_REQUIRE(mguid::get<"r">(static_test_color) == 255);
    STATIC_REQUIRE(mguid::get<"g">(static_test_color) == 255);
    STATIC_REQUIRE(mguid::get<"b">(static_test_color) == 255);

    constexpr cgfs::Color3 static_color_sub = []() {
      cgfs::Color3 c1{200, 200, 200};
      cgfs::Color3 c2{100, 150, 250};
      c1 -= c2;
      return c1;
    }();
    STATIC_REQUIRE(mguid::get<"r">(static_color_sub) == 100);
    STATIC_REQUIRE(mguid::get<"g">(static_color_sub) == 50);
    STATIC_REQUIRE(mguid::get<"b">(static_color_sub) == 0);
  }

  SECTION("Clamping Behavior") {
    // Clamping for Multiplication *=
    cgfs::Color3 color1{200, 200, 200};

    color1 *= 2; // This should clamp to 255
    REQUIRE(mguid::get<"r">(color1) == 255);
    REQUIRE(mguid::get<"g">(color1) == 255);
    REQUIRE(mguid::get<"b">(color1) == 255);

    // Clamping for Division /=
    cgfs::Color3 color2{1, 1, 1};

    color2 /= 5; // This should clamp to 0
    REQUIRE(mguid::get<"r">(color2) == 0);
    REQUIRE(mguid::get<"g">(color2) == 0);
    REQUIRE(mguid::get<"b">(color2) == 0);

    // Clamping for Addition +=
    cgfs::Color3 color3{250, 240, 230};

    color3 += 20; // This should clamp to 255
    REQUIRE(mguid::get<"r">(color3) == 255);
    REQUIRE(mguid::get<"g">(color3) == 255);
    REQUIRE(mguid::get<"b">(color3) == 250);

    // Clamping for Subtraction -=
    cgfs::Color3 color4{10, 20, 30};

    color4 -= 50; // This should clamp to 0
    REQUIRE(mguid::get<"r">(color4) == 0);
    REQUIRE(mguid::get<"g">(color4) == 0);
    REQUIRE(mguid::get<"b">(color4) == 0);

    // Clamping for Color3 Addition +=
    cgfs::Color3 color5{250, 240, 230};
    cgfs::Color3 color6{10, 20, 30};

    color5 += color6; // Should clamp to 255
    REQUIRE(mguid::get<"r">(color5) == 255);
    REQUIRE(mguid::get<"g">(color5) == 255);
    REQUIRE(mguid::get<"b">(color5) == 255);

    // Clamping for Color3 Subtraction -=
    cgfs::Color3 color7{10, 10, 10};
    cgfs::Color3 color8{20, 20, 20};

    color7 -= color8; // Should clamp to 0
    REQUIRE(mguid::get<"r">(color7) == 0);
    REQUIRE(mguid::get<"g">(color7) == 0);
    REQUIRE(mguid::get<"b">(color7) == 0);

    // Static Tests for Clamping Behavior
    constexpr cgfs::Color3 static_test_clamp_max = []() {
      cgfs::Color3 c{200, 200, 200};
      c *= 2;
      return c;
    }();
    STATIC_REQUIRE(mguid::get<"r">(static_test_clamp_max) == 255);
    STATIC_REQUIRE(mguid::get<"g">(static_test_clamp_max) == 255);
    STATIC_REQUIRE(mguid::get<"b">(static_test_clamp_max) == 255);

    constexpr cgfs::Color3 static_test_clamp_min = []() {
      cgfs::Color3 c{10, 10, 10};
      c -= 20;
      return c;
    }();
    STATIC_REQUIRE(mguid::get<"r">(static_test_clamp_min) == 0);
    STATIC_REQUIRE(mguid::get<"g">(static_test_clamp_min) == 0);
    STATIC_REQUIRE(mguid::get<"b">(static_test_clamp_min) == 0);
  }
}
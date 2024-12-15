/**
 * @author Matthew Guidry (github: mguid65)
 * @date 2024-10-11
 *
 * @cond IGNORE_LICENSE
 *
 * MIT License
 *
 * Copyright (c) 2024 Matthew Guidry
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @endcond
 */

#ifndef MGUID_CGFS_CANVAS_HPP
#define MGUID_CGFS_CANVAS_HPP

#include <CGFS/Common.hpp>
#include <CGFS/Color.hpp>

#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <vector>

#include <fmt/format.h>

namespace cgfs {

inline std::string get_ascii_esc_color_block(uint8_t r, uint8_t g, uint8_t b) {
  return fmt::format("\033[48;2;{};{};{}m   \033[0m", +r, +g, +b);
}

class Renderer {
public:
    Renderer(int width, int height)
        : m_width(width), m_height(height) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            throw std::runtime_error("Failed to initialize SDL");
        }

        window = SDL_CreateWindow("Pixel Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
        if (!window) {
            throw std::runtime_error("Failed to create SDL window");
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            throw std::runtime_error("Failed to create SDL renderer");
        }

        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (!texture) {
            throw std::runtime_error("Failed to create SDL texture");
        }

        pixelBuffer = new uint8_t[width * height * 3](); // RGB24 format
    }

    ~Renderer() {
        delete[] pixelBuffer;
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void put_pixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b) {
        if (x < 0 || x >= m_width || y < 0 || y >= m_height) return;

        // Calculate the pixel index in the buffer
        int index = (y * m_width + x) * 3; // RGB24: 3 bytes per pixel
        pixelBuffer[index] = r;
        pixelBuffer[index + 1] = g;
        pixelBuffer[index + 2] = b;
    }

    void render() {
        // Update the texture with the pixel buffer
        SDL_UpdateTexture(texture, nullptr, pixelBuffer, m_width * 3);

        // Clear the renderer and copy the texture to it
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);

        // Present the renderer
        SDL_RenderPresent(renderer);
    }

    void clear(Color3 color) {
        for (int y = 0; y < m_height; ++y) {
            for (int x = 0; x < m_width; ++x) {
                put_pixel(x, y, color.get<"r">(), color.get<"g">(), color.get<"b">());
            }
        }
    }

private:
    int m_width, m_height;
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    SDL_Texture* texture{nullptr};
    uint8_t* pixelBuffer{nullptr};
};

struct RGBRaw {
  uint8_t r{0}, g{0}, b{0};
};

template <size_t Height, size_t Width>
using Buffer2D = std::array<std::array<RGBRaw, Width>, Height>;

template <size_t Height, size_t Width>
using Buffer = mguid::NamedTuple<mguid::NamedType<"buffer", Buffer2D<Height, Width>>>;

template <size_t Height, size_t Width>
struct StaticCanvas : DimensionsU32, BBoxi32, Buffer<Height, Width>, Renderer {
  using BBoxi32::get;
  using Buffer<Height, Width> ::get;
  using DimensionsU32::get;
  using Renderer::put_pixel;

  constexpr StaticCanvas()
      : DimensionsU32{Width, Height},
        BBoxi32{
            -static_cast<int32_t>(get<"width">()) / 2,
            get<"width">() / 2,
            -static_cast<int32_t>(get<"height">()) / 2,
            get<"height">() / 2,
        },
        Buffer<Height, Width>{},
        Renderer{Width, Height} {}

  void put_pixel(int32_t x, [[maybe_unused]] int32_t y, Color3 color) {
    const uint8_t r = color.get<"r">();
    const uint8_t g = color.get<"g">();
    const uint8_t b = color.get<"b">();

    const auto width = get<"width">();
    const auto height = get<"height">();

    x = (static_cast<int32_t>(width) / 2) + x;
    y = (static_cast<int32_t>(height) / 2) - y;

    if (x < 0 || x >= static_cast<int32_t>(width) || y < 0 || y >= static_cast<int32_t>(height)) {
      return;
    }

    this->put_pixel(x, y, r, g, b);
  }
  //
  // [[nodiscard]] int show() const {
  //   const auto width = get<"width">();
  //   const auto height = get<"height">();
  //
  //   try {
  //     Renderer renderer(width, height);
  //
  //     bool running = true;
  //     SDL_Event event;
  //
  //     while (running) {
  //       while (SDL_PollEvent(&event)) {
  //         if (event.type == SDL_QUIT) {
  //           running = false;
  //         }
  //       }
  //
  //       // Example: Draw a gradient
  //       for (int y = 0; y < height; ++y) {
  //         for (int x = 0; x < width; ++x) {
  //           renderer.put_pixel(x, y, uint8_t(x % 256), uint8_t(y % 256), uint8_t((x + y) % 256) });
  //         }
  //       }
  //
  //       renderer.render();
  //     }
  //   } catch (const std::exception& e) {
  //     std::cerr << e.what() << std::endl;
  //     return 1;
  //   }
  //
  //   return 0;
  // }
};

}  // namespace cgfs

#endif  // MGUID_CGFS_CANVAS_HPP

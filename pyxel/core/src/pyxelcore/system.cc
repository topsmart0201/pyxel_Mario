#include "pyxelcore/system.h"

#include "pyxelcore/canvas.h"
#include "pyxelcore/constants.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>

namespace pyxelcore {

System::System(int32_t width,
               int32_t height,
               const char* caption,
               int32_t scale,
               const int32_t* palette,
               int32_t fps,
               int32_t border_width,
               int32_t border_color) {
  width_ = width;
  height_ = height;
  caption_ = caption ? std::string(caption) : DEFAULT_CAPTION;
  scale_ = scale != -1 ? scale : DEFAULT_SCALE;

  palette = palette ? palette : DEFAULT_PALETTE;
  for (int32_t i = 0; i < COLOR_COUNT; i++) {
    palette_[i] = palette[i];
  }

  fps_ = std::max(fps != -1 ? fps : DEFAULT_FPS, 1);
  border_width_ = border_width != -1 ? border_width : DEFAULT_BORDER_WIDTH;
  border_color_ = border_color != -1 ? border_color : DEFAULT_BORDER_COLOR;

  screen_ = new Canvas(width, height, COLOR_COUNT);

  image_ = new Canvas*[IMAGE_BANK_COUNT];
  for (int32_t i = 0; i < IMAGE_BANK_COUNT; i++) {
    image_[i] = new Canvas(IMAGE_BANK_WIDTH, IMAGE_BANK_HEIGHT, COLOR_COUNT);
  }

  printf("bbb\n");

  SDL_Init(SDL_INIT_VIDEO);

  window_ = SDL_CreateWindow(caption_.c_str(), SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, width_, height_, 0);
  renderer_ = SDL_CreateRenderer(window_, -1, 0);

  // int32_t flags = IMG_INIT_PNG;
  // int initted = IMG_Init(flags);
  // if ((initted & flags) != flags) {
  //  printf("IMG_Init: Failed to init required jpg and png support!\n");
  //  printf("IMG_Init: %s\n", IMG_GetError());
  //  return;
  // }

  // SDL_Surface *image = IMG_Load("../examples/assets/pyxel_logo_152x64.png");

  // if (!image) {
  //  printf("IMG_Load: %s\n", IMG_GetError());
  //  return;
  // }

  // temp_texture_ = SDL_CreateTextureFromSurface(renderer_, image);

  screen_texture_ =
      SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGB888,
                        SDL_TEXTUREACCESS_STREAMING, width_, height_);
}

System::~System() {
  for (int32_t i = 0; i < IMAGE_BANK_COUNT; i++) {
    delete image_[i];
  }
  delete[] image_;

  delete screen_;
}

Canvas* System::GetImage(int32_t img, bool system) {
  if (img < 0 || img >= IMAGE_BANK_COUNT) {
    // error
  }

  if (img == IMAGE_BANK_COUNT - 1 && !system) {
    // error
  }

  return image_[img];
}

Tilemap* System::GetTilemap(int32_t tm) {
  //
  return tilemap_[tm];
}

void System::RunApplication(void (*update)(), void (*draw)()) {
  SDL_Event ev;

  double one_frame_time = 1000.0f / fps_;
  double next_update_time = SDL_GetTicks();

  while (1) {
    double sleep_time = next_update_time - SDL_GetTicks();

    if (sleep_time > 0) {
      SDL_Delay(static_cast<int32_t>(sleep_time / 2));
      continue;
    }

    int32_t update_frame_count =
        std::min(static_cast<int32_t>(-sleep_time / one_frame_time) + 1, 10);

    next_update_time += one_frame_time * update_frame_count;

    for (int32_t i = 0; i < update_frame_count; i++) {
      while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT)
          return;
      }

      update();
    }

    draw();

    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    UpdateScreenTexture();
    SDL_RenderCopy(renderer_, screen_texture_, NULL, NULL);

    SDL_RenderPresent(renderer_);
  }
}

void System::QuitApplication() {}

void System::UpdateScreenTexture() {
  int32_t* pixel;
  int32_t pitch;
  size_t size = width_ * height_;

  SDL_LockTexture(screen_texture_, NULL, (void**)&pixel, &pitch);

  int32_t* framebuffer = screen_->Data();

  for (size_t i = 0; i < size; i++) {
    pixel[i] = palette_[framebuffer[i]];
  }

  SDL_UnlockTexture(screen_texture_);
}

void System::LoadImage(Canvas* image,
                       int32_t x,
                       int32_t y,
                       const char* filename) {
  //
}

void System::LoadAsset(const char* filename) {
  //
}

void System::SaveAsset(const char* filename) {
  //
}

}  // namespace pyxelcore

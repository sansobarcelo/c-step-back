#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

// Framebuffer (32-bit ARGB)
uint32_t framebuffer[WIDTH * HEIGHT];

// Function to create a 32-bit ARGB color
uint32_t create_color(int r, int g, int b) {
  return (255 << 24) | (r << 16) | (g << 8) | b; // 0xAARRGGBB
}

// Function to set a pixel in the framebuffer
void set_pixel(int x, int y, int r, int g, int b) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    framebuffer[y * WIDTH + x] = create_color(r, g, b);
  }
}

// Draw a simple gradient
void draw_gradient() {
  for (int y = 0; y < HEIGHT; y++) {
    int color_value = (y * 255) / HEIGHT;
    for (int x = 0; x < WIDTH; x++) {
      // int r = (x * 255) / WIDTH;
      // int g = (y * 255) / HEIGHT;
      // int b = 128; // Fixed blue tone
      int r = color_value;
      int g = color_value;
      int b = color_value;
      set_pixel(x, y, r, g, b);
    }
  }
}

void clear_color(int r, int g, int b) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      set_pixel(x, y, r, g, b);
    }
  }
}

// Save framebuffer to a PPM file (convert `uint32_t` to RGB)
void save_ppm(const char *filename) {
  FILE *f;
  fopen_s(&f, filename, "wb");
  if (!f) {
    perror("Failed to open file");
    return;
  }

  // Write PPM header
  fprintf(f, "P6\n%d %d\n255\n", WIDTH, HEIGHT);

  // Write pixel data (convert 32-bit ARGB to 24-bit RGB)
  for (int i = 0; i < WIDTH * HEIGHT; i++) {
    uint32_t color = framebuffer[i];
    uint8_t rgb[3] = {
        (color >> 16) & 0xFF, // Extract Red
        (color >> 8) & 0xFF,  // Extract Green
        (color) & 0xFF        // Extract Blue
    };
    fwrite(rgb, 1, 3, f);
  }
  fclose(f);
}

// Plot line
void plot_line(int x0, int y0, int x1, int y1, int r, int g, int b) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        set_pixel(x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) break;  // Exit condition

        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }  // Move in x direction
        if (e2 <= dx) { err += dx; y0 += sy; }  // Move in y direction
    }
}

int main() {
  // draw_gradient();        // Draw something
  clear_color(0, 0, 0);
  plot_line(100, 100, 300, 300, 10, 244, 10);
  plot_line(300, 500, 500, 500, 10, 244, 10);
  plot_line(550, 500, 700, 300, 10, 244, 10);
  save_ppm("output.ppm"); // Save as PPM
  printf("Saved output.ppm\n");
  return 0;
}

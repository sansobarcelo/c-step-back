#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
  f = fopen(filename, "wb");
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

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void fill_span(int y, int x_start, int x_end, uint8_t r, uint8_t g, uint8_t b) {
  if (y < 0 || y >= HEIGHT)
    return; // Clipping
  if (x_start > x_end)
    swap(&x_start, &x_end);

  if (x_end < 0 || x_start >= WIDTH)
    return; // Clipping

  for (int x = (x_start < 0 ? 0 : x_start);
       x <= (x_end >= WIDTH ? WIDTH - 1 : x_end); x++) {
    set_pixel(x, y, r, g, b);
  }
}

// Function to draw a filled triangle
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2,
                          uint8_t r, uint8_t g, uint8_t b) {
  // Sort vertices by Y-coordinate (y0 <= y1 <= y2)
  if (y0 > y1) {
    swap(&x0, &x1);
    swap(&y0, &y1);
  }
  if (y0 > y2) {
    swap(&x0, &x2);
    swap(&y0, &y2);
  }
  if (y1 > y2) {
    swap(&x1, &x2);
    swap(&y1, &y2);
  }

  // Compute inverse slopes
  float dx0 = (y1 - y0) ? (float)(x1 - x0) / (y1 - y0) : 0;
  float dx1 = (y2 - y0) ? (float)(x2 - x0) / (y2 - y0) : 0;
  float dx2 = (y2 - y1) ? (float)(x2 - x1) / (y2 - y1) : 0;

  // Rasterize top half (flat-bottom)
  float x_left = x0, x_right = x0;
  for (int y = y0; y < y1; y++) {
    fill_span(y, (int)x_left, (int)x_right, r, g, b);
    x_left += dx0;
    x_right += dx1;
  }

  // Rasterize bottom half (flat-top)
  x_left = x1;
  for (int y = y1; y < y2; y++) {
    fill_span(y, (int)x_left, (int)x_right, r, g, b);
    x_left += dx2;
    x_right += dx1;
  }
}

// Plot line
void plot_line(int x0, int y0, int x1, int y1, int r, int g, int b) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while (1) {
    set_pixel(x0, y0, r, g, b);
    if (x0 == x1 && y0 == y1)
      break; // Exit condition

    e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    } // Move in x direction
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    } // Move in y direction
  }
}

// Function to compute perpendicular offset points
void compute_perpendicular_offset(int x0, int y0, int x1, int y1, float width,
                                  int *ox0, int *oy0, int *ox1, int *oy1) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float length = sqrtf(dx * dx + dy * dy);

  if (length == 0)
    return; // Avoid division by zero

  // Normalize perpendicular vector
  float nx = -dy / length;
  float ny = dx / length;

  // Scale by half the line width
  float half_width = width * 0.5f;
  nx *= half_width;
  ny *= half_width;

  // Compute the two offset points for each endpoint
  *ox0 = (int)(x0 + nx);
  *oy0 = (int)(y0 + ny);
  *ox1 = (int)(x1 + nx);
  *oy1 = (int)(y1 + ny);
}

// Function to draw a variable-width line as two triangles
void draw_variable_width_line(int x0, int y0, int x1, int y1, float width,
                              uint8_t r, uint8_t g, uint8_t b) {
  int ax0, ay0, ax1, ay1;
  int bx0, by0, bx1, by1;

  // Compute offset points for both sides of the line
  compute_perpendicular_offset(x0, y0, x1, y1, width, &ax0, &ay0, &ax1, &ay1);
  compute_perpendicular_offset(x0, y0, x1, y1, -width, &bx0, &by0, &bx1, &by1);

  // Draw two triangles to form the rectangle
  draw_filled_triangle(ax0, ay0, ax1, ay1, bx0, by0, r, g, b);
  draw_filled_triangle(ax1, ay1, bx0, by0, bx1, by1, r, g, b);
}

int main() {
  // draw_gradient();        // Draw something
  clear_color(0, 0, 0);
  plot_line(100, 100, 300, 300, 10, 244, 10);
  plot_line(300, 500, 500, 500, 10, 244, 10);
  plot_line(550, 500, 700, 300, 10, 244, 10);
  draw_variable_width_line(100, 100, 300, 200, 10.0f, 255, 0,
                           0); // Red thick line
  save_ppm("output.ppm");      // Save as PPM
  printf("Saved output.ppm\n");
  return 0;
}

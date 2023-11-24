#include <stdio.h>
#include <stdlib.h>
#include "read_print_terrain_map.h"

#define MAX_WIDTH 1000
#define MAX_HEIGHT 1000

int pixel_eq(pixel_t p1, pixel_t p2) {
    return p1.red == p2.red && p1.green == p2.green && p1.blue == p2.blue;
}

enum cell_types get_type_of_pixel(pixel_t p) {
    pixel_t color_types[] = {{120, 120, 120},
                           {210, 230, 130},
                           {50, 100, 60},
                           {170, 140, 70},
                           {50, 70, 100}};
    for (int i = 0; i < 5; i++) {
        if (pixel_eq(p, color_types[i])) {
            return (enum cell_types)i;
        }
    }
    return CELL_INVALID;
}

void free_map(map_t map) {
  for (int i = 0; i < map.width; i++)
    free(map.data[i]);
  free(map.data);
}

map_t map_from_ppm(const char* file_path) {
  FILE* file = fopen(file_path, "rb");

  if (file == NULL) {
    perror("read_map_from_ppm");
    exit(EXIT_FAILURE);
  }

  char header[3];
  (void) fgets(header, sizeof(header), file);

  // TODO: Support more than just the P6 PPM.
  if (header[0] != 'P' || header[1] != '6') {
    fprintf(stderr, "File does not have the correct magic bytes.\n");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  // P6: read the width, height, and maximum color value
  int width, height, max_color;
  (void) fscanf(file, "%d %d %d", &width, &height, &max_color);

  // check if the color is 8 bit and image is proper size
  if (width >= MAX_WIDTH && height >= MAX_HEIGHT && max_color != 255) {
    fprintf(stderr, "Unsupported size or color depth.\n");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  // skip the newline character after the header
  if (fgetc(file) != '\n') {
    fprintf(stderr, "Expected newline was not in fact a new line char.\n");
    fclose(file);
    exit(EXIT_FAILURE);
  };

  map_t terrain_map;
  terrain_map.width = width;
  terrain_map.height = height;
  terrain_map.data = (int**) calloc(width, sizeof(int*));

  if (terrain_map.data == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  pixel_t temp;
  for (int i = 0; i < width; i++) {
    terrain_map.data[i] = (int*) calloc(height, sizeof(int));
    if (terrain_map.data[i] == NULL) {
      fprintf(stderr, "Memory allocation failed\n");
      fclose(file);
      for (int g = 0; g < i; g++)
        free(terrain_map.data[g]);
      free(terrain_map.data);
      exit(EXIT_FAILURE);
    }
    for (int j = 0; j < height; j++) {
      (void) fread(&temp, sizeof(pixel_t), 1, file);
      terrain_map.data[i][j] = get_type_of_pixel(temp);
    }
  }

  fclose(file);
  return terrain_map;
}

// TODO: Move this out.
int main() {
  char file_path[1024];
  printf("Enter file path: ");
  (void) scanf(" %s", file_path);

  map_t terrain_map = map_from_ppm(file_path);

  for (int i = 0; i < terrain_map.width; i++) {
    for (int j = 0; j < terrain_map.height; j++) {
      printf("%1d", terrain_map.data[i][j]);
    }
    printf("\n");
  }

  free_map(terrain_map);
  printf("Thank you for using our software :)\n");

  return 0;
}

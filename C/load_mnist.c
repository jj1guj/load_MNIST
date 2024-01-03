#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int load_file(char* file_path, unsigned char** buf) {
  FILE* file;
  file = fopen(file_path, "rb");
  if (file == NULL) {
    fprintf(stderr, "failed to open file.\n");
    return -1;
  }

  // get file length
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *buf = (char*)malloc(file_size);
  size_t bytes_read = fread(*buf, 1, file_size, file);

  if (bytes_read != file_size) {
    fprintf(stderr, "failed to load data from file.\n");
    fclose(file);
    free(*buf);
    return -1;
  }

  fclose(file);
  return 0;
}

int main() {
  char* img_path = "../train-images.idx3-ubyte";
  char* label_path = "../train-labels.idx1-ubyte";
  unsigned char *imgs_byte, *labels_byte;
  int stat;

  // load images binary
  stat = load_file(img_path, &imgs_byte);
  if (stat != 0)
    return stat;

  // load labels binary
  stat = load_file(label_path, &labels_byte);
  if (stat != 0)
    return stat;

  size_t data_num = ntohl(*((size_t*)(&imgs_byte[4])));
  size_t img_height = ntohl(*((size_t*)(&imgs_byte[8])));
  size_t img_width = ntohl(*((size_t*)(&imgs_byte[12])));
  size_t pixel_num = img_height * img_width;
  printf("data_num: %d, img_height: %d, img_width: %d\n", data_num, img_height,
         img_width);

  // alloc images
  unsigned char*** imgs =
      (unsigned char***)malloc(data_num * sizeof(unsigned char**));
  for (int i = 0; i < data_num; ++i) {
    imgs[i] = (unsigned char**)malloc(img_height * sizeof(unsigned char*));
    for (int j = 0; j < img_height; ++j) {
      imgs[i][j] = (unsigned char*)malloc(img_width * sizeof(unsigned char));
    }
  }

  // alloc labels
  unsigned char* labels =
      (unsigned char*)malloc(data_num * sizeof(unsigned char));

  int img_offset = 16;
  int label_offset = 8;
  for (int i = 0; i < data_num; ++i) {
    // load image
    for(int j = 0; j < img_height; ++j) {
      for(int k = 0; k < img_width; ++k) {
        imgs[i][j][k] = imgs_byte[img_offset];
        ++img_offset;
      }
    }

    // load label
    labels[i] = labels_byte[label_offset];
    ++label_offset;
  }

  // disp image
  printf("image:\n");
  for(int i = 0; i < img_height; ++i) {
    for(int j = 0; j < img_width; ++j) {
      printf("%03d ", imgs[data_num - 1][i][j]);
    }
    printf("\n");
  }

  // disp label
  printf("label: %d", labels[data_num - 1]);

  // Free allocated memory
  free(imgs_byte);
  free(labels_byte);

  return 0;
}

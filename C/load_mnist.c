#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Function to load file into buffer
int load_file(const char* file_path, unsigned char** buf, long* file_size) {
  FILE* file = fopen(file_path, "rb");
  if (file == NULL) {
    fprintf(stderr, "failed to open file: %s\n", file_path);
    return -1;
  }

  fseek(file, 0, SEEK_END);
  *file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *buf = (unsigned char*)malloc(*file_size);
  if (*buf == NULL) {
    fprintf(stderr, "failed to allocate memory.\n");
    fclose(file);
    return -1;
  }

  size_t bytes_read = fread(*buf, 1, *file_size, file);
  fclose(file);

  if (bytes_read != *file_size) {
    fprintf(stderr, "failed to load data from file.\n");
    free(*buf);
    return -1;
  }

  return 0;
}

// Function to allocate 3D unsigned char array
unsigned char*** allocate_3d_array(size_t dim1, size_t dim2, size_t dim3) {
  unsigned char*** array =
      (unsigned char***)malloc(dim1 * sizeof(unsigned char**));
  if (array == NULL) {
    fprintf(stderr, "failed to allocate memory for 3D array.\n");
    return NULL;
  }

  for (size_t i = 0; i < dim1; ++i) {
    array[i] = (unsigned char**)malloc(dim2 * sizeof(unsigned char*));
    if (array[i] == NULL) {
      fprintf(stderr, "failed to allocate memory for 2D array.\n");
      // Free previously allocated memory
      for (size_t j = 0; j < i; ++j) {
        free(array[j]);
      }
      free(array);
      return NULL;
    }

    for (size_t j = 0; j < dim2; ++j) {
      array[i][j] = (unsigned char*)malloc(dim3 * sizeof(unsigned char));
      if (array[i][j] == NULL) {
        fprintf(stderr, "failed to allocate memory for 1D array.\n");
        // Free previously allocated memory
        for (size_t k = 0; k < j; ++k) {
          free(array[i][k]);
        }
        free(array[i]);
        // Free previously allocated memory for dim1
        for (size_t k = 0; k < i; ++k) {
          free(array[k]);
        }
        free(array);
        return NULL;
      }
    }
  }

  return array;
}

// Function to free 3D unsigned char array
void free_3d_array(unsigned char*** array, size_t dim1, size_t dim2) {
  if (array == NULL) {
    return;
  }

  for (size_t i = 0; i < dim1; ++i) {
    for (size_t j = 0; j < dim2; ++j) {
      free(array[i][j]);
    }
    free(array[i]);
  }

  free(array);
}

int main() {
  char* img_path = "../train-images.idx3-ubyte";
  char* label_path = "../train-labels.idx1-ubyte";
  unsigned char *imgs_byte, *labels_byte;
  int stat;

  long img_file_size, label_file_size;

  // load images binary
  stat = load_file(img_path, &imgs_byte, &img_file_size);
  if (stat != 0)
    return stat;

  // load labels binary
  stat = load_file(label_path, &labels_byte, &label_file_size);
  if (stat != 0) {
    // Free memory allocated for images
    free(imgs_byte);
    return stat;
  }

  size_t data_num = ntohl(*((size_t*)(&imgs_byte[4])));
  size_t img_height = ntohl(*((size_t*)(&imgs_byte[8])));
  size_t img_width = ntohl(*((size_t*)(&imgs_byte[12])));
  printf("data_num: %zu, img_height: %zu, img_width: %zu\n", data_num,
         img_height, img_width);

  // alloc images
  unsigned char*** imgs = allocate_3d_array(data_num, img_height, img_width);
  if (imgs == NULL) {
    // Free memory allocated for images and labels
    free(imgs_byte);
    free(labels_byte);
    return -1;
  }

  // alloc labels
  unsigned char* labels =
      (unsigned char*)malloc(data_num * sizeof(unsigned char));
  if (labels == NULL) {
    fprintf(stderr, "failed to allocate memory for labels.\n");
    // Free memory allocated for images and 3D array
    free_3d_array(imgs, data_num, img_height);
    free(imgs_byte);
    free(labels_byte);
    return -1;
  }

  int img_offset = 16;
  int label_offset = 8;
  for (size_t i = 0; i < data_num; ++i) {
    // load image
    for (size_t j = 0; j < img_height; ++j) {
      for (size_t k = 0; k < img_width; ++k) {
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
  for (size_t i = 0; i < img_height; ++i) {
    for (size_t j = 0; j < img_width; ++j) {
      printf("%03d ", imgs[data_num - 1][i][j]);
    }
    printf("\n");
  }

  // disp label
  printf("label: %d", labels[data_num - 1]);

  // Free allocated memory
  free_3d_array(imgs, data_num, img_height);
  free(imgs_byte);
  free(labels_byte);
  free(labels);

  return 0;
}

#include <stdio.h>
#include <stdlib.h>

int load_file(char* file_path, char* buf) {
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

  char* buffer = (char*)malloc(file_size);
  size_t bytes_read = fread(buffer, 1, file_size, file);

  if (bytes_read != file_size) {
    fprintf(stderr, "failed to load data from file.\n");
    fclose(file);
    free(buffer);
    return -1;
  }

  fclose(file);
  return 0;
}

int main() {
  char* img_path = "../train-images.idx3-ubyte";
  char* label_path = "../train-labels.idx1-ubyte";
  char *imgs_byte, *labels_byte;
  int stat;

  // load images binary
  load_file(img_path, imgs_byte);
  // load labels binary
  load_file(label_path, labels_byte);

  return 0;
}
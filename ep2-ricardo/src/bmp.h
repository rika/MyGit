#ifndef BMP_H
#define BMP_H

typedef struct {
  unsigned long sizeX;
  unsigned long sizeY;
  char *data;
} Image;

int ImageLoad(char *filename, Image *image);

#endif

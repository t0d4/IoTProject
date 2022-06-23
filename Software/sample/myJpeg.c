#include "myJpeg.h"

int jpegFileReadDecode(BITMAPDATA_t *bitmapData, const char* filename){
  struct jpeg_decompress_struct jpeg;
  struct jpeg_error_mgr err;

  FILE *fi;
  int j;
  JSAMPLE *tmp;

  jpeg.err = jpeg_std_error(&err);

  fi = fopen(filename, "rb");
  if(fi == NULL){
    printf("%sは開けません\n", filename);
    return -1;
  }

  jpeg_create_decompress(&jpeg);
  jpeg_stdio_src(&jpeg, fi);
  jpeg_read_header(&jpeg, TRUE);
  jpeg_start_decompress(&jpeg);

  printf("width = %d, height = %d, ch = %d\n", jpeg.output_width, jpeg.output_height, jpeg.out_color_components);

  bitmapData->data =
    (unsigned char*)malloc(sizeof(unsigned char) * jpeg.output_width * jpeg.output_height * jpeg.out_color_components);
  if(bitmapData->data == NULL){
    printf("data malloc error\n");
    fclose(fi);
    jpeg_destroy_decompress(&jpeg);
    return -1;
  }

  for(j = 0; j < jpeg.output_height; j++){
    tmp = bitmapData->data + j * jpeg.out_color_components * jpeg.output_width;
    jpeg_read_scanlines(&jpeg, &tmp, 1);
  }

  bitmapData->height = jpeg.output_height;
  bitmapData->width = jpeg.output_width;
  bitmapData->ch = jpeg.out_color_components;

  jpeg_finish_decompress(&jpeg);
  jpeg_destroy_decompress(&jpeg);

  fclose(fi);

  return 0;
}

int jpegFileEncodeWrite(BITMAPDATA_t *bitmapData, const char *filename){
  struct jpeg_compress_struct jpeg;
  struct jpeg_error_mgr err;
  FILE *fo;
  JSAMPLE *address;
  int j;

  jpeg.err = jpeg_std_error(&err);
  jpeg_create_compress(&jpeg);

  fo = fopen(filename, "wb");
  if(fo == NULL){
    printf("%sは開けません\n", filename);
    jpeg_destroy_compress(&jpeg);
    return -1;
  }

  jpeg_stdio_dest(&jpeg, fo);

  jpeg.image_width = bitmapData->width;
  jpeg.image_height = bitmapData->height;
  jpeg.input_components = bitmapData->ch;
  // jpeg.in_color_space = JCS_RGB;
  jpeg.in_color_space = JCS_GRAYSCALE;
  jpeg_set_defaults(&jpeg);

  jpeg_set_quality(&jpeg, 50, TRUE);

  jpeg_start_compress(&jpeg, TRUE);

  for (j = 0; j < jpeg.image_height; j++ ) {
    address = bitmapData->data + (j * bitmapData->width * bitmapData->ch);
    jpeg_write_scanlines(&jpeg, &address, 1);
  }

  jpeg_finish_compress(&jpeg);

  jpeg_destroy_compress(&jpeg);

  return 0;
}

int freeBitmapData(BITMAPDATA_t *bitmap){
  free(bitmap->data);
  return 0;
}
#include "myJpeg.h"

int main(int argc, char *argv[]){

  BITMAPDATA_t bitmap;
  int i, j, c;
  int ave, sum;
  char outname[256];

  FILE *fo;

  if(argc != 2){
    printf("ファイル名が指定されていません\n");
    return -1;
  }

  if(jpegFileReadDecode(&bitmap, argv[1]) == -1){
    printf("jpegFileReadDecode error\n");
    return -1;
  }

  printf("bitmap->data = %p\n", bitmap.data);
  printf("bitmap->width = %d\n", bitmap.width);
  printf("bitmap->height = %d\n", bitmap.height);
  printf("bitmap->ch = %d\n", bitmap.ch);


  /* グレースケール用のbitmap作成 */
  BITMAPDATA_t gray_bitmap;
  gray_bitmap.width = bitmap.width;
  gray_bitmap.height = bitmap.height;
  gray_bitmap.ch = 1;
  gray_bitmap.data
    = (unsigned char*)malloc(sizeof(unsigned char) * gray_bitmap.width * gray_bitmap.height * gray_bitmap.ch);
  
  /* グレースケールに変換 */
  for(j = 0; j < bitmap.height; j++){
    for(i = 0; i < bitmap.width; i++){
      sum = 0;
      for(c = 0; c < bitmap.ch; c++){
        sum += bitmap.data[bitmap.ch * (i + j * bitmap.width) + c];
      }
      ave = sum / bitmap.ch;
      for(c = 0; c < bitmap.ch; c++){
        bitmap.data[bitmap.ch * (i + j * bitmap.width) + c] = ave;
      }
      gray_bitmap.data[i + j * gray_bitmap.width] = ave;
    }
  }

  freeBitmapData(&bitmap);
  sprintf(outname, "%s", "output.jpg");

  printf("gray_bitmap->data = %p\n", gray_bitmap.data);
  printf("gray_bitmap->width = %d\n", gray_bitmap.width);
  printf("gray_bitmap->height = %d\n", gray_bitmap.height);
  printf("gray_bitmap->ch = %d\n", gray_bitmap.ch);

  if(jpegFileEncodeWrite(&gray_bitmap, outname) == -1){
    printf("jpegFileEncodeWrite error\n");
    freeBitmapData(&gray_bitmap);
    return -1;
  }

  freeBitmapData(&gray_bitmap);

  return 0;
}
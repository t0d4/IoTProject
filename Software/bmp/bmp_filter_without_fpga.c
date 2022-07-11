#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEIGHT 512
#define MAX_WIDTH 512

int r_filter[9];
const int r_filter_4n[] = {0, 1, 0, 1, -4, 1, 0, 1, 0}; // 4 neighborhood
const int r_filter_8n[] = {1, 2, 1, 2, -14, 2, 1, 2, 1}; // 8 neighborhood

char filtering(char pixels[3][3][3]) {
  char gray[3][3];

  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      int ave = 0;
      for(int k = 0; k < 3; k++) {
        ave += pixels[i][j][k];
      }
      ave = ave / 3;
      gray[i][j] = (char)ave;
    }
  }

  int res = 0;
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      res += (int)gray[i][j] * r_filter[3*i + j];
    }
  }

  if(res < 0) res = 0;
  if(res > 255) res = 255;

  return (char)(res);
}

int main(int argc, char **argv)
{
  if(argc < 4) {
    printf("You need to pass 3 arguments. <input.bmp> <output.bmp> <-4n|-8n>\n");
    exit(-1);
  }
  if(strcmp(argv[3], "-4n") == 0) {
    memcpy(r_filter, r_filter_4n, sizeof(r_filter_4n));
  } else if(strcmp(argv[3], "-8n") == 0) {
    memcpy(r_filter, r_filter_8n, sizeof(r_filter_8n));
  } else {
    printf("Invalid third argument.\n");
  }

  //-------------------READ BMP FILE-----------------//
  unsigned char img[MAX_HEIGHT][MAX_WIDTH][3];
  unsigned char BitMapFileHeader[14];
  unsigned int biSize;
  int biWidth;
  int biHeight;
  unsigned char BitMapInfoHeader[28];

  FILE *fp;

  fp = fopen(argv[1], "rb");

  fread(&BitMapFileHeader, sizeof(char), 14, fp); 
  fread(&biSize, sizeof(int), 1, fp);             
  fread(&biWidth, sizeof(int), 1, fp);            
  fread(&biHeight, sizeof(int), 1, fp);           
  fread(&BitMapInfoHeader, sizeof(char), 28, fp); 
  for (int i = 0; i < biHeight; i++)
  {
    for (int j = 0; j < biWidth; j++)
    {
      for (int c = 0; c < 3; c++)
      {                                            
        fread(&img[i][j][c], sizeof(char), 1, fp); 
      }
    }
  }
  fclose(fp);


  //-------------------EDIT BMP FILE-----------------//
  unsigned char filtered_img[MAX_HEIGHT][MAX_WIDTH][3];

  for (int i = 0; i < biHeight; i++)
  {
    for (int j = 0; j < biWidth; j++)
    {
      if(i >= biHeight-2 || j >= biWidth-2) {
        for(int c = 0; c < 3; c++) {
          filtered_img[i][j][c] = 0;
        }
        continue;
      }

      char pixels[3][3][3];
      for(int h_pos = 0; h_pos < 3; h_pos++) {
        for(int w_pos = 0; w_pos < 3; w_pos++) {
          for(int c = 0; c < 3; c++) {
            pixels[h_pos][w_pos][c] = img[i+h_pos][j+w_pos][c];
          }
        }
      }

      char ret = filtering(pixels);
      for(int c = 0; c < 3; c++) {
        filtered_img[i][j][c] = ret;
      }
    }
  }
  
  //-------------------WRITE BMP FILE-----------------//
  fp = fopen(argv[2], "wb");
  fwrite(&BitMapFileHeader, sizeof(char), 14, fp); 
  fwrite(&biSize, sizeof(int), 1, fp);             
  fwrite(&biWidth, sizeof(int), 1, fp);            
  fwrite(&biHeight, sizeof(int), 1, fp);           
  fwrite(&BitMapInfoHeader, sizeof(char), 28, fp); 
  for (int i = 0; i < biHeight; i++)
  {
    for (int j = 0; j < biWidth; j++)
    { 
      for (int c = 0; c < 3; c++)
      {                                             
        fwrite(&filtered_img[i][j][c], sizeof(char), 1, fp); 
      }
    }
  }
  fclose(fp);

}
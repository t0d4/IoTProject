#include <stdio.h>

#define MAX_HEIGHT 512
#define MAX_WIDTH 512

int main(int argc, char **argv)
{
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

  // printf("height : %d\n", biHeight);
  // printf("width  : %d\n", biWidth);

  // grayscaling
  for (int i = 0; i < biHeight; i++)
  {
    for (int j = 0; j < biWidth; j++)
    {
      int su = 0;
      for (int c = 0; c < 3; c++)
      {                                            
        su = su + (int)(img[i][j][c]);
      }
      su = su / 3;
      // printf("(%3d, %3d) bf: %3d, %3d, %3d   ", 
      //   i, j, (int)img[i][j][0], (int)img[i][j][1], (int)img[i][j][2]);
      unsigned char ave = (char)su;
      for (int c = 0; c < 3; c++)
      {                                            
        img[i][j][c] = ave;
      }
      // printf("af: %3d, %3d, %3d\n", (int)img[i][j][0], (int)img[i][j][1], (int)img[i][j][2]);
    }
  }

  // filtering
  unsigned char filtered_img[MAX_HEIGHT][MAX_WIDTH][3];
  int filter[9] = {0, 1, 0, 1, -4, 1, 0, 1, 0};
  // int filter[9] = {1, 1, 1, 1, -8, 1, 1, 1, 1};
  for(int i = 0; i < biHeight; i++) {
    for(int j = 0; j < biWidth; j++) {
      if(i >= biHeight-2 || j >= biWidth-2) {
        for(int c = 0; c < 3; c++) {
          filtered_img[i][j][c] = 0;
        }
        continue;
      }
      int sum = img[i][j][0]*filter[0] + img[i][j+1][0]*filter[1] + img[i][j+2][0]*filter[2] +
                img[i+1][j][0]*filter[3] + img[i+1][j+1][0]*filter[4] + img[i+1][j+2][0]*filter[5] +
                img[i+2][j][0]*filter[6] + img[i+2][j+1][0]*filter[7] + img[i+2][j+2][0]*filter[8];
      if(sum < 0) sum = 0;
      if(sum > 255) sum = 255;
      for(int c = 0; c < 3; c++) {
        filtered_img[i][j][c] = (char)sum;
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
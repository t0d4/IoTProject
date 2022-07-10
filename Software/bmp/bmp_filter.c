#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdint.h>

//Constants to do mmap and get access to FPGA peripherals
#define HPS_FPGA_BRIDGE_BASE 0xC0000000
#define HW_REGS_BASE ( HPS_FPGA_BRIDGE_BASE )
#define HW_REGS_SPAN ( 0x00400000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )
#define OFFSET 0x00006000 //FPGA On-Chip RAM address relative to H2F bridge

//MACROS TO CONTROL THE TRANSFER
#define DMA_TRANSFER_SIZE 36
#define USE_ACP			1  //0 do not use acp, 1 use acp
#define DMA_BUFF_PADD	(HPS_FPGA_BRIDGE_BASE + OFFSET)
#define PREPARE_MICROCODE_WHEN_OPEN 0

// CONSTANTS FOR IMAGE FILTER
#define PIXEL_SIZE 9

// MAX SIZE OF INPUT BMP
#define MAX_HEIGHT 512
#define MAX_WIDTH 512

void printbuff(char* buff, int size) {
  int i;
  printf("[");
  for (i=0; i<size; i++)
  {
    printf("%x",buff[i]);
    if (i<(size-1)) printf(",");
  }
  printf("]");
  printf("\n");
}

char filtering(char pixels[3][3][3], int f) {
  char buffer[DMA_TRANSFER_SIZE];

  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      for(int k = 0; k < 4; k++) {
        if(k == 3) buffer[12*(2-i) + 4*j + k] = 0;
        else buffer[12*(2-i) + 4*j + k] = pixels[i][j][k];
      }
    }
  }

    //Write uP buffer to FPGA
    if (f < 0){
      perror("Failed to open /dev/dma_pl330 on write...");
      return errno;
    }
    int ret = write(f, buffer, DMA_TRANSFER_SIZE);
    if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
    }

    char output;

    // Read from FPGA to uP
    if (f < 0){
      perror("Failed to open /dev/dma_pl330 on read...");
      return errno;
    }
    // we only have to read 1 byte
    ret = read(f, &output, 1);
    if (ret < 0){
      perror("Failed to read the message from the device.");
      return errno;
    }

    return output;
}

int main(int argc, char **argv) {

  //-------GENERATE ADRESSES TO ACCESS FPGA MEMORY FROM PROCESSOR---------//

  void *virtual_base;
  int fd;
  if( ( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ) ) ) == -1 ) {
	  printf( "ERROR: could not open \"/dev/mem\"...\n" );
	  return( 1 );
  }

  virtual_base = mmap( NULL, HW_REGS_SPAN, ( PROT_READ | PROT_WRITE ),
    MAP_SHARED, fd, HW_REGS_BASE );

  if( virtual_base == MAP_FAILED ) {
	  printf( "ERROR: mmap() failed...\n" );
	  close( fd );
	  return( 1 );
  }

  // virtual address of the FPGA buffer
  void *on_chip_RAM_vaddr_void = virtual_base
  + ((unsigned long)(OFFSET) & (unsigned long)( HW_REGS_MASK ));
  uint32_t* on_chip_RAM_vaddr = (uint32_t *) on_chip_RAM_vaddr_void;


  // RESETTING THE PERIPHERAL IS NOT NECESSARY.
  // SO THIS SECTION IS DELETED.


  //----------------CONFIGURE THE DMA DRIVER THROUGH SYSFS---------------//
  //Configure the driver through sysfs
  int f_sysfs;
  char d[14];

  sprintf(d, "%u", (uint32_t) DMA_BUFF_PADD);
  f_sysfs = open("/sys/dma_pl330/pl330_lkm_attrs/dma_buff_padd", O_WRONLY);
  if (f_sysfs < 0){
    printf("Failed to open sysfs for dma_buff_padd.\n");
    return errno;
  }
  write (f_sysfs, &d, 14);
  close(f_sysfs);

  sprintf(d, "%d", (int) USE_ACP);
  f_sysfs = open("/sys/dma_pl330/pl330_lkm_attrs/use_acp", O_WRONLY);
  if (f_sysfs < 0){
    printf("Failed to open sysfs for use_acp.\n");
    return errno;
  }
  write (f_sysfs, &d, 14);
  close(f_sysfs);

  sprintf(d, "%d", (int) PREPARE_MICROCODE_WHEN_OPEN);
  f_sysfs = open("/sys/dma_pl330/pl330_lkm_attrs/prepare_microcode_in_open",
    O_WRONLY);
  if (f_sysfs < 0){
    printf("Failed to open sysfs for prepare_microcode_in_open.\n");
    return errno;
  }
  write (f_sysfs, &d,14);
  close(f_sysfs);

  sprintf(d, "%d", (int) DMA_TRANSFER_SIZE);
  f_sysfs = open("/sys/dma_pl330/pl330_lkm_attrs/dma_transfer_size", O_WRONLY);
  if (f_sysfs < 0){
    printf("Failed to open sysfs for dma_transfer_size.\n");
    return errno;
  }
  write (f_sysfs, &d, 14);
  close(f_sysfs);


  //-------------------READ BMP FILE-----------------//
  unsigned char img[MAX_HEIGHT][MAX_WIDTH][3], filtered_img[MAX_HEIGHT][MAX_WIDTH][3];
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


  //-----------------EDIT BMP FILE-----------------//

  int f=open("/dev/dma_pl330",O_RDWR);
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

      char ave = filtering(pixels, f);
      for(int c = 0; c < 3; c++) {
        filtered_img[i][j][c] = ave;
      }
    }
  }
  close(f);

	// --------------clean up our memory mapping and exit -----------------//
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

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

	return( 0 );
}

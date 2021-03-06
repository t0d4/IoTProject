// データ転送、fpgaの計算が正しく行われているかのテスト

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
#define DMA_TRANSFER_SIZE 	32
#define USE_ACP			1  //0 do not use acp, 1 use acp
#define DMA_BUFF_PADD	(HPS_FPGA_BRIDGE_BASE + OFFSET)
#define PREPARE_MICROCODE_WHEN_OPEN 0 


void printbuff(char* buff, int size)
{
  int i;
  printf("[");
  for (i=0; i<size; i++)
  {
    printf("%u",buff[i]);
    if (i<(size-1)) printf(",");
  }
  printf("]");
  printf("\n");
}

int main() {
  int i;
  
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

  //virtual address of the FPGA buffer
  void *on_chip_RAM_vaddr_void = virtual_base 
  + ((unsigned long)(OFFSET) & (unsigned long)( HW_REGS_MASK ));
  uint8_t* on_chip_RAM_vaddr = (uint8_t *) on_chip_RAM_vaddr_void;


  //-----------CHECK IF THE FPGA OCR IS ACCESSIBLE AND RESET IT----------//
  //Check the on-chip RAM memory in the FPGA
  uint8_t* ocr_ptr = on_chip_RAM_vaddr;
  for (i=0; i<DMA_TRANSFER_SIZE; i++)
  {
    *ocr_ptr = (uint8_t) i;
    if (*ocr_ptr != (uint8_t)i)
    {
      printf ("Error when checking On-Chip RAM in Byte %d\n", i);
      return 0;
    }
    ocr_ptr++;
  }
  printf("Check On-Chip RAM OK\n");
  

  //Reset all memory
  ocr_ptr = on_chip_RAM_vaddr;
  for (i=0; i<DMA_TRANSFER_SIZE; i++)
  {
    *ocr_ptr = 0;
    if (*ocr_ptr != 0)
    {
      printf ("Error when resetting On-Chip RAM in Byte %d\n", i);
      return 0;
    }
    ocr_ptr++;
  }
  printf("Reset On-Chip RAM OK\n");
 

  //----------------CONFIGURE THE DMA DRIVER THROUGH SYSFS---------------//
  //Configure the driver through sysfs
  int f_sysfs;
  char d[14];
  
  printf("\nConfig. DMA_PL330 module using sysfs entries in /sys/dma_pl330\n");
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
  printf("Sysfs configuration correct.\n");
  printf("Sysfs values:");
  printf(" dma_buff_p:0x%x,", (unsigned int) DMA_BUFF_PADD);
  printf(" use_acp:%d,", USE_ACP);
  printf(" prepare_microcode_in_open:%d,", PREPARE_MICROCODE_WHEN_OPEN);
  printf(" dma_transfer_size:%d\n", DMA_TRANSFER_SIZE);

  
  //-----------------WRITE THE FPGA USING THE DMA DRIVER-----------------//
  //Fill uP buffer and show uP and FPGA buffers
  printf("\nWRITE: Copy a %d Bytes from uP to FPGA on physical address %x\n", 
    (int) DMA_TRANSFER_SIZE, (unsigned int) DMA_BUFF_PADD); 
  // char buffer[DMA_TRANSFER_SIZE] = {
  //   1, 0, 0,  2, 3, 4,  1, 1, 1,
  //   2, 3, 4,  1, 1, 1,  2, 3, 4,
  //   1, 1, 1,  2, 3, 4,  1, 1, 1,
  //   0, 0, 0,  0, 0
  // };
  // expected return from fpga : 8
  // 3 + 3 - 4 + 3 + 3 = 8
  char buffer[DMA_TRANSFER_SIZE] = {
    10, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 0, 0, 0, 0, 0
  };

  // for (i=0; i<DMA_TRANSFER_SIZE;i++) buffer[i] = 2;
  printf("Input  = "); 
  printbuff(buffer, DMA_TRANSFER_SIZE);

  //Write uP fuffer to FPGA
  printf("Writing on /dev/dma_pl330...\n");
  int f=open("/dev/dma_pl330",O_RDWR);
  if (f < 0){
    perror("Failed to open /dev/dma_pl330 on write...");
    return errno;
  }
	int ret = write(f, buffer, DMA_TRANSFER_SIZE);
	if (ret < 0){
	  perror("Failed to write the message to the device.");
	  return errno;
	}
	close(f);
	
  //print the result of the Write
	if(memcmp((void*)buffer, on_chip_RAM_vaddr_void,(size_t)DMA_TRANSFER_SIZE)==0)
    printf("Write Successful!\n");
  else
    printf("Write Error. Buffers are not equal\n");

  //-------------------READ THE FPGA USING THE DMA DRIVER-----------------//
   //Fill uP buffer and show uP and FPGA buffers
  printf("\nREAD: Copy a %d Bytes from FPGA on physical address %x to uP\n", 
    (int) DMA_TRANSFER_SIZE, (unsigned int) DMA_BUFF_PADD); 
  for (i=0; i<DMA_TRANSFER_SIZE;i++) buffer[i] = 255;

  //Read from FPGA to uP
  printf("Reading from /dev/dma_pl330...\n");
  f=open("/dev/dma_pl330",O_RDWR);
  if (f < 0){
    perror("Failed to open /dev/dma_pl330 on read...");
    return errno;
  }
  // ret = read(f, buffer, DMA_TRANSFER_SIZE);
  ret = read(f, buffer, 32);
  if (ret < 0){
    perror("Failed to read the message from the device.");
    return errno;
  }
  close(f);
  
  //print the result of the Read
  printf("Output = "); 
  printbuff(buffer, DMA_TRANSFER_SIZE);


	// --------------clean up our memory mapping and exit -----------------//
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}
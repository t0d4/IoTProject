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

  // virtual address of the FPGA buffer
  void *on_chip_RAM_vaddr_void = virtual_base
  + ((unsigned long)(OFFSET) & (unsigned long)( HW_REGS_MASK ));
  uint32_t* on_chip_RAM_vaddr = (uint32_t *) on_chip_RAM_vaddr_void;
  printf("Base address of the peripheral: %p\n", on_chip_RAM_vaddr);


  // RESETTING THE PERIPHERAL IS NOT NECESSARY.
  // SO THIS SECTION IS DELETED.


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

  #define USE_CHAR
  // #define USE_INT

  #ifdef USE_CHAR
    void printbuff(char* buff, int size)
    {
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
  #else
    void printbuff(int* buff, int size)
    {
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
  #endif

  #ifdef USE_CHAR
    char buffer[DMA_TRANSFER_SIZE] = {
      48, 48, 48, 0,
      48, 48, 48, 0,
      48, 48, 48, 0,
      48, 48, 48, 0,
      0, 0, 0, 0,
      48, 48, 48, 0,
      48, 48, 48, 0,
      48, 48, 48, 0,
      48, 48, 48, 0
    };
  //               ^^^
  //               |||
  //  THESE 2 PIECES OF CODE SHOULD BEHAVE IDENTICALLY
  //               |||
  //               vvv
  #else
    int buffer[PIXEL_SIZE] = {
      (48 << 16) + (48 << 8) + 48,
      (48 << 16) + (48 << 8) + 48,
      (48 << 16) + (48 << 8) + 48,
      (48 << 16) + (48 << 8) + 48,
      0,
      (48 << 16) + (48 << 8) + 48,
      (48 << 16) + (48 << 8) + 48,
      (48 << 16) + (48 << 8) + 48,
      (48 << 16) + (48 << 8) + 48
    };
  #endif

  printf("Input  = ");
  #ifdef USE_CHAR
    printbuff(buffer, DMA_TRANSFER_SIZE);
  #else
    printbuff(buffer, PIXEL_SIZE);
  #endif

  //Write uP buffer to FPGA
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
	}else{
	  printf("write() returned successfully.\n");
	}
	close(f);

/////////////////////////////////////////////////////////////////
// DO NOT TRY TO READ unless you save the output,              //
// because the output of the imagefilter will be RESET TO ZERO //
// once it is read.                                            //
/////////////////////////////////////////////////////////////////

  //-------------------READ THE FPGA USING THE DMA DRIVER-----------------//
  // Fill uP buffer and show uP and FPGA buffers
  printf("\nREAD: Copy 1 Byte from FPGA on physical address %x to uP\n",
    (unsigned int) DMA_BUFF_PADD);
  for (i=0; i<DMA_TRANSFER_SIZE;i++) buffer[i] = 255;

  // Read from FPGA to uP
  printf("Reading from /dev/dma_pl330...\n");
  f=open("/dev/dma_pl330",O_RDWR);
  if (f < 0){
    perror("Failed to open /dev/dma_pl330 on read...");
    return errno;
  }
  // we only have to read 1 byte
  ret = read(f, buffer, 1);
  if (ret < 0){
    perror("Failed to read the message from the device.");
    return errno;
  }
  close(f);

  // buffer[0] has the output
  printf("conv_out = %d\n", (int) buffer[0]);


	// --------------clean up our memory mapping and exit -----------------//
	if( munmap( virtual_base, HW_REGS_SPAN ) != 0 ) {
		printf( "ERROR: munmap() failed...\n" );
		close( fd );
		return( 1 );
	}

	close( fd );

	return( 0 );
}

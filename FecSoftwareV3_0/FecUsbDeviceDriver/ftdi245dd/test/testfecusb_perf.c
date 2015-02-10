
#include <stdio.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <linux/ioctl.h>
#include <fcntl.h>

/*#include "ftdipio_ioc.h"*/
#include "ftdipio_func.h"

unsigned long usb_old_rw(int fd, int rw, long address, long data)
{
        //int a;
        int canal=0;
        unsigned long  ft_status=0;
        unsigned long byte_ecrit=0;
        unsigned long  dwBytesToWrite=0; //dwByteToRead;
    //  LPDWORD lpdwBytesWritten;
        //LPVOID lpBuffer;
        unsigned char buffer[80];
        unsigned char RxBuffer[80];
        unsigned long BytesReceived=0,a_lire=0;
        unsigned long  txBytes=0xffff,rxBytes=0xffff,eventWord=0xffff;
  
    canal = 1;
        memset(buffer,0,sizeof(buffer)); 
	memset(RxBuffer,0,sizeof(RxBuffer));                                                                        
        buffer[0] = 0;  // sync byte
//      buffer[1] = 9; // nombre de byte
                                                                                
        buffer[2] = rw;  //canal  == 1  -> ecriture
        buffer[3] = address & 0xff; buffer[4] = (address & 0xff00) >> 8;
//address
        buffer[5] = 0; buffer[6] = 0;  //address
        buffer[7] = data & 0xff ; buffer[8] = (data &0xff00) >> 8;
        buffer[9] = (data & 0xff0000) >> 16 ; buffer[10] = (data & 0xff000000) >> 24 ; // data
                                                                                
        if(rw == 1) {
                        dwBytesToWrite = 11;   // ecriture
                        buffer[1] = 9;
        }
        if(rw == 2) {
                        dwBytesToWrite = 5;    // lecture
                        buffer[1] = 3;
        }
        /*printf("usb_old_rw: about to write.\n");*/
 

        ft_status = write(fd,buffer,dwBytesToWrite);
        if (ft_status != dwBytesToWrite) printf("Problems in write. %d - %s\n",
						ft_status,strerror(errno));
 
        /*printf("usb_old_rw: write done. %d/%d bytes \n",ft_status,dwBytesToWrite);*/
                                                                                
// lecture



    a_lire = 4;
        /*printf("usb_old_rw: about to read.\n");*/
        ft_status = read(fd,RxBuffer,a_lire);
	if(ft_status != a_lire) printf("Problems in read. %d - %s\n",
						ft_status,strerror(errno));
        /*printf("usb_old_rw: read done. %d/%d \n",ft_status,a_lire);*/
                                                                                
	/*printf("Status bytes: 0x%2x 0x%2x \n",(int)RxBuffer[0],(int)RxBuffer[1]); */                                                     
                                                                                
                                                                                
    return (unsigned long)(RxBuffer[3] + ( RxBuffer[2] << 8) + ( RxBuffer[1] <<
16) + (RxBuffer[0] << 24));
                                                                                
}



int main(int argc, char *argv[]) { 
   int fd = -1 ; 
   long result = 0; 
   int i ; 
   int loops =10 ; 
   unsigned char buffer[256];
   struct timeval start,stop,res; 
   double elapsed ; 

   unsigned char latency = 0 ; 

	if (argc>1) { 
	loops = atoi(argv[1]); 
	} 


	if (loops<=0) return 1 ; 

	fd = ftdipio_open("/dev/usb/ftdipio0") ; 

	if (fd<0) { 
			printf("Can not open device.\n"); 
			printf("errno is : %d message : %s\n",errno,strerror(errno));
			return (-1); 
	} 
	/* result = read(fd,buffer,2);
	if (result<0) printf("Problems reading err: %d - %s \n",result,strerror(errno));
	printf("received: %d bytes\n",result); 
	for(i=0; i<result; i++ ) 
	   printf(" 0x%2x",(int)buffer[i]);
	   printf("\n");*/
	/*for(i=0;i<10;i++) { 
	result = read(fd,buffer,256);
	if (result<0) break ; 
	}*/

	result = ftdipio_reset(fd);
	if (result) { printf("Problems with reseting device.\n"); }

	result = ftdipio_purge(fd);
        if (result) { printf("Problems with purging device buffers.\n"); }

        result = ftdipio_get_latency_timer(fd,&latency);
        if (result) { printf("Problems with getting latency timer from device.\n"); }

	printf("Latency is set to %d \n",(int) latency);

	latency = 2 ; 

	printf("Changing it to %d \n",(int) latency);

        result = ftdipio_set_latency_timer(fd,latency);
        if (result) { printf("Problems with setting latency timer in device.\n"); }
	latency = 0 ; 

        result = ftdipio_get_latency_timer(fd,&latency);
        if (result) { printf("Problems with getting latency timer from device.\n"); }
                                                                                
        printf("Latency is set to %d \n",(int) latency);


	gettimeofday(&start,NULL);	

	for(i=0;i<loops;i++) { 
	result = usb_old_rw(fd,2,8,1);
	/*printf("CR0 result is : %x \n",result);*/
	result = usb_old_rw(fd,2,0xc,1);
	/*printf("CR1 result is : %x \n",result);i*/
	}

	gettimeofday(&stop,NULL);

	timersub(&stop,&start,&res);

	elapsed = (double) res.tv_sec + (double) res.tv_usec * 1.0e-6 ; 
	elapsed /= (loops*2) ; 

	printf("one access is done in : %lf ms.\n",elapsed*1000.0 );

/*	result = read(fd,buffer,4);
	if (result<0) printf("Problems reading err: %d - %s \n",result,strerror(errno));
	printf("received: %d bytes\n",result); 
	for(i=0; i<result; i++ ) 
	   printf(" 0x%2x",(int)buffer[i]);
	printf("\n");

*/ 

	printf("Now closing device..\n");
	result = close(fd);
	if (result) { printf("Problems closing device %d.\n",fd); }
	printf("Done.\n");

	return 0 ; 
}

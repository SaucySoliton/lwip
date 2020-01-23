

#include <errno.h>
#include <sys/time.h>
#include <unistd.h>  // usleep 
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/time.h>
#include <dirent.h>
#include <termios.h>
#include "sys_p2.h"
u32_t sys_now(void)
{
	struct timeval curtime;
	//unsigned int timecount;
	if( gettimeofday(&curtime,NULL)!=0) return -1;
	return curtime.tv_sec*1000 +  curtime.tv_usec/1000;
}
u32_t sys_jiffies(void)
{
	struct timeval curtime;
	//unsigned int timecount;
	if( gettimeofday(&curtime,NULL)!=0) return -1;
	return curtime.tv_usec;  // FIXME, this is probably a security risk if used for seeds
}


int sys_ppp_serial_fd;

// http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("error from tcgetattr");//, errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // ignore break signal
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        tty.c_iflag &= ~(INLCR |ICRNL | IGNCR | IUTF8);      // stop 0x0d->0x0e translation
        tty.c_oflag &= ~(ONLCR |OCRNL );      // stop 0x0d->0x0e translation

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                printf("error from tcsetattr");//, errno);
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                printf("error from tggetattr");//, errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 0;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf("error setting term attributes");//, errno);
}



void sys_init(void)
{
	const char *devicename="/dev/ttyUSB0";

	sys_ppp_serial_fd = open (devicename, O_RDWR | O_NOCTTY | O_SYNC);
	if (sys_ppp_serial_fd < 0)
	{
        	//printf("error %d opening %s: %s", errno, devicename, strerror (errno));
        	printf("error opening serial\n");
	        return ;
	}
	set_interface_attribs (sys_ppp_serial_fd, B115200, 0);  // set speed to 115,200 bps, 8n1 (no parity)
	set_blocking (sys_ppp_serial_fd, 0);                // set no blocking

	printf("pc init ok\n");
}

// These routines are for testing purposes only. 
// Don't consider them "best practices."
int sys_serial_putch( u8_t data )
{
	char buf[8];
	buf[0]=data;
	return write (sys_ppp_serial_fd, buf , 1); 
}

int sys_serial_getch( void )
{
	unsigned char buf[8];
	int n = read (sys_ppp_serial_fd, buf, 1 );
	if( n==1 )
	{
		//printf("%2x\n",buf[0]);
		return buf[0];    // if signed char, result is sign extened, 
		// causing 0xff to be indistunguishable from -1
	}
	else
	{
		//usleep(10); 
		return -1;
	}
}

int sys_serial_getbuf( unsigned char *buf )

{
	
	//unsigned char buf[8];
	int n = read (sys_ppp_serial_fd, buf, 128 );
	if (n==0)
		usleep(1000);
	return n;
}




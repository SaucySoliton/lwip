
#include "sys_p2.h"

#include <propeller2.h>
#include "FullDuplexSerial2.h"

unsigned int p2_cycles_per_millis;
FullDuplexSerial2 p2_ppp_fds;

uint64_t
_p2_getcntll()
{
    uint32_t hi1, lo, hi2;
    do {
        hi1 = _cnth();
        lo = _cnt();
        hi2 = _cnth();
    } while (hi1 != hi2);
    return lo | (((uint64_t)hi1) << 32LL);
}

void sys_init(void)
{
	p2_cycles_per_millis = _clockfreq() / 1000;
        FullDuplexSerial2_start(&p2_ppp_fds,52,54,0,115200);//(self,rx,tx,mode,baud)
	printf("p2 init ok\n");
}

u32_t sys_jiffies(void)
{
	return  _cnt();
}
u32_t sys_now(void)
{
	return _p2_getcntll()/p2_cycles_per_millis;
}

int sys_serial_getch( void )
{
	return FullDuplexSerial2_rxcheck(&p2_ppp_fds,0);
}

int sys_serial_putch( u8_t data )
{
	FullDuplexSerial2_tx(&p2_ppp_fds,data,0);
	return 1;
}



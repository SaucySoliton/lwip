
#include <stdio.h>
//#include <arch/cc.h>
#include <lwip/arch.h>


// uint64_t _p2_getcntll(void);

void sys_init(void);

u32_t sys_jiffies(void);

u32_t sys_now(void);

int sys_serial_getch( void );
int sys_serial_putch( u8_t data );


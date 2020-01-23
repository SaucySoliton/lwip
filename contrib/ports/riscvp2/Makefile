
#CC=gcc
#CC=clang
CC=/opt/riscv-none-gcc/bin/riscv-none-embed-gcc -T riscvp2.ld -march=rv32imc -Os -g
#         none       -Os 
#  imc   connect   lcp fail with OPTIMIZE_CMP_ZERO
#  im    connect   connect 
#-Os 
# CC=gcc -DPC_TEST=1
CCDEP?=$(CC)
# CCDEP=$(CC)
# ive turned off: -Werror -g 
CFLAGS+=-Wall -pedantic  \
	-Wparentheses -Wsequence-point -Wswitch-default \
	-Wextra -Wundef -Wshadow -Wpointer-arith -Wcast-qual \
	-Wc++-compat -Wwrite-strings -Wold-style-definition -Wcast-align \
	-Wmissing-prototypes -Wredundant-decls -Wnested-externs \
	-Wunreachable-code -Wuninitialized -Wmissing-prototypes \
	-Wredundant-decls -Waggregate-return -Wlogical-not-parentheses 
#	-Wconversion -Wsign-compare -Wmissing-include-dirs

ifeq (,$(findstring clang,$(CC)))
# CFLAGS+= -Wlogical-op -Wc90-c99-compat -Wtrampolines
CFLAGS+= -Wlogical-op -Wtrampolines
# if GCC is newer than 4.8/4.9 you may use:
#CFLAGS:=$(CFLAGS) -fsanitize=address -fstack-protector -fstack-check -fsanitize=undefined -fno-sanitize=alignment
else
# we cannot sanitize alignment on x86-64 targets because clang wants 64 bit alignment
CFLAGS+= -fsanitize=address -fsanitize=undefined -fno-sanitize=alignment -Wdocumentation -Wno-documentation-deprecated-sync
endif

CONTRIBDIR?=../..
ARFLAGS?=rs
 
#Set this to where you have the lwip core module checked out from git
#default assumes it's a dir named lwip at the same level as the contrib module
LWIPDIR?=$(CONTRIBDIR)/../src
LWIPARCH?=.

CFLAGS+=-I. \
	-I$(CONTRIBDIR) \
	-I$(LWIPDIR)/include \
	-I$(LWIPARCH)/include

# Add include path and link to mbedTLS lib if available
MBEDTLSDIR?=$(CONTRIBDIR)/../mbedtls
ifneq (,$(wildcard $(MBEDTLSDIR)/include/mbedtls/*.h))
LDFLAGS+=-L$(MBEDTLSDIR)/library -lmbedtls -lmbedcrypto -lmbedx509
CFLAGS+=-I$(MBEDTLSDIR)/include -Wno-redundant-decls -DLWIP_HAVE_MBEDTLS=1 -Wno-c90-c99-compat
endif

include $(CONTRIBDIR)/Filelists.mk
include $(LWIPDIR)/Filelists.mk

# LWIPFILES: All the above.
LWIPFILES=$(LWIPNOAPPSFILES) $(ARCHFILES)
LWIPOBJS=$(notdir $(LWIPFILES:.c=.o))

LWIPLIBCOMMON=liblwipcommon.a
$(LWIPLIBCOMMON): $(LWIPOBJS)
	$(AR) $(ARFLAGS) $(LWIPLIBCOMMON) $?

APPFILES=$(CONTRIBAPPFILES) $(LWIPAPPFILES)
APPLIB=liblwipapps.a
APPOBJS=$(notdir $(APPFILES:.c=.o))
$(APPLIB): $(APPOBJS)
	$(AR) $(ARFLAGS) $(APPLIB) $?

%.o:
	$(CC) $(CFLAGS) -c $<
# -f .depend


# MAKEFSDATAOBJS=$(notdir $(MAKEFSDATAFILES:.c=.o))

clean:
	rm -f *.o $(LWIPLIBCOMMON) $(APPLIB) example_app makefsdata *.s .depend* *.core core

depend dep: .depend

include .depend

.depend: $(LWIPFILES) $(APPFILES) # $(MAKEFSDATAFILES)
	$(CCDEP) $(CFLAGS) -MM $^ > .depend || rm -f .depend

example_app: .depend $(LWIPLIBCOMMON) $(APPLIB) default_netif.o test.o
	$(CC) $(CFLAGS) -o example_app test.o default_netif.o -Wl,--start-group $(APPLIB) $(LWIPLIBCOMMON) -Wl,--end-group $(LDFLAGS)

APPFLAGS=-I ../../../src/include/lwip/apps/ 
APPFLAGS+=-I $(CONTRIBDIR)/apps/ping/ $(CONTRIBDIR)/apps/ping/ping.c 
APPFLAGS+=-I ../../apps/tcpecho_raw/ ../../apps/tcpecho_raw/tcpecho_raw.c 
APPFLAGS+=../../../src/apps/http/http_client.c 
APPFLAGS+=$(LWIPDIR)/apps/http/httpd.c  $(LWIPDIR)/apps/http/fs.c 
pc: $(APPLIB)
	$(CC) -o ppptest testppp3.c sys_pc.c $(CFLAGS) $(LWIPLIBCOMMON) $(APPFLAGS)

p2: $(LWIPLIBCOMMON)
	$(CC) -o ppptest.elf testppp3.c FullDuplexSerial2.c sys_p2.c $(CFLAGS) $(LWIPLIBCOMMON) $(APPFLAGS)
	
run: ppptest.elf
	loadp2 -t -b 230400 ppptest.elf

# makefsdata: .depend $(MAKEFSDATAOBJS)
# 	$(CC) $(CFLAGS) -o makefsdata $(MAKEFSDATAOBJS)


# ======================================================================
#  Makefile for CoreMarkPro
# ======================================================================
#  This Makefile builds an application *.elf for Aquila.
#
#  Chun-Jen Tsai, Aug/28/2025
#  Copy From CoreMark/Makefile modified by CY HSU April/18/2026
# ======================================================================

# ======================================================================
# tools
RISCV_ABI = -march=rv32ima_zicsr_zifencei -mabi=ilp32
GCC_LIB = rv32im/ilp32

CROSS = riscv32-unknown-elf
CCPATH = $(RISCV)/bin

CC = $(CCPATH)/$(CROSS)-gcc
LD = $(CCPATH)/$(CROSS)-ld
AR = $(CCPATH)/$(CROSS)-ar
OD = $(CCPATH)/$(CROSS)-objdump
SP = $(CCPATH)/$(CROSS)-strip

GCCVERSION = $(shell $(CC) --version | grep gcc | sed 's/^.* //g')

# ======================================================================
# elibc
LIBC = elibc
LIBC_OBJS = $(LIBC)/crt0.o $(LIBC)/stdio.o $(LIBC)/stdlib.o $(LIBC)/string.o $(LIBC)/time.o $(LIBC)/uart.o \
			al_stub.o # stub for fileIO
CCFLAGS = -Wall -O3 -I$(LIBC) -fno-builtin $(RISCV_ABI) -mstrict-align -fno-inline-small-functions

# mith
MITHALLIB = mith/al/include
MITHLIB = mith/include
CCFLAGS += -I$(MITHALLIB) -I$(MITHLIB)

# fdlibm
FDLIBM_DIR = mith/al/fdlibm
FDLIBM_SRC = $(wildcard $(FDLIBM_DIR)/*.c)
FDLIBM_OBJ = $(FDLIBM_SRC:.c=.o)
CCFLAGS += -I$(FDLIBM_DIR)

# benchmark
CCFLAGS += -Ibenchmarks/core

# other flags
XCFLAGS = -DFAKE_FILEIO=0 -DHAVE_FILEIO=0 -DHAVE_SYS_STAT_H=0 -DHAVE_PTHREAD=0 \
          -DUSE_NATIVE_PTHREAD=0 -DUSE_SINGLE_CONTEXT=1 -DHAVE_STDRUP=0 \
          -DSTUB_STAT=1 -DNO_ALIGNED_MALLOC=1 -DNO_ALIGNED_ALLOC \
          -DUSE_CLOCK=1 -DHOST_EXAMPLE_CODE=1

LDFLAGS = -L. -Map=$*.map
LD_GCC_LIB = -L$(RISCV)/lib/gcc/riscv32-unknown-elf/$(GCCVERSION)/$(GCC_LIB) -lgcc

OCFLAGS = -O binary
ODFLAGS = -d

# ======================================================================
# project
FDLIBMSRC = $(wildcard mith/al/fdlibm/*.c)
FDLIBMOBJ = $(FDLIBMSRC:.c=.o)

MITHALOBJ = mith/al/src/al_single.o mith/al/src/th_al.o mith/al/src/al_smp.o

MITHSRC   = $(wildcard mith/src/*.c)
MITHOBJ   = $(MITHSRC:.c=.o)

BENCHMARKSSRC = $(wildcard benchmarks/core/*.c)
BENCHMARKSOBJ = $(BENCHMARKSSRC:.c=.o)

# ======================================================================
# Workload: Change the SW as needed
OBJS = workloads/core/core.o $(MITHALOBJ) $(MITHOBJ) $(FDLIBMOBJ) $(BENCHMARKSOBJ)
PROJ = core
FLAGS_STR = "$(CCFLAGS) $(XCFLAGS)"

# ======================================================================
# rules

all:
	@mkdir -p build/
	make build/CoreMarkPro.elf

clean:
	rm -rf build/

%.o: %.c
	$(CC) $(CCFLAGS) $(XCFLAGS) $(LCFLAGS) -DFLAGS_STR=\"$(FLAGS_STR)\" -c $< -o $@

libelibc.a : $(LIBC_OBJS)
	$(AR) r libelibc.a $(LIBC_OBJS)
	rm $^

%.elf: $(OBJS) libelibc.a
	$(LD) $(LDFLAGS) -Tlink.ld $(OBJS) -lelibc $(LD_GCC_LIB) -o $@
	$(OD) $(ODFLAGS) $@ > $*.objdump
	$(SP) -g $@
	rm $^

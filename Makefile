TARGET = ../progs
OBJS = \
main.o \
bg_lib.o \
client.o \
combat.o \
g_cmd.o \
g_main.o \
g_mem.o \
g_spawn.o \
g_syscalls.o \
g_utils.o \
mathlib.o \
light.o \
player.o \
q_shared.o \
subs.o \
world.o 

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_NEWLIB_LIBC = 1

INCDIR =

CFLAGS = -O3 -G0 -Wall -ffast-math -fno-strict-aliasing -Dstricmp=strcasecmp -g -DKLIBC
ASFLAGS = $(CFLAGS)

LIBDIR =
LIBS = -lc -lm -lpspuser

LDFLAGS = -nostartfiles

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

	

ROOT = .
BINNAME = testserver

znet_inc = $(ROOT)/src
znet_src = $(ROOT)/src

common_inc = $(ROOT)/common/include
common_arch_inc =$(ROOT)/common/include/arch/unix
common_mem_src = $(ROOT)/common/memory
common_locks_src = $(ROOT)/common/locks/unix
common_misc_src = $(ROOT)/common/misc

evloop_inc = $(ROOT)/evloop
evloop_src = $(ROOT)/evloop

main_src = $(ROOT)
main_inc = $(ROOT)

obj = $(ROOT)/obj
dbgobj = $(ROOT)/dbgobj
dst = $(ROOT)/bin

bin = $(dst)/$(BINNAME)


#################################################################
#  Customization of the implicit rules

CC = gcc -funsigned-char
#CC = gcc
DEFS = -DHAS_THREADS -DEVLOOP_EPOLL -DHAVE_CLOCK_MONOTONIC
IFLAGS1 = -I$(znet_inc) -I$(main_inc) 
IFLAGS2 = -I$(common_inc) -I$(common_arch_inc)
IFLAGS3 = -I$(evloop_inc)
CFLAGS = -g -Wall $(IFLAGS1) $(IFLAGS2) $(IFLAGS3) 
SYSLIBS = -lpthread -lrt
LFLAGS = 

LIBS = $(SYSLIBS) 

RM = /bin/rm -f
COMPILE.c = $(CC) $(DEFS) $(CFLAGS) -c
LINK = $(CC) $(CFLAGS) $(LFLAGS) -o


#################################################################
#  Modules

znet_incs = $(wildcard $(znet_inc)/*.h)
znet_sources = $(wildcard $(znet_src)/*.c)
znet_objs = $(patsubst $(znet_src)/%.c,$(obj)/%.o,$(znet_sources))

evloop_incs = $(wildcard $(evloop_inc)/*.h)
evloop_sources = $(wildcard $(evloop_src)/*.c)
evloop_objs = $(patsubst $(evloop_src)/%.c,$(obj)/%.o,$(evloop_sources))

common_incs = $(wildcard $(common_inc)/*.h)
common_arch_incs = $(wildcard $(common_arch_inc)/*.h)
common_mem_srcs = $(wildcard $(common_mem_src)/*.c)
common_mem_objs = $(patsubst $(common_mem_src)/%.c,$(obj)/%.o,$(common_mem_srcs))

common_locks_srcs = $(wildcard $(common_locks_src)/*.c)
common_locks_objs = $(patsubst $(common_locks_src)/%.c,$(obj)/%.o,$(common_locks_srcs))

common_misc_srcs = $(wildcard $(common_misc_src)/*.c)
common_misc_objs = $(patsubst $(common_misc_src)/%.c,$(obj)/%.o,$(common_misc_srcs))

main_objs = $(obj)/main.o	

objs =  $(common_mem_objs) $(common_locks_objs) $(evloop_objs) $(znet_objs) $(common_misc_objs) $(main_objs)

dbgobjs = $(patsubst $(obj)/%.o,$(dbgobj)/%.o,$(objs))


#################################################################
#  Standard Rules

.PHONY: all clean

all: clean $(bin) 
clean: 
	$(RM) $(objs)
$(bin): $(objs) 
	$(LINK) $@ $? $(LIBS)

#################################################################
#  Additional Rules

$(obj)/%.o: $(main_src)/%.c 
	$(COMPILE.c) $< -o $@

$(obj)/%.o: $(common_mem_src)/%.c $(common_incs)
	$(COMPILE.c) $< -o $@

$(obj)/%.o: $(common_misc_src)/%.c $(common_incs) $(common_arch_incs)
	$(COMPILE.c) $< -o $@

$(obj)/%.o: $(common_locks_src)/%.c $(common_incs) $(common_arch_incs)
	$(COMPILE.c) $< -o $@

$(obj)/%.o: $(evloop_src)/%.c $(evloop_incs)
	$(COMPILE.c) $< -o $@

$(obj)/%.o: $(znet_src)/%.c $(znet_incs)
	$(COMPILE.c) $< -o $@


CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}gcc

OBJDIR = obj
OBJ= main.o user_handler.o execute_regime.o device_com.o
OBJ_PATH = $(addprefix ${OBJDIR}/,${OBJ})

BINDIR = bin
OUT_RELEASE = ${BINDIR}/app

SRCDIR = src
COMMON_INCDIR = ../common
INCDIR = inc
LIB = -lpthread

all: app

before_build:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

%.o :  ${SRCDIR}/%.c before_build
	$(CC) $(COPTS) -c $< -o $(OBJDIR)/$@ -I $(INCDIR) -I $(COMMON_INCDIR)
	
app: $(OBJ)
	$(LD) -o $(BINDIR)/$@ $(OBJ_PATH) $(LIB)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

.PHONY: clean 

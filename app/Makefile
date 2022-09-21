
CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}gcc

OBJDIR = obj
OBJ= main.o user_handler.o execute_regime.o
OBJ_PATH = $(addprefix ${OBJDIR}/,${OBJ})

BINDIR = bin
OUT_RELEASE = ${BINDIR}/app

SRCDIR = src
INCDIR = inc
LIB = -lpthread

all: app

before_build:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

%.o :  ${SRCDIR}/%.c before_build
	$(CC) -c $< -o $(OBJDIR)/$@ -I $(INCDIR)
	
app: $(OBJ)
	$(LD) -o $(BINDIR)/$@ $(OBJ_PATH) $(LIB)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

.PHONY: clean 


CC = ${CROSS_COMPILE}gcc
LD = ${CROSS_COMPILE}gcc

OBJDIR = obj
OBJ_RELEASE = ${OBJDIR}/main.o

BINDIR = bin
OUT_RELEASE = ${BINDIR}/app

SRCDIR = src

SRC = src

all: release

before_build:
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(OBJDIR_)/example_1.o: before_build
	$(CC) -c $(SRC)/main.c -o $(OBJ_RELEASE)
	
release: $(OBJDIR_)/example_1.o
	$(LD) -o $(OUT_RELEASE) $(OBJ_RELEASE) 
clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

.PHONY: clean 

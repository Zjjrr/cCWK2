VPATH =	src

src_dir := $(CURDIR)/src
lib_dir := $(CURDIR)/lib
lib_cjson := $(lib_dir)/cJSON

CC := gcc
CFLAGS := -Wall -O -Wno-unused-result -I$(lib_cjson)
LIBS := -lSDL2 -lSDL2_image
MAKE := make

EXE := game
SRC := Game.c GameData.c main.c
OBJ := $(SRC:.c=.o)


.PHONY: all $(EXE) clean

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

$(EXE): $(OBJ) lib_cjson
	$(CC) $(OBJ) cJSON.o -o $(EXE) $(LIBS)
	
lib_cjson:
	cd $(lib_cjson); \
	$(MAKE); \
	cp $(lib_cjson)/cJSON.o $(CURDIR)
	
clean:
	rm -rf *.o
	rm -rf $(EXE)
	cd $(lib_cjson); \
	make clean

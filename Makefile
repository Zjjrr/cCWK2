VPATH =	src

src_dir := $(CURDIR)/src
lib_dir := $(CURDIR)/lib
lib_cjson := $(lib_dir)/cJSON

CC := gcc
CFLAGS := -Wall -O -Wno-unused-result -I$(lib_cjson)
LDFLAGS := 
MAKE := make

exe_files := game
src_files := Game.c GameData.c main.c 
obj_files := $(addsuffix .o,$(basename $(src_files)))
obj_files += cJSON.o

.PHONY: all game clean
all: $(exe_files)
game: lib_cjson
	cd $(src_dir); \
	$(CC) -c $(CFLAGS) $(src_files); \
	$(CC) -o $(LDFLAGS) game $(obj_files)
lib_cjson:
	cd $(lib_cjson); \
	$(MAKE); \
	cp $(lib_cjson)/cJSON.o $(src_dir)
clean:
	cd $(src_dir); \
	rm -rf *.o; \
	rm -rf $(exe_files)
	cd $(lib_cjson); \
	make clean

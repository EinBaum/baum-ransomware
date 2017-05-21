CC := gcc
FLAGS := -Wall -pedantic -std=c99 -flto -Os
FLAGS_LD := -static -s -Wl,--gc-sections
BIN := baum-ransomware
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

SSL_DIR := libressl
SSL_DIRLIB := $(SSL_DIR)/crypto/.libs
SSL_LIB := crypto
SSL_LIBPATH := $(SSL_DIRLIB)/lib$(SSL_LIB).so

UPX_DIR := upx
UPX_BINPATH := $(UPX_DIR)/src/upx.out

LIBEXTRA := -L$(SSL_DIRLIB) -l$(SSL_LIB) -lpthread
INCLUDES := -I$(SSL_DIR)/include

README := README.md

.PHONY: all update create_readme clean distclean

all: $(BIN)

update:
	git pull
	git submodule update --init --recursive

create_readme: $(BIN)
	echo "\`\`\`"		> $(README)
	./$(BIN) --help		>> $(README)
	echo "\`\`\`"		>> $(README)
	echo ""			>> $(README)
	cat INFO.md		>> $(README)
	echo ""			>> $(README)
	cat TODO.md		>> $(README)

$(BIN): $(OBJ) $(SSL_LIBPATH) $(UPX_BINPATH)
	$(CC) $(FLAGS) $(FLAGS_LD) $(OBJ) $(LIBEXTRA) -o $(BIN)
	$(UPX_BINPATH) $(BIN)

.c.o:
	$(CC) $(FLAGS) $(INCLUDES) -c $< -o $@

$(SSL_LIBPATH):
	cd $(SSL_DIR)	; \
	./autogen.sh	; \
	./configure	; \
	make

$(UPX_BINPATH):
	cd $(UPX_DIR)	; \
	make all

clean:
	-rm -f $(OBJ) $(BIN)

distclean: clean
	-make -C $(SSL_DIR) distclean
	-make -C $(UPX_DIR) distclean

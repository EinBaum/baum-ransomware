CC := gcc
CFLAGS := -Wall -pedantic -std=c99 -flto -Os
LDFLAGS := -static -s -Wl,--gc-sections
BIN := baumcrypt
SRCDIR := src
SRC := $(wildcard $(SRCDIR)/*.c)
OBJ := $(SRC:.c=.o)
HEADERS := $(wildcard $(SRCDIR)/*.h)

SSL_DIR := libressl
SSL_DIRLIB := $(SSL_DIR)/crypto/.libs
SSL_LIB := crypto
SSL_LIBPATH := $(SSL_DIRLIB)/lib$(SSL_LIB).a

UPX_DIR := upx
UPX_BINPATH := $(UPX_DIR)/src/upx.out

LIBEXTRA := -L$(SSL_DIRLIB) -l$(SSL_LIB) -lpthread
INCLUDES := -I$(SSL_DIR)/include

README := README.md

.PHONY: all update create_readme clean distclean

all: $(BIN)

release: $(BIN) $(UPX_BINPATH)
	$(UPX_BINPATH) $(BIN)

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

$(BIN): $(OBJ) $(SSL_LIBPATH)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) $(LIBEXTRA) -o $(BIN)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

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

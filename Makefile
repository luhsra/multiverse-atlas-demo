CC ?= gcc

PLUGIN_DIR=multiverse/gcc-plugin
PLUGIN=$(PLUGIN_DIR)/multiverse.so
LIBRARY_DIR=multiverse/libmultiverse
LIBRARY= -lpthread $(LIBRARY_DIR)/libmultiverse.a

CFLAGS += -Wall -Wextra -fplugin=$(PLUGIN) -I$(LIBRARY_DIR) -O2 -fno-ipa-ra
LDFLAGS += -L$(LIBRARY_DIR) 
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %,%.o,$(basename $(SOURCES)))
PRODUCTS = demo 01-multiverse 02-migrate 03-concurrent

all: ${LIBRARY} ${PLUGIN} ${PRODUCTS}

.PHONY: clean always

demo: demo.o
	$(CC) $(LDFLAGS) $^ -o $@ ${LIBRARY}

01-multiverse: 01-multiverse.o
	$(CC) $(LDFLAGS) $^ -o $@ ${LIBRARY}

02-migrate: 02-migrate.o
	$(CC) $(LDFLAGS) $^ -o $@ ${LIBRARY}

03-concurrent: 03-concurrent.o
	$(CC) $(LDFLAGS) $^ -o $@ ${LIBRARY}

test: test.o
	$(CC) $(LDFLAGS) $^ -o $@ ${LIBRARY}

%.o: %.c ${MAKEFILE_LIST}
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIBRARY): always
	$(MAKE) -s -C $(LIBRARY_DIR)

$(PLUGIN): always
	$(MAKE) -s -C $(PLUGIN_DIR)


clean:
	rm -f $(PRODUCTS) $(OBJECTS)

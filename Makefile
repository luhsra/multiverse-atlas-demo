CC := gcc

PLUGIN_DIR=../multiverse/gcc-plugin
PLUGIN=$(PLUGIN_DIR)/multiverse.so
LIBRARY_DIR=../multiverse/libmultiverse
LIBRARY=$(LIBRARY_DIR)/libmultiverse.a

CFLAGS += -Wall -Wextra -fplugin=$(PLUGIN) -I$(LIBRARY_DIR) -O2
LDFLAGS += -L$(LIBRARY_DIR) -lpthread
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %,%.o,$(basename $(SOURCES)))
PRODUCTS = demo 01-multiverse

all: ${LIBRARY} ${PLUGIN} ${OBJECTS} ${PRODUCTS}

.PHONY: clean always

demo: demo.o
	$(CC) $(LDFLAGS) $^ -o $@ ${LIBRARY}

01-multiverse: 01-multiverse.o
	$(CC) $(LDFLAGS) $^ -o $@ ${LIBRARY}

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIBRARY): always
	$(MAKE) -s -C $(LIBRARY_DIR)

$(PLUGIN): always
	$(MAKE) -s -C $(PLUGIN_DIR)


clean:
	rm -f $(PRODUCTS) $(OBJECTS)

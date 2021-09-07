CC := gcc

PLUGIN_DIR=../multiverse/gcc-plugin
PLUGIN=$(PLUGIN_DIR)/multiverse.so
LIBRARY_DIR=../multiverse/libmultiverse
LIBRARY=$(LIBRARY_DIR)/libmultiverse.a

CCFLAGS += -Wall -Wextra -fplugin=$(PLUGIN) -I$(LIBRARY_DIR) -O0
LDFLAGS += -L$(LIBRARY_DIR) -lpthread
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %,%.o,$(basename $(SOURCES)))
PRODUCT = demo

.PHONY: clean always

$(PRODUCT): $(OBJECTS) $(LIBRARY)
	$(CC) $(LDFLAGS) $^ -o $@

$(LIBRARY): always
	$(MAKE) -C $(LIBRARY_DIR)

$(PLUGIN): always
	$(MAKE) -C $(PLUGIN_DIR)

%.o: %.c $(PLUGIN)
	$(CC) $(CCFLAGS) -c -o $@ $<

clean:
	rm -f $(PRODUCT) $(OBJECTS)

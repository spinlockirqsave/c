CC		= gcc
CFLAGS		= -c -g -Wall -D_GNU_SOURCE
LDFLAGS		= -lpcap
SOURCES		= packet_sniffer.c
INCLUDES	= -I.
OBJECTS		= $(SOURCES:.c=.o)
TARGET		= packet_sniffer

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)

CC		= gcc
CFLAGS		= -c -g -Wall -D_GNU_SOURCE
LDFLAGS		= #-lpcap
SOURCES		= main.c
INCLUDES	= -I.
OBJECTS		= $(SOURCES:.c=.o)
TARGET		= complement

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	rm -rf $(OBJECTS) $(TARGET)

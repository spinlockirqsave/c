CXX			= gcc
LDFLAGS		= -lpthread -lm
SOURCES		= buffer.c main.c
INCLUDES	= -I.
OBJECTS		= $(SOURCES:.c=.o)
TARGET		= circ_buf

all: $(SOURCES) $(TARGET)

debug:	CXXFLAGS += -DDEBUG -g3 -O0 -Wall -D_GNU_SOURCE -std=gnu99
debug:	$(SOURCES) $(TARGET)

release:	CXXFLAGS += -O3 -Wall -D_GNU_SOURCE -std=gnu99
release:	$(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

.c.o:
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $<

.PHONY:
clean:
	rm -rf $(OBJECTS) $(TARGET)

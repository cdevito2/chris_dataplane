cc=gcc
CFLAGS=-g -Wall -Wextra -std=c99
INCLUDES=-Iinclude

TARGET=chris_dp

OBJS= main.o dp_netdev.o dp_buffer.o src/util/dp_log.o

$(TARGET): $(OBJS)
	$(cc) $(CFLAGS) -o $@ $^

main.o: src/main.c include/dp_netdev.h include/dp_buffer.h include/dp_log.h
	$(cc) $(CFLAGS) $(INCLUDES) -c src/main.c

dp_netdev.o: src/dp_netdev.c include/dp_netdev.h
	$(cc) $(CFLAGS) $(INCLUDES) -c src/dp_netdev.c

dp_buffer.o: src/dp_buffer.c include/dp_buffer.h
	$(cc) $(CFLAGS) $(INCLUDES) -c src/dp_buffer.c

src/util/dp_log.o: src/util/dp_log.c include/dp_log.h
	$(cc) $(CFLAGS) $(INCLUDES) -c src/util/dp_log.c -o src/util/dp_log.o

all: $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
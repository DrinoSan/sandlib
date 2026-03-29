CC     = gcc
CFLAGS = -Wall -Wextra -std=c11

SRCS = Sand_string.c Sand_vector.c Sand_string_view.c
OBJS = $(SRCS:.c=.o)
LIB  = libsand.a

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(LIB)

.PHONY: all clean

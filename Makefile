CC = clang
CFLAGS = -g
TARGET = oblaka
oblaka:	oblaka.h
	$(CC) $(CFLAGS) $@.c -o $@

test:	test.c
	$(CC) $(CFLAGS) $@.c -o $@
	./$@

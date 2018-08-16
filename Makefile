CC = clang
CFLAGS = -g
TARGET = oblaka
oblaka:

test:	test.c
	$(CC) $@.c -o $@
	./$@

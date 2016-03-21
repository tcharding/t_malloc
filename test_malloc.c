/* test drive memory allocator */
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "t_malloc.h"

static int testSimple(void);
static int testRepeat(void);
static Boolean randB(void);
static size_t randS(void);
static void writeZeros(char *buf, size_t size);


int
main()
{
	int err;

	err = testSimple();
	if (err) {
		fprintf(stderr, "testSimple failed: %d\n", err);
	}

	err = testRepeat();
	if (err) {
		fprintf(stderr, "testRepeat failed: %d\n", err);
	}

	exit(EXIT_SUCCESS);
}

static int
testSimple(void)
{
	char *dynamic;
	char *s = "test string";

	dynamic = t_malloc(strlen(s)+1);
	if (dynamic == NULL) {
		fprintf(stderr, "malloc error\n");
		return 1;
	}
	strcpy(dynamic, s);

	if (strcmp(s, dynamic) != 0) {
		fprintf(stderr, "strings do not match\n");
		return 2;
	}

	t_free(dynamic); 
	
	return 0;
}

#define MIN_ALLOC 1
#define MAX_ALLOC 5 * 4096
#define NUM_ALLOCS 100

static int
testRepeat(void)
{
	Boolean free, write;
	size_t size;
	int i;
	void *mem;

	srand(time(NULL));

	for (i = 0; i < NUM_ALLOCS; i++) {
		free = randB();
		write = randB();
		size = randS();

		mem = t_malloc(size);
		if (write)
			writeZeros((char *)mem, size);
		if (free)
			t_free(mem);
	}

	return 0;
}

static Boolean
randB(void)
{
	int r;


	r = rand() % 2;

	return (r == 0) ? TRUE : FALSE;
}

static size_t
randS(void)
{
	int r;

	r = rand() % (MAX_ALLOC + MIN_ALLOC);

	return r - MIN_ALLOC;
}

static void
writeZeros(char *buf, size_t size)
{
	char *zeros = "00000000000000000000";
	size_t len = strlen(zeros);
	int i = 0;
		
	while (size > 0) {
		if (size > len) {
			strcpy(buf+i, zeros);
			i += len;
			size -= len;
		} else {
			strncpy(buf+i, zeros, size);
			i += size;
			size -= size;
		}
	}
}

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "t_malloc.h"

#define PB_ALLOC_SIZE 4096	/* size to increase program break by */

struct fchunk {			/* free memory chunk */
	size_t size;
	struct fchunk *next;
	struct fchunk *prev;
	void *chunk;
};

/* prototypes */
static struct fchunk *firstFit(size_t bytes);
static struct fchunk *fcFromMemory(void *memory);
static void returnFc(struct fchunk *fc);

static int initFreeList(void);
static Boolean fchunkWillHold(struct fchunk *fc, size_t require);
struct fchunk *newFchunk(size_t bytes);
static int size(void);

static void printFreeList(const char *msg);

/* free memory list */
struct fchunk *head;

/* t_malloc: allocate and return bytes */
void *t_malloc(size_t bytes)
{
	struct fchunk *fc;

	fc = firstFit(bytes);
	if (fc == NULL) {
		return NULL;
	}

	return (void *)fc+size();
}

/* t_free: free memory allocated with t_malloc */
void
t_free(void *memory)
{
	struct fchunk *fc;

	fc = fcFromMemory(memory);
	returnFc(fc);
}

/* firstFit: return chunk of memory using first fit algorithm */
static struct fchunk *
firstFit(size_t bytes)
{
	struct fchunk *ptr, *prev, *next;

	if (head == NULL) {	/* first time called */
		if (initFreeList() != 0)
			return NULL;
	}

	for (ptr = head; ptr != NULL; ptr = ptr->next) {
		if (fchunkWillHold(ptr, bytes)) {
			prev = ptr->prev;
			next = ptr->next;
			if (prev != NULL)
				prev->next = next;
			else
				head = next;
			
			return ptr;
		}
	}

	return newFchunk(bytes);
}

/* fcFromMemory: return pointer to fchunk enclosing memory */
static struct fchunk *
fcFromMemory(void *memory)
{
	return (struct fchunk *) (memory - size());
}

/* returnFc: return fchunk to free list */
static void
returnFc(struct fchunk *fc)
{
				/* just put it at the front for now */
	fc->next = head;
	fc->prev = NULL;
	head = fc;
}

/* initFreeList: initialise list of free chunks (head) */
static int
initFreeList(void)
{
	void *oldPB;

	oldPB = sbrk(PB_ALLOC_SIZE);
	if (oldPB == NULL)
		return 1;

	head = (struct fchunk *) oldPB;
	head->size = PB_ALLOC_SIZE;
	head->prev = NULL;
	head->next = NULL;

	return 0;
}

/* fchunkWillHold: true if fc can hold bytes */
static Boolean
fchunkWillHold(struct fchunk *fc, size_t require)
{
	size_t have = fc->size - size();

	return (have >= require);
}

/* newFchunk: create a new chunk by increasing the program break */
struct fchunk *
newFchunk(size_t bytes)
{
	struct fchunk *new;
	size_t allocate;

	allocate = (bytes > PB_ALLOC_SIZE) ? bytes : PB_ALLOC_SIZE;
	
	new = sbrk(allocate);
	if (new == NULL)
		return NULL;
	new->size = allocate;
	new->prev = NULL;
	new->next = NULL;

	return new;
}

/* size: returns number of bytes used to represent size */
static int
size(void)
{
	struct fchunk f;

	bzero(&f, sizeof(f));
	return sizeof(f.size);
}

static void
printFreeList(const char *msg)
{
	struct fchunk *ptr;

	fprintf(stderr, "fchunk (%s)\n", msg);
	for (ptr = head; ptr != NULL; ptr = ptr->next)
		fprintf(stderr, "\tsize %ld\n", (long) ptr->size);
}

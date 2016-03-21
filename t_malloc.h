#ifndef T_MALLOC_H
#define T_MALLOC_H

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

typedef enum { FALSE, TRUE } Boolean;

/* Tobin's toy memory allocator */
void *t_malloc(size_t bytes);
void t_free(void *memoryg);

/* Define bzero() as a macro if it's not in standard C library. */
#ifndef	HAVE_BZERO
#define	bzero(ptr,n)		memset(ptr, 0, n)
#endif

#define eprintf(args...) fprintf (stderr, args)

#endif	/* T_MALLOC_H */

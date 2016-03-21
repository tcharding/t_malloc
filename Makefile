CC = gcc
CFLAG = -std=c99 -D_XOPEN_SOURCE=600 \
		-D_DEFAULT_SOURCE \
		-g -I${TLPI_INCL_DIR} \
		-pedantic \
		-Wall \
		-W \
		-Wmissing-prototypes \
		-Wno-sign-compare \
		-Wno-unused-parameter
EXE = test
SPLINT = lint.out 

.PHONY: all

all: $(EXE)

test: test_malloc.o t_malloc.o
	$(CC) -o test test_malloc.o t_malloc.o

%.o: %.c t_malloc.h
	$(CC) $(CFLAG) -c $<

# lint rules
lint:
	@if [ -f "$$SPLINT" ]; then rm "$$SPLINT"; fi 
	@for f in  test_malloc t_malloc; do \
	if [ -f $$f.c ]; then \
	splint $$f.c -I../include -warnposix -nullderef -nullpass -unrecog \
	>> $(SPLINT); \
	fi; done

clean:
	@for f in *.o $(EXE); do \
	if [ -f $$f ]; then \
	rm "$$f"; \
	echo "rm $$f" ; \
	fi; done



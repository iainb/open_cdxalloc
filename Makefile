PROJECT = libcedarxalloc.a
OBJECTS = cdxalloc.o
CFLAGS  = -Wall

all: $(PROJECT)

.c.o:
	gcc -c $(CFLAGS) $<

$(PROJECT): $(OBJECTS)
	ar rcs $(PROJECT) $(OBJECTS)

clean:
	@rm -f $(PROJECT)
	@rm -f $(OBJECTS)

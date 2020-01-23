#type 'make' to run demo
#type 'make clean' to generated files

#COMPILER
CC := gcc

#SOURCE DIRECTORY
SRCDIR := src/

#BUILD DIRECTORY
BUILDDIR := build/

#COMPILER FLAGS
CFLAGS := -Iinclude -O0

#LIBRARY FLAGS
LDFLAGS := -lm

#Finding Source Files
SRCS := $(wildcard $(SRCDIR)*.c)

#Generating Object Files
OBJS := $(SRCS:$(SRCDIR)%.c=$(BUILDDIR)%.o)

DEPS = $(OBJS:.o=.d)

#Executable Name
EXEC := driver

#Recipe

demo: test
	mkdir -p demo/
	./$(EXEC)

.PHONY: $(BUILDDIR)
test: $(BUILDDIR) $(OBJS)
	$(CC) -c $(CFLAGS) test/driver.c -o $(BUILDDIR)test.o
	$(CC) -o $(EXEC) $(OBJS) $(BUILDDIR)test.o $(LDFLAGS)

$(BUILDDIR)%.o: $(SRCDIR)%.c
	$(CC) -c $(CFLAGS)  $< -o $@
	$(CC) -MM $(CFLAGS)  $< > $(BUILDDIR)$*.d

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

-include $(DEPS)	

#Cleaning Recipe

.PHONY: clean
clean:
	rm -f $(EXEC) $(BUILDDIR)*.o $(BUILDDIR)*.d
	rm -f demo/*.eps demo/*.svg
	rmdir build/ demo/

## The Java compiler (for demos only)

# What we use to compile Java files.
JAVAC = javac

# The flags we use to compile Java files.
JFLAGS = -g

## Implicit rules
.SUFFIXES: .java .class

.java.class:
	$(JAVAC) $(JFLAGS) $<

all:	jtest
	@:
.PHONY: all

.PHONY: jtest
jtest: 	jtest.class Date.class Holiday.class List.class Tree.class
	@:

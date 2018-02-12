#Choose a compiler
COMPILER=gcc

#Choose C Flags
C_FLAGS=

#Set up our make settings
all: dash

%.o: %.c
	$(COMPILER) $(C_FLAGS) -c -o $@ $<

dash: dash.o commands.o
	$(COMPILER) -o $@ $^

clean:
	rm -f *.o *~ core dash

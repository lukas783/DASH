#Choose a compiler
COMPILER=gcc

#Choose C Flags
C_FLAGS=-std=gnu11

#Set up our make settings
all: dash

%.o: %.c
	$(COMPILER) $(C_FLAGS) -c -o $@ $<

dash: dash.o commands.o signalHandle.o
	$(COMPILER) -o $@ $^

clean:
	rm -f *.o *~ core dash

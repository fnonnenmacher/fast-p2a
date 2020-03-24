
CFILES = LemireBitUnpacking.cpp SWParquetReader.cpp SWParquetReaderDelta.cpp
OBJFILES = $(CFILES:.cpp=.o)

all: ptoa.a

%.o: %.c
	gcc -c $@ $<

ptoa.a: $(OBJFILES)
	rm -f $@ # remove first, otherwise it will keep adding .o files to it
	ar q $@ $^

.phony: clean
clean:
	rm -f $(OBJFILES) ptoa.a

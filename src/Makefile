.SILENT:

BIN_DIR = .

CFLAGS = -g -I$(INC_DIR)
LFLAGS = -g -L$(LIB_DIR)

.cpp.o:
	echo "Compiling $<"
	g++ -c $(CFLAGS) $<

all: $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat

CConcat.o: CConcat.cpp

$(BIN_DIR)/CConcat: CConcat.o
	echo "Linking CConcat"
	g++ -o $(BIN_DIR)/CConcat $(LFLAGS) CConcat.o

CUnconcat.o: CUnconcat.cpp

$(BIN_DIR)/CUnconcat: CUnconcat.o
	echo "Linking CUnconcat"
	g++ -o $(BIN_DIR)/CUnconcat $(LFLAGS) CUnconcat.o

clean:
	rm -f $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat *.o

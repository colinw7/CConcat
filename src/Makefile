.SILENT:

CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: dirs $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat $(BIN_DIR)/CConcatFind

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

SRC1 = CConcat.cpp
SRC2 = CUnconcat.cpp
SRC3 = CConcatFind.cpp
SRC4 = CConcatBase.cpp

OBJ1 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC1))
OBJ2 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC2))
OBJ3 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC3))
OBJ4 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC4))

CFLAGS = $(CDEBUG) -std=c++14 -I$(INC_DIR) -I. -I../../CGlob/include
LFLAGS = $(LDEBUG) -L$(LIB_DIR) -L../../CGlob/lib

LIBS = -lCGlob 

$(OBJ1): $(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	$(CC) -c $(CFLAGS) $< -o $(OBJ_DIR)/$*.o

$(OBJ2): $(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	$(CC) -c $(CFLAGS) $< -o $(OBJ_DIR)/$*.o

$(OBJ3): $(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	$(CC) -c $(CFLAGS) $< -o $(OBJ_DIR)/$*.o

$(OBJ4): $(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	$(CC) -c $(CFLAGS) $< -o $(OBJ_DIR)/$*.o

$(BIN_DIR)/CConcat: $(OBJ1) $(OBJ4)
	@echo "Linking CConcat"
	g++ -o $(BIN_DIR)/CConcat $(LFLAGS) $(OBJ1) $(OBJ4) $(LIBS)

$(BIN_DIR)/CUnconcat: $(OBJ2) $(OBJ4)
	@echo "Linking CUnconcat"
	g++ -o $(BIN_DIR)/CUnconcat $(LFLAGS) $(OBJ2) $(OBJ4) $(LIBS)

$(BIN_DIR)/CConcatFind: $(OBJ3) $(OBJ4)
	@echo "Linking CConcatFind"
	g++ -o $(BIN_DIR)/CConcatFind $(LFLAGS) $(OBJ3) $(OBJ4) $(LIBS)

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat $(BIN_DIR)/CConcatFind

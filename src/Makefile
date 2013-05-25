CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat

SRC1 = CConcat.cpp
SRC2 = CUnconcat.cpp

OBJ1 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC1))
OBJ2 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC2))

CFLAGS = -I$(INC_DIR) -I.
LFLAGS = -L$(LIB_DIR)

$(OBJ1): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(OBJ2): $(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CConcat: $(OBJ_DIR)/CConcat.o
	g++ -o $(BIN_DIR)/CConcat $(LFLAGS) $(OBJ_DIR)/CConcat.o

$(BIN_DIR)/CUnconcat: $(OBJ_DIR)/CUnconcat.o
	g++ -o $(BIN_DIR)/CUnconcat $(LFLAGS) $(OBJ_DIR)/CUnconcat.o

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat

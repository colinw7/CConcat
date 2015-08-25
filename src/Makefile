#.SILENT:

CC = g++
AR = ar
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat $(BIN_DIR)/CConcatFind

SRC1 = CConcat.cpp
SRC2 = CUnconcat.cpp
SRC3 = CConcatFind.cpp

OBJ1 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC1))
OBJ2 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC2))
OBJ3 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC3))

CFLAGS = -I$(INC_DIR) -I.
LFLAGS = -L$(LIB_DIR)

$(OBJ1): $(OBJ_DIR)/%.o: %.cpp
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) $< -o $(OBJ_DIR)/$*.o

$(OBJ2): $(OBJ_DIR)/%.o: %.cpp
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) $< -o $(OBJ_DIR)/$*.o

$(OBJ3): $(OBJ_DIR)/%.o: %.cpp
	echo "Compiling $<"
	$(CC) -c $(CFLAGS) $< -o $(OBJ_DIR)/$*.o

$(BIN_DIR)/CConcat: $(OBJ1)
	@echo "Linking CConcat"
	g++ -o $(BIN_DIR)/CConcat $(LFLAGS) $(OBJ1)

$(BIN_DIR)/CUnconcat: $(OBJ2)
	@echo "Linking CUnconcat"
	g++ -o $(BIN_DIR)/CUnconcat $(LFLAGS) $(OBJ2)

$(BIN_DIR)/CConcatFind: $(OBJ3)
	@echo "Linking CConcatFind"
	g++ -o $(BIN_DIR)/CConcatFind $(LFLAGS) $(OBJ3)

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CConcat $(BIN_DIR)/CUnconcat $(BIN_DIR)/CConcatFind

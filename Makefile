CC = gcc
CFLAG = -lpthread

TARGET = Potato
D_SRC = src
D_OBJ = obj
D_INC = -I ./src

SRC_C = $(wildcard $(D_SRC)/*.c)

OBJ_C = $(addprefix $(D_OBJ)/, $(patsubst %.c,%.o,$(notdir $(SRC_C))))

BUILDDIR = obj

all:$(BUILDDIR) $(TARGET)

$(BUILDDIR):
	mkdir $(BUILDDIR)

$(TARGET):$(OBJ_C)
	$(CC) $^ -o $@ $(CFLAG)

$(D_OBJ)/%.o:$(D_SRC)/%.c
	$(CC) -c $(D_INC) $< -o $@ 

#.PHONY：clean
clean:
	rm -f $(TARGET)
	rm -r obj

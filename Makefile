CC=gcc
CCFLAGS=-g `cups-config --cflags` `cups-config --libs`
OBJ=printer_connection.o

all: ${OBJ} epilog install

epilog: cups-epilog.c
	${CC} ${CCFLAGS} -o epilog ${OBJ} cups-epilog.c 
	
%.o:%.c
	$(CC) ${CCFLAGS} -c $? -o $@

        
install: epilog
	./install.sh
	
clean: 
	rm -rf epilog
CC=gcc
#TODO: REMOVE no-format. just there to keep eclipse from bugging me
CCFLAGS=-Wno-format -g `cups-config --cflags` `cups-config --libs`
OBJ=printer_connection.o

#TODO: REMOVE INSTALL FROM TARGET
all: ${OBJ} epilog install

epilog: cups-epilog.c
	${CC} ${CCFLAGS} -o epilog ${OBJ} cups-epilog.c 
	
%.o:%.c
	$(CC) ${CCFLAGS} -c $? -o $@

        
install: epilog
	cp epilog /usr/lib/cups/backend/
	sudo /etc/init.d/cups restart
	
clean: 
	rm -rf epilog
GCJ = gcj
COMPOPTS = -O3 -Wall
SOURCES=./quickfind.java \
	./quickcompare \
	./quickcompare.java \
	./flipy.java

all: quickcompare quickfind flipy

quickcompare:
	$(GCJ) ${COMPOPTS} --main=quickcompare -o $@ $@.java

quickfind:
	        $(GCJ) ${COMPOPTS} --main=quickfind -o $@ $@.java 

flipy:
	        $(GCJ) ${COMPOPTS} --main=flipy -o $@ $@.java

clean:
	rm quickcompare quickfind flipy

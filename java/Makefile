GCJ = gcj
COMPOPTS = -O3 -Wall
SOURCES=./find.java \
	./compare \
	./compare.java \
	./flipy.java

all: compare find flipy

compare:
	$(GCJ) ${COMPOPTS} --main=compare -o $@ $@.java

find:
	        $(GCJ) ${COMPOPTS} --main=find -o $@ $@.java 

flipy:
	        $(GCJ) ${COMPOPTS} --main=flipy -o $@ $@.java

clean:
	rm -f compare find flipy

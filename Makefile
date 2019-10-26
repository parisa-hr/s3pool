
all:
	$(MAKE) -C src/s3pool all
	$(MAKE) -C client/c all

prefix ?= /usr/local

install: all
	install -d ${prefix}/include ${prefix}/lib ${prefix}/bin
	install src/s3pool/s3pool ${prefix}/bin
	install client/c/s3pool.h ${prefix}/include
	install client/c/libs3pool.a ${prefix}/lib
	install client/c/{s3glob,s3cat,s3pull,s3push,s3refresh} ${prefix}/bin

clean:
	$(MAKE) -C src/s3pool clean
	$(MAKE) -C client/c clean


.PHONY: all install clean

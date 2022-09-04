

status: main.c main.h getinfo.c getinfo.h netinfo.c netinfo.h util.c util.h shared.h config.h
	gcc -O2 -o status main.c getinfo.c netinfo.c util.c

install: status
	cp -f status ~/.bin/

clean:
	rm status

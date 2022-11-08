CFLAGS=-O3 -Wall -lpam -lpam_misc
BIN=udo

all:
	mkdir -p bin
	gcc $(CFLAGS) $(wildcard src/*.c) -o bin/udo

install: all
	cp bin/udo /usr/local/bin/udo
	chown root:root /usr/local/bin/udo
	chmod 6755 /usr/local/bin/udo
	cp pam.d_udo /etc/pam.d/udo
	cp -n udo.conf /etc/udo.conf

uninstall:
	rm /usr/local/bin/udo
	rm /etc/pam.d/udo

clean:
	rm -r bin/*

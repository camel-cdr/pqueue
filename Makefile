pqueue: pqueue.c
	${CC} $< -o $@

install: pqueue
	cp -f pqueue /usr/local/bin/pqueue
	chmod 755 /usr/local/bin/pqueue

uninstall:
	rm -rf /usr/local/bin/pqueue

clean:
	rm -f pqueue

.PHONY: install uninstall clean

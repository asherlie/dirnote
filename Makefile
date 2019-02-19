CXX=gcc
all: no
no:
	$(CXX) notif.c fsys.c -o no -Wall -Wextra -Wpedantic

clean:
	rm -f no

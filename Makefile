CXX=gcc
all: no
no:
	$(CXX) notif.c fsys.c -o no -Wall -Wextra -Wpedantic -Wunused

clean:
	rm -f no

CXX=gcc
all: no
no:
	$(CXX) notif.c fsys.c -o no -Wall -Wextra -Wpedantic -Wunused -O3

clean:
	rm -f no

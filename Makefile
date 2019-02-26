CXX=gcc
all: no
no:
	$(CXX) notif.c fsys.c fname_hash.c -o no -Wall -Wextra -Wpedantic -Wunused -O3 -lpthread
db:
	$(CXX) notif.c fsys.c fname_hash.c -o no -Wall -Wextra -Wpedantic -Wunused -g -lpthread

clean:
	rm -f no

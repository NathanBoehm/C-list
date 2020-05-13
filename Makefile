

test:
	gcc -O2 -Wall -Wextra -pedantic -g -I. clist_test.c -o clist_test

clean:
	@[ -f clist_test ] && rm clist_test || echo "No output file"

runtest:
	@[ -f clist_test ] && ./clist_test
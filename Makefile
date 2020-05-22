

test: clist_test_default.c clist.h
	gcc -O0 -Wall -g -I. clist_test_default.c -o clist_test_default

clean:
	@[ -f clist_test ] && rm clist_test & rm gcc_test || echo "No output file"

runtest:
	@[ -f clist_test ] && ./clist_test_default

gcc_test: gcc_test.c clist.h
	gcc -g -I. gcc_test.c -o gcc_test
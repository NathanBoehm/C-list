
INC=-I. -I../test/
FLAGS=-O2 -Wall -g
CC=gcc

.POHNY: buildtest
buildtest:
	$(CC) $(FLAGS) $(INC) clist_test.c -o clist_test

.PHONY: runtest
runtest:
	./clist_test

.PHONY: test
test: buildtest runtest

.PHONY: custom_free_test
custom_free_test:
	$(CC) $(FLAGS) $(INC) custom_free_test.c -o custom_free_test

.PHONY: clean
clean:
	@[ -f clist_test ] && rm clist_test || echo "no clist_test"
	@[ -f debug_app ] && rm debug_app || echo "no debug_app"
	@[ -f custom_free_test ] && rm custom_free_test || echo "no custom_free_test"

.PHONY: debug_app
debug_app:
	$(CC) $(FLAGS) $(INC) ../include/clist.h ../examples/debug_app.c -o debug_app

.PHONY: profiler_app
profiler_app:
	$(CC) $(FLAGS) -pg $(INC) ../include/clist.h ../examples/debug_app.c -o profiler_app
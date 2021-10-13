SearchNewBooks: SearchNewBooks.cc
	g++ -Wall -Werror -o $@ $^
clean:
	rm -f SearchNewBooks
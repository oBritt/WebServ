#include "Reader.hpp"
#include "Tomweb.hpp"
Reader::Reader() : writer()
{
	errNbr = 200;
	readingDone = 0;
	contentLength = 0;
	autoIndex = 0;
	method = "";
	URI = "";
	fdReadingFrom = -1;
	have_read = "";
	have_read_2 = "";
	openFile = 0;
	cnect_close = 0;
	post = 0;
	dir = NULL;
}

void	Reader::reset()
{
	
}


#include "Connection.hpp"
#include <unistd.h>
#include "Tomweb.hpp"
Connection::Connection(int fd) : socket_fd(fd), reader()
{
	// std::cerr << "new Connect, socket, fd = " << socket_fd << ", " << fd << std::endl;
	IsAfterResponseClose = 1;
	readingHeaderDone = 0;
	have_read = "";
	reader.have_read = "";
	reader.have_read_2 = "";
	reader.cnect_close = 0;
	reader.contentLength = 0;
	reader.errNbr = 200;
	reader.fdReadingFrom = -1;
	reader.method = "";
	reader.openFile = 0;
	reader.post = 0;
	reader.readingDone = 0;
	reader.URI = "";
	reader.writer.fdWritingTo = -1;
	reader.writer.writeString = "";
	reader.writer.writingDone = 0;
	reader.errFuncCall = 0;
	time_out = time(NULL);
	reader.time_out = time(NULL);
}

Connection::~Connection()
{
}

void Connection::reset()
{
	IsAfterResponseClose = 0;
	readingHeaderDone = 0;
	have_read = reader.have_read;
	reader.have_read = "";
	reader.have_read_2 = "";
	reader.cnect_close = 0;
	reader.contentLength = 0;
	reader.errNbr = 200;
	change_option_poll(socket_fd, POLLIN);
	if (reader.fdReadingFrom != socket_fd && reader.fdReadingFrom != -1)
	{
		remove_from_poll(reader.fdReadingFrom);
		close(reader.fdReadingFrom);
	}
	reader.fdReadingFrom = -1;
	reader.method = "";
	reader.openFile = 0;
	reader.post = 0;
	reader.readingDone = 0;
	reader.URI = "";
	if (reader.writer.fdWritingTo != socket_fd && reader.writer.fdWritingTo != -1)
	{
		remove_from_poll(reader.writer.fdWritingTo);
		close(reader.writer.fdWritingTo);
	}
	reader.writer.fdWritingTo = -1;
	reader.writer.writeString = "";
	reader.writer.writingDone = 0;
	reader.errFuncCall = 0;
	time_out = time(NULL);
	reader.time_out = time(NULL);
	
	reader.cookies = "";
	reader.pid = -1;
	reader.readCGI = 0;
	if (reader.file_name1 != "")
		std::remove(reader.file_name1.c_str());
	reader.file_name1 = "";
}

#include "../Tomweb.hpp"

void	del_connect(Server &server, Connection &cnect, int j)
{
	std::cerr << "CONNECTION " <<  cnect.socket_fd  << " DELETED" << std::endl;
	// std::cout << "cnect.have_read = " << cnect.have_read << std::endl;
	// std::cout << "cnect.IsAfterResponseClose = " << cnect.IsAfterResponseClose << std::endl;
	
	// std::cout << "cnect.socket_fd = " << cnect.socket_fd << std::endl;
	// std::cout << "cnect.reader.autoIndex = " << cnect.reader.autoIndex << std::endl;
	// std::cout << "cnect.reader.contentLength = " << cnect.reader.contentLength << std::endl;
	// std::cout << "cnect.reader.errFuncCall = " << cnect.reader.errFuncCall << std::endl;
	// std::cout << "cnect.reader.errNbr = " << cnect.reader.errNbr << std::endl;
	// std::cout << "cnect.reader.fdReadingFrom = " << cnect.reader.fdReadingFrom << std::endl;
	// std::cout << "cnect.reader.have_read = " << cnect.reader.have_read << std::endl;
	// std::cout << "cnect.reader.have_read_2 = " << cnect.reader.have_read_2 << std::endl;
	// std::cout << "cnect.reader.host = " << cnect.reader.host << std::endl;
	// std::cout << "cnect.reader.method = " << cnect.reader.method << std::endl;
	// std::cout << "cnect.reader.openFile = " << cnect.reader.openFile << std::endl;
	// std::cout << "cnect.reader.post = " << cnect.reader.post << std::endl;
	// std::cout << "cnect.reader.URI = " << cnect.reader.URI << std::endl;
	// std::cout << "cnect.reader.readingDone = " << cnect.reader.readingDone << std::endl;
	// std::cout << "cnect.reader.writer.fdWritingTo = " << cnect.reader.writer.fdWritingTo << std::endl;
	// std::cout << "cnect.reader.writer.writeString = " << cnect.reader.writer.writeString << std::endl;
	// std::cout << "--------------------------- " << std::endl;
	// std::cout << "cnect.reader.cnect_close = " << cnect.reader.cnect_close << std::endl;
	// std::cout << "cnect.reader.writer.writingDone = " << cnect.reader.writer.writingDone << std::endl;
	// std::cout << "cnect.readingHeaderDone = " << cnect.readingHeaderDone << std::endl;
	// std::cout << "cnect.reader.writer.writingDone = " << cnect.reader.writer.writingDone << std::endl;
	// std::cout << "------------------------------------------------------------------------------------------------------------ " << std::endl;
	int fd1 = cnect.socket_fd;
	int	fd2 = cnect.reader.fdReadingFrom;
	int	fd3 = cnect.reader.writer.fdWritingTo;
	
	if (fd2 == fd1)
		fd2 = -1;
	if (fd3 == fd1)
		fd3 = -1;
	remove_from_poll(fd1);
	close(fd1);
	if (fd2 != -1)
	{
		remove_from_poll(fd2);
		close(fd2);
	}
	if (fd3 != -1)
	{
		remove_from_poll(fd3);
		close(fd3);
	}
	std::cout << "server.connections.size() before = " << server.connections.size() << std::endl;
	// sleep(1);
	std::cout << " j = " << j << std::endl;
	if (server.connections.size() == 1)
		server.connections.clear();
	else
		server.connections.erase(server.connections.begin() + j);
	std::cout << "server.connections.size() after = " << server.connections.size() << std::endl;
	// sleep(1);
}

int	reading_done(Server &server, Connection &cnect, Reader &reader)
{
	cnect.readingHeaderDone = 1;
	cnect.reader.readingDone = 0;
	cnect.reader.time_out = clock();
	// std::cout << "reader.host = {" << reader.host << "}" << std::endl;
	// std::cout << "reader.method = {" << reader.method << "}" << std::endl;
	// std::cout << "cnect.have rad = " << cnect.have_read << std::endl;
	// std::cout << "reader.URI = {" << reader.URI << "}" << std::endl;
	if (reader.method != "GET" && reader.method != "POST" && reader.method != "DELETE")
		return (reader.errNbr = 405, 1);

	if (reader.URI.length() > 160)
		return (reader.errNbr = 414, 1);
	std::vector<std::string> a = get_data(reader.host, reader.method, reader.URI, server);
	std::cout << "a[0] = " << a[0] << std::endl;
	std::cout << "a[1] = " << a[1] << std::endl;
	std::cout << "a[2] = " << a[2] << std::endl;
	std::cout << "a[3] = " << a[3] << std::endl;
	std::cout << "a[4] = " << a[4] << std::endl;
	//URI	if (*reader.URI.begin() != '/')
		reader.URI = "/" + reader.URI;
	reader.URI = a[2];
	//a[0] is host:post ok?
	// std::cout << "cnect.reader.errNbr = " << cnect.reader.errNbr << std::endl;
	std::cout << reader.method << std::endl;
	std::cout << a[0] << std::endl;
	if (a[0] == "0")
	{
		std::cout << 1 << std::endl;
		return (reader.errNbr = 400, 1);
	}
	else if (reader.contentLength > server.body_size_max)
	{
		return (reader.errNbr = 413, 1);
	}
	else if (a[1] == "0") //method not allowed
	{
		return (reader.errNbr = 405, 1);
	}
	else
	{
		reader.autoIndex = 1;
		if (a[3] == "0") //auto index
			reader.autoIndex = 0;
	}
	if (a[4] != "")
		return (reader.URI = a[4], cnect.reader.errNbr = 301, 1);
	if (cnect.reader.method == "GET" || cnect.reader.method == "DELETE")
		if (cnect.reader.contentLength > 0)
			return (cnect.reader.errNbr = 400, 1);
	if (cnect.reader.method == "POST" && cnect.reader.errNbr < 300)
	{
		if (cnect.reader.contentLength < 0)
			return (cnect.reader.errNbr = 400, 1);
		cnect.reader.have_read = cnect.have_read;
		cnect.have_read = "";
	}
	if (cnect.reader.URI.find(".cgi") != std::string::npos)
		cnect.reader.readCGI = 1;
	std::cout << "cnect.reader.URI = " << cnect.reader.URI << std::endl;
	std::cout << "cnect.reader.readCGI = " << cnect.reader.readCGI << std::endl;
	return (1);
}

int	extract_IsAfterResponseClose(std::string &header_o)
{
	std::string	header = header_o;
	std::string	str = "\r\nConnection: ";
	ssize_t	start = header.find(str);
	if (start == static_cast<ssize_t>(std::string::npos))
		return (1);
	start += str.length();
	ssize_t end = header.find("\r\n", start);
	if (end == static_cast<ssize_t>(std::string::npos))
		return (1);
	std::string content = header.substr(start, end);	
	if (content.find("keep-alive") == std::string::npos
		|| content.find("keep-alive") >= 10)
		return (1);
	return (0);
}

int	extract_contentLength(std::string &header_o)
{
	std::string	header = header_o;
	std::string	str = "\r\nContent-Length:";
	ssize_t	start = header.find(str);
	if (start == static_cast<ssize_t>(std::string::npos))
		return (0);
	start += str.length();
	ssize_t end = header.find("\r\n", start);
	if (end == static_cast<ssize_t>(std::string::npos))
		return (0);
	std::string content = header.substr(start, end);
	std::stringstream socket_stream(content);
	int value;
	socket_stream >> value;
	return (value);
}

std::string	extract_host(std::string &header_o)
{
	std::string	header = header_o;
	std::string	str = "\r\nHost:";
	ssize_t	start = header.find(str);
	if (start == static_cast<ssize_t>(std::string::npos))
		return ("");
	start += str.length();
	ssize_t end = header.find("\r\n", start);
	if (end == static_cast<ssize_t>(std::string::npos))
		return ("");
	std::string content = header.substr(start, end);
	std::stringstream socket_stream(content);
	std::string value;
	socket_stream >> value;
	return (value);
}

std::string	extract_cookies(std::string &header_o)
{
	std::string	header = header_o;
	std::string	str = "\r\nCookies:";
	ssize_t	start = header.find(str);
	if (start == static_cast<ssize_t>(std::string::npos))
		return ("");
	start += str.length();
	ssize_t end = header.find("\r\n", start);
	if (end == static_cast<ssize_t>(std::string::npos))
		return ("");
	return (header.substr(start, end));
}

int	header_extract(Connection &cnect, std::string &header_o)
{
	std::string host;

	cnect.IsAfterResponseClose = extract_IsAfterResponseClose(header_o);
	cnect.reader.contentLength = extract_contentLength(header_o);
	cnect.reader.host = extract_host(header_o);
	cnect.reader.cookies = extract_cookies(header_o);
	return (1);
}

int	request_line(Connection &cnect)
{
	ssize_t		request_line_end;

	request_line_end = cnect.have_read.find("\r\n");

	std::string request_line = cnect.have_read.substr(0, request_line_end);
	std::stringstream socket_stream(request_line);
	std::string method = "";
	std::string URI = "";
	std::string HTTP_version = "";

	if (!(socket_stream >> method >> URI >> HTTP_version))
		return (cnect.reader.errNbr = 400, cnect.readingHeaderDone = 1, 1);
	if (method != "GET" && method != "POST" && method != "DELETE")
		return (cnect.reader.errNbr = 405, cnect.readingHeaderDone = 1, 1);
	// if (HTTP_version != "HTTP/1.1")
	// {
	// 	return (cnect.reader.errNbr = 400, cnect.readingHeaderDone = 1, 1); // need to change
	// }
	cnect.reader.method = method;
	cnect.reader.URI = URI;
	return (1);
}


//WARING WARNING WARNING:
//need to handle, so that 1 space enter also isn't allowed, must separate requestline and header part.
int	request_header(Server &server, Connection &cnect)
{
	if (cnect.have_read.length() == 0)
		return (1);
	ssize_t	header_end = cnect.have_read.find("\r\n\r\n");
	if (header_end == static_cast<ssize_t>(std::string::npos))
	{
		if (cnect.have_read.length() > 2000)
			return (cnect.readingHeaderDone = 1, cnect.IsAfterResponseClose = 1, cnect.reader.errNbr = 431);
		return (0);
	}
	request_line(cnect);
	header_extract(cnect, cnect.have_read);
	header_end = cnect.have_read.find("\r\n\r\n");
	cnect.have_read.erase(0, header_end + 4);
	return (reading_done(server, cnect, cnect.reader));
}

int reading_header(Server &server, Connection &connect)
{
	int		check;
	char	buffer[BUFFERSIZE + 1];

	if (check_fds(connect.socket_fd) & POLLIN)
	{
		check = read(connect.socket_fd, buffer, BUFFERSIZE);
		if (check == -1)
		{
			std::cout << "reader in connection = -1" << std::endl;
			// sleep(1);
			// return (1);
			return (2);
			// return (connect.reader.errNbr = 500, connect.readingHeaderDone = 1, 1);
		}
		if (check == 0)
		{
			std::cout << "return 2" << std::endl;
			return (2);
		}
		// std::string a = "";
		// a.append(buffer, check);
		// std::cout << a << std::endl;
		connect.have_read.append(buffer, check);
	}
	return (request_header(server, connect));
}

void	connection_level(std::vector<Server> &servers)
{
	// time_t	now;
	// int	revents;
	unsigned int	j;
	Connection 		*cnect;

	for (unsigned int i = 0; i < servers.size(); i++)
	{
		j = 0;
		while (j < servers[i].connections.size())
		{
			cnect = &servers[i].connections[j];
			// revents = check_fds(servers[i].connections[j].socket_fd);
			if (cnect->reader.cnect_close == 1
				|| (cnect->IsAfterResponseClose == 1 && cnect->reader.writer.writingDone == 1))
				// || revents & POLLHUP)
			{
				std::cout << "Maybe revents = POLLHUP" << std::endl;
				del_connect(servers[i], *cnect, j);
			}
			else if (cnect->reader.writer.writingDone == 1 )
			{
				cnect->reset();
				j++;
			}
			else if (cnect->readingHeaderDone == 0)
			{
				if (reading_header(servers[i], *cnect) == 2)
					del_connect(servers[i], *cnect, j);
				else
					j++;
			}
			else
				j++;
		}
	}
}

// #define RESPONSE "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 0\r\n\r\n"
// void	connection_level(std::vector<Server> &servers)
// {
// 	unsigned int	j;

// 	for (unsigned int i = 0; i < servers.size(); i++)
// 	{
// 		j = 0;
// 		while (j < servers[i].connections.size())
// 		{
// 			if (check_fds(servers[i].connections[j].socket_fd) == POLLOUT)
// 			{
// 				write(servers[i].connections[j].socket_fd, RESPONSE, strlen(RESPONSE));
// 				close(servers[i].connections[j].socket_fd);
// 				remove_from_poll(servers[i].connections[j].socket_fd);
// 			}
// 			else
// 				j++;
// 		}
// 	}
// }
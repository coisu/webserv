enum Method { DEFAULT, GET, HEAD, POST, PUT, DELETE, OPTIONS, TRACE };
enum URIType { DIRECTORY, FILE, FILE_TO_CREATE, CGI_PROGRAM };
enum TransferType { GENERAL, CHUNKED };
enum Phase { READY, ON_HEADER, ON_BODY, COMPLETE };

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
// The only difference between read() and recv() is the presence of flags.

// GET / HTTP/1.1
// Host: www.example.com
// User-Agent: Mozilla/5.0
// Accept: *
// Accept-Language: en-GB,en;q=0.5
// Accept-Encoding: gzip, deflate, br
// Connection: keep-alive

bool Server::runRecvAndSolve(Connection& connection)
{
	try
    {
		recvRequest(connection, connection.get_m_request());
	}
    catch (int status_code)\
    {
		createResponse(connection, status_code);
		return (true);
	}
    catch (Server::IOError& e)
    {
		throw (e);
	}
    catch (std::exception& e)
    {
		ft::log(ServerManager::log_fd, std::string("[Failed][Request] Failed to create request because ") + e.what());
		createResponse(connection, 50001);
		return (true);
	}

	const Request& request = connection.get_m_request();

	if (request.get_m_phase() == Request::COMPLETE)
	{
		writeCreateNewRequestLog(request);
		connection.set_m_status(Connection::ON_EXECUTE);
		solveRequest(connection, connection.get_m_request());
		return (true);
	}
	return (false);
}
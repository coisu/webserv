#include "Service.hpp"

/* This constructor is called when none parameter was given to the program
 * first get instance of ParsingConfig class, 
 * check if any error happened while parsing file 
 * if there had not error setup,  displayAvailableServer, and run service
 * display available port during the setup */

Service::Service():_instance(ParseConfig::getInstance("./resources/default.conf")), _fdSel(0)
{
	if (_instance.getErrorHappened() == true)// if the parsing has detected an error
	{
		std::cout << "EXIT PROGRAM" << std::endl;
		return ;
	}
// 	int ret = setUpService();
// 	if (ret == SUCCESS)
// 	{
// 		//displayAvailableServer();
// 		runService();
// 	}
}

/* This constructor is called when a parameter was given to the program
 * first get instance of ParsingConfig class, 
 * check if any error happened while parsing file 
 * if there had not error setup,  displayAvailableServer, and run service
 * i display available port during the setup 
 * param FileName configle file */

Service::Service(std::string fileName):_instance(ParseConfig::getInstance(fileName)), _fdSel(0)
{
	if (_instance.getErrorHappened() == true)// if the parsing has detected an error
	{
		std::cout << "EXIT PROGRAM" << std::endl;
		return ;
	}
	// int ret = setUpService();
	// if (ret == SUCCESS)
	// {
	// 	//displayAvailableServer();
	// 	runService();
	// }
}

/*Destructor*/

Service::~Service(){}

/* This do nothing else than display available server socket and the port which it is tie
 * each server has own port, server object are stored in linked list 
 * linked list contain a server object at each node 
 * this function skim the linked list and display ports of server of each node
 * it mean each iteration is tie at a server object */

void	Service::displayServer(){

	std::cout << "\nAvailable Servers:" << std::endl << std::endl;
	for (std::list<TcpServer>::iterator it = _listServer.begin(); it != _listServer.end(); it++)
		std::cout << "-server = " << it->getServerSocketFd() << " = port = " << it->getPort() << " = " << std::endl;
	std::cout << std::endl << std::endl;
}

/* Set Up Service object
 * all_ports : this vector contains all port of config file
 * the goal of this function it is to create a linked list which has an instance of TcpServer class in each node
 * there will been as much node than the number of port
 * each port is tie in a TcpServer object, it mean we create a TcpServer object for each port 
 * return int 0 for a success or error for an -1 error happened some where */

int	Service::setUpService(){

	std::vector<int> &all_ports = ParseConfig::getPorts();

	for (_nFdServ = 0; _nFdServ < _instance.numberOfServer(); _nFdServ++)
	{
		TcpServer server;/* instance a server over each iteration*/
		if (all_ports.size() > _nFdServ)
			server.setPort(all_ports[_nFdServ]);
		server.setUpServer();
		// if (server.get_hasError() == true)/*check if a error happened while setup of server it stop the loop*/
		// 	return (ERROR); /* return this when an error occurred while the setup of server */
		_fdSel[_nFdServ] = server.getServerSocketFd();
		_listServer.push_back(server);
	}
	return (SUCCESS);
}

/**
 * @brief 
 * allow to handle event
 * event can be on existed files descriptor or can be on  new files file descriptor which has to be accept by a server like  new file descriptor  
 * 
 * this function do two thing main:
 * 		one: if new connection came, add it in the vector "_socket_client" and poll array
 * 		two: if is existing socket go read request, and send reponse
 * 
 * it important to know than "_listServer"  this linked list contain a server on each node
 * this function loop through this list to look up the socket client which has triggered even
 *
 *	Each server have own vector of "sokect client" which are connected to it
 *
 *	the loop allow to loop through each Server inside linked list
 *
 * when event is  new on  new files file descriptor:
 * 		-> this file descriptor is add in an array  which belong server it has send request
 * 		-> likewise, this file descriptor is add in array of poll too
 * 
 *	when event is   on  existing file descriptor:
 *		-> go head, to recovery data inside
 *
 * 	variable "old_size" is initialized at size of array "vect_socket_client" this let me to update poll array :
 *	if the size of "_sockect_clients" has grown, variable "old_size" contain the old size of vect_socket_client, 
 *	with the difference between new size of "_sockect_clients"  and old size allow to update poll array with function addFdsToPollFds(..))
 * @param index on file descriptor of server to handler
 */

// void	Service::manageServer(size_t &index)
// {
// 	size_t	prev_size;
// 	for (std::list<TcpServer>::iterator it = _listServer.begin(); it != _listServer.end(); it++)
// 	{
// 		TcpServer	&currentServer = *it;
// 		std::vector<int> & vect_socket_client = currentServer.get_sockect_clients();
// 		prev_size = vect_socket_client.size();
// 		if (_fdSel[index] == currentServer.getServerSocketFd())// meaning new incoming connection
// 		{
// 			currentServer.accept_all_incoming_connections();// new socket has been added to "_sockect_clients"
// 			addFdsToPollFds(vect_socket_client, prev_size);// poll array should update to by adding the new socket on it
// 		}
// 		else if ((std::find(vect_socket_client.begin(), vect_socket_client.end(), _fdSel[index].fd)) != vect_socket_client.end())
// 		{
// 			//std::cout << "========= handle existing connection ===============" << std::endl;
// 			_closeConnexion = currentServer.handle_existing_connections(&_fdSel[index]); // return true if the connection is closed
// 			if (_closeConnexion == true)
// 			{
// 				_compressFdSel = true;// In this case the array  poll will be  squeeze
// 			}
// 		}
// 	}
// }


/* This function triggered when a signal ctrl-c was sended
 * throw exception for stopping running program
 * param sig value of signal
 */
void    handleSignal(int sig){

	std::cout << "Caught signal number = " << sig << std::endl;
}

/**
 * @brief 
 * this is the loop of program
 * function "signal" let to catch when a signal and stopping the program
 * function poll expect a event occurs
 * the loop "for" skim the array of poll to check if there a file descriptore has event
 * "handlerServer" it called only if an event happened to one of file descriptor of array poll 
 * _pollFds[index].revents == 0 if  there are none event
 */
// void    Service::runService()
// {
// 	int	ret;
// 	try
// 	{
// 		while (true)
// 		{
// 			signal(SIGINT, handleSignal);
// 			ret = poll(_pollFds, _nfds, TIMEOUT);
// 			if (ret == ERROR && errno == EINTR)// check if poll is termined by ctrl-c 
// 				return ;//ici met throw("exit program");
// 			checkError(ret, "poll() failed");
// 			if (ret == 0)
// 				throw("poll() timed out");
// 			for (size_t index = 0; index < _nfds; index++) //loop through the array of sokect to monitoring an event 
// 			{
// 				//std::cout << "==========_pollFds[index].fd = " << _pollFds[index].fd << " index = " << index << "===========" << std::endl;
// 				if (_pollFds[index].revents == NONE_EVENT)//loop as long the are not event happened
// 					continue;
// 				handlerServer(index);// an event happened, handle it
// 			}
// 			squeeze_tab_poll();
// 		}
// 	}
// 	catch (const char * msg_error)
// 	{
// 		std::cerr << msg_error << '\n';
// 	}
// }
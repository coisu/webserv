# ifndef SERVICE_HPP
#define SERVICE_HPP

#include "utils.h"
#include "ParseConfig.hpp"
#include "TcpServer.hpp"
#include <signal.h>

class Service{
    private:
    
    // Methods

        int     setUpService( void );
        void    displayServer( void );
        void    manageServer( size_t &index );
    
    // Members variables

        ParseConfig             &_instance;
        char                    _buffer[BUFFER_SIZE];
        std::vector<int>        _fdSel;
        size_t                  _nFdServ; //number file descriptor server
        std::list<TcpServer>    _listServer;
        bool                    _closeConnexion;
        bool                    _compressFdSel;


    public:
    
    // Constructors
        Service( void );
        Service( std::string fileName );
        ~Service( void );
    
    // Public Methods
        void    runService( void );
};

#endif
/*
I took the connectTCP function from the Comer and Stevens text and wrote a 
lot more comments and simplified the code for readability.  You may use this 
function for your lab.

I left in a few old C library calls for the sake of reviewing them.

For the sake of keeping peace with other universities, please do your testing 
using Ramapo College.  Once you are sure that your program works, you may 
then choose a foreign site for your final runs.  The following are some sites 
that I have tested:

    aeneas.mit.edu - does not respond
    cs.toronto.edu - does not respond any more.
    cs.utah.edu   -   Does not respond any more.
    time.nist.gov - gives UTC time  Comes from Boulder Col.  Does not do echo
    www-2.cs.cmu.edu - does not respond any more

I hope this helps,
Vic
*/

/*
 *      To demonstrate Client Operations.
 */
//#include <windows.h>

#include <iostream>
using namespace std;

#include <Winsock2.h>

/* The maximum size of a line from the data server. */
#define LINESZ  2560

/* Function to connect to a remote TCP/IP server. */
/* Static means that is will only be seen in this 1 cpp file */
/* it forces the function to be local */
static SOCKET connectTCP( char *host, char *service );

#pragma comment(lib, "Ws2_32.lib")

/**/
/*
NAME

    client - to make a request of a server process and report the response.

SYNOPSIS
    /* [optional]: can fill in client and the host name or client and */
    /* or you could just write client
    client [host [service]]

DESCRIPTION

    The client process will make a connection to a remote host.  It assumes
    that the host will respond to the connection by sending a single
    response and close the connection.  This response will be displayed.
    "host" is the name or the address of the remote host.  "service" is the
    name or port number of the service.  If the service is not given it
    defaults to "daytime" which returns the current time.  If the remote
    host is not given, it defaults to "phobos".

    Note: I did leave in some printf's in order to remind you of C functions.

    NOTE: THIS CODE DOES NOT REPORT ERRORS CORRECTLY.  WHY????????????????????????????????????????
*/
/**/
int main( int argc, char *argv[] )
{
    // argc = 3
    // argv: would be an array of pointers as a string
    // Make sure that we can connect:
    // First arguement is a word
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    // A macro to create a word with low order and high order bytes as specified.
    wVersionRequested = MAKEWORD(1, 1);  

    // Test that version 1.1 is available.
    // Passing in the version request and expect to get back from 
    // them our structure
    err = WSAStartup(wVersionRequested, &wsaData);

    if(err != 0) {

        // Tell the user that we couldn't find a useable winsock.dll. */
        cerr << "Could not find useable DLL" << endl;
        return 1;
    }

    // Confirm that the Windows Sockets DLL supports version 1.1.
    // Note that if the DLL supports versions greater than 1.1 in addition 
    //  to 1.1, it will still return1.1 in wVersion since that is the version we
    // requested.  Note the use of macros.  Further not that this check is not 
    // really needed.

    if( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ) {

        // Tell the user that we couldn't find a useable winsock DLL. 
        WSACleanup();  // Terminates the use of the 
        cerr << "Could not find useable DLL" << endl;
        return 1;
    }
    
    char *host = "cs.ramapo.edu";    /* The name of the remote host. */
    char *service = "daytime";/* The service to be accessed. */
    SOCKET s;       /* The socket for the connection to the remote host. */
    int nb;         /* The number of bytes read from a single read. */
    int tnb;        /* The total number of bytes read. */
    char buff[LINESZ + 1];  /* Response data from the server. */
    
    
    /* Fill in the remote host name and service name from the run-time
     * parameters. */
    switch( argc ) {

    /* If there are no run-time parameters the default values will be
     * used. */
    case 1:
        break;

    /* Record the host name. */
    case 2:
        host = argv[1];
        break;

    /* If there are two run-time parameters, record the service name
     * and the host name. */
    case 3:
        host = argv[1];
        service = argv[2];
        break;

    /* If there are more than 2 run-time parameters, the user is in
     * error. */
    default:
        //it will display an error to the screen
        //displays to the screen when if you try to diplay it
        //to the file
        //**This is really a c call**
        fprintf( stderr, "Usage: client [host [service]]\n" );
        return 1;
    }
    /* Connect to the server process. */
    s = connectTCP( host, service );

    /* Read the response from the server.  Recall that the response is
     * stream oriented and may take more than one read.  (not likely) 
     * A zero length read implies that the server has broken the
     * connection. */
    tnb = 0;
    while( (nb = recv( s, &buff[tnb], LINESZ - tnb, 0 )) > 0 ) {
        tnb += nb;
    }
    /* If there was an error on the read, report it. */
    if( nb < 0 ) {
        perror( "read" );
        return 1;
    }
    /* Make the response NULL terminated and display it.  Using C output */
    printf( "tnb = %d\n", tnb );
    buff[tnb] = '\0';
    puts( buff ) ;

    return 0;
}
/**/
/*
NAME

    connectTCP - to make a connection to a TCP/IP server.

SYNOPSIS
*/
static SOCKET connectTCP( char *host, char *service )
/*
DESCRIPTION

    The "connectTCP" function will connect this client to a TCP/IP server.
    "host" is the name or IP address of the system on which the server
    resides.  "service" is the name or port number of the service to be
    accessed.

RETURNS

    This function returns the file descriptor of the connected socket
    if it is successful and does not return if it fails.  The error
    is displayed in the latter case.
*/
/**/
{
    struct hostent *he; /* Information about the remote host. */
    struct servent *se; /* Information about the requested service. */
    struct sockaddr_in sin; /* Internet end point address.  How do I find the format of this struct? */
    SOCKET s;              /* File descriptor for the connected socket. */


    /* Create an endpoint address for the server.   Structure for address
     * found in: netinet/in.h */
    memset( (char *) &sin, 0, sizeof(sin) ); /* sets the address to zero. */
    sin.sin_family = AF_INET;  /* Record the fact that we are using the
                                * internet family of addresses. */

    /* Get the portnumber.  Note: services found in /etc/services. */
    if( (se = getservbyname( service, "tcp" )) != NULL ) {
        sin.sin_port = se->s_port;
        printf( "port No. %ld\n", (long int) ntohs(se->s_port) );

    } else if( (sin.sin_port = htons( (short int) atoi( service ) ))
      == 0 ) {
        perror( "Can't get the port number of the service." );
        exit( 1 );
    }

    /* Get the address of the remote host.  The host's name is looked up
     * on a name server. */
    if( (he = gethostbyname( host )) != NULL ) {
        memcpy(  (char *) &sin.sin_addr,he->h_addr, he->h_length );

    } else if( (sin.sin_addr.s_addr = inet_addr( host )) == INADDR_NONE ) {
        perror( "Invalid host address" );
        exit( 1 );

    }
    // Display the IP address in hexadecimal.  Let's convert it to dotted 
    // notation in class.
    printf( "IP address = 0x%x\n", sin.sin_addr.s_addr );

    /* Allocate a socket from which the connection to the server will be
     * be made.  Note: by setting the third argument to 0, means that we
     * are using the default protocol (since there is usually one choice,
     * 0 seems like a good choice for the argument. */
    if( (s = socket(PF_INET, SOCK_STREAM, 0 )) < 0 ) {
        perror( "Could not allocate socket." );
        exit( 1 );
    }
    /* Connect to server. */
    if( connect( s, (struct sockaddr *) &sin, sizeof(sin) ) < 0 ) {
        perror( "Attempt to connect to server failed." );
        exit( 1 );
    }
    /* Return the file descriptor for the connected socket. */
    return( s );
}
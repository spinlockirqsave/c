
/// @file:   sossrv.c
/// @author: piter cf16 eu
/// @date Apr 21, 2015, 08:08 PM

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include <linux/list.h>

#define MAXLINE 0x400
#define LISTENQ 10


void usage(const char *name)
{
    if(!name) return;
    fprintf( stderr, "\nusage:\t %s <command> [-p tcp_port]\n",name);
    fprintf( stderr, "     \t -p <tcp_port>     :  tcp port on which to listen\n");
    fprintf( stderr, "\nPossible options:\n");
    fprintf( stderr, "      START       : START <-p tcp_port>\n");
    fprintf( stderr, "      STOP        : STOP\n");
    fprintf( stderr, "\n");
}

void
sig_chld(int signo)
{
	pid_t		pid;
	int		stat;

	while ((pid = waitpid( -1, &stat, WNOHANG)) > 0) {
		printf( "child %d terminated\n", pid);
	}
        return;
}

void
sig_int(int signo)
{
	fprintf(stderr,"SIGINT received with status %d.\n",signo);
	fprintf(stderr,"Exiting....\n");
	exit(0);
}

static int pos(int l, int a)
{
    return 0;
}

static int sos(void)
{
    return 0;
}
 
void
do_it_all(int sockfd)
{
	char		line[MAXLINE],tmp;
	ssize_t		n=0,bytes=0;
        int 		errsv;
        sleep(7);
	for ( ;bytes<MAXLINE; ) {
                if((n = read(sockfd,&tmp,1)) < 0)
                { 
                    errsv = errno;
	            fprintf(stderr,"Error reading socket. %s\n",strerror(errsv));
                    exit(EXIT_FAILURE);
                }
                if(n==0)
                {
                    fprintf(stderr,"End of message.\n");
                    return;
                }
                if(tmp != 0x2)
	        {
                    fprintf(stderr,"Bad message encoding: %c.\n",tmp);
                    continue;
                }
                do {
		    if ( ( n = read(sockfd, line + bytes, 1)) == 0)
		    {
                        errsv = errno;
	                fprintf(stderr,"Connection closed by peer. %s\n",strerror(errsv));
                        return;		// connection closed by other end
                    }
		    if (n < 0)
		    {
                        errsv = errno;
	                fprintf(stderr,"Error reading socket. %s\n",strerror(errsv));
                        return;
                    }
                    if(line[bytes] == 0x3) break;
                    ++bytes;
                } while(1);
                // process the message
                switch(line[0])
                {
                    case '0':
                        fprintf(stderr,"POS\n");
                        return;
                    case '1':
                        fprintf(stderr,"SOS\n");
                        return;
                    default:
                        fprintf(stderr,"Unknown message code\n");
                        return;
                }
	}
}

/*
 * 
 */
int
main(int argc, char **argv)
{
    int				listenfd, connfd;
    pid_t			childpid;
    socklen_t			clilen;
    struct sockaddr_in          cliaddr, srvaddr;
    unsigned int		tcpport;
    char			*pCh;
    int 			c,idx,errsv;

    while((c = getopt(argc,argv,"p:"))!=-1)
    {
        switch (c)
        {
            case 'p':
                tcpport = strtoul(optarg,&pCh,10);
                if((pCh == argv[3]) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                if(optopt == 'p')
                    fprintf(stderr,"Option -%c requires argument.\n",optopt);
                else if (isprint(optopt))
                    fprintf(stderr,"Unknown option '-%c'.\n",optopt);
                else {
                    fprintf(stderr,"Unknown option character '\\x%x'.\n",optopt);
                }
            default:
                usage(argv[0]);
                exit(EXIT_FAILURE);
         }
    }
    // parse nonoption arguments
    for(idx=optind;idx<argc;++idx)
        fprintf(stderr,"Non-option argument %s\n",argv[idx]);
    if((argc - optind) != 1)
    {
        fprintf(stderr,"Invalid number of non-option arguments. There should be exactly "
                "1 argument (START/STOP) but %d given.\n",argc-optind);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if(strcmp(argv[optind],"START")==0)
    {
        // start to listen
        if((listenfd = socket( AF_INET, SOCK_STREAM, 0))<0)
        {
            errsv = errno;
            fprintf(stderr,"Can't create the socket. %s\n",strerror(errsv));
            exit(EXIT_FAILURE);
        }   
	memset( &srvaddr, 0, sizeof(srvaddr));
	srvaddr.sin_family      = AF_INET;
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvaddr.sin_port        = htons(tcpport);

        /* SO_REUSEADDR allows a new server to be started
         * on the same port as an existing server that is
         * bound to the wildcard address, as long as each
         * instance binds a different local IP address.
         * This is common for a site hosting multiple HTTP
         * servers using the IP alias technique */
        int reuseaddr_on = 1;
        if( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR,
                &reuseaddr_on, sizeof( reuseaddr_on)) < 0)
        {
            errsv = errno;
            fprintf(stderr,"Can't so_reuseaddr. %s\n",strerror(errsv));
            exit(EXIT_FAILURE);
        }
        if(bind(listenfd,(struct sockaddr*)&srvaddr,sizeof(srvaddr))<0)
        {
            errsv = errno;
            fprintf(stderr,"Can't bind socket. %s\n",strerror(errsv));
            exit(EXIT_FAILURE);
        }
        // LISTENQ specifies the length of the queue for pending connections.
        // When the queue fills, new clients attempting to connect fail with
        // ECONNREFUSED error until the server calls accept to accept new
        // connections from the queue
	if(listen( listenfd, LISTENQ) < 0)
        {
            errsv = errno;
            fprintf(stderr,"Can't listen on socket. %s\n",strerror(errsv));
            exit(EXIT_FAILURE);
        }
	if(signal( SIGCHLD, sig_chld) == SIG_ERR)
        {
            errsv = errno;
            fprintf(stderr,"Can't set the SIGCHLD handler. %s\n",strerror(errsv));
            exit(EXIT_FAILURE);
        }
	if(signal( SIGINT, sig_int) == SIG_ERR)
        {
            errsv = errno;
            fprintf(stderr,"Can't set the SIGINT handler. %s\n",strerror(errsv));
            exit(EXIT_FAILURE);
        }
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		if ( ( connfd = accept( listenfd, (struct sockaddr*) &cliaddr, &clilen)) < 0) {
			if ( errno == EINTR)
				continue;		// try again
			else {
                                errsv = errno;
				fprintf(stderr, "Accept error. %s",strerror(errsv));
                                continue;              // try again
                        }
		}

		if ( ( childpid = fork()) == 0) {	// child process
			close( listenfd);               // close listening socket
			do_it_all( connfd);              // process the request
			exit(EXIT_SUCCESS);
		}
		close( connfd);			// parent closes connected socket
	}
    }
}


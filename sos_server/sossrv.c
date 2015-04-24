
/// @file:   sossrv.c
/// @author: peter cf16 eu
/// @date Apr 21, 2015, 08:08 PM

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>

#include "sos_common.h"


#define MAXLINE 		0x400
#define LISTENQ 		10
#define MAXNAMELENGTH 		100
#define THREADSMAX 		4

struct sos_unit
{
    int 			id;
    int 			fd;
    pthread_t 			thread;
};

int 				threads_n;
struct sos_unit 		sos_threads[THREADSMAX];

struct sos_link
{
    struct sos_link		*next,*prev;
    void			*data;
};

struct sos_link			*ships = NULL;

struct sos_link* sos_link_create()
{
    // init list of ships
    struct sos_link *link = malloc(sizeof(*link));
    memset(link,0,sizeof(*link));
    return link;
}

struct sos_link* sos_link_add(struct sos_link* new_link)
{
    // init list of ships
    if(!new_link) return NULL;
    if(!ships)
    {
        // add as root
        new_link->next = new_link->prev = NULL;
        ships = new_link;
        return ships;
    }
    // add at the end
    struct sos_link *link = ships;
    while(link->next)
        ++link;
    link->next = sos_link_create();
    link->next->prev = link->next;
    return link->next;
}

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

static int pos( struct sos_ship ship)
{
    // update position
    return 0;
}

static int sos(void)
{
    return 0;
}
 
void*
do_it_all(void *data)
{
	char				line[MAXLINE],tmp;
	int				n=0,bytes=0;
        int 				errsv,msg_sz;
        struct sos_ship			ship_desc;
        struct sos_unit			*unit = data;
        fprintf(stderr,"Thread with id %d created & running.\n",unit->id);
	for ( ;bytes<MAXLINE; ) {
                if((n = read(unit->fd,&tmp,1)) < 0)
                { 
                    errsv = errno;
	            fprintf(stderr,"Error reading socket. %s\n",strerror(errsv));
                    fprintf(stderr,"Thread with id %d exiting. Cleaning up...\n",unit->id);
                    unit->id = -1;
                    --threads_n;
                    exit(EXIT_FAILURE);
                }
                if(n==0)
                {
                    fprintf(stderr,"End of message.\n");
                    fprintf(stderr,"Thread with id %d exiting. Cleaning up...\n",unit->id);
                    unit->id = -1;
                    --threads_n;
                    return;
                }
                if(tmp != 0x2)
	        {
                    fprintf(stderr,"Bad message encoding: %c.\n",tmp);
                    fprintf(stderr,"Thread with id %d exiting. Cleaning up...\n",unit->id);
                    unit->id = -1;
                    --threads_n;
                    continue;
                }
                fprintf(stderr,"Found message begin...\n");
                read(unit->fd,&msg_sz,sizeof msg_sz); 
                do {
		    if ( ( n = read(unit->fd, line + bytes, msg_sz - bytes)) == 0)
		    {
                        errsv = errno;
	                fprintf(stderr,"Connection closed by peer. %s\n",strerror(errsv));
                        fprintf(stderr,"But message terminator hasn't been found yet\n");
                        fprintf(stderr,"Thread with id %d exiting. Cleaning up...\n",unit->id);
                        unit->id = -1;
                        --threads_n;
                        return;		// connection closed by other end
                    }
		    if (n < 0)
		    {
                        errsv = errno;
	                fprintf(stderr,"Error reading socket. %s\n",strerror(errsv));
                        fprintf(stderr,"Thread with id %d exiting. Cleaning up...\n",unit->id);
                        unit->id = -1;
                        --threads_n;
                        return;
                    }
                    bytes += n;
                } while(bytes<msg_sz);

                read(unit->fd,&tmp,1);
                if(tmp == 0x3) {
                    fprintf(stderr,"Found message end...\n");
		} else {
                    fprintf(stderr,"Bad message terminator %c.\n",tmp);
                    exit(EXIT_FAILURE);
                }
                // process the message
                n = SOS_MIN(sizeof ship_desc,sizeof line);
                memcpy((void*)&ship_desc,line,n);
                // parse
                int 				found = 0;
                switch(ship_desc.cmd)
                {
                    case '0':
                        fprintf(stderr,"POS\n");
                        struct sos_link *ship;
                        struct sos_ship *shipp;
                        // search
                        if(!ships)
                        {
                            // add as root
                            ships = sos_link_create();
                            ships->data = malloc(sizeof(struct sos_ship));
                            memcpy(ships->data,&ship_desc,sizeof(struct sos_ship));
                        } else {
                            // search
                            ship = ships;
                            do {
                                shipp = (struct sos_ship*) ship->data;
                                if(strcmp(shipp->name,ship_desc.name)==0)
                                {
                                    // found
                                    found = 1;
                                    break;
                                }   
                            } while(ship->next);
                            if(found)
                            {
                                // update ship
                                shipp->longitude = ship_desc.longitude;
                                shipp->latitude = ship_desc.latitude;
                                shipp->signal = ship_desc.signal;
                                shipp->speed = ship_desc.speed;
                                
                            } else {
                                // insert
                                struct sos_link *new_link = sos_link_create();
                                struct sos_ship *new_ship = malloc(sizeof(*new_ship));
                                memset(new_ship,0,sizeof(*new_ship));
                                new_ship->longitude = ship_desc.longitude;
                                new_ship->latitude = ship_desc.latitude;
                                new_ship->signal = ship_desc.signal;
                                new_ship->speed = ship_desc.speed;
                                strcpy(new_ship->name, ship_desc.name);
                                new_link->data = new_ship;
                                sos_link_add(new_link);
                            }
                        }
                        unit->id = -1;
                        --threads_n;
                        fprintf(stderr,"Thread with id %d exiting...\n",unit->id);
                        return;
                    case '1':
                        fprintf(stderr,"SOS\n");
                        unit->id = -1;
                        --threads_n;
                        fprintf(stderr,"Thread with id %d exiting...\n",unit->id);
                        return;
                    default:
                        fprintf(stderr,"Unknown message code\n");
                        unit->id = -1;
                        --threads_n;
                        fprintf(stderr,"Thread with id %d exiting...\n",unit->id);
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
        // init threads
        int i = 0;
        for( ; i < THREADSMAX; ++i)
        {
            sos_threads[i].id = -1;
            //sos_threads[i].thread = NULL;   
        }
        // init list of ships
        // ships = malloc(sizeof(*ships));
        // memset(ships,0,sizeof(*ships));
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

		if (threads_n < THREADSMAX)
                {
                    // find first freed unit
                    i = 0;
                    while(i<THREADSMAX)
                    {
                        if(sos_threads[i].id == -1) break;
                        ++i;
                    }
                    sos_threads[i].id = threads_n;
                    sos_threads[i].fd = connfd;
                    // create thread & process data
                    if((errsv = pthread_create(&sos_threads[i].thread, NULL, do_it_all, (void*)&sos_threads[i])) < 0)
                    {
			errsv = errno;
                        fprintf(stderr, "Can't create the thread. %s\n",strerror(errsv));
                        close(connfd);
		    }
	            fprintf(stderr, "Thread with id %d created.\n",threads_n);
                    ++threads_n;
	        } else {
	            fprintf(stderr, "Max number of threads [%d] exceeded. Try again.\n",threads_n);
                }
       }
       return 0;
    } else {
        fprintf(stderr,"Invalid nonoption argument %s\n",argv[optind]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

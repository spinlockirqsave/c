
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
#include <math.h>

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
    double			distance;
};

struct sos_link			*ships = NULL, *ships_sorted = NULL;
int				ships_sz;
static sigset_t			signal_mask;

struct sos_link* sos_link_create()
{
    // init list of ships
    struct sos_link *link = malloc(sizeof(*link));
    memset(link,0,sizeof(*link));
    link->distance = -1;
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
        link = link->next;
    new_link->next = NULL;
    new_link->prev = link;
    link->next = new_link;
    return link->next;
}

void sos_link_free(struct sos_link *link)
{
    if (!link) return;
    free(link->data);
    free(link);
    --ships_sz;
}

void sos_link_free_adjust(struct sos_link *link)
{
    if (!link) return;
    // adjust the list
    struct sos_link *left = link->prev;
    struct sos_link *right = link->next;
    if(left)
        if(right)
            left->next = right;
        else
            left->next = NULL;
    else
        if(right)
            right->prev = NULL;
    free(link->data);
    free(link);
    --ships_sz;
}

void ships_free()
{
    if(ships_sz)
    {
        struct sos_link *link_n;
        struct sos_link *link = ships;
        struct sos_ship *ship;
        do {
            link_n = link->next;
            sos_link_free(link);
            link = link_n;
        } while(link);
    }
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

void ships_print()
{
        fprintf(stderr,"\nMessage list size: %d.\n", ships_sz);
        if(ships_sz)
        {
            fprintf(stderr,"Content:\n");
            struct sos_link *shipl = ships;
            struct sos_ship *ship;
            do {
                ship = shipl->data;
                fprintf(stderr,"Name: %-20s, signal: %*d, longitude: %*lf, latitude %*lf, speed: %*lf, distance: %*lf\n",
                         ship->name, 10,ship->signal, 10,ship->longitude, 10,ship->latitude, 10,ship->speed, 10,shipl->distance);
            } while ( shipl = shipl->next);
        }
}

void ships_print_sorted()
{
        if(!ships_sorted) return;
        fprintf(stderr,"\nMessage list size: %d.\n", ships_sz);
        if(ships_sz)
        {
            fprintf(stderr,"Content:\n");
            struct sos_link *shipl = ships_sorted;
            struct sos_ship *ship;
            do {
                ship = shipl->data;
                fprintf(stderr,"Name: %-20s, signal: %*d, longitude: %*lf, latitude %*lf, speed: %*lf, distance: %*lf\n",
                         ship->name, 10,ship->signal, 10,ship->longitude, 10,ship->latitude, 10,ship->speed, 10,shipl->distance);
            } while ( shipl = shipl->next);
        }
}

void
sig_int(int signo)
{
	fprintf(stderr,"SIGINT received with status %d.\n",signo);
	if(ships_sorted)
        {
            fprintf(stderr,"\nSorted:\n");
            ships_print_sorted();
        }
        else {
            fprintf(stderr,"\nAs is:\n");
            ships_print();
        }
        fprintf(stderr,"\nFreeing resources....\n");
        ships_free();
        fprintf(stderr,"\nOK. Exiting....\n");
	exit(0);
}

static int pos(struct sos_ship ship_desc)
{
    // insert/update entry
    struct sos_link *ship;
    struct sos_ship *shipp;
    int found = 0;
    // search
    if(!ships)
    {
        // add as root
        ships = sos_link_create();
        ships->data = malloc(sizeof(struct sos_ship));
        memcpy(ships->data,&ship_desc,sizeof(struct sos_ship));
        ++ships_sz;
        fprintf(stderr,"Added as root. Number of ships: %d\n",ships_sz);
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
        } while(ship = ship->next);
            if(found)
            {
                // update ship
                shipp->longitude = ship_desc.longitude;
                shipp->latitude = ship_desc.latitude;
                shipp->signal = ship_desc.signal;
                shipp->speed = ship_desc.speed;
                fprintf(stderr,"Updated. Number of ships: %d\n",ships_sz);     
            } else {
                // insert
                struct sos_link *new_link = sos_link_create();
                struct sos_ship *new_ship = malloc(sizeof(*new_ship));
                memset(new_ship,0,sizeof(*new_ship));
                memcpy(new_ship,&ship_desc,sizeof(struct sos_ship));
                new_link->data = new_ship;
                sos_link_add(new_link);
                ++ships_sz;
                fprintf(stderr,"Added. Number of ships: %d\n",ships_sz);
            }
        }
    return 0;
}

static void sos_insert_sorted(struct sos_link *shiplink)
{
    if(ships_sz)
    {
        struct sos_link *shipl = ships_sorted, *left, *right, *new_link;
        struct sos_ship *ship;
        do {
            if(!ships_sorted)
            {
                // insert as root
                ships_sorted = sos_link_create(); // inserted
                ships_sorted->data = shiplink->data;
                ships_sorted->distance = shiplink->distance;
                return;
            }
            ship = shipl->data;
            if(shiplink->distance <= shipl->distance)
            {
                // insert before shipl
                new_link = sos_link_create(); // inserted
                new_link->data = shiplink->data;
                new_link->distance = shiplink->distance;
                new_link->next = shipl; 
                if(left = shipl->prev)
                {
                    left->next = new_link;
                    new_link->prev = left;
                    return;
                }
                // shipl is root
                shipl->prev = ships_sorted = new_link;
                return;
            }
            if(!shipl->next)
            {
                // insert on tail
                new_link = sos_link_create(); // inserted
                new_link->data = shiplink->data;
                new_link->distance = shiplink->distance;
                shipl->next = new_link;
                new_link->prev = shipl;
                return;
            }
        } while ( shipl = shipl->next);
    }
}
 
static int sos(struct sos_ship ship_desc)
{
    // in decimal degrees
    double fi1,fi2,dfi,dlambda,a,c,R;
    R = 6371; // kilometers 
    // calculate distances
    if(ships_sz)
    {
        fprintf(stderr,"\nComputing distances...\n");
        struct sos_link *shipl = ships;
        struct sos_ship *ship;
        do {
            ship = shipl->data;
            fi1 = SOS_DEG_TO_RADIANS(ship_desc.longitude);
            fi2 = SOS_DEG_TO_RADIANS(ship->longitude);
            dfi = SOS_DEG_TO_RADIANS(ship->latitude - ship_desc.latitude);
            dlambda = SOS_DEG_TO_RADIANS(ship->longitude - ship_desc.longitude);
            a = sin(dfi/2.0)*sin(dfi/2.0) + cos(fi1)*cos(fi2)*sin(dlambda/2.0)*sin(dlambda/2.0);
            c = 2*atan2(sqrt(a),sqrt(1-a));
              
            shipl->distance = R * c;
            fprintf(stderr,"Name: %-20s, signal: %*d, longitude: %*lf, latitude %*lf, speed: %*lf, distance: %*lf\n",
                         ship->name, 10, ship->signal, 10,ship->longitude, 10,ship->latitude, 10,ship->speed, 10,shipl->distance);
            sos_insert_sorted(shipl);
        } while ( shipl = shipl->next);
    }
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
        sigemptyset(&signal_mask);
        sigaddset(&signal_mask, SIGINT);
        if((errsv = pthread_sigmask(SIG_BLOCK,&signal_mask,NULL)) != 0)
        {
            fprintf(stderr,"Can't block SIGINT for thread [%d].\n",unit->id);
        }
	for ( ;bytes<MAXLINE; ) {
                if((n = read(unit->fd,&tmp,1)) < 0)
                { 
                    errsv = errno;
	            fprintf(stderr,"Error reading socket. %s\n",strerror(errsv));
                    unit->id = -1;
                    --threads_n;
                    exit(EXIT_FAILURE);
                }
                if(n==0)
                {
                    fprintf(stderr,"End of message.\n");
                    unit->id = -1;
                    --threads_n;
                    return;
                }
                if(tmp != 0x2)
	        {
                    fprintf(stderr,"Bad message encoding: %c.\n",tmp);
                    unit->id = -1;
                    --threads_n;
                    continue;
                }
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
                if(tmp != 0x3) {
                    fprintf(stderr,"Bad message terminator %c.\n",tmp);
                    exit(EXIT_FAILURE);
                }
                // process the message
                n = SOS_MIN(sizeof ship_desc,sizeof line);
                memcpy((void*)&ship_desc,line,n);
                // parse
                switch(ship_desc.cmd)
                {
                    case '0':
                        fprintf(stderr,"\nPOS message received\n");
                        pos(ship_desc);
                        unit->id = -1;
                        --threads_n;
                        return;
                    case '1':
                        fprintf(stderr,"\nSOS message received\n");
                        sos(ship_desc);
                        unit->id = -1;
                        --threads_n;
                        return;
                    default:
                        fprintf(stderr,"\nUnknown message received. Exiting...\n");
                        unit->id = -1;
                        --threads_n;
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
    ships_print();
    if(ships_sz)
    {
        fprintf(stderr,"\nFreeing resources....\n");
        ships_free();
        fprintf(stderr,"\nOK\n");
    }
    fprintf(stderr,"Exiting....\n");
    return 0;
}


/// @file:   soscli.c
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


#define MAXNAMELENGTH 		100
#define MAXLINE 		0x400


void usage(const char *name)
{
    if(!name) return;
    fprintf( stderr, "\nusage:\t %s <call_id> <-i ip -p port -s signal [-n name -l longitude -a latitude -v speed]>\n",name);
    fprintf( stderr, "     \t call_id           :  POS,SOS\n");
    fprintf( stderr, "     \t -i <ip>           :  IPv4 server address\n");
    fprintf( stderr, "     \t -p <port>         :  server port\n");
    fprintf( stderr, "     \t -n <name>         :  name of the ship\n");
    fprintf( stderr, "     \t -s <signal>       :  signal\n");
    fprintf( stderr, "     \t -l <longtude>     :  longitude position\n");
    fprintf( stderr, "     \t -a <lattitude>    :  latitude position\n");
    fprintf( stderr, "     \t -v <speed>        :  speed\n");
    fprintf( stderr, "\nPossible options:\n");
    fprintf( stderr, "      POS       : POS <-i ip -p port -n name -s signal -l longitude -a latitude -v speed>\n");
    fprintf( stderr, "      SOS       : SOS <-i ip -p port -n name -s signal -l longitude -a latitude -v speed>\n");
    fprintf( stderr, "\n");
}

void
do_it_all(int sockfd, struct sos_ship ship)
{
	long		arg1, arg2;
	char		c, line[MAXLINE];
        int 	 	bytes = 0, n = 0, msg_sz = 0, errsv;

    switch(ship.cmd)
    {
        case '0':
            // POS
            c = 0x02;
            send(sockfd,&c,1,0);
            n = sizeof ship;
            send(sockfd,&n,sizeof n,0);
            while( bytes<n)
            {
                bytes += send(sockfd,&ship,n-bytes,0);
            }
            c = 0x03;
            send(sockfd,&c,1,0);
            fprintf(stderr, "POS message sent\n");
            break;

        case '1':
            // SOS
            c = 0x02;
            send(sockfd,&c,1,0);
            n = sizeof ship;
            send(sockfd,&n,sizeof n,0);
            while( bytes<n)
            {
                bytes += send(sockfd,&ship,n-bytes,0);
            }
            c = 0x03;
            send(sockfd,&c,1,0);
            fprintf(stderr, "SOS message sent\n");
            bytes = 0;
            read(sockfd,&msg_sz,sizeof msg_sz);
            fprintf(stderr, "Awaiting for list of ships [%d bytes]...\n",msg_sz);
                do {
		    if ( ( n = read(sockfd, line + bytes, msg_sz - bytes)) == 0)
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
                    bytes += n;
                } while(bytes<msg_sz);
            fprintf(stderr, "OK. %d bytes received\n",msg_sz);
            fprintf(stderr,"Content:\n");
            struct sos_ship *ship = (struct sos_ship*)line;
            double *distance;
            while((char*)ship < line + bytes)
            {
                fprintf(stderr,"Name: %-20s, signal: %*d, longitude: %*lf, latitude %*lf, speed: %*lf",
                         ship->name, 4,ship->signal, 12,ship->longitude, 12,ship->latitude, 12,ship->speed);
                distance = (double*)(ship + 1);
                fprintf(stderr,", distance: %*lf\n",12,*distance);
                ship = (struct sos_ship*)(distance + 1);
            }
            break;

        default:
            fprintf(stderr, "[do_it_all] Bad message code\n");
            exit(EXIT_FAILURE);
    }
}

/*
 * 
 */
int
main(int argc, char **argv)
{
    int				sockfd;
    struct sockaddr_in          servaddr;
    char			*srvip;
    unsigned int 		srvport;

    char			*pCh;
    int 			c,idx,errsv;
    struct sos_ship		ship;
    memset(&ship,0,sizeof ship);
    ship.signal = -1;
    ship.speed = -1.0;

    while((c = getopt(argc,argv,"i:p:n:s:l:a:v:"))!=-1)
    {
        switch (c)
        {
            case 'i':
                srvip = optarg;
                break;

            case 'p':
                srvport = strtoul(optarg,&pCh,10);
                if((pCh == optarg) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid 'p' argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'n':
                strcpy(ship.name, optarg);
                break;

            case 's':
                ship.signal = strtol(optarg,&pCh,10);
                if((pCh == optarg) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid 's' argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'l':
                ship.longitude = atof(optarg);
                break;
            case 'a':
                ship.latitude = atof(optarg);
                break;
            case 'v':
                ship.speed = atof(optarg);
                break;
            case '?':
                if(optopt == 'n' || optopt == 's' || optopt == 'l' || optopt == 'a' || optopt == 'v')
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
        fprintf(stderr,"\nInvalid number of non-option arguments. There should be exactly "
                "1 argument (POS/SOS) but %d given.\n",argc-optind);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if(strcmp(argv[optind],"POS")==0)
    {
        ship.cmd = '0';
        if(srvip == 0 || srvport == 0 || strlen(ship.name)==0 || ship.signal == -1 || ship.speed < 0)
        {
            fprintf(stderr,"Bad option:s given. POS command requires ip,port,name,signal,longitude,latitude and speed.\n");
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    } else if (strcmp(argv[optind],"SOS")==0) {
        ship.cmd = '1';
        if(srvip == 0 || srvport == 0 || strlen(ship.name)==0 || ship.signal == -1)
        {
            fprintf(stderr,"Bad option:s given. SOS command requires ip,port,name and signal.\n");
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr,"Bad cmd option: %s. Valid options are POS/SOS.\n",argv[optind]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }     

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( srvport);
	inet_pton(AF_INET, srvip, &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
        do_it_all(sockfd,ship);
}


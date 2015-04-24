
/// @file:   sossrv.c
/// @author: peter cf16 eu
/// @date Apr 21, 2015, 08:08 PM


void usage(const char *name)
{
    if(!name) return;
    fprintf( stderr, "\nusage:\t %s <call_number> <-s signal [-n name -l longitude -a latitude -v speed]>\n",name);
    fprintf( stderr, "     \t -n <name>         :  name of the ship\n");
    fprintf( stderr, "     \t -s <signal>       :  signal\n");
    fprintf( stderr, "     \t -l <longtude>     :  longitude position\n");
    fprintf( stderr, "     \t -a <lattitude>    :  latitude position\n");
    fprintf( stderr, "     \t -v <speed>        :  speed\n");
    fprintf( stderr, "\nPossible options:\n");
    fprintf( stderr, "      POS       : 100 <-n name -s signal -l longitude -a latitude -v speed>\n");
    fprintf( stderr, "      SOS       : 200 <-s signal>\n");
    fprintf( stderr, "\n");
}


struct sos_ship
{
    char			name[MAXNAMELENGTH];
    int				signal;
    long int			longitude;
    long int			latitude;
    double			speed;
    char			cmd;
};

void
do_it_all(int sockfd)
{
	long		arg1, arg2;
	ssize_t		n;
	char		line[MAXLINE];

    switch(ship.c)
    {
        case '0':
            // POS
            break;

        case '1':
            // SOS
            break;

        default:
            fprintf(stderr, "[do_it_all] Bad message code\n");
            exit(EXIT_FAILURE);
    }
	for ( ; ; ) {
		if ( ( n = Readline(sockfd, line, MAXLINE)) == 0)
			return;		/* connection closed by other end */

		if ( sscanf( line, "%ld%ld", &arg1, &arg2) == 2)
			snprintf( line, sizeof(line), "%ld\n", arg1 + arg2);
		else
			snprintf( line, sizeof(line), "input error\n");

		n = strlen( line);
		Writen( sockfd, line, n);
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
    struct sockaddr_in          cliaddr, servaddr;

    char			*pCh;
    int 			c,idx,errsv;
    struct sos_ship		ship;

    while((c = getopt(argc,argv,"n:s:l:a:v:"))!=-1)
    {
        switch (c)
        {
            case 's':
                ship.signal = strtol(optarg,&pCh,10);
                if((pCh == optarg) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;

            case 's':
                ship.signal = strtol(optarg,&pCh,10);
                if((pCh == optarg) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'l':
                ship.longitude = strtoul(optarg,&pCh,10);
                if((pCh == optarg) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'a':
                ship.latitude = strtoul(optarg,&pCh,10);
                if((pCh == optarg) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'v':
                ship.speed = strtol(optarg,&pCh,10);
                if((pCh == optarg) || (*pCh != '\0'))
                {
                    fprintf(stderr,"Invalid argument\n");
                    fprintf(stderr,"Parameter conversion error, nonconvertible part is %s\n",pCh);
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            case '?':
                if(optopt == 'n' || optopt == 's' || optopt == 'l' || optopt == 'a' || optotpt == 'v')
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
                "1 argument (POS/SOS) but %d given.\n",argc-optind);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    if(strcmp(argv[optind],"POS")==0)

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
}


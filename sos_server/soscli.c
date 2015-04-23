
/// @file:   sossrv.c
/// @author: piter cf16 eu
/// @date Apr 21, 2015, 08:08 PM


void usage(const char *name)
{
    if(!name) return;
    fprintf( stderr, "\nusage:\t %s <call_number> <-si signal [-n name -l longitude -la latitude -sp speed]>\n",name);
    fprintf( stderr, "     \t -n <name>         :  name of the ship\n");
    fprintf( stderr, "     \t -si <signal>      :  signal\n");
    fprintf( stderr, "     \t -lo <longtude>    :  longitude position\n");
    fprintf( stderr, "     \t -la <lattitude>   :  latitude position\n");
    fprintf( stderr, "     \t -sp <speed>       :  speed\n");
    fprintf( stderr, "\nPossible options:\n");
    fprintf( stderr, "      POS       : 100 <-n name -si signal -l longitude -la latitude -sp speed>\n");
    fprintf( stderr, "      SOS       : 200 <-si signal>\n");
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
do_it_all(int sockfd)
{
	long		arg1, arg2;
	ssize_t		n;
	char		line[MAXLINE];

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
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in              cliaddr, servaddr;

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons( SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
}


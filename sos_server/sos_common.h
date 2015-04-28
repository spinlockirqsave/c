
/// @file:   sos_common.h
/// @author: peter cf16 eu
/// @date Apr 21, 2015, 08:08 PM


#define MAXNAMELENGTH 		100
#define MAXLINE 		0x400


#define SOS_MIN(a,b) ((a) < (b) ? (a) : (b))
#define SOS_DEG_TO_RADIANS(deg) ((deg) * M_PI / 180.0)

struct sos_ship
{
    char			name[MAXNAMELENGTH];
    int				signal;
    // in decimal degrees
    double			longitude;
    double			latitude;
    double			speed;
    char			cmd;
};

struct sos_link
{
    struct sos_link		*next,*prev;
    void			*data;
    double			distance;
};

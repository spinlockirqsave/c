#include <linux/module.h>
#include <linux/init.h>
#include <linux/in.h>
#include <net/sock.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/inet.h>

#define SERVER_PORT 5555
static struct socket *udpsocket1=NULL, *udpsocket2=NULL;
static struct socket *clientsocket=NULL;

static DECLARE_COMPLETION( threadcomplete );
struct workqueue_struct *wq1;

struct wq_wrapper{                          // istream
        struct work_struct worker;
	struct sock * sk;
};

struct wq_wrapper wq_data1, wq_data2;

static void cb_data(struct sock *sk, int bytes){
        if(in_interrupt()) printk("udpsrvcallback: in interrupt2\n");
        if(in_atomic()) printk("udpsrvcallback: in atomic2\n");
	wq_data1.sk = sk;
        // for each reader of this istream queue work on reader's rtp_q
	queue_work(wq1, &wq_data1.worker);
}

//static void cb_data2(struct sock *sk, int bytes){
//        if(in_interrupt()) printk("udpsrvcallback: in interrupt2");
//        if(in_atomic()) printk("udpsrvcallback: in atomic2");
//	wq_data2.sk = sk;
//	queue_work(wq2, &wq_data2.worker);
//}

void send_answer(struct work_struct *data){
        // reader rtp_q, get istream and reader from container_of(data)
        // queue bytes on corresponding kfifo
	struct  wq_wrapper *foo;
	int len;
        if(in_interrupt()) printk("udpsrvcallback: in interrupt3\n");
        if(in_atomic()) printk("udpsrvcallback: in atomic3\n");
	foo = container_of(data, struct  wq_wrapper, worker);
	/* as long as there are messages in the receive queue of this socket*/
	while((len = skb_queue_len(&foo->sk->sk_receive_queue)) > 0){
		struct sk_buff *skb = NULL;
		unsigned short * port;
		int len;
		struct msghdr msg;
		struct iovec iov;
		mm_segment_t oldfs;
		struct sockaddr_in to;

		/* receive packet */
		skb = skb_dequeue(&foo->sk->sk_receive_queue);
		printk("message len: %i message: %s\n", skb->len - 8, skb->data+8); /*8 for udp header*/

		/* generate answer message */
		memset(&to,0, sizeof(to));
		to.sin_family = AF_INET;
		to.sin_addr.s_addr = in_aton("10.31.5.84");  
		port = (unsigned short *)skb->data;
		to.sin_port = *port;
		memset(&msg,0,sizeof(msg));
		msg.msg_name = &to;
		msg.msg_namelen = sizeof(to);
		/* send the message back */
		iov.iov_base = skb->data+8;
		iov.iov_len  = skb->len-8;
		msg.msg_control = NULL;
		msg.msg_controllen = 0;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;
		/* adjust memory boundaries */	
		oldfs = get_fs();
		set_fs(KERNEL_DS);
		len = sock_sendmsg(clientsocket, &msg, skb->len-8);
		set_fs(oldfs);
		/* free the initial skb */
		kfree_skb(skb);
	}
}

static int __init server_init( void )
{
	struct sockaddr_in server1, server2;
	int servererror;
        if(in_interrupt()) printk("udpsrvcallback: in interrupt4\n");
        if(in_atomic()) printk("udpsrvcallback: in atomic4\n");
	printk("INIT MODULE\n");
	/* socket to receive data */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &udpsocket1) < 0) {
		printk( KERN_ERR "server: Error creating udpsocket 1.\n" );
		return -EIO;
	}
	server1.sin_family = AF_INET;
	server1.sin_addr.s_addr = INADDR_ANY;
	server1.sin_port = htons( (unsigned short)SERVER_PORT);
	servererror = udpsocket1->ops->bind(udpsocket1, (struct sockaddr *) &server1, sizeof(server1 ));
	if (servererror) {
		sock_release(udpsocket1);
		return -EIO;
	}
	udpsocket1->sk->sk_data_ready = cb_data;
	
	/* socket 2 to receive data */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &udpsocket2) < 0) {
		printk( KERN_ERR "server: Error creating udpsocket 2.\n" );
		return -EIO;
	}
	server2.sin_family = AF_INET;
	server2.sin_addr.s_addr = INADDR_ANY;
	server2.sin_port = htons( (unsigned short)SERVER_PORT+1);
	servererror = udpsocket2->ops->bind(udpsocket2, (struct sockaddr *) &server2, sizeof(server2 ));
	if (servererror) {
		sock_release(udpsocket2);
		return -EIO;
	}
	udpsocket2->sk->sk_data_ready = cb_data;
	
	/* create work queue */	
	INIT_WORK(&wq_data1.worker, send_answer);
	wq1 = create_singlethread_workqueue("myworkqueue"); 
	if (!wq1){
		return -ENOMEM;
	}
	
	/* socket to send data */
	if (sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &clientsocket) < 0) {
		printk( KERN_ERR "server: Error creating clientsocket.n" );
		return -EIO;
	}
	return 0;
}

static void __exit server_exit( void )
{
	if (udpsocket1)
		sock_release(udpsocket1);
	if (udpsocket2)
		sock_release(udpsocket2);
	if (clientsocket)
		sock_release(clientsocket);

	if (wq1) {
                flush_workqueue(wq1);
                destroy_workqueue(wq1);
	}
	printk("EXIT MODULE");
}

module_init(server_init);
module_exit(server_exit);
MODULE_LICENSE("GPL");

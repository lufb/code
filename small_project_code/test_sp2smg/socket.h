/*socket²Ù×÷¶¨Òå*/
#ifndef _YL_SOCKET_H_
#define _YL_SOCKET_H_

extern int			init_network();
extern void			destroy_network(void);
extern int			create_listen(unsigned short port);
extern int			setnonblocking( int s);
extern int			send_nonblock( int sockfd, char* buf, int size );
extern int			rcv_non_block( int sockfd, char* buf, int size, int timeout = 3);
extern int			select_socket(  const int* pfd,  int* pflag,  int count,  int sec, int usec );
extern int			block_connect(char *ip, unsigned short port);
extern void			set_sock_buf(int sock, size_t sockBufSize);
extern void			re_bind_sock(int sock);

#endif

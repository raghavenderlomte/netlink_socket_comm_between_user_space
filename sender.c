#include <sys/socket.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
struct sockaddr_nl src_addr,dest_addr;
struct iovec iov;
struct msghdr msg;
struct nlmsghdr *nlh;
int ret=0;
#define SOURCE_ID 5
#define DESTINATION_ID 6
int main()
{
	/*socket initialization with netlink family specifications
	  where it returns fd to communicate further 
	  we have to mention the protocol type as zero, if we have
	  to communicate with another user space program*/
	int fd = socket(AF_NETLINK, SOCK_RAW, 0);
	if(fd == -1)
	{
		perror("socket error\n");
		return -1;
	}
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	/*mentioning the id of sender program*/
	  
	src_addr.nl_pid = SOURCE_ID;
	src_addr.nl_groups = 0;
	ret = bind (fd,(struct sockaddr *)&src_addr, sizeof(src_addr));
	if(ret < 0 )
	{
		perror("bind error\n");
		return -1;
	}
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	/*mentioning the id of RECEIVER with whom
	  it has to communicate with */
	dest_addr.nl_pid = DESTINATION_ID;
	dest_addr.nl_groups = 0;
	nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(1024));
	nlh->nlmsg_len = NLMSG_SPACE(1024);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_flags = 0;
	strcpy(NLMSG_DATA(nlh), "Hello i am sending this message from user space");
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name= (void *)&dest_addr;
	msg.msg_namelen=sizeof(dest_addr);
	msg.msg_iov=&iov;
	msg.msg_iovlen = 1;
	ret =sendmsg(fd, &msg, 0);
	if(ret == -1)
	{
		perror("sendmsg error\n");
		return -1;
	}
	close(fd);
	return 0;

}

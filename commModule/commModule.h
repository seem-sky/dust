#ifndef _COMMMDULE_H_
#define _COMMMDULE_H_

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "../tools/tools.h"

/*
 * Error in this module
 * */
enum CommError {
   COMM_SUCCESS = 0,
   COMM_INVALID_LOGICNAME,
   COMM_CREATE_FD_ERROR,
   COMM_CONNECT_FAILED,
};

/*
 * type of logicname
 * */
enum SortOfCommunication {
   TCPCLIENT = 0,
   TCPSERVER,
   UDP,
   SERIAL,
   CAN,
};

/*
 * map of logicName and file descriptors.
 * gLinkMap points to a piece of memory, which have such struct:
 *    type     size           meaning
 *    Mapinfo  4              sum of map  
 *    Map1     4              pointer of logicname1 string
 *             4              type of logicname1
 *             4              pointer of logicname1 link info struct
 *             4              sum of fd is MaxLink1
 *             4 * MaxLink1   
 *    MapN      ……            other map, same as Map1 
 *       
 * */
extern void* gLinkMap;

/*
 * call this function before use other functions in commModule
 * */
extern int initComm(const char* configFilePath);

/*
 * interface of recv data
 * recvBuf and recvLen are all Value-result parameter.
 * */
extern int commRecv(int fd, char* recvBuf, int* recvLen);

/*
 * interface of send data
 * */
extern int commSend(int fd, const char* sendBuf, int sendLen);

/*
 * get all file descriptors associated with logicName.
 * put fd in pFd, and sum of fd in sumFd.
 * */
extern int getAliveLink(const char* logicName, int* sumFd, int** pFd);

extern int getSizeOfGLinkMap(void);

#ifdef TCP_CLIENT_MODE
typedef struct {
   char* destIp;
   int   destPort;
   int   localPort;
}TcpClientInfoObject;
#endif


#endif


/*
 * This file is part of the DFC.
 *
 * Copyright (C) 2012 by bbdlg <shiluo.110@163.com>
 *
 * The DFC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The DFC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DFC. If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef _COMMMDULE_H_
#define _COMMMDULE_H_
/****************************************************************************
 * name:     comm
 * version:  v1.0.0
 * time:     2012-11-26 20:40:54
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * function:
 *    Set parameter in config file, CommModule can read config file and finish 
 * all things automatically. Eg. you want to listen port 1986, just set it in 
 * config file, and program can listen 1986. So easy.
 *    
*****************************************************************************/

#ifdef WIN32
#include <windows.h>
#include <WinSock.h>
#include <WinError.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#endif

#include <stdio.h>
#include <errno.h>
#include "moduleTools.h"
#include "moduleLg.h"
#ifdef LOG
   #include "moduleLog.h"
   #include "moduleErr.h"
#endif

/*
 * Version and Compile Info
 * */
extern const char* commVersion;
extern const char* commCompileDate;
extern const char* commCompileTime;

/*
 * Error in this module
 * */
enum commErrNo {
   COMM_SUCCESS = 0,
   COMM_INVALID_FD,
   COMM_INVALID_LOGICNAME,
   COMM_INVALID_MAPTYPE,
   COMM_INVALID_PORT,
   COMM_CREATE_FD_ERROR,
   COMM_BIND_FAILED,
   COMM_LISTEN_FAILED,
   COMM_CONNECT_FAILED,
   COMM_SET_SOCKET_FAILED,
   COMM_UNKNOWN_UDP_MODE,
   COMM_DISCONNECTED,
   COMM_TRY_AGAIN,
   commMAXERRNO
};
extern const char* commErrInfo[commMAXERRNO];

/*
 * type of logicname
 * */
enum SortOfCommunication {
#ifdef TCP_CLIENT_MODE
   TCPCLIENT,
#endif
#ifdef TCP_SERVER_MODE
   TCPSERVER,
#endif
#ifdef UDP_MODE
   UDPSERVER,
   UDPCLIENT,
   UDP_MULTICAST_SERVER,
   UDP_MULTICAST_CLIENT,
#endif
   //SERIAL,
   //CAN,
   UNKNOWN_TYPE
};
#define checkMapType(type)    (type>=UNKNOWN_TYPE ? COMM_INVALID_MAPTYPE : COMM_SUCCESS)

/*
 * type of state
 * */
enum TypeOfState {
   CONNECTED = 0,
   DISCONNECTED,
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
 *    MapN      ...           other map, same as Map1 
 *       
 * */
extern void* gLinkMap;

/*
 * free all fds in gLinkMap
 * but can't avoid TIME_WAIT status
 */
extern void commFreeAllFds(void);

/*
 * call this function before use other functions in commModule
 * */
extern int commInit(const char* configFilePath);

/*
 * interface of recv data
 * recvBuf and recvLen are all Value-result parameter.
 * if fd is tcp, from is null.
 * */
extern int commRecv(int *fd, char* recvBuf, int* recvLen, void* from);

/*
 * interface of send data
 * if fd is tcp, to is null.
 * */
extern int commSend(int fd, const char* sendBuf, int* sendLen, void* to);

/*
 * get all file descriptors associated with logicName.
 * put fd in pFd, and sum of fd in sumFd.
 * */
extern int commGetAliveLinks(const char* logicName, int* sumFd, int** pFd);

/*
 * return the map position of logicName in gLinkMap.
 * */
extern int commGetMposByLogicName(const char* logicName);

/*
 * return the map position of fd in gLinkMap.
 * */
extern int commGetMposByFd(const int fd);

/*
 * if logicName is null, it will maintain all logicNames,
 * or the logicName only.
 * */
extern int commConnect(const char* logicName);

/*
 * if logicName is null, it will maintain all logicNames,
 * or the logicName only.
 * */
extern int commSelect(const char* logicName);

typedef void RegisterFunc(char* recvbuf, int* recvlen);
typedef void DataProcFunc(const char* logicName, const int fd, const char* recvbuf, const int recvlen, void* from);
extern int commSetFunc(const char* logicName, RegisterFunc* registerFunc, DataProcFunc* dataProcFunc);

/* 
 * before call this function, you must call commInit() and commConnect(NULL);
 * you call this function periodically.
 * */
extern int commProcess(struct timeval curTimeval);

/* function for lg module, pre-cmd is "trace" */
extern void lgCmdFuncTrace(int argc, char* argv[]);

#define MAX_LEN_VALUE   100
#define MAX_LEN_BUF     4096
#define MsumOfMap                   (char*)gLinkMap
#define MbaseMap(sizeOfGLinkMap)    (MsumOfMap + sizeOfGLinkMap)
#define MpLogicName(x)              MbaseMap(x)
#define MtypeOfMap(x)               (MpLogicName(x) + sizeof(char*))
#define MpMapLinkInfo(x)            (MtypeOfMap(x) + sizeof(int))
#define MsumOfFd(x)                 (MpMapLinkInfo(x) + sizeof(int*))
#define MbasePoolOfFd(x)            (MsumOfFd(x) + sizeof(int))
#define fdInitValue                 -1
#define MinitPoolOfFd(x)            memset(MbasePoolOfFd(x), fdInitValue, (*(int*)MsumOfFd(x) * sizeof(int)))
extern int getSizeOfGLinkMap(void);
extern void destorySockFd(const char* logicName);
extern void printGLinkMap(const char* logicName);

#define ON     1
#define OFF    0
typedef struct {
   char tcpClientTrace;
   char tcpClientPrint;
   char tcpServerTrace;
   char tcpServerPrint;
   char udpClientTrace;
   char udpClientPrint;
   char udpServerTrace;
   char udpServerPrint;
}CommFlag;
extern CommFlag gCommFlag;

typedef struct {
   int recvPacket;
   int recvByte;
   int sendPacket;
   int sendByte;
}RecordObj;

#ifdef TCP_CLIENT_MODE
typedef struct {
   RegisterFunc* registerFunc;
   DataProcFunc* dataProcFunc;
   char* destIp;
   int   destPort;
   int   localPort;
   int   state;
   int   timesReconnect;
   int   timeNextReconnect;
   RecordObj recordObj;
}TcpClientInfoObject;
#endif

#ifdef TCP_SERVER_MODE
typedef struct {
   RegisterFunc* registerFunc;
   DataProcFunc* dataProcFunc;
   int   serverPort;
   int   state;
   RecordObj recordObj;
}TcpServerInfoObject;
#endif

#ifdef UDP_MODE
/*
 * udpserver: destIp null, destPort null, localPort not null.
 * udpclient: destIp not null, destPort not null, localPort any.
 * udp multicast server: destIp not null, destPort null, localPort not null.
 * udp multicast client: destIp not null, destPort not null, localPort null.
 * multicast destIp must be in 224.0.0.0 ~ 239.255.255.255.
 * */
typedef struct {
   RegisterFunc* registerFunc;
   DataProcFunc* dataProcFunc;
   char* destIp;
   int   destPort;
   int   localPort;
   int   state;
   RecordObj recordObj;
}UdpInfoObject;
#endif

#endif


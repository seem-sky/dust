#ifndef _TOOL_H_
#define _TOOL_H_
/****************************************************************************
 * name:     工具模块
 * version:  v1.0
 * time:     2012-9-26 16:04:37
 * author:   bbdlg
 * blog:     http://my.oschina.net/bbdlg
 * 
 * 功能：
 *    提供了屏蔽/恢复标准输出的两个宏；
 *    提供了从配置文件中读取值的函数；
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/* Error in this module */
enum toolsErrNo {
   TOOLS_SUCCESS = 0,
   TOOLS_CANNOT_OPEN_CONF,
   TOOLS_INVALID_PARA,
   TOOLS_CANNOT_FIND_VALUES,
   TOOLS_ROWLEN_OVERFLOW,
   TOOLS_SECTIONLEN_OVERFLOW,
   toolsMAXERRNO
};

#define ERRORTIP do{ \
   printf("error: "format"\n", ##__VA_ARGS__); \
   fflush(stdout); \
   exit(1); \
}while(0)
#define WARNINGTIP do{ \
   printf("warning: "format"\n", ##__VA_ARGS__); \
   fflush(stdout); \
}while(0)

#define DEBUGERROR(format, ...)    printf("[%s]<line:%d> "format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); fflush(stdout)
#define DEBUGINFO(format, ...)    printf("[%s]<line:%d>"format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__); fflush(stdout)
#define DEBUG DEBUGINFO

/* hide and resume print on the screen */
#define HIDE_STDOUT  do { \
   int fd = open("/dev/null",  O_APPEND); \
   if(-1 == fd) {\
      printf("open dev/null error!\n");\
      fflush(stdout);\
      exit(1);\
   }\
   int bak_fd = dup(STDOUT_FILENO);\
   int new_fd = dup2(fd, STDOUT_FILENO);

#define RESUME_STDOUT   \
   dup2(bak_fd, new_fd);\
}while(0);

/* read value from config file
 * content of config file like this:
 *
 *** # key1 is the first test
 *** key1 =   value1
 *** key2 =   value2
 *** # other config content
 *
 */
extern int readValueFromConf(const char* filePath, const char* key, char* res, int* size);

/* extend to readValueFromConf
 * config file like this:
 *
 * [Field1]
 * key1  = value1;
 *
 * [Field1]
 * key1  = value2;
 *
 * [Field2]
 * key3  = value3;
 *
 * return how many times 'field' appears in filePath, if times=0;
 * return TOOLS_CANNOT_FIND_VALUES, it means there's no values found;
 * return TOOLS_SUCCESS, it means success...
 */
extern int readValueFromConf_ext(const char* filePath, const int times, const char* field, const char* key, char* res, int* size);

#define MAX_LEN_ROW        200
#define MAX_LEN_SECTION    4096
extern int getResultFromSystemCall(const char* pCmd, char* pResult, int* size);
extern int getSection(const char* filePath, const int times, const char* field, char* section);
extern int getValueOfKey(const char* section, const char* key, char* res, int* size);

#endif

# WinSock
# WinSock
对WinSocket库封装而成的类  
**LoadSocektLib()/UnloadSocketLib()**                               装/卸WinSocket库  
**CreateSocket()/CreateWSASocket()**                                创建socket套接字/WSAsocket套接字  
**char*** **GetLocalIP()**                                              获得本地IP  
**Bind(unsigned short port)**                                       bind函数的封装,绑定端口port与ip  
**Accept()**                                                        accept()函数的封装，连接一个客户端并返回含客户端信息的WinSock对象  
**Recv(char*** **buf, int bufsize)**                                    recv()的封装  
**Send(const char*** **buf, int size)**                                 send()的封装  
**SetBlock(bool isblock)**                                          设置WinSock对象的阻塞属性  
# 使用（Visual Studio）
1、WinSock.h和WinSock.cpp直接丢进项目文件夹#include"WinSock.h"  
2、或者你直接弄个动态链接库什么的  

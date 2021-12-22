#pragma once
#define MAXLINE 1024
#define MAXFILE 16 * 1024 * 1024
#define MAXARGV 256
struct Terminal
{
    char user[MAXLINE];   // 用户名
    char mach[MAXLINE];   // 计算机名
    char root[MAXLINE];   // 根目录
    char wdir[MAXLINE];   // 工作目录
    char strin[MAXFILE];  // 重定向标准输入
    char strout[MAXFILE]; // 重定向标准输出
};
extern Terminal gTerm;

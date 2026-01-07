/* 
 * Автор: Поздняков Алексей Васильевич
 * Email: avp70ru@mail.ru
 * GitHub: @AVPscan
 * 
 * Copyright (c) 2026 Алексей Поздняков
 * 
 * Лицензия: 
 * Этот проект распространяется под условиями лицензии Apache-2.0.
 * Полный текст лицензии можно найти в файле LICENSE в корневом каталоге.
 */
 
#ifndef LIBOPT_H
#define LIBOPT_H

#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

extern int enc_mode; 

#define HideCursor    "\033[?25l"
#define ShowCursor    "\033[?25h"
#define ClearScreen   "\033[2J\033[H"
#define SaveCursor    "\033[s"
#define LoadCursor    "\033[u"
#define ClearCurEnd   "\033[K"
#define CBlack   "\033[30m"
#define CRed     "\033[31m"
#define CGreen   "\033[32m"
#define CYellow  "\033[33m"
#define CBlue    "\033[34m"
#define CMagenta "\033[35m"
#define CCyan    "\033[36m"
#define CWhite   "\033[37m"
#define CReset   "\033[0m"
#define CBBlack   "\033[1;30m"
#define CBRed     "\033[1;31m"
#define CBGreen   "\033[1;32m"
#define CBYellow  "\033[1;33m"
#define CBBlue    "\033[1;34m"
#define CBMagenta "\033[1;35m"
#define CBCyan    "\033[1;36m"
#define CBWhite   "\033[1;37m"

int   MemCmp(const void *s1, const void *s2, size_t n);
void  MemCpy(void *d, const void *s, int n);
void* MemSet(void *s, int c, int n);
void* MemMove(void *dest, const void *src, size_t n);

size_t StrLenB(const char *s);
int    StrLen(const char *s);
char*  StrCpy(char *dest, const char *src);
char*  StrCat(char *dest, const char *src);
char*  StrNCat(char *dest, const char *src, int n);
int    StrCmp(const char *s1, const char *s2);
int    StrNCmp(const char *s1, const char *s2, int n);
char*  StrChr(const char *s, int c);
char*  StrDup(const char *s);
char*  Replays(const char *input, const char *find, const char *repl);
char*  GetBuf();
int    StringBC(const char *s, int *c);

const char* SetColor(int c);
const char* CursorXY(int x, int y);
const char* CursorUp(int y);
const char* CursorPos(int x);
const char* CursorPosUp(int x, int y);
const char* SetMode(int i);
const char* SetColMod(int c, int m);
void        GetCursorXY(int *x, int *y);

void        SetInputMode(int raw);
const char* GetKeyName();

int  IsXDigit(int c);
int  ToLower(int c);

void        SWD();
const char* DateStr();
int         IsEncrypted(const char *fname);
int         SaveEncrypted(const char *fname, const char *email, const char *pass);
int         DecodeToEmailAndPass(const char *fname, char *email, char *pass);
int         AutoEncryptOrValidate(const char *fname);
int         SendMailSecure(const char *fname, const char *target_html);
int         TxtToHtml(const char *src, const char *dst, const char *cfg);
void        AnalyzeFormat(unsigned char *buf, ssize_t size);

#endif


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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#define IO_SIZE 4096
#define HideCursor  "\033[?25l"
#define ShowCursor  "\033[?25h"
#define ClearScreen "\033[2J\033[H"
#define SaveCursor  "\033[s"
#define LoadCursor  "\033[u"
#define ClearCurEnd "\033[K"
#define CBlack   "\033[30m"     // Обычный (внимание: на черном фоне не виден!)
#define CRed     "\033[31m"
#define CGreen   "\033[32m"
#define CYellow  "\033[33m"
#define CBlue    "\033[34m"
#define CMagenta "\033[35m"
#define CCyan    "\033[36m"
#define CWhite   "\033[37m"
#define CReset   "\033[0m"
#define CBBlack   "\033[1;30m"  // Темно-серый
#define CBRed     "\033[1;31m"  // Ярко-красный
#define CBGreen   "\033[1;32m"  // Ярко-зеленый
#define CBYellow  "\033[1;33m"  // Ярко-желтый
#define CBBlue    "\033[1;34m"  // Ярко-синий
#define CBMagenta "\033[1;35m"  // Ярко-пурпурный
#define CBCyan    "\033[1;36m"  // Ярко-голубой
#define CBWhite   "\033[1;37m"  // Чисто белый (жирный)

extern int MemCmp(const void *s1, const void *s2, size_t n);
extern void MemCpy(void *d, const void *s, int n);
extern void *MemSet(void *s, int c, int n);
extern void *MemMove(void *dest, const void *src, size_t n);
extern size_t StrLenB(const char *s);
extern char *StrCpy(char *dest, const char *src);
extern char* StrCat(char *dest, const char *src);
extern char* StrNCat(char *dest, const char *src, int n);
extern int StrCmp(const char *s1, const char *s2);
extern int StrNCmp(const char *s1, const char *s2, int n);
extern char *StrChr(const char *s, int c);
extern int StrLen(const char *s);
extern int StringBC(const char *s, int *c);
extern char* GetBuf();
extern char *StrDup(const char *s);
extern char* Replays(const char *input, const char *find, const char *repl);
extern const char* SetColor(int c);
extern const char* CursorXY(int x, int y);
extern const char* CursorUp(int y);
extern const char* CursorPos(int x);
extern const char* CursorPosUp(int x, int y);
extern const char* SetMode(int i);
extern const char* SetColMod(int c, int m);
extern void GetCursorXY(int *x, int *y);
extern void SetInputMode(int raw);
typedef struct { const char *seq; const char *name; } KeyMap;
extern const char* GetKeyName();
extern void SWD();
extern const char* DateStr();
extern int IsXDigit(int c);
extern int ToLower(int c);
extern int AutoEncryptOrValidate(const char *fname);
extern int SendMailSecure(const char *fname, const char *target);
extern int enc_mode;
extern void AnalyzeFormat(unsigned char *buf, ssize_t size);
extern int TxtToHtml(const char *src, const char *dst, const char *cfg);

#ifdef __cplusplus
}
#endif
#endif // LIBOPT_H

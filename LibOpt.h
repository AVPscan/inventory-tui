/*
 * Copyright 2026 [Алексей Васильевич Поздняков AVPscan]
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     www.apache.org
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

/* --- ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ --- */
/* Позволяет new.c видеть текущий режим кодировки (UTF-8/CP1251) */
extern int enc_mode; 

/* --- МАКРОСЫ ANSI (Управление терминалом) --- */
#define HideCursor    "\033[?25l"
#define ShowCursor    "\033[?25h"
#define ClearScreen   "\033[2J\033[H"
#define SaveCursor    "\033[s"
#define LoadCursor    "\033[u"
#define ClearCurEnd   "\033[K"

/* --- РАБОТА С ПАМЯТЬЮ --- */
int   MemCmp(const void *s1, const void *s2, size_t n);
void  MemCpy(void *d, const void *s, int n);
void* MemSet(void *s, int c, int n);
void* MemMove(void *dest, const void *src, size_t n);

/* --- РАБОТА СО СТРОКАМИ (БАЙТЫ И UTF-8) --- */
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

/* --- ТЕРМИНАЛ: ЦВЕТ И КУРСОР --- */
const char* SetColor(int c);
const char* CursorXY(int x, int y);
const char* CursorUp(int y);
const char* CursorPos(int x);
const char* CursorPosUp(int x, int y);
const char* SetMode(int i);
const char* SetColMod(int c, int m);
void        GetCursorXY(int *x, int *y);

/* --- КЛАВИАТУРА И ВВОД --- */
void        SetInputMode(int raw);
const char* GetKeyName();

/* --- ВСПОМОГАТЕЛЬНЫЕ (Hex/Case) --- */
int  IsXDigit(int c);
int  ToLower(int c);

/* --- СИСТЕМНЫЕ И ШИФРОВАНИЕ --- */
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


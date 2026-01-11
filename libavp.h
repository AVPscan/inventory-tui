/* 
 * libavp.h — заголовочный файл библиотеки AVP (C11, Linux)
 * Автор: Поздняков Алексей Васильевич
 * Email: avp70ru@mail.ru
 * 
 * Copyright (c) 2026 Алексей Поздняков
 * Лицензия: Apache-2.0
 */

#ifndef LIBAVP_H
#define LIBAVP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* Константы терминала */
#define HideCursor  "\033[?25l"
#define ShowCursor  "\033[?25h"
#define ClearScreen "\033[2J\033[H"
#define SaveCursor  "\033[s"
#define LoadCursor  "\033[u"
#define ClearCurEnd "\033[K"
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

/* Константы буфера и калибровки */
#define IO_SIZE 4096
extern unsigned char io_buf[IO_SIZE];
extern int enc_mode;
extern int ui_delay_factor;

/* Константы для калибровки задержек */
#define UI_CALIBRATION_TARGET_MS 100
#define UI_MIN_MEASURED_MS 50
#define UI_MAX_MEASURED_MS 150
#define UI_MIN_FACTOR 8
#define UI_MAX_FACTOR 25
#define UI_DEFAULT_FACTOR 12

/* Структуры */
typedef struct {
    const char *seq;
    const char *name;
} KeyMap;

/* Утилиты времени */
void delay_ms(int ms);
void CalibrateUIDelay(void);
void UIDelay(int base_ms);

/* Память */
int MemCmp(const void *s1, const void *s2, size_t n);
void MemCpy(void *d, const void *s, int n);
void *MemSet(void *s, int c, int n);
void *MemMove(void *dest, const void *src, size_t n);

/* Строки (байтовые) */
size_t StrLenB(const char *s);
char *StrCpy(char *dest, const char *src);
char *StrCat(char *dest, const char *src);
char *StrNCat(char *dest, const char *src, int n);
int StrCmp(const char *s1, const char *s2);
int StrNCmp(const char *s1, const char *s2, int n);
char *StrChr(const char *s, int c);

/* Строки (UTF-8 aware) */
int StrLen(const char *s);
int StringBC(const char *s, int *c);

/* Утилиты строк */
char *GetBuf(void);
char *StrDup(const char *s);
char *Replays(const char *input, const char *find, const char *repl);

/* Терминал/курсор/цвета */
const char *SetColor(int c);
const char *CursorXY(int x, int y);
const char *CursorUp(int y);
const char *CursorPos(int x);
const char *CursorPosUp(int x, int y);
const char *SetMode(int i);
const char *SetColMod(int c, int m);
void GetCursorXY(int *x, int *y);

/* Ввод/клавиатура */
void SetInputMode(int raw);
const char *GetKeyName(void);

/* Рабочая директория */
void SWD(void);

/* Дата/время */
const char *DateStr(void);

/* Хелперы */
int IsXDigit(int c);
int ToLower(int c);
char *GenerateRN(const char *fname);

/* Крипто/шифрование */
int AutoEncryptOrValidate(const char *fname);
int SendMailSecure(const char *fname, const char *target);

/* Конвертация текста */
void AnalyzeFormat(unsigned char *buf, ssize_t size);
int TxtToHtml(const char *src, const char *dst, const char *cfg);

#ifdef __cplusplus
}
#endif

#endif /* LIBAVP_H */

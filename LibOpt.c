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
 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

/* --- РАБОТА С ПАМЯТЬЮ --- */
// Оптимизированное сравнение (по 8 байт)
int MemCmp(const void *s1, const void *s2, size_t n) {
    if (n == 0) return 256;
    const unsigned char *p1 = (const unsigned char *)s1; 
    const unsigned char *p2 = (const unsigned char *)s2;
    while (n >= 8) { 
        if (*(const unsigned long long *)p1 != *(const unsigned long long *)p2) break;
        p1 += 8; p2 += 8; n -= 8; }
    while (n--) { 
        if (*p1 != *p2) return (*p1 - *p2);
        p1++; p2++; }
    return 0; }
void MemCpy(void *d, const void *s, int n) {
    char *dc = (char *)d; const char *sc = (const char *)s; 
    while (n-- > 0) *dc++ = *sc++; }
void *MemSet(void *s, int c, int n) {
    unsigned char *p = (unsigned char *)s; 
    while (n-- > 0) *p++ = (unsigned char)c;
    return s; }
void *MemMove(void *dest, const void *src, size_t n) {
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (unsigned char *)src;
    if (d == s || n == 0) return dest;
    if (d > s && d < s + n) { 
        size_t i = n;
        while (i > 0) { i--; d[i] = s[i]; } }
    else {
        for (size_t i = 0; i < n; i++) d[i] = s[i]; }
    return dest; }
/* --- РАБОТА СО СТРОКАМИ (БАЙТЫ) --- */
size_t StrLenB(const char *s) { 
    if (!s) return 0;
    size_t l = 0; while (s[l]) l++; 
    return l; }
char *StrCpy(char *dest, const char *src) {
    char *d = dest; while ((*d++ = *src++)); 
    return dest; }
char* StrCat(char *dest, const char *src) {
    char *d = dest; while (*d) d++;
    while ((*d++ = *src++));
    return dest; }
char* StrNCat(char *dest, const char *src, int n) {
    char *d = dest; while (*d) d++; 
    while (n-- > 0 && *src) *d++ = *src++;
    *d = '\0'; return dest; }
int StrCmp(const char *s1, const char *s2) {
    if (!s1 || !s2) return (s1 == s2) ? 0 : (s1 ? 1 : -1); 
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char *)s1 - *(unsigned char *)s2; }
int StrNCmp(const char *s1, const char *s2, int n) {
    if (n <= 0) return 0;
    while (n > 1 && *s1 && (*s1 == *s2)) { s1++; s2++; n--; }
    return *(unsigned char *)s1 - *(unsigned char *)s2; }
char *StrChr(const char *s, int c) {
    while (*s) { if (*s == (char)c) return (char *)s; s++; } 
    return (c == 0) ? (char *)s : NULL; }
/* --- РАБОТА С UTF-8 (СИМВОЛЫ) --- */
int StrLen(const char *s) {
    if (!s) return 0; 
    const unsigned char *p = (const unsigned char *)s;
    int count = 0; 
    while (*p) { if ((*p++ & 0xC0) != 0x80) count++; }
    return count; }
int StringBC(const char *s, int *c) { 
    int b = 0, chars = 0; if (!s) { if (c) *c = 0; return 0; } 
    while (s[b]) { if ((s[b] & 0xC0) != 0x80) chars++; b++; } 
    if (c) *c = chars; 
    return b; }
char* GetBuf() {
    static char buffers[16][128]; static int idx = 0;
    idx = (idx + 1) & 15; return buffers[idx]; }
char *StrDup(const char *s) {
    if (!s) return NULL;
    size_t len = StrLenB(s) + 1; 
    char *d = (char *)malloc(len); 
    if (d) MemCpy(d, s, (int)len);
    return d; }
char* Replays(const char *input, const char *find, const char *repl) {
    static char buffers[8][2048];
    static int b_idx = 0;
    b_idx = (b_idx + 1) & 7;
    char *dest = buffers[b_idx];
    if (!input || *input == '\0') return (char*)""; 
    if (!find || *find == '\0') return (char*)input;
    size_t in_len = StrLenB(input);
    size_t f_len = StrLenB(find);
    if (f_len > in_len) return (char*)input;
    if (in_len == f_len && MemCmp(input, find, f_len) == 0) return (repl) ? (char*)repl : (char*)"";
    size_t r_len = (repl) ? StrLenB(repl) : 0;
    size_t d_ptr = 0;
    size_t i = 0;
    size_t limit = in_len - f_len;
    while (i < in_len) {
        if (i <= limit && MemCmp(input + i, find, f_len) == 0) { 
            if (r_len > 0 && d_ptr + r_len < 2047) { 
                MemCpy(dest + d_ptr, repl, (int)r_len); 
                d_ptr += r_len; }
            i += f_len; } 
        else { 
            if (d_ptr < 2047) dest[d_ptr++] = input[i++];
            else break; } }
    dest[d_ptr] = '\0'; return dest; }
const char* SetColor(int c) {
    char *b = GetBuf(); snprintf(b, 32, "\033[%dm", (c < 0) ? -c : c); return b; }
const char* CursorXY(int x, int y) {
    char *b = GetBuf(); snprintf(b, 32, "\033[%d;%dH",y,x); return b; }
const char* CursorUp(int y) {
    char *b = GetBuf(); snprintf(b, 32, "\033[%dF",y); return b; }
const char* CursorPos(int x) {
    char *b = GetBuf(); snprintf(b, 32, "\033[%dG",x); return b; }
const char* CursorPosUp(int x, int y) {
    char *b = GetBuf(); snprintf(b, 32, "\033[%dF\033[%dG",y,x);  return b; }
const char* SetMode(int i) {
    char *b = GetBuf(); if (i < 0) i = -i;
    if (i != 8) { i %= 8; if (i == 6) i = 9; }
    else i = 21;
    snprintf(b, 32, "\033[%dm", i); return b; }
const char* SetColMod(int c, int m) {
    char *b = GetBuf(); snprintf(b, 64, "%s%s", SetMode(m), SetColor(c)); return b; }
void GetCursorXY(int *x, int *y) {
    char *buf=GetBuf(); int i = 0; write(1, "\033[6n", 4);
    while (i < 31 && read(0, &buf[i], 1) > 0) if (buf[i++] == 'R') break;
    buf[i] = 0; for (i=0; buf[i]; i++) if (buf[i] == '[') sscanf(&buf[i+1], "%d;%d", y, x); }
#define HideCursor  "\033[?25l"
#define ShowCursor  "\033[?25h"
#define ClearScreen "\033[2J\033[H"
#define SaveCursor  "\033[s"
#define LoadCursor  "\033[u"
#define ClearCurEnd "\033[K"
int IsXDigit(int c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
int ToLower(int c) {
    return (c >= 'A' && c <= 'Z') ? (c + 32) : c; }
static int HexVal(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    return (ToLower(c) - 'a' + 10); }
static void ValToHex(unsigned char b, char *out) {
    const char *h = "0123456789ABCDEF";
    out[0] = h[b >> 4]; 
    out[1] = h[b & 0x0F]; }

void SetInputMode(int raw) {
    static struct termios oldt;
    if (raw) {
        tcgetattr(0, &oldt);
        struct termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO | ISIG);
        tcsetattr(0, TCSANOW, &newt);
        fcntl(0, F_SETFL, O_NONBLOCK); }
    else { tcsetattr(0, TCSANOW, &oldt);
        fcntl(0, F_SETFL, 0); } }
typedef struct { const char *seq; const char *name; } KeyMap;
KeyMap key_table[] = {
    {"[A", "Up"}, {"[B", "Down"}, {"[C", "Right"}, {"[D", "Left"},
    {"[H", "Home"}, {"[F", "End"}, {"[1~", "Home"}, {"[4~", "End"},
    {"[2~", "Insert"}, {"[3~", "Delete"}, {"[5~", "Page Up"}, {"[6~", "Page Down"},
    {"OP", "F1"}, {"OQ", "F2"}, {"OR", "F3"}, {"OS", "F4"},
    {"[15~", "F5"}, {"[17~", "F6"}, {"[18~", "F7"}, {"[19~", "F8"},
    {"[20~", "F9"}, {"[21~", "F10"}, {"[23~", "F11"}, {"[24~", "F12"} };
const char* GetKeyName() {
    static char out_buf[16];
    unsigned char c;
    if (read(0, &c, 1) <= 0) return "";
    if (c >= 0x80) {
        out_buf[0] = c;
        int len = (c >= 0xF0) ? 3 : (c >= 0xE0) ? 2 : (c >= 0xC0) ? 1 : 0;
        int i = 0;
        while (i < len) i += read(0, &out_buf[i + 1], len - i);
        out_buf[len + 1] = '\0';
        return out_buf; }
    if (c == 27) {
        char seq[8] = {0};
        usleep(1000);
        int i = 0;
        while (i < 7 && read(0, &seq[i], 1) > 0) i++;
        if (i == 0) return "Escape";
        for (int j = 0; j < (int)(sizeof(key_table)/sizeof(KeyMap)); j++) {
            const char *s1 = seq, *s2 = key_table[j].seq;
            while (*s1 && *s1 == *s2) { s1++; s2++; }
            if (*s1 == *s2) return key_table[j].name; }
        return ""; }
    switch (c) {
        case 3:   return "Ctrl+C";
        case 9:   return "Tab";
        case 10:  return "Enter";
        case 32:  return "Space";
        case 127: return "Backspace";
        default: if (c < 32) {
                out_buf[0] = 'C'; out_buf[1] = 't'; out_buf[2] = 'r'; out_buf[3] = 'l';
                out_buf[4] = '+'; out_buf[5] = c + 64; out_buf[6] = '\0'; } 
            else { out_buf[0] = c; out_buf[1] = '\0'; }
    return out_buf; } }
void SWD() {
    char path[4096];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len <= 0) return;
    path[len] = '\0'; for (char *p = path + len; p > path; p--) if (*p == '/') { *p = '\0'; break; }
    chdir(path); }
const char* DateStr() {
    char *buf = GetBuf();
    if (!buf) return "";
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    static const char *weeks[] = { "первой", "второй", "третей", "четвертой", "пятой"};
    static const char *months[] = { "января", "февраля", "марта", "апреля", "мая", "июня", "июля", "августа", "сентября", "октября", "ноября", "декабря" };
    static const char *days[] = { "воскресенье", "понедельник", "вторник", "среда", "четверг", "пятница", "суббота" };
    int w_idx = (tm->tm_mday - 1) / 7;
    snprintf(buf, 128, "%s %s недели %s", days[tm->tm_wday], weeks[w_idx], months[tm->tm_mon]);
    return buf; }
    
/* --- ОСНОВНАЯ ЛОГИКА ШИФРОВАНИЯ С ОЧИСТКОЙ ПАМЯТИ --- */
static void Crypt(unsigned char *buf, int len) {
    unsigned char salt[] = {0xAC, 0x77, 0x5F, 0x12, 0x88, 0x33, 0x22, 0x11}; 
    for (int i = 0; i < len; i++) {
        int idx = len - 1 - i;
        unsigned char key = (unsigned char)((idx ^ salt[idx % 8]) + (idx % 11));
        key ^= (unsigned char)((idx >> 3) | (idx << 5));
        key ^= salt[(idx + 3) & 7];
        buf[i] ^= key; } }
static off_t SkipBOM(int fd) {
    unsigned char b[3];
    lseek(fd, 0, SEEK_SET);
    if (read(fd, b, 3) == 3 && b[0] == 0xEF && b[1] == 0xBB && b[2] == 0xBF) return 3;
    return lseek(fd, 0, SEEK_SET); }
int IsEncrypted(const char *fname) {
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return 0;
    SkipBOM(fd);
    char h[4];
    if (read(fd, h, 4) < 4) { close(fd); return 0; }
    for (int i = 0; i < 4; i++) { if (!IsXDigit(h[i])) { close(fd); return 0; } }
    int le = (HexVal(h[0]) << 4) | HexVal(h[1]);
    int lp = (HexVal(h[2]) << 4) | HexVal(h[3]);
    close(fd);
    return (le >= 5 && le <= 64 && lp >= 8 && lp <= 64); }
int SaveEncrypted(const char *fname, const char *email, const char *pass) {
    int le = (int)StrLenB(email), lp = (int)StrLenB(pass);
    char header[262]; 
    unsigned char raw[130]; 
    MemSet(header, 0, sizeof(header));
    MemSet(raw, 0, sizeof(raw));
    ValToHex((unsigned char)le, &header[0]);
    ValToHex((unsigned char)lp, &header[2]);
    MemCpy(raw, email, le);
    MemCpy(raw + le, pass, lp);
    Crypt(raw, le + lp);
    for (int i = 0; i < (le + lp); i++) ValToHex(raw[i], &header[4 + i * 2]);
    int total_len = 4 + (le + lp) * 2;
    header[total_len++] = '\n';
    int fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd >= 0) {
        write(fd, "\xEF\xBB\xBF", 3);
        write(fd, header, total_len);
        close(fd); }
    // ОЧИСТКА: затираем сырые данные и зашифрованный заголовок
    MemSet(raw, 0, sizeof(raw));
    MemSet(header, 0, sizeof(header));
    return (fd < 0) ? -1 : 0; }
int DecodeToEmailAndPass(const char *fname, char *email, char *pass) {
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return 1;
    SkipBOM(fd);
    char h[4];
    if (read(fd, h, 4) < 4) { close(fd); return 1; }
    int le = (HexVal(h[0]) << 4) | HexVal(h[1]);
    int lp = (HexVal(h[2]) << 4) | HexVal(h[3]);
    char hex[260];
    if (read(fd, hex, (le + lp) * 2) < (le + lp) * 2) { 
        MemSet(hex, 0, sizeof(hex));
        close(fd); return 1; }
    close(fd);
    unsigned char raw[130];
    MemSet(raw, 0, sizeof(raw));
    for (int i = 0; i < (le + lp); i++) {
        raw[i] = (unsigned char)((HexVal(hex[i * 2]) << 4) | HexVal(hex[i * 2 + 1])); }
    Crypt(raw, le + lp);
    MemCpy(email, raw, le); email[le] = 0;
    MemCpy(pass, raw + le, lp); pass[lp] = 0;
    // ОЧИСТКА: затираем временные буферы
    MemSet(raw, 0, sizeof(raw));
    MemSet(hex, 0, sizeof(hex));
    return 0; }
int AutoEncryptOrValidate(const char *fname) {
    if (IsEncrypted(fname)) return 0;
    int fd = open(fname, O_RDONLY);
    if (fd < 0) return 1;
    char buf[1024]; 
    MemSet(buf, 0, sizeof(buf));
    int r = (int)read(fd, buf, sizeof(buf) - 1);
    close(fd);
    if (r <= 0) return 1;
    buf[r] = '\0';
    char *at = StrChr(buf, '@');
    if (!at) { MemSet(buf, 0, sizeof(buf)); return 2; }
    char *e_start = at;
    while (e_start > buf && (unsigned char)*(e_start - 1) > 32) e_start--;
    char *e_end = at;
    while (*e_end && (unsigned char)*e_end > 32) e_end++;
    char *p_start = e_end;
    while (*p_start && (unsigned char)*p_start <= 32) p_start++;
    char *p_end = p_start;
    while (*p_end && (unsigned char)*p_end > 32) p_end++;
    int le = (int)(e_end - e_start), lp = (int)(p_end - p_start);
    if (le < 5 || lp < 8 || le > 64 || lp > 64) {
        MemSet(buf, 0, sizeof(buf));
        return 3; }
    char em[65], pw[65];
    MemSet(em, 0, sizeof(em));
    MemSet(pw, 0, sizeof(pw));
    MemCpy(em, e_start, le); em[le] = 0;
    MemCpy(pw, p_start, lp); pw[lp] = 0;
    // ОЧИСТКА основного буфера файла перед шифрованием
    MemSet(buf, 0, sizeof(buf));
    int res = SaveEncrypted(fname, em, pw);
    // ОЧИСТКА временных данных
    MemSet(pw, 0, sizeof(pw));
    MemSet(em, 0, sizeof(em));
    return res; }
int SendMailSecure(const char *fname, const char *target_html) {
    char email[65], pass[65], server[128], cmd[1024];
    MemSet(email, 0, sizeof(email));
    MemSet(pass, 0, sizeof(pass));
    if (DecodeToEmailAndPass(fname, email, pass) != 0) return 1;
    char *domain = StrChr(email, '@');
    if (!domain) {
        MemSet(pass, 0, sizeof(pass));
        return 1; }
    MemSet(server, 0, sizeof(server));
    MemCpy(server, "smtp.", 5);
    StrCpy(server + 5, domain + 1);
    MemSet(cmd, 0, sizeof(cmd));
    snprintf(cmd, sizeof(cmd), 
        "curl -s --url 'smtps://%s:465' --user '%s:%s' "
        "--mail-from '%s' --mail-rcpt '%s' --upload-file '%s'",
        server, email, pass, email, email, target_html);
    int res = system(cmd);
    // ОЧИСТКА: затираем команду (содержащую пароль) и сам пароль
    MemSet(cmd, 0, sizeof(cmd));
    MemSet(pass, 0, sizeof(pass));
    return (res == 0) ? 0 : (res >> 8); }
#define IO_SIZE 4096
static unsigned char io_buf[IO_SIZE];
int enc_mode = 0;
void AnalyzeFormat(unsigned char *buf, ssize_t size) {
    int u8 = 0, cp = 0, ko = 0;
    for (ssize_t i = 0; i < size - 1 && i < 4000; i++) {
        if ((buf[i] == 0xD0 || buf[i] == 0xD1) && (buf[i+1] >= 0x80 && buf[i+1] <= 0xBF)) { u8 += 10; i++; }
        if (buf[i] == 0xEE || buf[i] == 0xE0) cp++;
        if (buf[i] == 0xCF || buf[i] == 0xC1) ko++; }
    if (u8 > 5) enc_mode = 0; else if (cp >= ko) enc_mode = 1; else enc_mode = 2; }
int TxtToHtml(const char *src, const char *dst, const char *cfg) {
    int fi = open(src, O_RDONLY), fo = open(dst, O_WRONLY|O_CREAT|O_TRUNC, 0666);
    if (fi < 0 || fo < 0) { if (fi >= 0) close(fi); return 1; }
    ssize_t n = read(fi, io_buf, IO_SIZE);
    if (n > 0) { AnalyzeFormat(io_buf, n); lseek(fi, 0, SEEK_SET); } else enc_mode = 0;
    char lg[65], pw[65]; MemSet(lg, 0, 65); MemSet(pw, 0, 65);
    if (DecodeToEmailAndPass(cfg, lg, pw) != 0) StrCpy(lg, "StoreReport");
    const char *ds = DateStr();
    dprintf(fo, "From: %s\r\nTo: %s\r\nSubject: Report %s\r\nContent-Type: text/html; charset=utf-8\r\n\r\n", lg, lg, ds);
    dprintf(fo, "<html><head><meta charset='utf-8'></head><body style='background:#f4f4f4; padding:10px;'><div style='background:#fff; border:1px solid #ccc; padding:15px; font-family:monospace; white-space:nowrap; font-size:13px; line-height:1.2;'><div style='color:#00afff; font-weight:bold; margin-bottom:10px;'>User: %s | Date: %s</div><hr style='border:0; border-top:1px solid #eee; margin-bottom:10px;'>", lg, ds);
    int cp = 0;
    while ((n = read(fi, io_buf, IO_SIZE)) > 0) {
        for (int i = 0; i < n; i++) {
            unsigned char c = io_buf[i];
            if (c == '\n') { if (cp == 0) write(fo, "&nbsp;", 6); write(fo, "<br>\n", 5); cp = 0; continue; }
            if (c == '\r') continue;
            if (cp <= 2) write(fo, "<span style='color:#888888;'>", 29);
            else if (cp <= 5) write(fo, "<span style='color:#0055ff;'>", 29);
            else if (c >= '0' && c <= '9') write(fo, "<span style='color:#008080;'>", 29);
            else write(fo, "<span style='color:#800000;'>", 29);
            if (c == ' ') write(fo, "&nbsp;", 6);
            else if (c == '<') write(fo, "&lt;", 4);
            else if (c == '>') write(fo, "&gt;", 4);
            else if (c == '&') write(fo, "&amp;", 5);
            else {
                if (c < 0x80 || enc_mode == 0) { write(fo, &c, 1);
                    if (enc_mode == 0 && (c & 0xE0) == 0xC0 && i + 1 < n) write(fo, &io_buf[++i], 1); }
                else { unsigned char out[2];
                    if (c >= 192) { out[0] = (c < 240) ? 0xD0 : 0xD1; out[1] = (c < 240) ? (c - 48) : (c - 112); write(fo, out, 2); }
                    else write(fo, &c, 1); } }
            write(fo, "</span>", 7);
            if (enc_mode != 0 || (c & 0xC0) != 0x80) cp++; } }
    write(fo, "</div></body></html>", 20);
    fsync(fo); close(fi); close(fo); return 0; }


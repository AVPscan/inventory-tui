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
#include <getopt.h>

#include "LibOpt.h"

#define BUF_A (1024 * 1024)
#define DB_NAME "продукты.txt"
#define fileres "res.txt"
#define filesendID "send.txt"
#define filesendhtml "temp.html"

typedef struct { 
  char *name;
  int price, col, len;
} Product;
Product *dict = NULL;
static int dict_cap = 0;
int dict_count = 0; 
int max_s_chars = 0; 
int max_n_chars = 0;
static unsigned char ext_buf[BUF_A];
const char *ccou = CWhite;   // №
const char *ccol = CBlue;    // Количество
const char *cpri = CCyan;    // Цена
const char *cnam = CMagenta; // Название
const char *ctxt = CGreen;   // Текст
const char *crst = CReset;   // Сброс

int GNL(int n) {
    if (n == 0) return 1;
    int l = 1; n = (n < 0) ? -n : n; while (n > 9) { n /= 10; l++; }
    return l; }
void ClearDic() {
    if (!dict) return;
    for (int i = 0; i < dict_count; i++) if (dict[i].name) { free(dict[i].name); dict[i].name = NULL; }
    free(dict); dict = NULL; dict_count = 0; dict_cap = 0; max_s_chars = 0; max_n_chars = 0; }
void addDic(int price, int col, const char *name) {
    if (!name || !*name) return;
    int low = 0, high = dict_count - 1;
    int pos = 0;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = StrCmp(dict[mid].name, name);       
        if (cmp == 0) {
            dict[mid].price = (price < 0) ? -price : price;
            int total = dict[mid].col + ((col >= 0) ? col % 10 : 0);
            dict[mid].col = (total > 9) ? 9 : total;
            int p_len = GNL(dict[mid].price);
            if (p_len > max_n_chars) max_n_chars = p_len;
            return; }
        if (cmp < 0) low = mid + 1;
        else high = mid - 1; }
    pos = low;
    if (dict_count >= dict_cap) {
        int new_cap = (dict_cap < 16) ? 16 : dict_cap * 2;
        Product *new_d = (Product *)realloc(dict, new_cap * sizeof(Product));
        if (!new_d) return;
        dict = new_d;
        dict_cap = new_cap; }
    if (pos < dict_count) MemMove(&dict[pos + 1], &dict[pos], (dict_count - pos) * sizeof(Product));
    dict[pos].name = StrDup(name);
    dict[pos].price = (price < 0) ? -price : price;
    dict[pos].col = (col >= 0) ? col % 10 : 0;
    dict[pos].len = StrLen(name); 
    if (dict[pos].len > max_s_chars) max_s_chars = dict[pos].len;
    int p_len = GNL(dict[pos].price);
    if (p_len > max_n_chars) max_n_chars = p_len;
    dict_count++; }
int Fpi(const char *s, int *i) {                                    // &i первое вхождение в словарь
    if (!dict || dict_count <= 0) { if (i) *i = -1; return 0; }     // b на выходе число совпадений i+0,..,i+b-1 
    if (!s || *s == '\0') { if (i) *i = 0; return dict_count; }
    int in_len = StrLenB(s);
    int low = 0, high = dict_count - 1;
    int first = -1;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = StrNCmp(dict[mid].name, s, in_len);
        if (cmp == 0) { first = mid; high = mid - 1; }
        else if (cmp < 0) low = mid + 1;
            else high = mid - 1; }
    if (first == -1) { if (i) *i = -1; return 0; }
    if (i) *i = first;
    low = first; 
    high = dict_count - 1;
    int last = first;
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = StrNCmp(dict[mid].name, s, in_len);
        if (cmp == 0) { last = mid; low = mid + 1; }
        else if (cmp < 0) low = mid + 1;
            else high = mid - 1; }
    return (last - first + 1); }
int NextIdUp(int i, int b, int current) {
    if (b <= 0) return -1;
    int next = (current - i - 1 + b) % b; return i + next; }
int NextIdDown(int i, int b, int current) {
    if (b <= 0) return -1;
    int next = (current - i + 1) % b; if (next < 0) next += b;
    return i + next; }
char* UCase(const char *src, int up) {
    unsigned char *d = (unsigned char *)GetBuf(); 
    unsigned char *start = d;
    unsigned char *s = (unsigned char *)src;
    while (*s) {
        if (*s < 0x80) {
            if (up) *d = (*s >= 'a' && *s <= 'z') ? (*s - 32) : *s;
            else    *d = (*s >= 'A' && *s <= 'Z') ? (*s + 32) : *s;
            d++; s++; } 
        else if (enc_mode == 0) {
            if (*s == 0xD0 || *s == 0xD1) {
                unsigned char s1 = *s, s2 = *(s + 1);
                if (!up) {
                    if (s1 == 0xD0 && s2 >= 0x90 && s2 <= 0x9F) { *d++ = 0xD0; *d++ = s2 + 0x20; }
                    else if (s1 == 0xD0 && s2 >= 0xA0 && s2 <= 0xAF) { *d++ = 0xD1; *d++ = s2 - 0x20; }
                    else if (s1 == 0xD0 && s2 == 0x81) { *d++ = 0xD1; *d++ = 0x91; }
                    else { *d++ = s1; *d++ = s2; } }
                else {
                    if (s1 == 0xD1 && s2 >= 0x80 && s2 <= 0x8F) { *d++ = 0xD0; *d++ = s2 + 0x20; }
                    else if (s1 == 0xD0 && s2 >= 0xB0 && s2 <= 0xBF) { *d++ = 0xD0; *d++ = s2 - 0x20; }
                    else if (s1 == 0xD1 && s2 == 0x91) { *d++ = 0xD0; *d++ = 0x81; }
                    else { *d++ = s1; *d++ = s2; } }
                s += 2; } 
            else {
                int len = ((*s & 0xE0) == 0xC0) ? 2 : ((*s & 0xF0) == 0xE0) ? 3 : 4;
                while (len-- && *s) *d++ = *s++; } }
        else {
            unsigned char c = *s++;
            if (enc_mode == 1) {
                if (!up) {
                    if (c >= 192 && c <= 223) c += 32;
                    else if (c == 168) c = 184; }
                else {
                    if (c >= 224) c -= 32;
                    else if (c == 184) c = 168; } }
            if (c >= 192) { 
                *d++ = (c < 240) ? 0xD0 : 0xD1;
                *d++ = (c < 240) ? (c - 48) : (c - 112); }
            else if (c == 184 || c == 168) {
                *d++ = (c == 184) ? 0xD1 : 0xD0;
                *d++ = (c == 184) ? 0x91 : 0x81; }
            else *d++ = c; } }
    *d = '\0'; return (char*)start; }
int LoadDic(const char *path) {
    int fd = open(path, O_RDONLY); if (fd < 0) return 1;
    char name_tmp[1024]; size_t n_ptr = 0; long long price = 0; int has_p = 0, has_n = 0, last_t = 0; ssize_t bytes;
    while ((bytes = read(fd, ext_buf, BUF_A)) > 0) {
        ssize_t valid_bytes = bytes;
        if ((unsigned char)ext_buf[bytes - 1] >= 0x80) { int back = 1;
            while (back < 4 && (bytes - back) >= 0 && ((unsigned char)ext_buf[bytes - back] & 0xC0) == 0x80) back++;
            unsigned char first = (unsigned char)ext_buf[bytes - back];
            int expected = (first < 0xE0) ? 2 : (first < 0xF0) ? 3 : 4;
            if (back < expected) {
                valid_bytes = bytes - back;
                lseek(fd, -(back), SEEK_CUR); } }
        if (n_ptr == 0 && price == 0) AnalyzeFormat(ext_buf, valid_bytes);
        for (ssize_t i = 0; i < valid_bytes; i++) {
            unsigned char c = ext_buf[i];
            int curr_t = (c >= '0' && c <= '9') ? 1 : 
                         (c > 127 || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '.' || c == '_') ? 2 : 0;
            if (has_p && has_n && curr_t == 1 && last_t == 2) {
                name_tmp[n_ptr] = '\0';
                addDic((int)price, 0, UCase(name_tmp, 0));
                price = 0; n_ptr = 0; has_p = 0; has_n = 0; }
            if (curr_t == 1) { price = price * 10 + (c - '0'); has_p = 1; }
            else if (curr_t == 2) { 
                if (n_ptr < sizeof(name_tmp) - 1) { 
                    name_tmp[n_ptr++] = (c == '.' || c == '_') ? ' ' : (char)c; 
                    has_n = 1; } }
            else if (has_p && has_n) { 
                name_tmp[n_ptr] = '\0';
                addDic((int)price, 0, UCase(name_tmp, 0));
                price = 0; n_ptr = 0; has_p = 0; has_n = 0; }
            last_t = curr_t; } }
    if (has_p && has_n) { name_tmp[n_ptr] = '\0'; addDic((int)price, 0, UCase(name_tmp, 0)); }
    close(fd); enc_mode = 0; return dict_count; }
char* GetRow(int num, int price, int col, const char *name, int mode) {
    static char buffers[8][512];
    static int b_idx = 0; b_idx = (b_idx + 1) & 7;
    char *d = buffers[b_idx]; int c; int b = StringBC(name, &c);
    int name_w = max_s_chars + (b - c); char col_out[16];
    if (mode==3 && col < 1) { StrCpy(d, ""); return d; }
    if ((mode == 2 || mode == 3) && col < 2) StrCpy(col_out, "   ");
    else sprintf(col_out, "%2d ", col);
    switch (mode) {
        case 1:
        case 2: sprintf(d, "%s%02d %s%s %s%*d %s%-*s%s", 
                    ccou, num, 
                    ccol, col_out,
                    cpri, max_n_chars, price,
                    cnam, name_w, name, 
                    crst); break;
        case 3: sprintf(d, "%02d %s %*d %-*s", 
                    num, col_out, max_n_chars, price, name_w, name); break;
        case 4: sprintf(d, "%*d %s", max_n_chars, price, name); break;
        default: StrCpy(d, ""); break; }
    return d; }
int ExportDict(const char *path) {
    if (!dict || dict_count <= 0) return 0;
    int num=0;
    if (!path || *path == '\0' || StrCmp(path, "Full") == 0) {
        int mode = (path && StrCmp(path, "Full") == 0) ? 1 : 2;
        for (int i = 0; i < dict_count; i++) { if (mode == 2 && dict[i].col <= 0) continue;
        printf("%s\n", GetRow(++num, dict[i].price, dict[i].col, dict[i].name, mode)); }
        fflush(stdout); return num; }
    FILE *f = fopen(path, "w"); if (!f) return 0;
    int mode = (StrCmp(path, DB_NAME) == 0) ? 4 : 3;
    if (mode == 3) { int sum=0;
        for (int i = 0; i < dict_count; i++) {
            if (dict[i].col <= 0) continue;
            sum+=dict[i].price*dict[i].col; fprintf(f, "%s\n", GetRow(++num, dict[i].price, dict[i].col, dict[i].name, mode)); } 
            if (sum!=0) fprintf(f, "      %*d",max_n_chars+1,sum); }
    else for (int i = 0; i < dict_count; i++) fprintf(f, "%s\n", GetRow(++num, dict[i].price, dict[i].col, Replays(dict[i].name, " ", "_"), mode));
    fclose(f); return num; }
char* Trans(const char *src) {
    if (!src || !*src) return (char*)"";
    unsigned char c1 = (unsigned char)src[0];
    unsigned char c2 = (unsigned char)src[1];
    if (c2 == '\0') {
        if (c1 >= '0' && c1 <= '9') { char *b = GetBuf(); b[0] = c1; b[1] = '\0'; return b; }
        if ((c1 >= 'A' && c1 <= 'Z') || (c1 >= 'a' && c1 <= 'z')) return UCase(src, 0);
        return (char*)""; }
    if (c1 == 'S' && StrCmp(src, "Space") == 0) return (char*)" ";
    if (c1 == 0xD0 || c1 == 0xD1) if ((c1 == 0xD0 && (c2 == 0x81 || (c2 >= 0x90 && c2 <= 0xBF))) || (c1 == 0xD1 && (c2 == 0x91 || (c2 >= 0x80 && c2 <= 0x8F)))) {
                                      if (src[2] == '\0') return UCase(src, 0); }
    return (char*)src; }
char* prw(const char *str1, int i) {
    char *res = GetBuf();
    StrCpy(res, UCase(str1, 1));
    if (i < 0 || i >= dict_count) return res;
    const char *full_name = dict[i].name;
    size_t offset = StrLenB(str1);
    if (StrLenB(full_name) > offset) StrCpy(res + offset, full_name + offset);
    return res; }
static int LH[128];
static int kpcount=0;
int EditField(char *final, int *nprice, int *ncol, int npos) { 
    char input[128]={0}; int b,i,current,Pleft=0, Pnum=0, sum=0;
    kpcount = 0;
    b=Fpi(input,&i);
    current=i;
    while (1) {
        if (current!=-1) { if (sum==0) *nprice=dict[current].price; }
        else *nprice=sum;
        printf("%s%s",LoadCursor,GetRow(npos,*nprice,*ncol,prw(input,current),1)); fflush(stdout);
        usleep(200000);
        const char *key = Trans(GetKeyName());
        if (key && *key) {
            if (StrCmp(key, "Enter") == 0) { if (kpcount > 0 && *nprice!=0) {
                                                if (b > 0) StrCpy(final, dict[i].name);
                                                else { StrCpy(final, input); current=-1; }
                                                printf("%s%s",LoadCursor,GetRow(npos,*nprice,*ncol,final,1)); fflush(stdout);
                                                break; }
                                             else continue; }
            if (StrCmp(key, "Escape") == 0) { *nprice=0; printf("%s%s",LoadCursor,ClearCurEnd); fflush(stdout); break; }
            if (StrCmp(key, "Backspace") == 0 || StrCmp(key, "Delete") == 0) {
                if (kpcount>0) { kpcount--; input[LH[kpcount]]='\0'; b=Fpi(input,&i); current=i;}
                else sum=0;
                continue; }
            if (StrCmp(key, "Left") == 0) { Pleft=1; continue; }
            if (StrCmp(key, "Right") == 0 || StrCmp(key, "Tab") == 0) { 
                if (b > 0 && i >= 0 && i < dict_count) {
                    if (StrCmp(input, dict[i].name) == 0 && b > 1) { i++; b--; }
                    const char *full_name = dict[i].name;
                    int current_bytes = StrLenB(input);
                    int total_bytes = StrLenB(full_name);
                    if (total_bytes > current_bytes) { if (kpcount < 127) LH[kpcount++] = current_bytes;
                        StrCpy(input + current_bytes, full_name + current_bytes);
                        b = Fpi(input, &i);
                        current = i; } }
                continue; }
            if (StrLen(key) == 1 ) {
                if (key[0]>='0' && key[0]<='9') { if (Pleft ) { *ncol=key[0]-'0'; Pleft=0; Pnum=0; } 
                                                  else { if (Pnum==0) sum=0;
                                                          Pnum=1; int pl=GNL(sum); if (pl<max_n_chars) sum=sum*10+key[0]-'0'; 
                                                          *nprice=sum; }
                                                  continue; }
                else { Pleft=0; Pnum=0; if (StrLen(input)+1 <= max_s_chars) { LH[kpcount++] = StrLenB(input); StrCat(input, key);b=Fpi(input,&i); current=i; } } } }
        current=NextIdDown(i,b,current); }
    return current; }
void help(const char *full_path) {
    const char *name = full_path;
    int i = 0;
    while (full_path[i] != '\0') i++;
    for (int j = i - 1; j >= 0; j--) {
        if (full_path[j] == '/' || full_path[j] == '\\') {
            name = &full_path[j + 1];
            break; } }
    snprintf((char*)ext_buf, BUF_A,
             "%s    Program:%s %s\n"
             "%sDescription: Designed for creating a shopping list with quick item\n"
             "             selection and total cost estimates; stores new items in the\n"
             "             database for future use.\n"
             "     Author:%s Alexey Poznyakov\n"
             "%s    Contact:%s avp70ru@mail.ru\n",
             ctxt, cnam, name, ctxt, cnam, ctxt, cnam);
    printf("%s", ext_buf);
    snprintf((char*)ext_buf, BUF_A,
             "%sTo receive results by email:\n"
             "To get results by mail, you need to create a file %s%s%s and\n"
             "write %semail@domain.ext password%s. Note that the password is\n"
             "an %s'password for accessing mail from external applications'%s. Upon first \n"
             "run, the file will be encrypted and you will start receiving results.%s",
             ctxt, cnam, filesendID, ctxt, cnam, ctxt, cnam, ctxt, crst);
    printf("%s", ext_buf); }
    
int main(int argc, char *argv[]) {
    int opt;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'}, {0, 0, 0, 0} };
   while ((opt = getopt_long_only(argc, argv, "h?", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
            case '?':
                help(argv[0]);
                return 0;
            default:
                break; } }
    dict = NULL; 
    dict_count = 0; 
    dict_cap = 0;
    char nname[128] = {0};
    int num,pos,ncol,nprice,nomail;
    SWD();
    int smail=1;
    if (AutoEncryptOrValidate(filesendID)) smail=0;
    SetInputMode(1);
    printf("%s",HideCursor);
    num=LoadDic(DB_NAME);
    if (max_s_chars<10) max_s_chars=10;
    if (max_n_chars<4)  max_n_chars=4;
    while (1) {
        printf("%s",ClearScreen);
        num=ExportDict("");
        printf("%s",SaveCursor);
        ncol=1;
        nprice=0;
        nname[0]='\0';
        pos=EditField(nname,&nprice,&ncol,++num);
        if (nprice==0) break;
        if (pos==-1) addDic(nprice,ncol,nname);
        else { dict[pos].col=ncol; dict[pos].price=nprice; } }
    num=0; for (int i = 0; i < dict_count; i++) { if (dict[i].col > 0) num+=dict[i].price*dict[i].col; }
    if (num) { printf("      %s%*d%s",CBRed,max_n_chars+1,num,crst); fflush(stdout); }
    pos=ExportDict(DB_NAME);
    pos=ExportDict(fileres);
    if (smail) { pos=TxtToHtml(fileres, filesendhtml, filesendID); pos=SendMailSecure(filesendID,filesendhtml);
                    unlink(filesendhtml); if (pos) printf("\n%sОшибка отправки письма: %d\n", ccol, pos); }
    SetInputMode(0);
    printf("%s",ShowCursor);
    ClearDic(); return 0; }

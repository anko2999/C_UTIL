#include "BSCStr.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief 空の文字列を作る
/// @return 作成した文字列。メモリ不足ならNULL。使い終わったら BSCstr_free で解放する
BSCStr *BSCStr_new(void)
{
    BSCStr *s = malloc(sizeof *s);
    if (s == NULL)
    {
        return NULL;
    }
    s->cap = sizeof(BSCStr);
    s->len = 0;
    s->buf = malloc(s->cap);
    if (s->buf == NULL)
    {
        free(s);
        return NULL;
    }
    s->buf[0] = '\0';
    return s;
}

/// @brief 文字列を解放する
/// @param s (in) 解放する文字列(NULLなら何もしない)
void BSCstr_free(BSCStr *s)
{
    if (s == NULL)
    {
        return;
    }
    free(s->buf);
    free(s);
}

/// @brief 文字列を空にする。確保済みメモリは保持して再利用する
/// @param s (i/o) 対象の文字列
void BSCstr_clear(BSCStr *s)
{
    s->len = 0;
    s->buf[0] = '\0';
}

/// @brief 追加で extra バイト(NUL除く)入るように領域を確保する
/// @param s (i/o) 対象の文字列
/// @param extra (in) 追加したいバイト数(NUL除く)
/// @return 0: 成功, -1: メモリ不足(この場合文字列は元のまま)
static int reserve(BSCStr *s, size_t extra)
{
    size_t need = s->len + extra + 1; /* NULを含めた必要サイズ */
    if (need <= s->cap)
    {
        return 0;
    }

    size_t newcap = s->cap;
    while (newcap < need)
    {
        newcap *= 2;
    }

    char *p = realloc(s->buf, newcap);
    if (p == NULL)
    {
        return -1;
    }
    s->buf = p;
    s->cap = newcap;
    return 0;
}

/// @brief バイト列を末尾に追記する
/// @param s (i/o) 追記先の文字列
/// @param p (in) 追記するバイト列
/// @param n (in) 追記するバイト数
/// @return 0: 成功, -1: メモリ不足(この場合文字列は元のまま)
int BSCStr_append(BSCStr *s, const char *p, size_t n)
{
    if (reserve(s, n) != 0)
    {
        return -1;
    }
    memcpy(s->buf + s->len, p, n);
    s->len += n;
    s->buf[s->len] = '\0';
    return 0;
}

/// @brief NUL終端文字列を末尾に追記する
/// @param s (i/o) 追記先の文字列
/// @param cstr (in) 追記するNUL終端文字列(NULL不可)
/// @return 0: 成功, -1: メモリ不足(この場合文字列は元のまま)
int BSCStr_append_cstr(BSCStr *s, const char *cstr)
{
    return BSCStr_append(s, cstr, strlen(cstr));
}

/// @brief printf形式の書式で整形し、末尾に追記する
/// @param s (i/o) 追記先の文字列
/// @param fmt (in) printf形式の書式文字列
/// @param ... (in) 書式に対応する引数
/// @return 0: 成功, -1: 書式エラーまたはメモリ不足(この場合文字列は元のまま)
int BSCStr_printf(BSCStr *s, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    int n = vsnprintf(NULL, 0, fmt, ap); /* 必要長を測る */
    va_end(ap);
    if (n < 0)
    {
        return -1;
    }

    if (reserve(s, (size_t)n) != 0)
    {
        return -1;
    }

    va_start(ap, fmt);
    vsnprintf(s->buf + s->len, (size_t)n + 1, fmt, ap);
    va_end(ap);

    s->len += (size_t)n;
    return 0;
}

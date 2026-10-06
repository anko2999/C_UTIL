#ifndef __BS_C_STR__
#define __BS_C_STR__

#include <stddef.h>

/// @brief 可変サイズ文字列
typedef struct
{
    /// @brief 文字列バッファ(NUL終端)
    char *buf;
    /// @brief 文字列長(NUL終端を含まない)
    size_t len;
    /// @brief 確保済みメモリサイズ(NUL含む)
    size_t cap;
} BSCStr;

/// @brief 空の文字列を作る
/// @return 作成した文字列。メモリ不足ならNULL。使い終わったら BSCstr_free で解放する
BSCStr *BSCStr_new(void);

/// @brief 文字列を解放する
/// @param s (in) 解放する文字列(NULLなら何もしない)
void BSCstr_free(BSCStr *s);

/// @brief 文字列を空にする。確保済みメモリは保持して再利用する
/// @param s (i/o) 対象の文字列
void BSCstr_clear(BSCStr *s);

/// @brief バイト列を末尾に追記する
/// @param s (i/o) 追記先の文字列
/// @param p (in) 追記するバイト列
/// @param n (in) 追記するバイト数
/// @return 0: 成功, -1: メモリ不足(この場合文字列は元のまま)
int BSCStr_append(BSCStr *s, const char *p, size_t n);

/// @brief NUL終端文字列を末尾に追記する
/// @param s (i/o) 追記先の文字列
/// @param cstr (in) 追記するNUL終端文字列(NULL不可)
/// @return 0: 成功, -1: メモリ不足(この場合文字列は元のまま)
int BSCStr_append_cstr(BSCStr *s, const char *cstr);

/// @brief printf形式の書式で整形し、末尾に追記する
/// @param s (i/o) 追記先の文字列
/// @param fmt (in) printf形式の書式文字列
/// @param ... (in) 書式に対応する引数
/// @return 0: 成功, -1: 書式エラーまたはメモリ不足(この場合文字列は元のまま)
int BSCStr_printf(BSCStr *s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3))); /* gcc用。書式チェックが効く */

/// @brief 文字列の中身(NUL終端)を取得する
/// @param s (in) 対象の文字列
/// @return 文字列へのポインタ。追記や解放で無効になるため、保持しないこと
static inline const char *BSCStr_body(const BSCStr *s) { return s->buf; }

/// @brief 文字列の長さを取得する
/// @param s (in) 対象の文字列
/// @return 文字列長(NUL終端を含まない)
static inline size_t BSCStr_len(const BSCStr *s) { return s->len; }

#endif

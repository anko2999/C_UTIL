#include "BSCMap.h"

#include <string.h>

/// @brief 文字列のハッシュ値を計算する(FNV-1a)
/// @param s (in) ハッシュ対象の文字列(NULL不可)
/// @return ハッシュ値
static size_t hash_str(const char *s)
{
    size_t h = 14695981039346656037ULL; /* FNV のオフセット基準値 */
    while (*s != '\0')
    {
        h ^= (unsigned char)*s++; /* 1バイトずつ XOR してから */
        h *= 1099511628211u;      /* FNV の素数を掛ける */
    }
    return h;
}

/// @brief 文字列を malloc で複製する
/// @param s (in) 複製元の文字列(NULL不可)
/// @return 複製した文字列。メモリ不足ならNULL。解放は呼び出し側の責任
static char *dup_str(const char *s)
{
    size_t n = strlen(s) + 1; /* 終端の '\0' を含めた長さ */
    char *p = malloc(n);
    if (p != NULL)
    {
        memcpy(p, s, n);
    }
    return p;
}

/// @brief バケット数を変更し、全要素を入れ直す(リハッシュ)
/// @param m (i/o) 対象のマップ
/// @param newn (in) 新しいバケット数
/// @return 0: 成功, -1: メモリ不足(この場合マップは元のまま)
static int map_resize(BSMap *m, size_t newn)
{
    entry **nb = calloc(newn, sizeof *nb);
    if (nb == NULL)
    {
        return -1;
    }
    for (size_t i = 0; i < m->nbuckets; i++)
    {
        entry *e = m->buckets[i];
        while (e != NULL)
        {
            entry *next = e->next;                /* 付け替える前に次を退避 */
            size_t idx = hash_str(e->key) % newn; /* 新しいバケット番号 */
            e->next = nb[idx];                    /* 新バケットの先頭に挿入 */
            nb[idx] = e;
            e = next;
        }
    }
    free(m->buckets); /* 古い配列のみ解放(要素自体は再利用している) */
    m->buckets = nb;
    m->nbuckets = newn;
    return 0;
}

/// @brief 空のマップを作る(初期バケット数は16)
/// @return 作成したマップ。メモリ不足ならNULL。使い終わったら map_free で解放する
BSMap *map_new(void)
{
    BSMap *m = malloc(sizeof *m);
    if (m == NULL)
    {
        return NULL;
    }
    m->nbuckets = 16;
    m->count = 0;
    m->buckets = calloc(m->nbuckets, sizeof *m->buckets); /* 全バケットを NULL で初期化 */
    if (m->buckets == NULL)
    {
        free(m);
        return NULL;
    }
    return m;
}

/// @brief キーと値を登録する。既存キーなら値を上書きする
/// @param m (i/o) 対象のマップ
/// @param key (in) キー文字列(内部で複製されるので、呼び出し後に破棄してよい)
/// @param value (in) 登録する値(所有権は呼び出し側に残る)
/// @return 0: 成功, -1: メモリ不足
int map_put(BSMap *m, const char *key, void *value)
{
    size_t idx = hash_str(key) % m->nbuckets;
    /* 既存キーを探し、あれば値だけ上書きして終了 */
    for (entry *e = m->buckets[idx]; e != NULL; e = e->next)
    {
        if (strcmp(e->key, key) == 0)
        {
            e->value = value;
            return 0;
        }
    }

    if (m->count + 1 > m->nbuckets * 3 / 4)
    { /* 負荷率0.75で拡張 */
        if (map_resize(m, m->nbuckets * 2) != 0)
        {
            return -1;
        }
        idx = hash_str(key) % m->nbuckets; /* バケット数が変わったので再計算 */
    }

    /* 新しい要素を作って、バケットの先頭に追加する */
    entry *e = malloc(sizeof *e);
    if (e == NULL)
    {
        return -1;
    }
    e->key = dup_str(key); /* 呼び出し側のキーが消えても大丈夫なよう複製 */
    if (e->key == NULL)
    {
        free(e);
        return -1;
    }
    e->value = value;
    e->next = m->buckets[idx];
    m->buckets[idx] = e;
    m->count++;
    return 0;
}

/// @brief キーに対応する値を取得する
/// @param m (in) 対象のマップ
/// @param key (in) 検索するキー
/// @return キーに対応する値。見つからなければNULL
void *map_get(const BSMap *m, const char *key)
{
    size_t idx = hash_str(key) % m->nbuckets;
    for (entry *e = m->buckets[idx]; e != NULL; e = e->next)
    {
        if (strcmp(e->key, key) == 0)
        {
            return e->value;
        }
    }
    return NULL;
}

/// @brief キーを削除する。キーは解放するが、値は解放しない
/// @param m (i/o) 対象のマップ
/// @param key (in) 削除するキー
/// @return 1: 削除した, 0: 無かった
int map_remove(BSMap *m, const char *key)
{
    size_t idx = hash_str(key) % m->nbuckets;
    entry **pp = &m->buckets[idx]; /* 「自分を指しているポインタ」へのポインタ。先頭の削除も同じ処理で済む */
    while (*pp != NULL)
    {
        entry *e = *pp;
        if (strcmp(e->key, key) == 0)
        {
            *pp = e->next; /* リストから切り離す */
            free(e->key);
            free(e); /* value は解放しない(呼び出し側の責任) */
            m->count--;
            return 1;
        }
        pp = &e->next;
    }
    return 0;
}

/// @brief マップと全エントリを解放する。値(value)の解放は呼び出し側の責任
/// @param m (in) 解放するマップ(NULLなら何もしない)
void map_free(BSMap *m)
{
    if (m == NULL)
    {
        return;
    }
    for (size_t i = 0; i < m->nbuckets; i++)
    {
        entry *e = m->buckets[i];
        while (e != NULL)
        {
            entry *next = e->next; /* 解放前に次を退避 */
            free(e->key);
            free(e);
            e = next;
        }
    }
    free(m->buckets);
    free(m);
}

int map_foreach(const BSMap *m, map_iter_fn fn, void *user)
{
    /* 戻り値: 中断したコールバックの戻り値 / 最後まで回ったら0 */
    for (size_t i = 0; i < m->nbuckets; i++)
    {
        for (entry *e = m->buckets[i]; e; e = e->next)
        {
            int r = fn(e->key, e->value, user);
            if (r != 0)
                return r;
        }
    }
    return 0;
}
#ifndef __CMAP__
#define __CMAP__

#include <stdlib.h>

/// @brief ハッシュテーブルの1要素。同じバケットに入ったものは next で連結する(チェイン法)
typedef struct entry
{
    /// キー文字列(map側で複製して所有する)
    char *key;
    /// 値(所有権は呼び出し側にある)
    void *value;
    /// 同じバケット内の次の要素
    struct entry *next;
} entry;

/// @brief ハッシュマップ本体
typedef struct
{
    /// バケット配列(各要素は連結リストの先頭)
    entry **buckets;
    /// バケット数
    size_t nbuckets;
    /// 登録されている要素数
    size_t count;
} BSMap;

/// @brief 空のマップを作る(初期バケット数は16)
/// @return 作成したマップ。メモリ不足ならNULL。使い終わったら map_free で解放する
BSMap *map_new(void);

/// @brief キーと値を登録する。既存キーなら値を上書きする
/// @param m (i/o) 対象のマップ
/// @param key (in) キー文字列(内部で複製されるので、呼び出し後に破棄してよい)
/// @param value (in) 登録する値(所有権は呼び出し側に残る)
/// @return 0: 成功, -1: メモリ不足
int map_put(BSMap *m, const char *key, void *value);

/// @brief キーに対応する値を取得する
/// @param m (in) 対象のマップ
/// @param key (in) 検索するキー
/// @return キーに対応する値。見つからなければNULL
void *map_get(const BSMap *m, const char *key);

/// @brief キーを削除する。キーは解放するが、値は解放しない
/// @param m (i/o) 対象のマップ
/// @param key (in) 削除するキー
/// @return 1: 削除した, 0: 無かった
int map_remove(BSMap *m, const char *key);

/// @brief マップと全エントリを解放する。値(value)の解放は呼び出し側の責任
/// @param m (in) 解放するマップ(NULLなら何もしない)
void map_free(BSMap *m);

/// 全走査用コールバック関数の型
typedef int (*map_iter_fn)(const char *key, void *value, void *user);

/// 全走査用コールバック
int map_foreach(const BSMap *m, map_iter_fn fn, void *user);

#endif

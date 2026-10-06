# 本プロジェクトについて

- C言語の便利関数の作成、実験を行うことが目的
- ビルドは `mingw32-make`(Windows)。成果物は他プロジェクトへ `.h` / `.c` を持ち込んで使う

# コーディングルール

## コメント

- 関数ヘッダコメントは必ず付与する(Doxygen形式)
    - 引数には `(in)` / `(out)` / `(i/o)` を付ける
    - 公開関数は `.h` と `.c` の両方に同じコメントを書く(`static` 関数は `.c` のみ)

    ```c
    /// @brief 文字列を malloc で複製する
    /// @param s (in) 複製元の文字列(NULL不可)
    /// @return 複製した文字列。メモリ不足ならNULL。解放は呼び出し側の責任
    static char *dup_str(const char *s)
    ```

- 構造体、クラス、クラスメンバ変数等には必ずコメントを付与する
    - メンバのコメントは、メンバの上の行に `///` で書く

    ```c
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
    ```

## 書き方

- `if` / `while` / `for` の中身は、1行であっても必ず `{}` を付ける
- 条件にポインタを使うときは `if (p)` / `if (!p)` ではなく、`if (p != NULL)` / `if (p == NULL)` と書く

    ```c
    if (p != NULL)
    {
        memcpy(p, s, n);
    }
    ```

## ファイル

- 文字コードは UTF-8、改行コードは LF

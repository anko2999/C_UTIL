#include "BSCMap.h"
#include "BSCStr.h"
#include "stdio.h"

void MapSample();
void UstringSample();

int main(int argc, char **argv)
{
    MapSample();
    UstringSample();
    return 0;
}

static int print_entry(const char *key, void *value, void *user)
{
    printf("%s -> %p\n", key, value);
    return 0;
}

void MapSample()
{
    const char *data1 = "hello";
    const char *data2 = "world";

    // Map
    BSMap *m = map_new();

    const char *key1 = "key1";
    const char *key2 = "key2";
    map_put(m, key1, (void *)data1);
    map_put(m, key2, (void *)data2);
    char *v = (char *)map_get(m, key1);
    if (v != NULL)
    {
        printf("%s", v);
    }
    v = (char *)map_get(m, key2);
    if (v != NULL)
    {
        printf("%s", v);
    }

    // マップの全走査処理
    map_foreach(m, print_entry, NULL);

    map_remove(m, key1);

    map_free(m);
}

void UstringSample()
{
    BSCStr *s = BSCStr_new(); /* 確保 */

    BSCStr_printf(s, "code=%s", "1234567"); /* printf形式で追記 */
    BSCStr_append(s, ", ", 2);              /* バイト列を追記 */
    BSCStr_printf(s, "n=%d", 42);

    printf("%s\n", s->buf);      /* 中身(char*) */
    printf("len=%zu\n", s->len); /* 長さ */

    BSCstr_clear(s); /* 空にする（領域は再利用） */
    BSCstr_free(s);  /* 解放 */
}
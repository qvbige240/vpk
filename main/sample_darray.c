/**
 * History:
 * ================================================================
 * 2019-09-23 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "vpk.h"

static int hash_int(void *data)
{
    return (int)data;
}

static int cmp_int(void *ctx, void *data)
{
    return (int)ctx - (int)data;
}

static Ret print_int(void *ctx, void *data)
{
    printf("%d ", (int)data);

    return RET_OK;
}

static Ret check_and_dec_int(void *ctx, void *data)
{
    int *expected = (int *)ctx;
    //assert(*expected == (int)data);
    printf("%d[%d] %s", data, *expected, (*expected + 1) % 8 ? " " : "\n");

    //(*expected)--;
    (*expected)++;

    return RET_OK;
}

static void test_int_darray(void)
{
	int i = 0;
	int n = 100;
	int data = 0;
	vpk_darray_t* darray = vpk_darray_create(32, NULL, NULL);

    // for (i = 0; i < n; i++)
    // {
    //     //assert(vpk_darray_append(darray, (void *)i) == RET_OK);
    //     assert(vpk_darray_add(darray, NULL, (void *)i) == RET_OK);
    //     assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
    //     printf("%d[%d] %s", data, i, (i+1)%8 ? " " : "\n");
    // }
    // printf("\n");

    // for (i = 0; i < n; i++)
    // {
    //     if (i == 0)
    //     assert(vpk_darray_delete(darray, 0) == RET_OK);
    //     assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
    //     printf("%d[%d] %s", data, i, (i+1)%8 ? " " : "\n");
    // }
    // printf("\n");

    // for (i = 0; i < n; i++)
    // {
    //     assert(vpk_darray_set_by_index(darray, i, (void *)(2 * i)) == RET_OK);
    //     assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
    //     printf("%d[%d] %s", i, data, (i+1)%8 ? " " : "\n");
    // }
    // printf("\n");

    // for (i = 0; i < n; i++)
    // {
    //     assert(vpk_darray_set_by_index(darray, i, (void *)i) == RET_OK);
    //     assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
    //     printf("%d[%d] %s", data, i, (i+1)%8 ? " " : "\n");
    // }
    // printf("\n");

    // for (i = 0; i < n; i++)
    // {
    //     assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
    //     printf("%d[%d] %s", data, i, (i+1)%8 ? " " : "\n");
    // }
    // printf("\n");
    // return;

    for (i = 0; i < n; i++)
    {
        //assert(vpk_darray_append(darray, (void *)i) == RET_OK);
        assert(vpk_darray_add(darray, NULL, (void *)i) == RET_OK);
        assert(vpk_darray_count(darray) == (i + 1));
        assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
        if (data != i)
            printf("data %d, i %d\n", data, i);
        assert(data == i);
        assert(vpk_darray_set_by_index(darray, i, (void *)(2 * i)) == RET_OK);
        assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);

        assert(data == 2 * i);
        assert(vpk_darray_set_by_index(darray, i, (void *)i) == RET_OK);
        assert(vpk_darray_find(darray, cmp_int, (void *)i) == i);
        //assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
        //printf("%d[%d] %s", i, data, (i+1)%8 ? " " : "\n");
    }
    printf("\n");

    for (i = 0; i < n; i++)
    {
        //assert(vpk_darray_set_by_index(darray, i, (void *)i) == RET_OK);
        assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
        printf("%d[%d] %s", data, i, (i + 1) % 8 ? " " : "\n");
    }
    printf("\n");

    for (i = 0; i < n; i++)
    {
        //assert(vpk_darray_get_by_index(darray, 0, (void**)&data) == RET_OK);
        //assert(data == (i));
        assert(vpk_darray_count(darray) == (n - i));
        assert(vpk_darray_delete(darray, i) == RET_OK);
        assert(vpk_darray_count(darray) == (n - i - 1));

        // int j;
        // for (j = 0; j < i+10; j++)
        // {
        //     assert(vpk_darray_get_by_index(darray, j, (void **)&data) == RET_OK);
        //     printf("%d[%d] %s", data, j, (j+1)%8 ? " " : "\n");
        // }
        // printf("\n");

        if ((i + 1) < n)
        {
            //assert(vpk_darray_get_by_index(darray, 0, (void**)&data) == RET_OK);
            assert(vpk_darray_get_by_index(darray, i + 1, (void **)&data) == RET_OK);
            if (data != i + 1)
                printf("data %d, i+1 %d\n", data, i + 1);
            assert((int)data == (i + 1));
        }
    }

    assert(vpk_darray_count(darray) == 0);

    for (i = 0; i < n; i++)
    {
        //assert(vpk_darray_set_by_index(darray, i, (void *)i) == RET_OK);
        assert(vpk_darray_get_by_index(darray, i, (void **)&data) == -2); // index not exists
        printf("%d[%d] %s", data, i, (i + 1) % 8 ? " " : "\n");
    }
    printf("\n");

    // for (i = 0; i < 10; i++)
    // {
    //     assert(vpk_darray_add(darray, NULL, (void*)i) == RET_OK);
    // }
    // assert(vpk_darray_add(darray, NULL, (void*)77) == RET_OK);

    // for (i = 0; i < n; i++)
    // {
    //     //assert(vpk_darray_set_by_index(darray, i, (void *)i) == RET_OK);
    //     assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
    //     printf("%d[%d] %s", data, i, (i+1)%8 ? " " : "\n");
    // }
    // printf("\n");

    size_t index = 0;
    for (i = 0; i < n; i++)
    {
        //assert(vpk_darray_prepend(darray, (void*)i) == RET_OK);
        assert(vpk_darray_add(darray, &index, (void *)i) == RET_OK);
        assert(vpk_darray_count(darray) == (i + 1));
        assert(vpk_darray_get_by_index(darray, index, (void **)&data) == RET_OK);
        assert(data == i);
        assert(vpk_darray_set_by_index(darray, index, (void *)(2 * i)) == RET_OK);
        assert(vpk_darray_get_by_index(darray, index, (void **)&data) == RET_OK);
        assert(data == 2 * i);
        assert(vpk_darray_set_by_index(darray, index, (void *)i) == RET_OK);
    }

    for (i = 0; i < n; i++)
    {
        //assert(vpk_darray_set_by_index(darray, i, (void *)i) == RET_OK);
        assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
        printf("%d[%d] %s", data, i, (i + 1) % 8 ? " " : "\n");
    }
    printf("\n");

    index = 55;
    assert(vpk_darray_delete(darray, index) == RET_OK);
    //i = n - 1;
    i = 0;
    assert(vpk_darray_foreach(darray, check_and_dec_int, &i) == RET_OK);
    printf("\n");

    for (i = 0; i < n; i++)
    {
        //assert(vpk_darray_set_by_index(darray, i, (void *)i) == RET_OK);
        if (i == index)
            assert(vpk_darray_get_by_index(darray, i, (void **)&data) == -2); // slot not exists
        else
            assert(vpk_darray_get_by_index(darray, i, (void **)&data) == RET_OK);
        printf("%d[%d] %s", data, i, (i + 1) % 8 ? " " : "\n");
    }
    printf("\n");

    vpk_darray_destroy(darray);

    return;
}

static int main_linear(int argc, char *argv[])
{
    test_int_darray();
    return 0;
}

#ifdef USE_ZLOG
#define SAMPLE_ZLOG_FILE_PATH           "."
#define SAMPLE_ZLOG_CONF_FILE           "./zlog.conf"
int sample_zlog_init(int procname)
{
    int rc;

    if (!vpk_exists(SAMPLE_ZLOG_FILE_PATH)) {
        int ret = 0;
        char tmp[256] = {0};
        vpk_pathname_get(SAMPLE_ZLOG_FILE_PATH, tmp);
        printf("full: %s, pathname: %s", SAMPLE_ZLOG_FILE_PATH, tmp);
        ret = vpk_mkdir_mult(SAMPLE_ZLOG_FILE_PATH);
        printf("vpk_mkdir_mult \'%s\' ret = %d\n", SAMPLE_ZLOG_FILE_PATH, ret);
    }

    rc = dzlog_init(SAMPLE_ZLOG_CONF_FILE, "sample");
    if (rc) {
        printf("zlog init failed\n");
        return -1;
    }

    LOG_D("hello, zlog");

    return 0;
}
#endif

int main(int argc, char *argv[])
{
    //int ret = 0;

#ifdef USE_ZLOG
    sample_zlog_init(0);
#endif // USE_ZLOG

    main_linear(argc, argv);

    return 0;
}

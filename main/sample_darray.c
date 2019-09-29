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
	long data = 0;
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
    //     //if (i == 0)
    //     //assert(vpk_darray_delete(darray, 0) == RET_OK);
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

/******************** darray list ********************/
struct sample_server_info
{
    unsigned int        id;
    char                name[64];
    char                system[64];     /* system release type */
    char                location[64];   /* place */
    char                processor[64];  /* cpu model */
    unsigned long       bandwidth;      /* network */
    unsigned long       memory;         /* memory total KB */
};

typedef struct _PrivInfo
{
    struct sample_server_info info;
} PrivInfo;

static void print_object(da_object_t *object)
{
    if (object)
    {
        DECL_PRIV(object, priv);
        LOG_D("object  id name memory  slot[%d]( %d %s %lu )",
              object->slot, priv->info.id, priv->info.name, priv->info.memory);
    }
}
static int data_destroy(void *ctx, void *data)
{
    da_object_t *object = data;
    if (object)
    {
        LOG_D("free object %p slot[%d]", object, object->slot);
        free(object);
    }

    return 0;
}
static int data_visit(void *ctx, void *data)
{
    da_object_t *object = data;
    if (object)
    {
        DECL_PRIV(object, priv);
        LOG_D("object  id name memory  slot[%d]( %d %s %lu )",
              object->slot, priv->info.id, priv->info.name, priv->info.memory);
    }
    return 0;
}
static int data_modified(void *ctx, void *data)
{
    da_object_t *object = data;
    if (object)
    {
        DECL_PRIV(object, priv);
        strcat(priv->info.name, "modified first");
        LOG_D("object  id name memory  slot[%d]( %d %s %lu )",
              object->slot, priv->info.id, priv->info.name, priv->info.memory);
    }
    return 0;
}
static int list_data_destroy(void *ctx, void *data)
{
    da_object_t *object = data;
    if (object)
    {
        LOG_D("list free object %p slot[%d]", object, object->slot);
        vpk_dalist_delete(ctx, object->slot);
        printf("=========free\n");
        //free(object);
    }

    return 0;
}
#define SAMPLE_LIST_COUNT_AND_FOREACH                  \
    {                                                  \
        count = vpk_dalist_count(list);                \
        LOG_D("list count %d\n ==> for each:", count); \
        vpk_dalist_foreach(list, data_visit, NULL);    \
    }

static void test_darray_list()
{
    vpk_dalist_t *list = vpk_dalist_create(32, data_destroy, NULL);
    int i = 0;
    int n = 10;
    da_object_t *object[10];

    for (i = 0; i < n; i++)
    {
        object[i] = calloc(1, sizeof(da_object_t) + sizeof(PrivInfo));
        DECL_PRIV(object[i], priv);
        priv->info.id = i;
        sprintf(priv->info.name, "%s%d", "server", i);
        priv->info.memory = ((2UL + i) << 30);

        size_t slot = 0;
        vpk_dalist_add(list, &slot, object[i]);
        LOG_D("add object %p to slot %d", object[i], slot);
        object[i]->slot = slot;
    }

    size_t count;
    SAMPLE_LIST_COUNT_AND_FOREACH;

    printf("\n");
    da_object_t *target;
    size_t slot = 5;
    vpk_dalist_get_by_index(list, slot, &target);
    print_object(target);
    vpk_dalist_modify_by_index(list, slot, data_modified, NULL);

    da_object_t *tmp_object = calloc(1, sizeof(da_object_t) + sizeof(PrivInfo));
    memcpy(tmp_object, object[2], sizeof(da_object_t) + sizeof(PrivInfo));

    vpk_dalist_delete(list, object[7]->slot);
    vpk_dalist_delete(list, object[7]->slot);
    vpk_dalist_delete(list, object[2]->slot);
    vpk_dalist_delete(list, object[9]->slot);

    target = NULL;
    slot = object[2]->slot;
    if (vpk_dalist_get_by_index(list, slot, &target) == 0)
        print_object(target);
    else
        printf("=== get by index(%d) failed.\n\n", slot);

    SAMPLE_LIST_COUNT_AND_FOREACH;

    slot = 0;
    vpk_dalist_add(list, &slot, tmp_object);
    LOG_D("add object %p to slot %d", tmp_object, slot);
    tmp_object->slot = slot;

    SAMPLE_LIST_COUNT_AND_FOREACH;

    LOG_D("list count %d\n ==> for each destroy", count);
    vpk_dalist_foreach(list, list_data_destroy, list);

    for (i = 0; i < n; i++)
    {
        object[i] = calloc(1, sizeof(da_object_t) + sizeof(PrivInfo));
        DECL_PRIV(object[i], priv);
        priv->info.id = i;
        sprintf(priv->info.name, "%s%d", "server2", i);
        priv->info.memory = ((2UL + i) << 30);

        size_t slot = 0;
        vpk_dalist_add(list, &slot, object[i]);
        LOG_D("add object %p to slot %d", object[i], slot);
        object[i]->slot = slot;
    }

    SAMPLE_LIST_COUNT_AND_FOREACH;

    printf("\n\nclone list:\n");
    {
        list_t *head = vpk_dalist_clone_list(list, sizeof(da_object_t) + sizeof(PrivInfo));
        da_object_t *obj;
        list_t *pos, *n;
        list_for_each_safe(pos, n, head)
        {
            obj = container_of(pos, da_object_t, node);
            if (obj)
            {
                list_del(pos);
                print_object(obj);
                free(obj);
            }
        }
    }
    printf("\n\n");

    vpk_dalist_destroy(list);
}

/******************** struct test ********************/
typedef struct _test_placeholder
{
    int a;
    char b[5];
    char priv[0];
} test_placeholder;

#define test_svr_info_object()       \
    char c[6]; \
    unsigned int        id;                                             \
    char                name[16];                                       \
    char                system[15];     /* system release type */       \
    char                processor[14];  /* cpu model */                 \
    unsigned long       bandwidth;      /* network */                   \
    unsigned long       memory;         /* memory total KB */

    //char                location[64];   /* place */

#pragma pack(1)
struct test_server_info1
{
    test_svr_info_object();
};
#pragma pack()

typedef struct _MyPrivInfo
{
    struct test_server_info1 info;
} MyPrivInfo;

#pragma pack(1)
typedef struct test_server_info
{
    char b[5];
    test_svr_info_object();
} test_server_info;
#pragma pack()

void print_server_info(test_server_info *s)
{
    printf("addr: %p\n", s);
    printf("id: %d,\nname: %s,\nsystem: %s,\n"
           "processor: %s,\nbandwidth: %ld,\nmemory: %ld\n",
           s->id, s->name, s->system, s->processor, s->bandwidth, s->memory);
}
void test_place_holder()
{
    test_placeholder *obj = calloc(1, sizeof(test_placeholder) + sizeof(MyPrivInfo));
    obj->a = 1;
    strcpy(obj->b, "bbbb");
    MyPrivInfo *priv = obj->priv;

    strcpy(priv->info.c, "ccccc");
    priv->info.id = 1;
    strcpy(priv->info.name, "server1");
    strcpy(priv->info.system, "ubuntu");
    strcpy(priv->info.processor, "i7 9900k");
    priv->info.bandwidth = 200ul << 20;
    priv->info.memory = 8ul << 30;

    print_server_info((void *)obj + sizeof(int));
    //print_server_info(obj->priv);
}

static int main_linear(int argc, char *argv[])
{
    test_int_darray();
    printf("\n\n");
    printf(" ========== darray list test ==========\n");
    test_darray_list();
    printf("\n\n");
    test_place_holder();
    printf("\n");
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

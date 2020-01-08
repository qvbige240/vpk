/**
 * History:
 * ================================================================
 * 2015-04-02 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <stdarg.h>
// #include <pthread.h>

#include "vpk.h"
#include "jansson.h"

static int jsonf_item_set(char *file, char *key, int value)
{
    json_t *json_root = NULL;
    if (vpk_mmap_exist(file))
    {
        json_root = json_load_file(file, 0, NULL);
    }
    else
    {
        LOG_I("file not exist, and new json_root");
        json_root = json_object();
    }

    if (json_root)
    {
        json_t *jobject;

        jobject = json_object_get(json_root, key);
        if (!jobject)
        {
            LOG_W("jobject not exist, and new json object.");
            json_object_set(json_root, key, json_integer(value));

            char *update_dump = json_dumps(json_root, 0); /* to buffer */

            json_dump_file(json_root, file, 0); /* to file */

            LOG_D("update_dump setting:\n%s\n", update_dump);

            free(update_dump);
        }
        else
        {
            int curr_val = json_integer_value(jobject);
            if (curr_val != value)
            {
                json_object_set(json_root, key, json_integer(value));

                char *update_dump = json_dumps(json_root, 0); /* to buffer */

                json_dump_file(json_root, file, 0); /* to file */

                LOG_D("update_dump setting:\n%s\n", update_dump);

                free(update_dump);
            }
            else
            {
                LOG_D("no value change at \'%s\'", key);
            }
        }

        json_decref(json_root);
    }

    return 0;
}

static int json_item_set(char *data, char *key, int value)
{
    json_t *json_root = NULL;
    json_root = json_loads(data, 0, NULL);
    if (json_root)
    {
        json_t *jobject;

        jobject = json_object_get(json_root, key);
        if (!jobject)
            goto json_parse_end;

        int curr_val = json_integer_value(jobject);
        if (curr_val != value)
        {
            json_object_set(json_root, key, json_integer(value));

            char *update_dump = json_dumps(json_root, 0); /* to buffer */

            json_dump_file(json_root, "./dev_setting_pc.json", 0); /* to file */

            LOG_D("update_dump setting:\n%s\n", update_dump);

            free(update_dump);
        }

        json_decref(json_root);
        return 0;

    json_parse_end:
        LOG_E("\n ======= json_object_get error!!!! =======\n");
        json_decref(json_root);
    }

    return -1;
}

int test_findstr(const char *str)
{
    int r = 0;
    char buf[64];
    char *string = "ucustom -picsizeset MENUMOCK 1";

    char s1[64], s2[64];
    char *s = "1try234delete5";
    sscanf(s, "1%[^2]234%[^5]", s1, s2);
    LOG_D("s1: \'%s\', s2: \'%s\'", s1, s2);

    char *p = strstr(string, "MENUMOCK");
    if (p != NULL)
    {
        r = sscanf(string, "ucustom -%s MENUMOCK\n", buf);
        p = strstr(buf, "set");
        if (p)
        {
            int pos = p - buf;
            buf[pos] = '\0';
            LOG_D("buf: \'%s\'", buf);
            p = strstr(string, "MENUMOCK");
            if (p)
            {
                p = p + strlen("MENUMOCK");
                int value = atoi(p);

                LOG_D("p = %s, value(atoi) = %d", p, value);
            }
        }

        r = r;
    }

    printf("\n");

    return 0;
}

void test_sscanf_sprintf(void)
{
    double a = 0.0;
    double lat = 1.123456;
    char string[16] = {0};

    LOG_D("lat: %lf, a: %lf", lat, a);
    sprintf(string, "%lf", lat);
    LOG_D("string: %s", string);
    sscanf(string, "%s", (char *)&a);
    LOG_D("lat: %lf, a: %lf", lat, a);
    a = atof(string);
    LOG_D("lat: %lf, a: %lf", lat, a);
    a = atof("1");
    LOG_D("lat: %lf, a: %lf \n", lat, a);
}
static const char *const tele_keyword[][4] =
{
    {"MOBILE", "CMCC", "46000", "1"},
    {"UNICOM", "CUCC", "46001", "2"},
    {"MOBILE", "CMCC", "46002", "1"},
    {"TELECOM", "CTC", "46003", "3"},
    {"MOBILE", "CMCC", "46004", "1"},
    {"TELECOM", "CTC", "46005", "3"},
    {"UNICOM", "CUCC", "46006", "2"},
    {"MOBILE", "CMCC", "46007", "1"},
    {"UNICOM", "CUCC", "46009", "2"},
    {"TELECOM", "CTC", "46011", "3"},
    {"TIETONG", "CTTC", "46020", "0"},
};
void test_str(void)
{
    int i = 0, ret = 0;
    char *str1 = "";
    char str[] = ";;;sd;fdfs.adf.;;;";
    LOG_D("str1 len: %d", strlen(str1));
    LOG_D("before str: %s", str);
    char *p = vpk_strstrip(str, ';');
    LOG_D("after str: %s, p: %s", str, p);

    i = atoi(" 2");
    LOG_D("i(atoi) = %d\n", i);
    {
        char cur_ver[] = "K40.20170612.01";
        char new_ver[] = "K40.20170612.01";
        ret = strncmp(new_ver, "K40", strlen("K40"));
        LOG_D("ret = %d\n", ret);

        char *p = strstr(new_ver, ".");
        LOG_D("p: %s", p);
        p = p + 1;
        i = atoi(p);
        LOG_D("p: %s, i(atoi) = %d\n", p, i);

        ret = strcasecmp(new_ver, cur_ver);
        LOG_D("new_ver cmp cur_ver, ret = %d", ret);

#define TIMA_PROJECT "K40"
        char p1[16] = {0}, p2[16] = {0};
        //int p1, p2;
        //sscanf(cur_ver, TIMA_PROJECT".%[0-9].%[0-9]", p1, p2);
        char tmp[16];
        sscanf(cur_ver, "%[^.].%[0-9].%[0-9]", tmp, p1, p2);
        LOG_D("cur_ver: %s, tmp=%s", cur_ver, tmp);
        LOG_D("p1(len=%d): %s, p2(len=%d): %s", strlen(p1), p1, strlen(p2), p2);
    }

    const char *const_str = "1111111111";

    const_str = "const string test";
    LOG_D("const_str: %s", const_str);

    i = _countof(tele_keyword);
    LOG_D("_countof i: %d", i);

    const char *architecture = vpk_chip_architecture();
    LOG_D("architecture: %s", architecture);

    printf("\n");
}

void test_split_comma(void)
{
    int len = 0;
    char buff[256] = {0};
    char srcstrip[256] = {0};
    char *src = "2017_0912_000009_050A.TS, 2017_0912_000009_050B.TS,";
    char *p, *ptr = src;

    strcpy(srcstrip, src);
    vpk_strstrip(srcstrip, ' ');
    ptr = srcstrip;
    len = strlen(srcstrip);

    if (!src || strlen(src) == 0)
        return;

    while (ptr < srcstrip + len)
    {
        p = strstr(ptr, ",");
        if (p == NULL)
        {
            strcpy(buff, ptr);
            LOG_D("buff: %s", buff);
            break;
        }

        strncpy(buff, ptr, p - ptr);
        LOG_D("buff: %s", buff);
        ptr = p + 1;
    }
}

void test_null(void)
{
    //char buffer[64] = {0};
    char *str = NULL;
    int ret = -1;
    if (str && strlen(str))
        ret = strlen(str);
    //memcpy(buffer, NULL, 0);
    //strcpy(buffer, NULL);

    LOG_D("ret len: %d", ret);
    ret = strcasecmp("1", "");
    LOG_D("ret cmp: %d", ret);
    //const char* desc = "hello";
    const char *desc = NULL;
    LOG_D("desc: %s", desc);
    if (!desc)
        desc = "Device Exception Parse Error!";
    LOG_D("desc: %s", desc);

    {
        str = malloc(20);
        strcpy(str, "hello world");
        LOG_D("str: %s", str);
        free(str);
        LOG_D("first free str, str[%p] = %s", str, str);
        // cann't multi-free
        // *** Error in `./test': double free or corruption (fasttop): 0x00000000016978d0 ***
        //free(str);
        //LOG_D("second free str, str[%p] = %s", str, str);
        //free(str);
        //str = NULL;
        //LOG_D("third free str, str[%p] = %s", str, str);
    }

    {

        int count = 5;
        char *version = "v1.01";
        int *p[count];
        p[0] = (int *)1;
        p[1] = (int *)version;
        LOG_D("p[0]: %d, p[1]: %s", (int)(p[0]), (char *)(p[1]));

        char *wifiphrase = malloc(64);
        memcpy(wifiphrase, "testhello", 10);
        void **menu = malloc(count * sizeof(int *));
        menu[0] = (int *)3;
        menu[1] = (int *)version;
        menu[2] = (int *)wifiphrase;
        LOG_D("menu[0]: %d, menu[1]: %s, menu[2]: %s", (int)(menu[0]), (char *)(menu[1]), (char *)(menu[2]));
    }
    printf("\n");
}

typedef struct _MediaListTest
{
    char file[NAME_MAX + 1];
} MediaListTest;

typedef struct _MediaResTest
{
    int count;
    MediaListTest media[4];
} MediaResTest;

#define SAMPLE_ROOTCFG "/etc/sample/"
#define SAMPLE_SUBCFG "/media/ro/"
//#define MEDIA_GROUP_DIR		SAMPLE_ROOTCFG""(res.media->file)""SAMPLE_SUBCFG
#define DPRINTSTR(x) #x
#define MEDIA_GROUP_DIR SAMPLE_ROOTCFG "" DPRINTSTR(res.media->file) "" SAMPLE_SUBCFG

void test_struct(void)
{
    MediaResTest res = {0};
    int size = sizeof(res.media);
    LOG_D("struct size = %d", size);
    printf("\n");
#define HELLOWORLD
#if defined(HELLOWORLD)
    LOG_D("if defined(HELLOWORLD)");
#else
#endif

#ifdef HELLOWORLD
    LOG_D("ifdef HELLOWORLD");
#else
#endif

#ifdef MACRODEF_TEST
    LOG_D("ifdef MACRODEF_TEST");
#else
    LOG_D("not ifdef MACRODEF_TEST");
#endif

    strcpy(res.media->file, "DIRDVR");

    LOG_D("dir: %s", MEDIA_GROUP_DIR);

    printf("\n");
}

int test_snprintf()
{
    char param[32] = {0};
    char action[128] = {0};
    char action_2[128] = {0};
    char action_3[128] = {0};
    int pos = 0;
    strncpy(action, "ucustom -gpsinfo ", strlen("ucustom -gpsinfo "));
    printf("prev action: %s\n", action);
    snprintf(action, sizeof(action), "%s %s", action, "open");
    printf("next action: %s\n", action);

    strncpy(action_2, "ucustom -gpsinfo ", strlen("ucustom -gpsinfo "));
    printf("prev action_2: %s\n", action_2);
    sprintf(action_2, "%s %s", action_2, "open");
    printf("next action_2: %s\n", action_2);
    sprintf(action_2, "%s", "open");
    printf("next action_2: %s\n", action_2);
    sprintf(action_2, "%s %s", action_2, param);
    printf("next action_2 param: %s\n", action_2);

    strncpy(action_3, "ucustom -gpsinfo ", strlen("ucustom -gpsinfo "));
    printf("prev action_3: %s\n", action_3);
    //ige_str_snprintf(action_3, &pos, sizeof(action_3), "%s %s", action_3, "open");
    pos = strlen(action_3);
    vpk_snprintf(action_3, (unsigned int *)&pos, sizeof(action_3), " %s", "open");
    printf("next action_3: %s\n", action_3);

    //char param1[32] = {0};
    printf("sprintf test string\n\n");
    //sprintf(param1, "%s", NULL);		// 'Segmentation fault' will happen

    return 0;
}

static int file_read_test()
{
    int result = 0;
    char buffer[1024] = {0};
    FILE *fp = fopen("11.txt", "rb");

    int cnt = 0;
    while (1)
    {

        result = fread(buffer, 1, 256, fp);
        if (result < 0)
        {
            printf("read file end!");
            break;
        }

        printf("cnt = %d, result = %d\n", cnt++, result);
    }

    return 0;
}
int read_data(char *filename)
{
#define MAX_DATA_CHUNK_SIZE 256
    FILE *fp = 0;
    size_t ret = 0, offset = 0, size = MAX_DATA_CHUNK_SIZE;
    int file_size = 0;
    char data[MAX_DATA_CHUNK_SIZE + 1] = {0};

    return_val_if_fail(filename != NULL, -1);
    fp = fopen(filename, "r");
    return_val_if_fail(fp != NULL, -1);
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    size = (offset + size) <= file_size ? size : file_size;
    LOG_D("file size: %d, unit size: %d", file_size, size);

    int cnt = 0;
    while (offset < file_size)
    {
        fseek(fp, offset, SEEK_SET);
        //ret = fread(data, size, 1, fp);
        ret = fread(data, 1, size, fp);
        if (ret < 0)
        {
            LOG_E("fread file error.");
            break;
        }
        //*((char *)data + size) = '\0';

        LOG_D("offset: %d, ret: %d, file size: %d, cnt: %d", offset, ret, file_size, cnt++);
        offset += ret;
    }

    fclose(fp);

    return 0;
}

int string_main(int argc, char *argv[])
{
    // 	int ret = 0;

    // 	vpk_system_init(argc, argv);
    // 	vpk_logging_level_set("DEBUG");

    char *menus = "{\"photo_resolution\": 0, \"video_resolution\": 0, \"video_time\": 0, \"vibration_sensor\": 0, \"parking_mode\": 0, \"off_voltage\": 0, \"record_sound\": 0, \"HDR\": 0, \"time_watermark\": 0, \"upgrade\": 0, \"switch_machine\": 0, \"car_warning\": 0, \"driving\": 0, \"abnormal_equipment\": 0, \"format_tf\": 0, \"factory_reset\": 0, \"version\": \"0\"}";
    char *data = malloc(strlen(menus) + 1);
    strcpy(data, menus);
    //LOG_D("data: %s\n", data);
    test_split_comma();
    test_snprintf();
    test_struct();
    test_str();
    test_null();
    test_sscanf_sprintf();
    test_findstr(NULL);
    jsonf_item_set("./setting_pc.json", "photo_resolution", 1);
    jsonf_item_set("./setting_pc.json", "photo_resolution", 2);

    json_item_set(data, "photo_resolution", 1);

    return 0;
}

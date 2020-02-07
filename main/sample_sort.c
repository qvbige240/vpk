/**
 * History:
 * ================================================================
 * 2017-06-12 qing.zou created
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>

#include "vpk.h"

#if defined(_X86_)
#else
#endif

#include <errno.h>

static int compare_desc(const void *a, const void *b)
{
    return ((int *)a)[0] - ((int *)b)[0];
}
static int compare_asc(const void *a, const void *b)
{
    return ((int *)b)[0] - ((int *)a)[0];
}

static int test_sort()
{
    int nsize = 0, i = 0;
    int array_num[] = {5, 9, 2, 4, 15, 28, 6, 3, 40, 7, 5, 1, 8};

    nsize = _countof(array_num);
    printf("\n\nselection sort\nbefore sort: \n");
    for (i = 0; i < nsize; i++)
        printf("%2d ", array_num[i]);

    printf("\nafter desc sort: \n");
    vpk_ssort(array_num, nsize, sizeof(int), compare_desc);

    for (i = 0; i < nsize; i++)
        printf("%2d ", array_num[i]);

    printf("\nafter asc sort: \n");
    vpk_ssort(array_num, nsize, sizeof(int), compare_asc);

    for (i = 0; i < nsize; i++)
        printf("%2d ", array_num[i]);
    printf("\n\n");

    return 0;
}

static int test_merge_sort()
{
    int nsize = 0, i = 0;
    int array_num[] = {5, 9, 2, 4, 15, 28, 6, 3, 40, 7, 5, 1, 8};

    nsize = _countof(array_num);
    void *tmp = calloc(1, nsize * sizeof(int));
    printf("\n\nmerge sort\nbefore sort: \n");
    for (i = 0; i < nsize; i++)
        printf("%2d ", array_num[i]);

    printf("\nafter desc sort: \n");
    merge_sort(array_num, 0, nsize - 1, sizeof(int), tmp, compare_desc);

    for (i = 0; i < nsize; i++)
        printf("%2d ", array_num[i]);

    printf("\nafter asc sort: \n");
    merge_sort(array_num, 0, nsize - 1, sizeof(int), tmp, compare_asc);

    for (i = 0; i < nsize; i++)
        printf("%2d ", array_num[i]);
    printf("\n\n");

    return 0;
}

int sort_main(int argc, char *argv[])
{
    //int ret = 0;

    //vpk_system_init(argc, argv);
    //vpk_logging_level_set("DEBUG");

    char *type = "bubble";
    //if (argc > 1)
    //{
    //	type = argv[1];
    //}

    int o;
    static const struct option long_options[] = {
        { "help",        no_argument,            NULL,   'h' },
        { "version",     no_argument,            NULL,   'V' },
        { "sample",      required_argument,      NULL,   'd' },
        { "type",        required_argument,      NULL,   't' },
        { "keycode",     required_argument,      NULL,   'k' },
        { "file",        required_argument,      NULL,   'f' },
        { "url",         required_argument,      NULL,   'u' },
        { "log",         optional_argument,      NULL,   'l' },
        { NULL,          0,                      NULL,   0   }
    };

    optind = 1;
    //LOG_I("22 optind = %d, argc = %d", optind, argc);
    while ((o = getopt_long(argc, argv, "hVd:t:k:f:u:l", long_options, NULL)) >= 0)
    {
        //printf("opt = %c\n", o);
        //printf("optarg = %s\n", optarg);
        //printf("optind = %d\n", optind);
        //printf("argv[optind - 1] = %s\n",  argv[optind - 1]);
        switch (o)
        {
        case 't':
            type = optarg;
            break;
        default:
            break;
        }
    }

    LOG_D("type = %s", type);

    double elapsed;
    struct timeval result, prev, next;

    gettimeofday(&prev, 0);

    test_sort();

    test_merge_sort();

    gettimeofday(&next, 0);
    vpk_timersub(&next, &prev, &result);
    //time_sub(&result, &prev, &next);
    elapsed = result.tv_sec + (result.tv_usec / 1.0e6);
    LOG_D("vpk time elapsed: %.6f, %d(s) %d(us) \n", elapsed, result.tv_sec, result.tv_usec);

    return 0;
}

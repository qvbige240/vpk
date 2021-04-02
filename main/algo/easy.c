#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/** maximum 6 9 number: num = 9669 -> 9969 **/
static int max_69number(int num)
{
    int i = 1, n = num;
    int flag = 0, out = num;
    int high, low, remain;

    while (n > 0)
    {
        remain = n % 10;
        n = n / 10;

        if (remain == 6)
            flag = i;

        i *= 10;
    }

    if (flag == 0)
        return out;

    high = num / (flag * 10) * (flag * 10);
    low = num % flag;
    out = high + 9 * flag + low;

    // printf("in = %d, out = %d\n", num, out);

    return out;
}
static void max_69num_leet()
{
    int in = 9999;
    int out = max_69number(in);

    printf("in = %d, out = %d\n", in, out);
}

/** leet 1 **/
static int *two_sum(int *nums, int size, int target, int *rsize)
{
    // int size = 0;
    int answer_size = *rsize;
    int *answer = malloc(answer_size);
    int i = 0, j = 0;

    for (i = 0; i < size; i++)
    {
        for (j = i + 1; j < size; j++)
        {
            if ((nums[i] + nums[j]) == target)
            {
                printf(" %d, %d\n", i, j);
                *(answer + 0) = i;
                *(answer + 1) = j;
                break;
            }
        }
    }

    return answer;
}
static void two_sum_leet_1()
{
    int num[] = {2, 7, 11, 15};
    int rsize = 2;
    printf("leet 1: ");
    two_sum(num, sizeof(num) / sizeof(num[0]), 13, &rsize);
}

// struct hashTable {
//     int key;
//     int val;
//     UT_hash_handle hh;
// };

// struct hashTable* hashtable;

// struct hashTable* find(int ikey) {
//     struct hashTable* tmp;
//     HASH_FIND_INT(hashtable, &ikey, tmp);
//     return tmp;
// }

// void insert(int ikey, int ival) {
//     struct hashTable* it = find(ikey);
//     if (it == NULL) {
//         struct hashTable* tmp = malloc(sizeof(struct hashTable));
//         tmp->key = ikey, tmp->val = ival;
//         HASH_ADD_INT(hashtable, key, tmp);
//     } else {
//         it->val = ival;
//     }
// }

// int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
//     hashtable = NULL;
//     for (int i = 0; i < numsSize; i++) {
//         struct hashTable* it = find(target - nums[i]);
//         if (it != NULL) {
//             int* ret = malloc(sizeof(int) * 2);
//             ret[0] = it->val, ret[1] = i;
//             *returnSize = 2;
//             return ret;
//         }
//         insert(nums[i], i);
//     }
//     *returnSize = 0;
//     return NULL;
// }

/** leet 7 **/
static int reverse(int x)
{
    int reverse = 0, remain, n;
    int min = 0x80000000; // 1 << 31
    int max = 0x80000000 - 1;
    int max_remain = max % 10;
    int max_val = max / 10;

    if (x == min || x == max || x == 0)
        return 0;

    n = abs(x);

    while (n > 0)
    {
        remain = n % 10;
        n /= 10;

        if (reverse > max_val || (reverse == max_val && remain > max_remain))
            return 0;

        reverse = 10 * reverse + remain;
    }

    if (((x >> 31) & 0x01) == 1)
        reverse = -reverse;

    return reverse;
}
static void reverse_leet_7()
{
    int x = -2147483412;
    int val = reverse(x);

    printf("leet 7: %d reverse %d\n", x, val);
}

int main(int argc, char *argv[])
{
    reverse_leet_7();
    two_sum_leet_1();
    max_69num_leet();
    return 0;
}

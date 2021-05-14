#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>

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
                // *(answer + 0) = i;
                // *(answer + 1) = j;
                answer[0] = i;
                answer[1] = j;
                break;
            }
        }
    }

    return answer;
}
static void leet_1_two_sum()
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
static int reverse_num(int x)
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
static void leet_7_reverse_num()
{
    int x = -2147483412;
    int val = reverse_num(x);

    printf("leet 7: %d reverse %d\n", x, val);
}

/** leet 9 **/
/*static bool is_palindrome(int x)
{
#define max_int (0x80000000 - 1)
    int max_remain = max_int % 10;
    int max_times = max_int / 10;
    int palindrome = 0, remain, n;

    if (x < 0)
        return false;

    n = x;

    while (n > 0)
    {
        remain = n % 10;
        n = n / 10;

        if (palindrome > max_times || (palindrome == max_times && remain > max_remain))
            return false;

        palindrome = palindrome * 10 + remain;
    }

    if (palindrome != x)
        return false;

    return true;
}*/
static bool is_palindrome(int x)
{
    int n = 0;

    if (x < 0 || (x % 10 == 0 && x != 0))
        return false;

    while (x > n)
    {
        n = n * 10 + x % 10;
        x = x / 10;
    }

    return x == n || x == n / 10;
}
static void leet_9_is_palindrome()
{
    int x = 121;
    // int x = -10;
    // int x = 2147483647;
    bool b = is_palindrome(x);
    printf("leet 9: %d is_palindrome %s\n", x, b ? "true" : "false");
}

/** leet 14 **/
/*static char *longest_common_prefix(char **strs, int strsSize)
{
    int size = strsSize;
    char *prefix = calloc(1, 200);

    if (size == 0)
        return prefix;

    if (size == 1)
    {
        strcpy(prefix, strs[0]);
        return prefix;
    }

    int i = 0;
    char *p = prefix;
    while (strs[0][i] && strs[1][i] && strs[0][i] == strs[1][i])
    {
        *p++ = strs[0][i];
        i++;
    }

    for (i = 2; i < size; i++)
    {
        char *src = strs[i];
        p = prefix;
        if (prefix == NULL)
            return NULL;
        while (*p && *src && *p == *src)
        {
            p++;
            src++;
        }
        *p = '\0';
    }

    return prefix;
}*/
static char *longest_common_prefix(char **strs, int strsSize)
{
    int size = strsSize;
    char *prefix = calloc(1, 200);
    if (size == 0)
        return prefix;

    strcpy(prefix, strs[0]);

    if (size == 1)
        return prefix;

    int i = 0, index = 0;
    char *p = prefix;
    while (*p != '\0')
    {
        for (i = 1; i < size; i++)
        {
            char *src = strs[i];
            if (*p != src[index])
            {
                *p = '\0';
                return prefix;
            }
        }
        p++;
        index++;
    }

    return prefix;
}
static void leet_14_longest_common_prefix()
{
    char *str[] = {"flower", "flow", "flight"};
    // const char *str[] = {"dog","racecar","car"};
    // const char *str[] = {};
    int size = 3;
    char *p = longest_common_prefix((char **)str, size);
    printf("leet 14: %d longest_common_prefix is  %s\n", size, p);
    // free(p);
}

/** leet 20 **/
static char bracket_pair(char c)
{
    if (c == ')') return '(';
    if (c == ']') return '[';
    if (c == '}') return '{';
    return 0;
}
static bool is_valid(char *s)
{
    int len = strlen(s);
    int a[len + 1];
    if (len % 2 == 1)
        return false;

    int top = 0;
    while (*s)
    {
        char c = bracket_pair(*s);
        if (c != 0)
        {
            if (top == 0 || a[top - 1] != c)
                return false;
            top--;
        }
        else
        {
            a[top++] = *s;
        }
        s++;
    }

    return top == 0;
}
static void leet_20_is_valid()
{
    char *s = "(]";
    bool b = is_valid(s);
    printf("leet 20: %s is_valid %s\n", s, b ? "true" : "false");
}

/** leet 26 **/
// static int array_remove_duplicates(int *nums, int numsSize)
// {
//     int i = 1;
//     int len = 0;
//     int size = numsSize;

//     if (size == 0 || size == 1)
//         return size;

//     for (i = 1; i < size; i++)
//     {
//         if (nums[i] == nums[i - 1])
//         {
//             len++;
//             continue;
//         }

//         if (len)
//         {
//             memmove(nums + i - len, nums + i, (size-i) * sizeof(int));
//             i = i - len;
//             size -= len;
//             len = 0;
//         }
//     }

//     if (len)
//         i = i - len;

//     return i;
// }
static int array_remove_duplicates(int *nums, int numsSize)
{
    int size = numsSize;

    if (size == 0 || size == 1)
        return size;
    int i = 0, j;
    for (j = 1; j < size; j++)
    {
        if (nums[j] != nums[i])
        {
            i++;
            nums[i] = nums[j];
        }
    }
    return i + 1;
}
static void leet_26_array_remove_duplicates()
{
    // int array[] = {1, 1, 2};
    int array[] = {0,0,1,1,1,2,2,3,3,4};
    // int array[] = {0,0,1,1,3};
    int size = sizeof(array) / sizeof(array[0]);
    printf("leet 26: array_remove_duplicates %d {", size);
    int i = 0;
    for (i = 0; i < size; i++)
    {
        printf("%d%s", array[i], i == size - 1 ? "} " : ", ");
    }
    int len = array_remove_duplicates(array, size);
    printf("to length %d {", len);
    for (i = 0; i < len; i++)
    {
        printf("%d%s", array[i], i == len - 1 ? "} \n" : ", ");
    }
}

/** leet 27 **/
// static int array_remove_element(int *nums, int numsSize, int val)
// {
//     if (numsSize == 0)
//         return 0;

//     int i = 0, j;
//     for (j = 0; j < numsSize; j++)
//     {
//         if (nums[j] != val)
//         {
//             nums[i++] = nums[j];
//         }
//     }
//     return i;
// }
static int array_remove_element(int *nums, int numsSize, int val)
{
    int size = numsSize;
    if (size == 0)
        return 0;

    int i = 0;
    for (i = 0; i < size; i++)
    {
        if (nums[i] == val)
        {
            while (nums[size - 1] == val)
            {
                if (i == size - 1)
                    return i;
                size--;
            }

            nums[i] = nums[--size];
        }
    }
    return i;
}
static void leet_27_array_remove_element()
{
    int value = 0;
    // int array[] = {1, 1, 2};
    // int array[] = {0,0,1,1,1,2,2,3,3,4};
    // int array[] = {0,1,2,2,3,0,4,2};
    int array[] = {1,2,2,3,0,0,0};
    int size = sizeof(array) / sizeof(array[0]);
    printf("leet 27: array_remove_element %d {", size);
    int i = 0;
    for (i = 0; i < size; i++)
    {
        printf("%d%s", array[i], i == size - 1 ? "} " : ", ");
    }
    int len = array_remove_element(array, size, value);
    printf("remove value %d to length(%d) {", value, len);
    for (i = 0; i < len; i++)
    {
        printf("%d%s", array[i], i == len - 1 ? "} \n" : ", ");
    }
}

/** leet 35 **/
static int search_insert(int *nums, int numsSize, int target)
{
    int l = 0, r = numsSize - 1;
    int mid = 0, result = numsSize;
    while (l <= r)
    {
        mid = (l + r) / 2;
        if (target == nums[mid])
            return mid;
        if (target < nums[mid])
        {
            r = mid - 1;
            result = mid;
        }
        else
        {
            l = mid + 1;
        }
    }
    return result;
}
static void leet_35_search_insert()
{
    int i = 0;
    int target = 7;
    int a[] = {1, 3, 4, 6};
    // int a[] = {4, 5, 6, 7, 0, 1, 2};
    int size = sizeof(a) / sizeof(a[0]);
    printf("leet 35: array {");
    for (i = 0; i < size; i++)
        printf("%d%s", a[i], i == size - 1 ? "} " : ", ");

    int index = search_insert(a, size, target);
    printf("search_insert \'%d\', index is %d\n", target, index);
}

/** leet 38 **/
struct local_buffer
{
    int total[2];
    int used[2];
    char *buffer[2];
};
static int buffer_append(struct local_buffer *b, int index, char c)
{
    int total = b->total[index];
    int used = b->used[index];
    if (total <= used)
    {
        total = total ? total + (total >> 1) : 128;
        b->buffer[index] = realloc(b->buffer[index], total);
        b->total[index] = total - 1;
    }
    char *p = b->buffer[index];
    p[used++] = c;
    p[used] = '\0';
    b->used[index] = used;
    return 0;
}
static char *count_and_say(int n)
{
    int i = 0, flag = 0;

    struct local_buffer b = {0};
    buffer_append(&b, 0, '1');
    // buffer_append(&b, 0, '\0');
    if (n == 1)
        return b.buffer[0];

    for (i = 1; i < n; i++)
    {
        char *p = b.buffer[flag];
        char c = *p;
        int cnt = 0;
        while (*p)
        {
            if (c == *p)
            {
                cnt++;
            }
            else
            {
                buffer_append(&b, (flag + 1) % 2, '0' + cnt);
                buffer_append(&b, (flag + 1) % 2, c);
                c = *p;
                cnt = 1;
            }

            p++;
        }
        if (cnt)
        {
            buffer_append(&b, (flag + 1) % 2, '0' + cnt);
            buffer_append(&b, (flag + 1) % 2, c);
        }
        // buffer_append(&b, (flag + 1) % 2, '\0');
        b.used[flag] = 0;
        flag = (flag + 1) % 2;
        // printf("\n%d  \"%s\"", i, b.buffer[flag]);
    }
    free(b.buffer[(flag + 1) % 2]);
    return b.buffer[flag];
}
static void leet_38_count_and_say()
{
    int n = 10;
    char *p = count_and_say(n);
    printf("leet 38: n = %d, \"%s\"\n", n, p);
}

/** leet 53 **/
// static int max_subarray(int *nums, int numsSize)
// {
//     if (numsSize == 1) return nums[0];
//     int i = 0;
//     int n = numsSize;
//     int max_sum = nums[0];
//     int sub[n];
//     sub[0] = nums[0];
//     for (i = 1; i < n; i++)
//     {
//         if (sub[i - 1] < nums[i] && sub[i-1] < 0)
//             sub[i] = nums[i];
//         else
//             sub[i] = sub[i - 1] + nums[i];
//         max_sum = fmax(sub[i], max_sum);
//     }
//     return max_sum;
// }
static int max_subarray(int *nums, int numsSize)
{
    if (numsSize == 1) return nums[0];
    int i = 0;
    int max_sum = nums[0];
    int prev = nums[0];
    for (i = 1; i < numsSize; i++)
    {
        if (prev < nums[i] && prev < 0)
            prev = nums[i];
        else
            prev = prev + nums[i];

        max_sum = fmax(prev, max_sum);
    }

    return max_sum;
}
static void leet_53_max_subarray()
{
    int i = 0;
    // int a[] = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    int a[] = {1, 2};
    int size = sizeof(a) / sizeof(a[0]);
    int result = max_subarray(a, size);
    printf("leet 53: array {");
    for (i = 0; i < size; i++)
        printf("%d%s", a[i], i == size - 1 ? "} " : ", ");

    printf("max_subarray is %d\n", result);
}

/** leet 58 **/
static int length_of_lastword(char *s)
{
    if (!s)
        return 0;
    int result = 0;
    char *p = s + strlen(s) - 1;
    while (p != s - 1)
    {
        char c = *p--;
        if (!isalpha(c))
        {
            if (result) break;
        }
        else
        {
            result++;
        }
    }
    return result;
}
static void leet_58_length_of_lastword()
{
    char *s = "a";
    int result = length_of_lastword(s);
    printf("leet 58: string \"%s\" length_of_lastword = %d\n", s, result);
}

/** leet 66 **/
static void swap(int *a, int *b)
{
    *a ^= *b;
    *b ^= *a;
    *a ^= *b;
}
static void reverse(int *p, int left, int right)
{
    while (left < right)
    {
        swap(p + left, p + right);
        left++;
        right--;
    }
}
static int *array_plus(int *digits, int digitsSize, int *returnSize)
{
    int i = 0;
    int size = digitsSize;
    int *result = calloc(1, (size + 1) * sizeof(int));
    result[0] = (digits[size - 1] + 1) % 10;
    result[1] = (digits[size - 1] + 1) / 10;
    for (i = 1; i < size; i++)
    {
        result[i + 1] = (digits[size - i - 1] + result[i]) / 10;
        result[i] = (digits[size - i - 1] + result[i]) % 10;
    }
    if (result[size] != 0)
        *returnSize = size + 1;
    else
        *returnSize = size;

    reverse(result, 0, (*returnSize-1));
    return result;
}
static void leet_66_array_plus()
{
    int i = 0;
    // int a[] = {1, 2, 3};
    int a[] = {9, 9, 9};
    int size = sizeof(a) / sizeof(a[0]);
    printf("leet 66: input array {");
    for (i = 0; i < size; i++)
        printf("%d%s", a[i], i == size - 1 ? "} " : ", ");
    int return_size = 0;
    int *result = array_plus(a, size, &return_size);
    printf(" array_plus_one len(%d) = {", return_size);
    for (i = 0; i < return_size; i++)
        printf("%d%s", result[i], i == return_size - 1 ? "}\n" : ", ");
    free(result);
}

/** leet 67 **/
static void swap_char(char *a, char *b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}
static void reverse_string(char *p, int left, int right)
{
    while (left < right)
    {
        swap_char(p + left, p + right);
        left++;
        right--;
    }
}
// static char *add_binary(char *a, char *b)
// {
//     int len1 = strlen(a);
//     int len2 = strlen(b);

//     char *p1 = a + len1 - 1;
//     char *p2 = b + len2 - 1;

//     int len = len1 > len2 ? len1 : len2;
//     char *result = calloc(1, len + 2);
//     char *src, *end, *p = result;
//     memset(result, '0', len - 1);

//     while (p1 != a - 1 && p2 != b - 1)
//     {
//         int val = *p1 - '0' + *p2 - '0';
//         *(p + 1) = (val + *p - '0') / 2 + '0';
//         *p = (val + *p - '0') % 2 + '0';
//         p++;
//         p1--;
//         p2--;
//     }

//     if (p1 == a - 1)
//     {
//         src = p2;
//         end = b - 1;
//     }
//     else
//     {
//         src = p1;
//         end = a - 1;
//     }

//     while (src != end)
//     {
//         int val = *src - '0';
//         *(p + 1) = (val + *p - '0') / 2 + '0';
//         *p = (val + *p - '0') % 2 + '0';
//         p++;
//         src--;
//     }

//     if (*p == '0')
//     {
//         *p = '\0';
//         p--;
//     }

//     reverse_string(result, 0, p - result);
//     return result;
// }
static char *add_binary(char *a, char *b)
{
    int len1 = strlen(a);
    int len2 = strlen(b);

    char *p1 = a + len1 - 1;
    char *p2 = b + len2 - 1;

    int len = len1 > len2 ? len1 : len2;
    char *result = calloc(1, len + 2);
    char *p = result;

    int carry = 0;

    while (p1 != a - 1 && p2 != b - 1)
    {
        carry += *p1 - '0' + *p2 - '0';
        *p = carry % 2 + '0';
        carry = carry / 2;
        p++;
        p1--;
        p2--;
    }

    while (p1 != a - 1)
    {
        carry += *p1 - '0';
        *p = carry % 2 + '0';
        carry = carry / 2;
        p++;
        p1--;
    }

    while (p2 != b - 1)
    {
        carry += *p2 - '0';
        *p = carry % 2 + '0';
        carry = carry / 2;
        p++;
        p2--;
    }

    if (carry)
        *p = '1';

    reverse_string(result, 0, p - result);
    return result;
}
static int binary_print(int a)
{
    if (a > 0)
    {
        binary_print(a >> 1);
        printf("%d", a % 2);
    }
}
static int num_add_binary(int a, int b)
{
    int x = a, y = b, answer, carry;
    while (y)
    {
        answer = x ^ y;
        carry = (x & y) << 1;
        x = answer;
        y = carry;
    }
    return x;
}
static void leet_67_add_binary()
{
    char *a = "1101";
    char *b = "1101";
    char *c = add_binary(a, b);
    printf("leet 67: add_binary \"%s\"+\"%s\"=\"%s\"\n", a, b, c);
    free(c);
    int a1 = 130;
    int b1 = 25;
    int c1 = num_add_binary(a1, b1);
    printf("leet 67: num_add_binary without(+-*/) %d+%d=%d, ", a1, b1, c1);
    binary_print(a1);
    printf("+");
    binary_print(b1);
    printf("=");
    binary_print(c1);
    printf("\n");
}
/** leet 69 **/
static int my_sqrt(int x)
{
    int l = 0, r = x;
    int result = 1;
    int mid = 0;
    while (l <= r)
    {
        mid = (l + r) / 2;
        if ((long long)mid * mid <= x)
        {
            result = mid;
            l = mid + 1;
        }
        else
        {
            r = mid - 1;
        }
    }
    return result;
}
static void leet_69_my_sqrt()
{
    int x = 8;
    int result = my_sqrt(x);
    printf("leet 69: %d sqrt is (int)%d\n", x, result);
}

/** leet 70 **/
// static int climb_stairs(int n)
// {
//     int a[3];

//     a[0] = 1;
//     a[1] = 1;
//     for (int i = 2; i <= n; i++)
//     {
//         // a[2] = a[1] + a[0];
//         a[i % 3] = a[(i - 1) % 3] + a[(i - 2) % 3];
//     }
//     return a[n % 3];
// }
static int climb_stairs(int n)
{
    int a = 1, b = 1;
    for (int i = 1; i < n; i++)
    {
        b = a + b;
        a = b - a;
    }
    return b;
}
static void leet_70_climb_stairs()
{
    int n = 10;
    int result = climb_stairs(n);
    printf("leet 70: climb_stairs %d stairs have %d methods\n", n, result);
}

/** leet 88 **/
static void array_merge(int *nums1, int nums1Size, int m, int *nums2, int nums2Size, int n)
{
    int i = m - 1, j = n - 1, index = m + n - 1;
    while (i >= 0 && j >= 0)
    {
        if (nums1[i] > nums2[j])
            nums1[index--] = nums1[i--];
        else
            nums1[index--] = nums2[j--];
    }

    while (j >= 0)
        nums1[index--] = nums2[j--];
}
static void leet_88_array_merge()
{
    int m1[] = {1, 2, 3, 0, 0, 0};
    int n1[] = {4, 5, 6};
    int m = 3;
    int n = 3;
    int i = 0;
    int size = sizeof(m1) / sizeof(m1[0]);
    printf("leet 88: input array {");
    for (i = 0; i < size; i++)
        printf("%d%s", m1[i], i == size - 1 ? "} merge {" : ", ");
    size = sizeof(n1) / sizeof(n1[0]);
    for (i = 0; i < sizeof(n1) / sizeof(n1[0]); i++)
        printf("%d%s", n1[i], i == size - 1 ? "} " : ", ");
    array_merge(m1, 0, m, n1, 0, n);
    printf(" ==>  {");
    size = sizeof(m1) / sizeof(m1[0]);
    for (i = 0; i < size; i++)
        printf("%d%s", m1[i], i == size - 1 ? "}\n" : ", ");
}

// /** leet 8 **/
// static int my_atoi(char *s)
// static void leet_8_my_atoi()

int main(int argc, char *argv[])
{
    // leet 136,                        // a ^ b ^ a = b ^ a ^ a = b ^ (a ^ a) = b
    leet_88_array_merge();
    leet_70_climb_stairs();             // dp, f(n) = f(n-1) + f(n-2)
    leet_69_my_sqrt();                  // 
    leet_67_add_binary();               // binary, string +-*/
    leet_66_array_plus();               // binary, string +-*/
    leet_58_length_of_lastword();
    leet_53_max_subarray();
    leet_38_count_and_say();
    leet_35_search_insert();
    leet_27_array_remove_element();
    leet_26_array_remove_duplicates();
    leet_20_is_valid();
    leet_14_longest_common_prefix();
    leet_9_is_palindrome();
    leet_7_reverse_num();               // int overflow
    leet_1_two_sum();
    max_69num_leet();
    return 0;
}

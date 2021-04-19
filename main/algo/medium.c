#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef min
#define min(a, b) (a < b ? a : b)
#endif

/** add_two_numbers leet 2 **/
struct ListNode
{
    int val;
    struct ListNode *next;
};

static struct ListNode *list_node_create(int a[], int size)
{
    struct ListNode *node = NULL;
    struct ListNode *head = NULL;

    int i = 0;

    head = calloc(1, sizeof(struct ListNode));
    node = head;
    node->val = a[0];
    node->next = NULL;

    for (i = 1; i < size; i++)
    {
        node->next = calloc(1, sizeof(struct ListNode));
        node->next->val = a[i];

        node = node->next;
    }

    return head;
}

static struct ListNode *add_two_numbers(struct ListNode *l1, struct ListNode *l2)
{
    int val = 0;
    int carry = 0;
    struct ListNode *head = NULL;
    struct ListNode *node = NULL;

    while (l1 || l2)
    {
        int n1 = l1 ? l1->val : 0;
        int n2 = l2 ? l2->val : 0;

        val = n1 + n2 + carry;
        if (!head)
        {
            head = calloc(1, sizeof(struct ListNode));
            head->val = val % 10;
            node = head;
        }
        else
        {
            node->next = calloc(1, sizeof(struct ListNode));
            node->next->val = val % 10;
            node = node->next;
        }
        carry = val / 10;
        l1 = l1 ? l1->next : NULL;
        l2 = l2 ? l2->next : NULL;
    }
    if (carry)
    {
        node->next = calloc(1, sizeof(struct ListNode));
        node->next->val = carry;
    }

    return head;
}
static void leet_2_add_two_list_num()
{
    int a[] = {2, 4, 3};
    int b[] = {5, 6, 4};
    struct ListNode *node1 = list_node_create(a, sizeof(a) / sizeof(a[0]));
    struct ListNode *node2 = list_node_create(b, sizeof(b) / sizeof(b[0]));

    struct ListNode *result = add_two_numbers(node1, node2);

    struct ListNode *node = result;
    printf("leet 2: {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}\n");
        node = node->next;
    }
}

/** length_of_longest_substring leet 3 **/
/*static int length_of_longest_substring(char *s)
{
    struct cache_char
    {
        char c;
        unsigned short index;
    };
    struct cache_char cache[128] = {0};
    int i = 0, max = 0;
    int len = 0;
    char c;

    for (i = 0; i < strlen(s); i++)
    {
        c = s[i];
        if (cache[c].c == 0)
        {
            cache[c].c = c;
            cache[c].index = i;
            len++;
        }
        else
        {
            max = len > max ? len : max;
            i = cache[c].index + 1;
            memset(cache, 0x00, sizeof(cache));
            c = s[i];
            cache[c].c = c;
            cache[c].index = i;
            len = 1;
        }
    }
    max = len > max ? len : max;

    return max;
}
static int length_of_longest_substring(char *s)
{
    struct cache_char
    {
        char c;
        unsigned short index;
    };
    struct cache_char cache[128] = {0};
    int i = 0, index = 0, max = 0;
    int len = 0;
    char c, tmp;

    for (i = 0; i < strlen(s); i++)
    {
        c = s[i];
        if (cache[c].c == 0)
        {
            cache[c].c = c;
            cache[c].index = i;
            len++;
        }
        else
        {
            max = len > max ? len : max;
            int j = index;
            for (j = index; j < cache[c].index; j++)
            {
                tmp = s[j];
                cache[tmp].c = 0;
                cache[tmp].index = 0;
            }
            index = cache[c].index + 1;
            len = i - cache[c].index;
            // cache[c].c = c;
            cache[c].index = i;
        }
    }
    max = len > max ? len : max;

    return max;
}*/
/* space optimization */
/*static int length_of_longest_substring(char *s)
{
    int max = 0;
    char *p;
    char *start, *end;

    if (*s == '\0')
        return 0;

    start = end = s;
    while (*(++end) != '\0')
    {
        p = end;
        while (*(--p) != *end)
        {
            if (p == start)
                break;
        }
        if (*p == *end)
        {
            max = (end - start) > max ? (end - start) : max;
            start = p + 1;
        }
    }

    max = (end - start) > max ? (end - start) : max;

    return max;
}*/
/* time optimization */
static int length_of_longest_substring(char *s)
{
    char cache[128] = {0};
    int max = 0, r = 0, l = 0;
    int len = strlen(s);
    char c;

    while (r < len)
    {
        c = s[r];

        while (cache[(int)c] != 0)
        {
            cache[(int)s[l]] = 0;
            l++;
        }

        cache[(int)c] = c;
        r++;

        max = (r - l) > max ? (r - l) : max;
    }

    return max;
}
static void leet_3_length_of_longest_substring()
{
    char *s = "abcabcbb";
    // char *s = "dvdf";
    // char *s = "pwwkew";
    // char *s = "";
    // char *s = " ";
    int len = length_of_longest_substring(s);
    printf("leet 3: %s length_of_longest_substring is %d\n", s, len);
}

/** leet 5 **/
// 动态规划
/*static char *longest_palindrome(char *s)
{
    char *max = NULL;
    int i = 0, l = 0, max_len = 0;
    int len = strlen(s);
    // char dp[len][len] = {0};
    // char **dp = calloc(1, len * sizeof(char *));
    char **dp = calloc(1, len * sizeof(char *) + len * len * sizeof(char));
    for (i = 0; i < len; i++)
    {
        // dp[i] = calloc(1, len * sizeof(char));
        dp[i] = (char *)dp + len * sizeof(char *) + len * sizeof(char) * i;
    }

    // p(x,y) -> p(x+1, y-1) && s[x] == s[y]

    for (l = 0; l < len; l++)
    {
        for (i = 0; i + l < len; i++)
        {
            int j = i + l;
            if (l == 0)
                dp[i][j] = 1;
            else if (l == 1)
                dp[i][j] = (s[i] == s[j]);
            else
                dp[i][j] = (s[i] == s[j] && dp[i + 1][j - 1]);

            if (dp[i][j] && l + 1 > max_len)
            {
                max_len = l + 1;
                max = s + i;
            }
        }
    }

    char *longest = calloc(1, max_len + 1);
    strncpy(longest, max, max_len);
    free(dp);

    return longest;
}*/
// 中心扩散
/*static int expand_around_center(char *s, int left, int right)
{
    int len = strlen(s);
    while (left >= 0 && right < len && s[left] == s[right])
    {
        left--;
        right++;
    }
    return right - left - 1;
}
static char *longest_palindrome(char *s)
{
    char *max = NULL;
    int i = 0, j = 0, max_len = 0;
    int len = strlen(s);

    for (i = 0; i < len; i++)
    {
        int c1 = expand_around_center(s, i, i);
        int c2 = expand_around_center(s, i, i + 1);
        int c = c1 > c2 ? c1 : c2;
        if (c > max_len)
        {
            max_len = c;
            max = s + i - (max_len - 1) / 2;
        }
    }

    char *longest = calloc(1, max_len + 1);
    strncpy(longest, max, max_len);

    return longest;
}*/
// manacher
/* ____|____i________|________i____|____
 *        -----             -----
 *     L    i'       C        i    R
 *
 *   ---------------   -------------??
 *     L    i'       C        i    R
 *
 *   i mirror, i' = 2*C - i
 */
static char *longest_palindrome(char *s)
{
    int C = -1, R = -1;
    int i = 0, max_len = 0;
    int len = strlen(s) * 2 + 1;
    char *str = calloc(1, len);
    int *array = calloc(1, len * sizeof(int)); // palindrome radius(R) array

    char *p = s;

    for (i = 0; i < len; i++)
    {
        str[i] = (i & 1) == 0 ? '#' : *p++;
    }
    // printf("%s -> %s\n", s, str);

    for (i = 0; i < len; i++)
    {
        array[i] = R > i ? min(R - i, array[2 * C - 1]) : 1;

        while (i - array[i] >= 0 && i + array[i] < len)
        {
            if (str[i + array[i]] == str[i - array[i]])
                array[i]++;
            else
                break;
        }

        if (i + array[i] > R)
        {
            R = array[i];
            C = i;
        }

        if (array[i] > max_len)
        {
            /**
             * palindrome radius: array[i]
             * palindrome diameter(fill '#'): array[i] * 2 - 1
             * palindrome len(origin): array[i] - 1
             */
            max_len = array[i];
            p = str + i - (array[i] - 1);
        }
    }

    char *longest = calloc(1, max_len);
    char *dst = longest;
    for (i = 0; i < max_len * 2 - 1; i++)
    {
        if (p[i] != '#')
            *dst++ = p[i];
    }
    // printf("palindrome diameter %d, %s\n", max_len * 2 - 1, p);

    free(str);
    free(array);

    return longest;
}
static void leet_5_longest_palindrome()
{
    // char *s = "babad";
    // char *s = "abbc";
    char *s = "aacdefcaac";
    // char *s = "a"; //"#a#"
    char *p = longest_palindrome(s);
    printf("leet 5: %s longest_palindrome is %s\n", s, p);
    free(p);
}

/** leet 6 **/
static char *z_convert(char *s, int numRows)
{
    int i = 0, j = 0;
    int len = strlen(s);
    int rows = numRows;
    int cycle = 2 * rows - 2;

    if (rows == 1)
        return s;

    char *output = calloc(1, len + 1);
    char *p = output;

    for (i = 0; i < rows; i++)
    {
        for (j = 0; j + i < len; j += cycle)
        {
            *p++ = s[j + i];
            if (i != 0 && i != rows - 1 && j + cycle - i < len)
            {
                *p++ = s[j + cycle - i];
            }
        }
    }
    return output;
}
static void leet_6_z_convert()
{
    char *s = "PAYPALISHIRING";
    int rows = 4;
    char *p = z_convert(s, rows);
    printf("leet 6: %s z_convert %d rows is %s\n", s, rows, p);
}

/** leet 8 **/
static int my_atoi(char *s)
{
    int out = 0, flag = 0, remain;
    int min = 0x80000000;
    int max = 0x80000000 - 1;
    int max_remain = max % 10;
    int max_val = max / 10;

    char *p = s;
    // while (!((*p >= '0' && *p <= '9') || *p == '+' || *p == '-'))
    while (*p == ' ' || *p == '\0')
        if (*p++ == '\0')
            return 0;

    if (*p == '-' || *p == '+')
    {
        if (*p++ == '-')
            flag = 1;
    }

    while ((*p >= '0' && *p <= '9') && *p != '\0')
    {
        remain = *p++ - '0';

        if (out > max_val || (out == max_val && remain > max_remain))
        {
            if (flag)
                return min;
            else
                return max;
        }

        out = out * 10 + remain;
    }

    if (flag)
        out = -out;

    return out;
}
static void leet_8_my_atoi()
{
    // char *s = "   -42";
    // char *s = "   +4193 with words";
    // char *s = "   -4193 with words";
    // char *s = "words and 987";
    char *s = "-91283472332";
    int n = my_atoi(s);
    printf("leet 8: string %s my_atoi is %d\n", s, n);
}

/** leet 11 **/
static int max_area(int *height, int heightSize)
{
    int size = heightSize;
    int head = 0;
    int tail = size - 1;
    int area = 0, max_area = 0;

    while (tail > head)
    {
        if (height[head] > height[tail])
        {
            area = height[tail] * (tail - head);
            tail--;
        }
        else
        {
            area = height[head] * (tail - head);
            head++;
        }

        max_area = area > max_area ? area : max_area;
    }

    return max_area;
}
static void leet_11_max_area()
{
    // int a[] = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    // int a[] = {4, 3, 2, 1, 4};
    // int a[] = {1, 1};
    int a[] = {1, 2, 1};
    int size = sizeof(a) / sizeof(a[0]);
    int max = max_area(a, size);
    printf("leet 11: array ");

    int i = 0;
    for (i = 0; i < size; i++)
    {
        printf(" %d,", a[i]);
    }
    printf("  max_area is %d\n", max);
}

/** leet 12 **/
static char *int_to_roman(int num)
{
    int nums[] = {1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    char *symbol[] = {"M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"};
    int i = 0;
    int size = sizeof(nums) / sizeof(nums[0]);
    char *roman = calloc(1, 16);

    for (i = 0; i < size; i++)
    {
        while (num >= nums[i])
        {
            strcat(roman, symbol[i]);
            num -= nums[i];
        }
    }

    return roman;
}
static void leet_12_int_to_roman()
{
    int n = 3204;
    char *roman = int_to_roman(n);
    printf("leet 12: %d to roman is %s\n", n, roman);
    free(roman);
}

/** leet 13 **/
static int get_value(char c)
{
    switch (c)
    {
    case 'I':
        return 1;
    case 'V':
        return 5;
    case 'X':
        return 10;
    case 'L':
        return 50;
    case 'C':
        return 100;
    case 'D':
        return 500;
    case 'M':
        return 1000;
    default:
        return 0;
    }
}
static int roman_to_int(char *s)
{
    int i = 0, num = 0, prev_num = 0, n = 0;
    int len = strlen(s);

    for (i = 0; i < len; i++)
    {
        num = get_value(*s++);
        if (num > prev_num)
            num -= 2 * prev_num;

        prev_num = num;
        n += num;
    }

    return n;
}
static void leet_13_roman_to_int()
{
    char *roman = "MMMCCIV";
    int n = roman_to_int(roman);
    printf("leet 13: %s to number is %d\n", roman, n);
}

/** leet 15 **/
struct local_list
{
    int val[4];
    struct local_list *next, *prev;
};
static void local_list_add_(struct local_list *new, struct local_list *prev, struct local_list *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}
// static void local_list_add(struct local_list *new, struct local_list *head)
// {
//     local_list_add_(new, head, head->next);
// }
static void local_list_add_tail(struct local_list *new, struct local_list *head)
{
    local_list_add_(new, head->prev, head);
}
#define local_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

static int compare(const void *a, const void *b)
{
    return *(int *)a - *(int *)b;
}
static int **three_sum(int *nums, int numsSize, int *returnSize, int **returnColumnSizes)
{
    struct local_list head = {0};
    head.next = &head;
    head.prev = &head;
    int result_count = 0;

    int size = numsSize;
    if (size < 2)
    {
        *returnSize = 0;
        return NULL;
    }

    int i = 0, j = 0, k = 0;
    // for (i = 0; i < size; i++)
    // {
    //     printf(" %d", nums[i]);
    // }
    // printf("\n");
    qsort(nums, size, sizeof(int), compare);
    // for (i = 0; i < size; i++)
    // {
    //     printf(" %d", nums[i]);
    // }
    // printf("\n");

    if (nums[0] > 0)
        return NULL;

    for (i = 0; i < size; i++)
    {
        if (i > 0 && nums[i] == nums[i - 1])
            continue;

        k = size - 1;
        for (j = i + 1; j < size; j++)
        {
            if (j > i + 1 && nums[j] == nums[j - 1])
                continue;
#if 0
            for (k = j + 1; k < size; k++)
            {
                if (k > j + 1 && nums[k] == nums[k - 1])
                    continue;
                if (nums[i] + nums[j] + nums[k] == 0)
                {
                    // printf("%d %d %d\n", nums[i], nums[j], nums[k]);
                    struct local_list *node = calloc(1, sizeof(struct local_list));
                    node->val[0] = nums[i];
                    node->val[1] = nums[j];
                    node->val[2] = nums[k];
                    local_list_add_tail(node, &head);
                    result_count++;
                }
            }
#else
            while (j < k && nums[i] + nums[j] + nums[k] > 0)
                k--;

            if (j == k)
                break;
            if (nums[i] + nums[j] + nums[k] == 0)
            {
                // printf("%d %d %d\n", nums[i], nums[j], nums[k]);
                struct local_list *node = calloc(1, sizeof(struct local_list));
                node->val[0] = nums[i];
                node->val[1] = nums[j];
                node->val[2] = nums[k];
                local_list_add_tail(node, &head);
                result_count++;
            }

#endif
        }
    }

    i = 0;
    int **result = calloc(1, sizeof(int *) * result_count);
    struct local_list *pos;
    local_list_for_each(pos, &head)
    {
        result[i++] = (int *)pos->val;
    }

    int *result_col = calloc(1, sizeof(int) * result_count);
    for (i = 0; i < result_count; i++)
        result_col[i] = 3;
    *returnColumnSizes = result_col;
    *returnSize = result_count;

    return result;
}
static void leet_15_three_sum()
{
    int result_num = 0;
    int *result_col = NULL;
    int num[] = {-1, 0, 1, 2, -1, -4, 2, 2, 3, 3};
    // int num[] = {-1};
    // int num[] = {};
    int size = sizeof(num) / sizeof(num[0]);
    int **array = three_sum(num, size, &result_num, &result_col);
    printf("leet 15: three_sum {");
    int i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        printf("%d%s", num[i], i == size-1 ? "}" : ", ");
    }
    printf(" have %d array: ", result_num);
    for (i = 0; i < result_num; i++)
    {
        printf(" {");
        for (j = 0; j < 3; j++)
            printf("%d%s", array[i][j], j == 2 ? "} " : ", ");
        // printf("}  ");
        free(array[i]);
    }
    printf("sum is 0\n");

    if (result_col)
        free(result_col);
    if (array)
        free(array);
}

/** leet 16 **/
static int three_sum_closest(int *nums, int numsSize, int target)
{
    int size = numsSize;
    int closest = 1e7;
    qsort(nums, size, sizeof(int), compare);

    int i = 0, l = 0, r = 0;
    for (i = 0; i < size; i++)
    {
        if (i > 0 && nums[i] == nums[i - 1])
            continue;

        l = i + 1;
        r = size - 1;
        while (l < r)
        {
            int sum = nums[i] + nums[l] + nums[r];
            if (sum == target)
                return sum;

            if (abs(sum - target) < abs(closest - target))
                closest = sum;

            if (sum > target)
            {
                int r0 = r - 1;
                while (l < r0 && nums[r0] == nums[r])
                    r0--;
                r = r0;
            }
            else
            {
                int l0 = l + 1;
                while (l0 < r && nums[l] == nums[l0])
                    l0++;
                l = l0;
            }
        }
    }
    return closest;
}
static void leet_16_three_sum_closest()
{
    int target = 10;
    int num[] = {-1, 0, 1, 2, -1, -4, 2, 2, 3, 3};
    int size = sizeof(num) / sizeof(num[0]);
    int val = three_sum_closest(num, size, target);
    printf("leet 16:  {");
    int i = 0;
    for (i = 0; i < size; i++)
    {
        printf("%d%s", num[i], i == size-1 ? "}" : ", ");
    }
    printf(" three_sum_closest target(%d) is %d \n", target, val);
}

/** leet 17 **/
static void backtrack(int index, char *digits, int len, char *cache, char *map[], char **combinations, int *return_size)
{
    if (index == len)
    {
        combinations[*return_size] = calloc(1, len + 1);
        strcpy(combinations[*return_size], cache);
        // printf(" %s\n", combinations[*return_size]);
        (*return_size)++;
    }
    else
    {
        int i = 0;
        char *str = map[digits[index] - '0'];
        for (i = 0; i < strlen(str); i++)
        {
            cache[index] = str[i];
            backtrack(index + 1, digits, len, cache, map, combinations, return_size);
            cache[index] = '\0';
        }
    }
}
static char **letter_combinations(char *digits, int *return_size)
{
    char *num_letter_map[10] = {"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
    *return_size = 0;
    int len = strlen(digits);
    if (len == 0)
        return NULL;
    int num = pow(4, len);
    char **combinations = calloc(1, sizeof(char *) * num);
    char *cache = calloc(1, len + 1);

    backtrack(0, digits, len, cache, num_letter_map, combinations, return_size);
    free(cache);

    return combinations;
}
static void leet_17_letter_combinations()
{
    // char *digits = "35";
    char *digits = "";
    int size = 0;
    char **combinations = letter_combinations(digits, &size);
    printf("leet 17: \"%s\" letter_combinations %d answer [", digits, size);
    int i = 0;
    for (i = 0; i < size; i++)
    {
        printf(" %s,", combinations[i]);
        free(combinations[i]);
    }
    printf("]\n");
    if (combinations)
        free(combinations);
}

/** leet 18 **/
static int **four_sum(int *nums, int numsSize, int target, int *returnSize, int **returnColumnSizes)
{
    struct local_list head = {0};
    head.next = &head;
    head.prev = &head;
    int result_count = 0;

    int size = numsSize;
    if (size < 4)
    {
        *returnSize = 0;
        return NULL;
    }
    qsort(nums, size, sizeof(int), compare);

    int i = 0, j = 0, l = 0, r = 0;
    for (i = 0; i < size - 3; i++)
    {
        if (i > 0 && nums[i] == nums[i - 1])
            continue;

        if (nums[i] + nums[i + 1] + nums[i + 2] + nums[i + 3] > target)
            break;
        if (nums[i] + nums[size - 3] + nums[size - 2] + nums[size - 1] < target)
            continue;

        for (j = i + 1; j < size - 2; j++)
        {
            if (j > i + 1 && nums[j] == nums[j - 1])
                continue;

            if (nums[i] + nums[j] + nums[j + 1] + nums[j + 2] > target)
                break;
            if (nums[i] + nums[j] + nums[size - 2] + nums[size - 1] < target)
                continue;

            l = j + 1;
            r = size - 1;
            while (l < r)
            {
                int val = nums[i] + nums[j] + nums[l] + nums[r];
                if (val == target)
                {
                    // printf("%d %d %d %d\n", nums[i], nums[j], nums[l], nums[r]);
                    struct local_list *node = calloc(1, sizeof(struct local_list));
                    node->val[0] = nums[i];
                    node->val[1] = nums[j];
                    node->val[2] = nums[l];
                    node->val[3] = nums[r];
                    local_list_add_tail(node, &head);
                    result_count++;

                    while (l < r && nums[l] == nums[l + 1])
                        l++;
                    l++;
                    while (l < r && nums[r] == nums[r - 1])
                        r--;
                    r--;
                }
                else if (val < target)
                    l++;
                else
                    r--;
            }
        }
    }

    i = 0;
    int **result = calloc(1, sizeof(int *) * result_count);
    struct local_list *pos;
    local_list_for_each(pos, &head)
    {
        result[i++] = (int *)pos->val;
    }

    int *result_col = calloc(1, sizeof(int) * result_count);
    for (i = 0; i < result_count; i++)
        result_col[i] = 4;
    *returnColumnSizes = result_col;
    *returnSize = result_count;

    return result;
}
static void leet_18_four_sum()
{
    int result_num = 0;
    int *result_col = NULL;
    int target = 0;
    int num[] = {-1, 0, 1, 2, -1, -4, 2, 2, 3, 3};
    // int num[] = {-1};
    // int num[] = {};
    // int num[] = {0, 0, 0, 1};
    int size = sizeof(num) / sizeof(num[0]);
    int **array = four_sum(num, size, target, &result_num, &result_col);
    printf("leet 18: four_sum {");
    int i = 0, j = 0;
    for (i = 0; i < size; i++)
    {
        printf("%d%s", num[i], i == size-1 ? "} " : ", ");
    }
    printf("have %d array: ", result_num);
    for (i = 0; i < result_num; i++)
    {
        printf(" {");
        for (j = 0; j < 4; j++)
            printf("%d%s", array[i][j], j == 3 ? "} " : ", ");
        free(array[i]);
    }
    printf("sum is target(%d)\n", target);

    if (result_col)
        free(result_col);
    if (array)
        free(array);
}

/** leet 19 **/
static struct ListNode *remove_nth_fromend(struct ListNode *head, int n)
{
    struct ListNode *dummy = calloc(1, sizeof(struct ListNode));
    dummy->next = head;

    struct ListNode *node = dummy;

    int count = 0;
    while (node->next)
    {
        count++;
        node = node->next;
    }

    int i = 1;
    node = dummy;
    while (i++ < count - n + 1)
    {
        node = node->next;
    }
    node->next = node->next->next;

    struct ListNode *result = dummy->next;
    free(dummy);

    return result;
}
static struct ListNode *remove_nth_fromend_2(struct ListNode *head, int n)
{
    struct ListNode *dummy = calloc(1, sizeof(struct ListNode));
    dummy->next = head;

    struct ListNode *node1 = dummy;
    struct ListNode *node2 = dummy;

    int i = 0;
    while (i++ < n)
    {
        node1 = node1->next;
    }

    while (node1->next)
    {
        node1 = node1->next;
        node2 = node2->next;
    }

    node2->next = node2->next->next;

    struct ListNode *result = dummy->next;
    free(dummy);

    return result;
}
static void leet_19_remove_nth_fromend()
{
    int n = 3;
    // int a[] = {2, 4, 3};
    int a[] = {1, 2, 3, 4, 5};
    struct ListNode *head = list_node_create(a, sizeof(a) / sizeof(a[0]));

    struct ListNode *node = head;
    printf("leet 19: {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}");
        node = node->next;
    }
    node = remove_nth_fromend(head, n);
    printf(" remove_%dth_fromend  method1: {", n);
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}");
        node = node->next;
    }

    head = list_node_create(a, sizeof(a) / sizeof(a[0]));
    node = remove_nth_fromend_2(head, n);
    printf(",  method2: {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}\n");
        node = node->next;
    }
}

/** leet 21 **/
static struct ListNode *merge_two_lists(struct ListNode *l1, struct ListNode *l2)
{
    // struct ListNode *head = calloc(1, sizeof(struct ListNode));
    struct ListNode head;
    struct ListNode *node = &head;

    while (l1 && l2)
    {
        if (l1->val < l2->val)
        {
            node->next = l1;
            l1 = l1->next;
        }
        else
        {
            node->next = l2;
            l2 = l2->next;
        }
        node = node->next;
    }

    if (l1 == NULL)
        node->next = l2;
    else
        node->next = l1;

    return head.next;
}
static void leet_21_merge_two_lists()
{
    int a[] = {2, 3, 6};
    int b[] = {5, 6, 9};
    struct ListNode *node1 = list_node_create(a, sizeof(a) / sizeof(a[0]));
    struct ListNode *node2 = list_node_create(b, sizeof(b) / sizeof(b[0]));

    struct ListNode *node = node1;
    printf("leet 21: merge_two_lists {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "} + {");
        node = node->next;
    }
    node = node2;
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "} ");
        node = node->next;
    }

    struct ListNode *result = merge_two_lists(node1, node2);
    node = result;
    printf("= {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}\n");
        node = node->next;
    }
}

/** leet 23 **/
// static struct ListNode *merge_k_lists(struct ListNode **lists, int listsSize)
// {
//     if (listsSize < 1) return NULL;
//     struct ListNode *list = lists[0];
//     int i = 1;
//     for (i = 1; i < listsSize; i++)
//     {
//         list = merge_two_lists(list, lists[i]);
//     }
//     return list;
// }
static struct ListNode *merge_divide(struct ListNode **lists, int l, int r)
{
    if (l == r)
        return lists[l];

    if (l > r)
        return NULL;

    int mid = (l + r) >> 1;
    // struct ListNode *left = merge_divide(lists, l, mid);
    // struct ListNode *right = merge_divide(lists, mid + 1, r);
    return merge_two_lists(merge_divide(lists, l, mid), merge_divide(lists, mid + 1, r));
}
static struct ListNode *merge_k_lists(struct ListNode **lists, int listsSize)
{
    if (listsSize < 1)
        return NULL;
    return merge_divide(lists, 0, listsSize - 1);
}
static void leet_23_merge_k_lists()
{
    int size = 5;
    int a[] = {2, 3, 6};
    int b[] = {5, 6, 9};
    int c[] = {-1, 7, 9};
    int d[] = {0, 11, 23};
    int e[] = {-5, 60};

    struct ListNode **lists = calloc(1, 5 * sizeof(struct ListNode *));
    lists[0] = list_node_create(a, sizeof(a) / sizeof(a[0]));
    lists[1] = list_node_create(b, sizeof(b) / sizeof(b[0]));
    lists[2] = list_node_create(c, sizeof(c) / sizeof(c[0]));
    lists[3] = list_node_create(d, sizeof(d) / sizeof(d[0]));
    lists[4] = list_node_create(e, sizeof(e) / sizeof(e[0]));
    struct ListNode *result = merge_k_lists(lists, size);
    struct ListNode *node = result;
    printf("leet 23: merge_k_lists %d list {", size);
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}\n");
        node = node->next;
    }
}

/** leet 24 **/
static inline struct ListNode *list_reverse(struct ListNode *head, struct ListNode *tail)
{
    struct ListNode *node = head;
    struct ListNode *next, *prev = tail->next;

    // node = prev->next;
    while (prev != tail)
    {
        next = node->next;

        node->next = prev;
        prev = node;
        node = next;
    }

    return tail;
}

struct ListNode *list_swap_pairs_method_1(struct ListNode *head)
{
    struct ListNode dummy;
    dummy.next = head;

    struct ListNode *left = head;
    struct ListNode *right = head;
    struct ListNode *next, *prev = &dummy;
    // int n = k - 1;
    // int i = 0;

    while (right)
    {
        // i = 0;
        // while (i++ < n)
        // {
        //     right = right->next;
        //     if (!right)
        //         return dummy.next;
        // }
        right = right->next;
        if (!right)
            return dummy.next;

        next = right->next;

        prev->next = list_reverse(left, right);

        prev = left;
        left = next;
        right = next;
    }

    return dummy.next;
}
static struct ListNode *list_swap_pairs_method_2(struct ListNode *head)
{
    if (head == NULL || head->next == NULL)
        return head;
    struct ListNode *first = head->next;
    head->next = list_swap_pairs_method_2(first->next);
    first->next = head;

    return first;
}

static void leet_24_list_swap_pairs()
{
    // int a[] = {1, 2, 3, 4, 5, 6};
    int a[] = {1, 2, 3, 4, 5};
    struct ListNode *node1 = list_node_create(a, sizeof(a) / sizeof(a[0]));
    struct ListNode *node = node1;
    printf("leet 24: list_swap_pairs list {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}");
        node = node->next;
    }

    // node = list_swap_pairs_method_1(node1);
    node = list_swap_pairs_method_2(node1);
    printf("  to  {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}\n");
        node = node->next;
    }
}

/** leet 25 **/
static struct ListNode *reverse_k_group(struct ListNode *head, int k)
{
    if (k == 1) return head;

    struct ListNode dummy;
    dummy.next = head;

    struct ListNode *left = head;
    struct ListNode *right = head;
    struct ListNode *next, *prev = &dummy;
    int n = k - 1;
    int i = 0;

    while (right)
    {
        i = 0;
        while (i++ < n)
        {
            right = right->next;
            if (!right)
                return dummy.next;
        }

        next = right->next;

        prev->next = list_reverse(left, right);

        prev = left;
        left = next;
        right = next;
    }

    return dummy.next;
}
static void leet_25_list_reverse_k_group()
{
    int k = 3;
    int a[] = {1, 2, 3, 4, 5, 6};
    // int a[] = {1, 2, 3, 4, 5};
    struct ListNode *node1 = list_node_create(a, sizeof(a) / sizeof(a[0]));
    struct ListNode *node = node1;
    printf("leet 25: list_reverse_k_group list {");
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}");
        node = node->next;
    }

    node = reverse_k_group(node1, k);
    printf(" every %d reverse to {", k);
    while (node)
    {
        printf("%d%s", node->val, node->next ? ", " : "}\n");
        node = node->next;
    }
}

/** leet 22 **/
static void dfs_generate_parenthesis(char *cache, int open, int close, int n, char **result, int *return_size)
{
    if (open == n && close == n)
    {
        result[*return_size] = calloc(1, 2 * n + 1);
        strcpy(result[*return_size], cache);
        // printf(" %s\n", result[*return_size]);
        (*return_size)++;
    }

    if (open < n)
    {
        cache[open + close] = '(';
        dfs_generate_parenthesis(cache, open + 1, close, n, result, return_size);
        // cache[open] = '\0';
    }

    if (close < open)
    {
        cache[open + close] = ')';
        dfs_generate_parenthesis(cache, open, close + 1, n, result, return_size);
        // cache[close] = '\0';
    }
}
// static int factorial_little(int n)
// {
//     int sum = 1;
//     for (int i = 1; i <= n; i++)
//         sum = sum * i;
//     return sum;
// }
int catalan_num(int n)
{
#define MAX_CATALAN_NUM 20
    int C[MAX_CATALAN_NUM][MAX_CATALAN_NUM];
    for (int i = 1; i <= 2 * n; ++i)
    {
        C[i][0] = C[i][i] = 1;
        for (int j = 1; j < i; ++j)
        {
            C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
        }
    }

    return C[2 * n][n] / (n + 1);
}
static char **generate_parenthesis(int n, int *returnSize)
{
    char *cache = calloc(1, 2 * n + 1);
    // C(n,2n) - C(n+1,2n) = C(n,2n) / (n+1)  //catalan number
    // int total = (2n)!/(n! * n!) - (2n)!/((n+1)!*(n-1)!);
    // int total = (2n)!/(n! * n!) / (n+1);
    // int factorial_n = factorial_little(n);
    // int total = factorial_little(2 * n) / (factorial_n * factorial_n * (n + 1));
    // int total = catalan_num(n);
    int catalan[] = {1, 1, 2, 5, 14, 42, 132, 429, 1430, 4862, 16796, 58786};
    int total = catalan[n];
    // printf("allocate total %d\n", total);
    char **result = calloc(1, total * sizeof(char *));
    dfs_generate_parenthesis(cache, 0, 0, n, result, returnSize);
    free(cache);
    return result;
}
static void leet_22_generate_parenthesis()
{
    // [1]1, [2]2, [3]5, [4]14, [5]42, [6]132, [7]429, [8]1430, [9]486
    int n = 3; // max 9, if is 8, terminal have little laggings
    int size = 0;
    char **s = generate_parenthesis(n, &size);
    printf("leet 22: %d generate_parenthesis %d answer [", n, size);
    for (int i = 0; i < size; i++)
    {
        printf("\"%s\"%s", s[i], i == size - 1 ? "] \n" : ", ");
        free(s[i]);
    }
    if (s)
        free(s);
}

/** leet 29 **/
static int divide(int dividend, int divisor) //negative
{
    int bit = 0, flag = 0, result = 0;

    if (dividend == divisor) return 1;
    if (divisor == 1) return dividend;
    if (divisor == -1)
    {
        if (dividend == 0x80000000)
            return 0x80000000 - 1;
        else
            return -dividend;
    }

    if ((dividend ^ divisor) >> 31)
        flag = 1;

    if (dividend > 0)
        dividend = -dividend;
    if (divisor > 0)
        divisor = -divisor;

    int num = dividend;
    int factor = divisor;

    while (num <= factor)
    {
        bit = 0;
        while (num - factor < factor)
        {
            bit++;
            factor = (factor << 1);
            // leetcode don't support: left shift of negative value
            // factor = -(-factor << 1);
        }

        num = num - factor;
        factor = divisor;

        result = result + (1 << bit);
    }
    // while (num > factor)
    // {
    //     bit = 0;
    //     while (num - factor >= factor)
    //     {
    //         bit++;
    //         factor = (factor << 1);
    //     }
    //     num = num - factor;
    //     factor = divisor;
    //     result = result + (1 << bit);
    // }

    if (flag)
        result = -result;
    return result;
}
static void leet_29_divide()
{
    int n = 0x80000000;     // 1 << 31
    // int divisor = 0x80000000 - 1; //0x7fffffff
    int divisor = 2;
    // int n = 120;
    // int divisor = -3;
    int val = divide(n, divisor);
    printf("leet 29: %d divide %d answer %d\n", n, divisor, val);
}

// /** leet 8 **/
// static int my_atoi(char *s)
// static void leet_8_my_atoi()

int main(int argc, char *argv[])
{
    leet_29_divide();
    // leet 28  kmp
    leet_25_list_reverse_k_group();
    leet_24_list_swap_pairs();
    leet_23_merge_k_lists();
    leet_22_generate_parenthesis();
    leet_21_merge_two_lists();
    leet_19_remove_nth_fromend();
    leet_18_four_sum();
    leet_17_letter_combinations();
    leet_16_three_sum_closest();
    leet_15_three_sum();
    leet_13_roman_to_int();
    leet_12_int_to_roman();
    leet_11_max_area();
    leet_8_my_atoi();
    leet_6_z_convert();
    leet_5_longest_palindrome();
    leet_3_length_of_longest_substring();
    leet_2_add_two_list_num();
    // two_sum_leet_1();
    return 0;
}

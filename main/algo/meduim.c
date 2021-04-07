#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
static void add_two_list_num_leet_2()
{
    int a[] = {2, 4, 3};
    int b[] = {5, 6, 4};
    struct ListNode *node1 = list_node_create(a, sizeof(a) / sizeof(a[0]));
    struct ListNode *node2 = list_node_create(b, sizeof(b) / sizeof(b[0]));

    struct ListNode *result = add_two_numbers(node1, node2);

    struct ListNode *node = result;
    printf("leet 2: ");
    while (node)
    {
        printf("%d ", node->val);
        node = node->next;
    }
    printf("\n");
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

        while (cache[c] != 0)
        {
            cache[s[l]] = 0;
            l++;
        }

        cache[c] = c;
        r++;

        max = (r - l) > max ? (r - l) : max;
    }

    return max;
}
static void length_of_longest_substring_leet_3()
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
static void longest_palindrome_leet_5()
{
    // char *s = "babad";
    // char *s = "abbc";
    char *s = "aacdefcaac";
    // char *s = "a";
    char *p = longest_palindrome(s);
    printf("leet 5: %s longest_palindrome is %s\n", s, p);
    free(p);
}

int main(int argc, char *argv[])
{
    longest_palindrome_leet_5();
    length_of_longest_substring_leet_3();
    add_two_list_num_leet_2();
    // two_sum_leet_1();
    return 0;
}

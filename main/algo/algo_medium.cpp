
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <unordered_map>

using namespace std;

class Solution
{
public:
    /** leet 6 **/
    string z_convert_1(string s, int numRows)
    {
        int rows_num = numRows;
        if (rows_num == 1)
            return s;

        vector<string> rows(min(rows_num, int(s.size())));
        int index = 0;
        int flag = -1;

        for (char c : s)
        {
            rows[index] += c;
            if (index == 0 || index == rows_num - 1)
                flag = -flag;
            index += flag;
        }

        string str;
        for (string row : rows)
            str += row;

        return str;
    }
    string z_convert(string s, int numRows)
    {
        int rows_num = numRows;
        if (rows_num == 1)
            return s;

        int i, j;
        string str;
        int len = s.size();
        int cycle = 2 * rows_num - 2;

        for (i = 0; i < rows_num; i++)
        {
            for (j = 0; j + i < len; j += cycle)
            {
                str += s[j + i];
                if (i != 0 && i != rows_num - 1 && j + cycle - i < len)
                {
                    str += s[j + cycle - i];
                }
            }
        }

        return str;
    }

public:
    string cache;
    vector<string> combinations;
    vector<string> num_letter_map = {"", "", "abc", "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz"};
    void dfs_letter_combinations(int index, string digits, int size)
    {
        if (index == size)
        {
            combinations.push_back(cache);
            return;
        }
        else
        {
            string str = num_letter_map[digits[index] - '0'];
            for (int i = 0; i < str.size(); i++)
            {
                cache.push_back(str[i]);
                dfs_letter_combinations(index + 1, digits, size);
                cache.pop_back();
            }
        }
    }

    vector<string> letter_combinations(string digits)
    {
        int len = digits.size();
        if (len == 0)
            return combinations;
        dfs_letter_combinations(0, digits, len);
        return combinations;
    }

public:
    void dfs_generate_parenthesis(vector<string> &result, string str, int left, int right)
    {
        if (left == 0 && right == 0)
        {
            result.push_back(str);
            return;
        }

        if (left > 0)
            dfs_generate_parenthesis(result, str + "(", left - 1, right);

        if (right > left)
            dfs_generate_parenthesis(result, str + ")", left, right - 1);
    }
    /** leet 22 **/
    vector<string> generate_parenthesis(int n)
    {
        vector<string> result;
        dfs_generate_parenthesis(result, "", n, n);
        return result;
    }

public:
    /** leet 39 **/
    int count = 0;
    int count1 = 0;
    int count2 = 0;
//     void dfs_combination_sum(vector<vector<int>> &combinations, vector<int> &array, int index, vector<int> &candidates, int size, int target)
//     {
//         if (index == size)
//             return;
//         if (target == 0)
//         {
//             combinations.push_back(array);
//             // combinations.emplace_back(array);
//             return;
//         }
//         count++;
// #if 1// sort_your_array
//         if (target - candidates[index] >= 0)
//         {
//             array.push_back(candidates[index]);
//             // array.emplace_back(candidates[index]);
//             // printf("push_back %d \n", candidates[index]);
//             count1++;
//             dfs_combination_sum(combinations, array, index, candidates, size, target - candidates[index]);
//             array.pop_back();
//             // printf("  pop_back %d \n", candidates[index]);
//             count2++;
//             dfs_combination_sum(combinations, array, index + 1, candidates, size, target);
//         }
// #else
//         dfs_combination_sum(combinations, array, index + 1, candidates, size, target);

//         if (target - candidates[index] >= 0)
//         {
//             array.push_back(candidates[index]);
//             // printf("push_back %d \n", candidates[index]);
//             count1++;
//             dfs_combination_sum(combinations, array, index, candidates, size, target - candidates[index]);
//             array.pop_back();
//             // printf("  pop_back %d \n", candidates[index]);
//             count2++;
//         }
// #endif
//     }

    void dfs_combination_sum(vector<vector<int>> &combinations, vector<int> &array, int index, vector<int> &candidates, int size, int target)
    {
        if (index == size)
            return;
        if (target == 0)
        {
            combinations.push_back(array);
            // combinations.emplace_back(array);
            return;
        }
        count++;

        // best method
        for (int i = index; i < size; i++)
        {
            if (target - candidates[i] < 0)
                break;
            array.push_back(candidates[i]);
            // printf("push_back %d \n", candidates[i]);
            count1++;
            dfs_combination_sum(combinations, array, i, candidates, size, target - candidates[i]);
            array.pop_back();
            // printf("  pop_back %d \n", candidates[i]);
            count2++;
        }
    }
    vector<vector<int>> combination_sum(vector<int> &candidates, int target)
    {
        vector<int> array;
        vector<vector<int>> combinations;
        sort(candidates.begin(), candidates.end());
        dfs_combination_sum(combinations, array, 0, candidates, candidates.size(), target);
        printf(" count = %d, count1 = %d, count2 = %d\n", count, count1, count2);
        return combinations;
    }

    /** leet 40 **/
    void dfs_combination_sum2(vector<vector<int>> &answer, vector<int> &array, int index, vector<int> &candidates, int size, int target)
    {
        if (target == 0)
        {
            answer.push_back(array);
            // answer.emplace_back(array);
            return;
        }

        for (int i = index; i < size; i++)
        {
            if (i > index && candidates[i] == candidates[i - 1])
                continue;
            if (target - candidates[i] < 0)
                break;
            array.push_back(candidates[i]);
            // array.emplace_back(candidates[i]);
            dfs_combination_sum2(answer, array, i + 1, candidates, size, target - candidates[i]);
            array.pop_back();
        }
    }
    vector<vector<int>> combination_sum2(vector<int> &candidates, int target)
    {
        vector<int> array;
        vector<vector<int>> answer;
        sort(candidates.begin(), candidates.end());
        dfs_combination_sum2(answer, array, 0, candidates, candidates.size(), target);
        return answer;
    }

    /** leet 47 **/
    vector<int> perm_status;
    void dfs_permute_unique(vector<vector<int>> &result, vector<int> &nums, vector<int> &permute, int index, int size)
    {
        if (index == size)
        {
            result.push_back(permute);
            return;
        }

        for (int i = 0; i < size; i++)
        {
            if (perm_status[i] || (i > 0 && nums[i] == nums[i-1] && perm_status[i-1]))
                continue;
            perm_status[i] = 1;
            permute.push_back(nums[i]);
            dfs_permute_unique(result, nums, permute, index+1, size);
            permute.pop_back();
            perm_status[i] = 0;
        }
    }
    vector<vector<int>> permute_unique(vector<int> &nums)
    {
        vector<int> permute;
        vector<vector<int>> answer;
        perm_status.resize(nums.size());
        sort(nums.begin(), nums.end());
        dfs_permute_unique(answer, nums, permute, 0, nums.size());
        return answer;
    }

    /** leet 48 **/
    void matrix_rotate(vector<vector<int>>& matrix) {
        int n = matrix.size();

        for (int i = 0; i < n/2; i++)
        {
            for (int j = 0; j < n; j++)
                swap(matrix[i][j], matrix[n-i-1][j]);
        }

        for (int i = 0; i < n; i++)
        {
            for (int j = i; j < n; j++)
                swap(matrix[i][j], matrix[j][i]);
        }
    }

    /** leet 49 **/
    // vector<vector<string>> group_anagrams(vector<string> &strs)
    // {
    //     unordered_map<string, vector<string>> mp;
    //     for (string &str : strs)
    //     {
    //         string key = str;
    //         sort(key.begin(), key.end());
    //         mp[key].emplace_back(str);
    //     }
    //     vector<vector<string>> result;
    //     for (auto it = mp.begin(); it != mp.end(); it++)
    //     {
    //         result.emplace_back(it->second);
    //     }
    //     return result;
    // }
    vector<vector<string>> group_anagrams(vector<string> &strs)
    {
        unordered_map<string, vector<string>> mp;
        string key;
        for (string &str : strs)
        {
            memset(counts, 0, sizeof(int) * 26);
            for (int i = 0; i < str.size(); i++)
            {
                counts[str[i] - 'a']++;
            }
            key = "";
            for (int j = 0; j < 26; j++)
            {
                if (counts[j] != 0)
                {
                    key += ('a' + j);
                    key += counts[j] + '0';
                }
            }
            mp[key].emplace_back(str);
        }
        vector<vector<string>> result;
        for (auto it = mp.begin(); it != mp.end(); it++)
        {
            result.emplace_back(it->second);
        }
        return result;
    }
    // vector<vector<string>> group_anagrams(vector<string> &strs)
    // {
    //     auto array_hash = [fn = hash<int>{}](const array<int, 26> &arr) -> size_t {
    //         return accumulate(arr.begin(), arr.end(), 0u, [&](size_t acc, int num) {
    //             return (acc << 1) ^ fn(num);
    //         });
    //     };

    //     unordered_map<array<int, 26>, vector<string>, decltype(array_hash)> mp(0, array_hash);

    //     for (string &str : strs)
    //     {
    //         array<int, 26> counts{};
    //         int length = str.length();
    //         for (int i = 0; i < length; ++i)
    //         {
    //             counts[str[i] - 'a']++;
    //         }
    //         mp[counts];
    //         cout << str << endl;
    //         mp[counts].emplace_back(str);
    //     }

    //     vector<vector<string>> result;
    //     for (auto it = mp.begin(); it != mp.end(); it++)
    //     {
    //         result.emplace_back(it->second);
    //     }
    //     return result;
    // }
};

/** leet 6 **/
string z_convert(string s, int numRows)
{
    int rows_num = numRows;
    if (rows_num == 1)
        return s;

    int i, j;
    string str;
    int len = s.size();
    int cycle = 2 * rows_num - 2;

    for (i = 0; i < rows_num; i++)
    {
        for (j = 0; j + i < len; j += cycle)
        {
            str += s[j + i];
            if (i != 0 && i != rows_num - 1 && j + cycle - i < len)
            {
                str += s[j + cycle - i];
            }
        }
    }

    return str;
}
static void leet_6_z_convert()
{
    // Solution foo;

    string s = "PAYPALISHIRING";
    int rows = 4;
    // string p = foo.z_convert(s, rows);
    string p = z_convert(s, rows);

    cout << "leet 6: " << s << " z_convert " << rows << " rows is " << p << endl;
}

static void leet_17_letter_combinations()
{
    Solution foo;

    // string digits = "365";
    string digits = "";
    vector<string> combinations = foo.letter_combinations(digits);
    cout << "leet 17: \"" << digits.c_str() << "\" letter_combinations " << combinations.size() << " answer [";
    for (string str : combinations)
        cout << str.c_str() << ",";
    cout << "] " << endl;
}
static void leet_22_generate_parenthesis()
{
    Solution foo;

    int n = 2;
    vector<string> result = foo.generate_parenthesis(n);
    cout << "leet 22: \"" << n << "\" generate_parenthesis " << result.size() << " answer [";
    for (string str : result)
        cout << "\""<< str.c_str() << "\", ";
    cout << "] " << endl;
}

static void leet_39_combination_sum()
{
    Solution foo;
    int target = 7;
    vector<int> candidates = {2, 6, 3, 7};
    // int target = 8;
    // vector<int> candidates = {2,3,5};
    vector<vector<int>> combinations = foo.combination_sum(candidates, target);
    cout << "leet 39: {";
    for (int i : candidates)
        cout << i << ",";
    cout << "} combination_sum = \"" << target << "\" have " << combinations.size() << " answer [ ";

    for (vector<int> a : combinations)
    {
        cout << "[";
        for (int i : a)
            cout << i << ",";
        cout << "] ";
    }
    cout << "] " << endl;
}

static void leet_40_combination_sum()
{
    Solution foo;
    // int target = 7;
    // vector<int> candidates = {2, 6, 3, 7, 1, 5};
    int target = 10;
    vector<int> candidates = {10,1,2,7,6,1,5};
    // vector<int> candidates = {10,1};
    // int target = 8;
    // vector<int> candidates = {2,3,5};
    vector<vector<int>> combinations = foo.combination_sum2(candidates, target);
    cout << "leet 40: {";
    for (int i : candidates)
        cout << i << ",";
    cout << "} combination_sum2 = \"" << target << "\" have " << combinations.size() << " answer [ ";

    for (vector<int> a : combinations)
    {
        cout << "[";
        for (int i : a)
            cout << i << ",";
        cout << "] ";
    }
    cout << "] " << endl;
}

static void leet_47_permute_unique()
{
    Solution foo;
    vector<int> nums = {1, 2, 1, 2};
    vector<vector<int>> result = foo.permute_unique(nums);
    cout << "leet 47: {";
    for (int i : nums)
        cout << i << ",";
    cout << "} permute_unique have" << result.size() << " answer [ ";
    for (vector<int> a : result)
    {
        cout << "[";
        for (int i : a)
            cout << i << ",";
        cout << "] ";
    }
    cout << "] " << endl;
}

static void leet_48_matrix_rotate()
{
    Solution foo;
    vector<vector<int>> matrix = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    cout << "leet 48: matrix [";
    for (vector<int> a : matrix)
    {
        cout << "[";
        for (int i : a)
            cout << i << ",";
        cout << "] ";
    }
    cout << "] matrix_rotate 90 to ";
    foo.matrix_rotate(matrix);
    for (vector<int> a : matrix)
    {
        cout << "[";
        for (int i : a)
            cout << i << ",";
        cout << "] ";
    }
    cout << "] " << endl;
}

static void leet_49_group_anagrams()
{
    Solution foo;
    vector<string> strs = {"eeat", "tea", "tan", "ate", "nat", "bat"};
    cout << "leet 49: input [";
    for (string &s : strs)
        cout << "\"" << s << "\", ";
    cout << "] group_anagrams [ ";
    vector<vector<string>> result = foo.group_anagrams(strs);
    for (vector<string> anagrams : result)
    {
        cout << "[";
        for (string &s : anagrams)
            cout << "\"" << s << "\", ";
        cout << "], ";
    }
    cout << "] " << endl;
}

static int test_accumulate()
{
    struct Grade
    {
        string name;
        int grade;
    };
    Grade subject[3] = {{"English", 80}, {"Biology", 70}, {"History", 90}};

    int sum = accumulate(subject, subject + 3, 0, [](int a, Grade b) { return a + b.grade; });
    cout << "test_accumulate: " << sum << endl;

    return 0;
}
// g++ -g -O0 -o test *.cpp -lpthread
int main(int argc, char *argv[])
{
    leet_49_group_anagrams();
    leet_48_matrix_rotate();
    leet_47_permute_unique();
    leet_40_combination_sum();
    leet_39_combination_sum();
    leet_22_generate_parenthesis();
    leet_17_letter_combinations();
    leet_6_z_convert();
    test_accumulate();

    return 0;
}

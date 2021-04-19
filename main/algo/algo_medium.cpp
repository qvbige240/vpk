
#include <string>
#include <vector>
#include <iostream>

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
            result.push_back(str);

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

// g++ -g -O0 -o test *.cpp -lpthread
int main(int argc, char *argv[])
{
    leet_22_generate_parenthesis();
    leet_17_letter_combinations();
    leet_6_z_convert();

    return 0;
}

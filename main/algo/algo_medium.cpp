
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
static void z_convert_leet_6()
{
    // Solution foo;

    string s = "PAYPALISHIRING";
    int rows = 4;
    // string p = foo.z_convert(s, rows);
    string p = z_convert(s, rows);

    cout << "leet 6: " << s << " z_convert " << rows << " rows is " << p << endl;
}

// g++ -g -O0 -o test *.cpp -lpthread
int main(int argc, char *argv[])
{
    z_convert_leet_6();

    return 0;
}

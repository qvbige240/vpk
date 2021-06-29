
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <queue>
#include <stack>

using namespace std;

class Node
{
public:
    int val;
    Node *left;
    Node *right;
    Node *next;

    Node() : val(0), left(NULL), right(NULL), next(NULL) {}
    Node(int _val) : val(_val), left(NULL), right(NULL), next(NULL) {}
    Node(int _val, Node *_left, Node *_right, Node *_next)
        : val(_val), left(_left), right(_right), next(_next) {}
};

struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

template <typename Type>
class Solution
{
public:
    /** leet 1942, offer 37, same as print_level_order **/
    string serialize(Type *root)
    {
        string result = "[";
        if (!root)
            return "[]";

        string tmp;
        Type *node = nullptr;
        queue<Type *> q;
        q.push(root);
        while (!q.empty())
        {
            node = q.front();
            q.pop();
            if (node)
            {
                result += tmp + to_string(node->val);
                q.push(node->left);
                q.push(node->right);
                tmp = ",";
            }
            else
            {
                tmp += "null,"; // Don't add tmp if string end with null
            }
        }
        result += "]";
        return result;
    }
    Type *deserialize(string data)
    {
        if (data.empty() || data == "[]")
            return nullptr;

        vector<Type *> result;
#if 0
        istringstream iss(data);
        string val;
        while (getline(iss, val, ','))      // how to process '[' ']'
        {
            if (val == "null")
                result.push_back(nullptr);
            else
                result.push_back(new Type(stoi(val)));
        }
#else
        char *str = (char *)calloc(1, data.length());
        // data.copy(p, data.size(), 0);        // copy data.size() from index 0
        data.copy(str, data.length() - 2, 1); // without char: '[' ']'
        char *p = strtok(str, ",");
        // if (!p) return nullptr;
        while (p != NULL)
        {
            string val = p;
            if (val == "null")
                result.push_back(nullptr);
            else
                result.push_back(new Type(stoi(val)));
            p = strtok(NULL, ",");
        }
        free(str);
#endif

        int pos = 1;
        int size = result.size();
        for (int i = 0; pos < size; i++)
        {
            if (result[i] == nullptr)
                continue;
            result[i]->left = result[pos++];
            if (pos < size)
                result[i]->right = result[pos++];
        }
        return result[0];
    }

    Type *connect(Type *root)
    {
        if (!root)
            return nullptr;

        Type *node = nullptr;
        queue<Type *> q;
        q.push(root);
        while (!q.empty())
        {
            int size = q.size();
            for (int i = 0; i < size; i++)
            {
                node = q.front();
                q.pop();
                if (i == size - 1)
                    node->next = nullptr;
                else
                    node->next = q.front();

                if (node->left)
                    q.push(node->left);
                if (node->right)
                    q.push(node->right);
            }
        }
        return root;
    }
};

// template <typename Type>
static void leet_0_serialize()
{
    Solution<Node> foo;
    int target = 22;
    // string data = "[1,2,5,3,4,null,6]";
    string data = "[1,2,6,3,4,null,7,null,null,5]";
    Node *root = foo.deserialize(data);
    // foo.flatten(root);
    cout << "leet 0: tree " << data << ", deserialize and serialize: ";
    cout << foo.serialize(root) << endl;
    // foo.tree_destroy(root);
}
static void leet_0_serialize_treenode()
{
    Solution<TreeNode> foo;
    int target = 22;
    // string data = "[1,2,5,3,4,null,6]";
    string data = "[1,2,6,3,4,null,7,null,null,5]";
    TreeNode *root = foo.deserialize(data);
    // foo.flatten(root);
    cout << "leet 0: tree " << data << ", deserialize and serialize: ";
    cout << foo.serialize(root) << endl;
    // foo.tree_destroy(root);
}

int main(int argc, char *argv[])
{
    leet_0_serialize();                     // serialize
    leet_0_serialize_treenode();
    return 0;
}

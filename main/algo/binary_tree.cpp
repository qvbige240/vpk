
#include <string>
#include <cstring>
#include <vector>
#include <queue>
#include <stack>
#include <climits>
#include <iostream>
#include <sstream>

using namespace std;

struct TreeNode
{
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

/**
 *        5
 *      /   \
 *     4    6
 *    / \  / \
 *   1  2 7  8
 */
// 前序遍历（中左右）：5 4 1 2 6 7 8
// 中序遍历（左中右）：1 4 2 5 7 6 8
// 后序遍历（左右中）：1 2 4 7 8 6 5
class Solution
{
public:
    /** preorder and inorder **/
    TreeNode *build_tree(vector<int> &preorder, vector<int> &inorder)
    {
        int pre_size = preorder.size();
        int ino_size = inorder.size();
        if (pre_size != ino_size || pre_size == 0 || ino_size == 0)
            return nullptr;
        return build(preorder, 0, pre_size - 1, inorder, 0, ino_size - 1);
    }
    TreeNode *build(vector<int> &preorder, int start, int end, vector<int> &inorder, int l, int r)
    {
        TreeNode *root = new TreeNode(preorder[start]);
        int i = l;
        while (i <= r && inorder[i] != preorder[start])
            i++;
        int left_size = i - l;
        int right_size = r - i;
        if (left_size > 0)
            root->left = build(preorder, start + 1, start + left_size, inorder, l, i - 1);
        if (right_size > 0)
            root->right = build(preorder, start + left_size + 1, end, inorder, i + 1, r);

        return root;
    }
    /** inorder and postorder **/
    TreeNode *build_in_post(vector<int> &inorder, int l, int r, vector<int> &postorder, int start, int end)
    {
        TreeNode *root = new TreeNode(postorder[end]);
        int i = l;
        while (i <= r && inorder[i] != postorder[end])
            i++;
        int left_size = i - l;
        int right_size = r - i;
        if (left_size > 0)
            root->left = build_in_post(inorder, l, i - 1, postorder, start, start + left_size - 1);
        if (right_size > 0)
            root->right = build_in_post(inorder, i + 1, r, postorder, start + left_size, end - 1);

        return root;
    }
    TreeNode *build_tree_in_post(vector<int> &inorder, vector<int> &postorder)
    {
        int in_size = inorder.size();
        int post_size = postorder.size();
        if (in_size == 0 || post_size == 0 || in_size != post_size)
            return nullptr;
        return build_in_post(inorder, 0, in_size - 1, postorder, 0, post_size - 1);
    }
    /** preorder and postorder **/
    TreeNode *build_pre_post(vector<int> &preorder, int l, int r, vector<int> &postorder, int start, int end)
    {
        TreeNode *root = new TreeNode(preorder[l]);

        if (l == r)
            return root;

        int i = start;
        while (i <= end && preorder[l + 1] != postorder[i])
            i++;
        int left_size = i - start;

        if (left_size >= 0)
        {
            root->left = build_pre_post(preorder, l + 1, l + 1 + left_size, postorder, start, i);
            root->right = build_pre_post(preorder, r - left_size, r, postorder, i + 1, end - 1);
        }
        return root;
    }
    TreeNode *build_tree_pre_post(vector<int> &preorder, vector<int> &postorder)
    {
        int pre_size = preorder.size();
        int post_size = postorder.size();
        if (pre_size == 0 || post_size == 0 || pre_size != post_size)
            return nullptr;
        return build_pre_post(preorder, 0, pre_size - 1, postorder, 0, post_size - 1);
    }

    /** leet 94 **/
    void inorder(TreeNode *root, vector<int> &result)
    {
        if (!root)
            return;
        /** left center right **/
        inorder(root->left, result);
        result.push_back(root->val);
        inorder(root->right, result);
    }
    void inorder_iterator(TreeNode *root, vector<int> &result)
    {
        if (!root) return;

        stack<TreeNode *> stk;
        while (root || !stk.empty())
        {
            while (root)
            {
                stk.push(root);
                root = root->left;
            }

            root = stk.top();
            stk.pop();
            result.push_back(root->val);
            root = root->right;
        }
    }
    vector<int> inorder_traversal(TreeNode *root)
    {
        vector<int> result;
        // inorder(root, result);
        inorder_iterator(root, result);
        return result;
    }

    void preorder(TreeNode *root, vector<int> &result)
    {
        if (!root)
            return;
        /** center left right **/
        result.push_back(root->val);
        preorder(root->left, result);
        preorder(root->right, result);
    }
    vector<int> preorder_traversal(TreeNode *root)
    {
        vector<int> result;
        preorder(root, result);
        return result;
    }

    void print_preorder(TreeNode *root)     /** center left right **/
    {
        if (root)
            printf("%d,", root->val);
        else
            return;
        print_preorder(root->left);
        print_preorder(root->right);
    }
    void print_inorder(TreeNode *root)      /** left center right **/
    {
        if (!root) return;
        print_inorder(root->left);
        printf("%d,", root->val);
        print_inorder(root->right);
    }
    void print_postorder(TreeNode *root)    /** left right center **/
    {
        if (!root) return;
        print_postorder(root->left);
        print_postorder(root->right);
        printf("%d,", root->val);
    }

    void level_order_print(TreeNode *root, int n)
    {
        if (!root) return;

        TreeNode *node = nullptr;
        queue<TreeNode *> q;
        q.push(root);
        while (!q.empty())
        {
            int size = q.size();
            for (int i = 0; i < size; i++)
            {
                node = q.front();
                q.pop();
                if (node)
                {
                    cout << node->val << ((n != 0) ? "," : "");
                    q.push(node->left);
                    q.push(node->right);
                    if (n-- == 0)
                        return;
                }
                else
                {
                    cout << "null,";
                }
            }
        }
    }

    /** leet 1942, offer 37, same as level_order_print **/
    string serialize(TreeNode *root)
    {
        string result = "[";
        if (!root) return "[]";

        string tmp;
        TreeNode *node = nullptr;
        queue<TreeNode *> q;
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
    TreeNode *deserialize(string data)
    {
        if (data.empty() || data == "[]") return nullptr;

        vector<TreeNode *> result;
#if 0
        istringstream iss(data);
        string val;
        while (getline(iss, val, ','))      // how to process '[' ']'
        {
            if (val == "null")
                result.push_back(nullptr);
            else
                result.push_back(new TreeNode(stoi(val)));
        }
#else
        char *str = (char *)calloc(1, data.length());
        // data.copy(p, data.size(), 0);        // copy data.size() from index 0
        data.copy(str, data.length()-2, 1);     // without char: '[' ']'
        char *p = strtok(str, ",");
        // if (!p) return nullptr;
        while (p != NULL)
        {
            string val = p;
            if (val == "null")
                result.push_back(nullptr);
            else
                result.push_back(new TreeNode(stoi(val)));
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
    /** leet 95 **/
    vector<TreeNode *> generate_trees(int start, int end)
    {
        if (start > end)
            return {nullptr};

        vector<TreeNode *> trees;
        for (int i = start; i <= end; i++)
        {
            vector<TreeNode *> all_left = generate_trees(start, i - 1);
            vector<TreeNode *> all_right = generate_trees(i + 1, end);

            for (auto &left : all_left)
            {
                for (auto &right : all_right)
                {
                    TreeNode *root = new TreeNode(i);
                    root->left = left;
                    root->right = right;
                    trees.emplace_back(root);
                }
            }
        }
        return trees;
    }
    vector<TreeNode *> generate_trees(int n)
    {
        if (n == 0) return {};
        return generate_trees(1, n);
    }

    /** leet 96 **/
    // int num_trees_cnt(int start, int end)
    // {
    //     if (start > end)
    //         return 1;
    //         // return 0;
    //     int cnt = 0;
    //     for (int i = start; i <= end; i++)
    //     {
    //         int left_cnt = num_trees_cnt(start, i - 1);
    //         int right_cnt = num_trees_cnt(i + 1, end);
    //         // if (left_cnt == 0 && right_cnt == 0)
    //         //     cnt += 1;
    //         // else if (left_cnt == 0)
    //         //     cnt += right_cnt;
    //         // else if (right_cnt == 0)
    //         //     cnt += left_cnt;
    //         // else
    //             cnt += left_cnt * right_cnt;
    //     }
    //     return cnt;
    // }
    // int num_trees(int n)
    // {
    //     if (n == 0)
    //         return 0;
    //     return num_trees_cnt(1, n);
    // }
    int num_trees(int num)
    {
        /**
         *  1 ... n  =  1...i-1, i, i+1...n  =  F(i,n)  // X * Y = F, left of i is X, right is Y
         *              -------     -------
         *    F(i,n) =   G(i-1)  *  G(n-i)  // n-i: it's not with the content
         *      of sequence(i+1...n), only with the length of sequence(i+1...n)
         *
         *      G(n) = sum( F(i,n) )  // i=[1,n]
         *      G(n) = sum( G(i-1)*G(n-i) )  // i=[1,n]
         *      G(0) = G(1) = 1
         */
        int G[num + 1];
        memset(G, 0, sizeof(G));
        G[0] = 1;
        G[1] = 1;

        for (int n = 2; n <= num; n++)
        {
            for (int i = 1; i <= n; i++)
            {
                int tmp = G[i - 1] * G[n - i];
                G[n] = G[n] + tmp;
                // G(n) += G(i-1) * G(n-i);
            }
        }
        return G[num];
    }
    // int num_trees(int n)
    // {
    //     /**
    //      * Catalan number:
    //      * C(0) = C(1) = 1
    //      * C(n) = 2(2n-1)/(n+1) * C(n-1)
    //      * C(n+1) = 2(2n+1)/(n+2) * C(n)
    //      */
    //     long long C = 1;
    //     for (int i = 0; i < n; ++i)
    //         C = C * 2 * (2 * i + 1) / (i + 2);
    //     return (int)C;
    // }

    /** leet 98 **/
    bool is_valid_bst(TreeNode *root)
    {
        // if (!root) return false;
        // return valid_bst(root, LONG_MIN, LONG_MAX);
        return valid_bst_iterator(root);
    }
    bool valid_bst(TreeNode *root, long min, long max)
    {
        if (!root) return true;

        if (root->val <= min || root->val >= max)
            return false;

        return valid_bst(root->left, min, root->val) && valid_bst(root->right, root->val, max);
    }
    bool valid_bst_iterator(TreeNode *root)
    {
        if (!root) return true;

        long long prev = LONG_MIN;
        stack<TreeNode *> stk;
        while (root || !stk.empty())
        {
            while (root)
            {
                stk.push(root);
                root = root->left;
            }

            root = stk.top();
            stk.pop();
            if (root->val <= prev)
                return false;

            prev = root->val;
            root = root->right;
        }
        return true;
    }
    /** leet 102 **/
    vector<vector<int>> level_order(TreeNode *root)
    {
        vector<vector<int>> result;
        if (!root) return result;

        TreeNode *node = nullptr;
        queue<TreeNode *> q;
        q.push(root);
        while (!q.empty())
        {
            int size = q.size();
            result.push_back(vector<int>());
            for (int i = 0; i < size; i++)
            {
                node = q.front();
                q.pop();
                result.back().push_back(node->val);
                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }
        }
        return result;
    }

};

/** leet 1942, offer 37 **/
static void leet_1942_serialize_deserialize()
{
    Solution foo;
    string data = "[1,2,3,null,null,4,5]";
    // string data = "[]";
    // string data = "[null]";
    TreeNode *root = foo.deserialize(data);
    cout << "leet 1942: string \"" << data << "\" deserialize and serialize is ";
    cout << foo.serialize(root) << endl;
}
static void leet_102_level_order()
{
    Solution foo;
#if 0
    vector<int> preorder = {5, 4, 1, 2, 6, 7, 8};
    vector<int> inorder = {1, 4, 2, 5, 7, 6, 8};
    TreeNode *root = foo.build_tree(preorder, inorder);
    cout << "leet 102: binary tree size(" << inorder.size() << ") {";
    foo.level_order_print(root, inorder.size() - 1);
    cout << "} level_order is { ";
#else
    string data = "[3,9,20,null,null,15,7]";
    TreeNode *root = foo.deserialize(data);
    cout << "leet 102: binary tree " << data << " serialize:";
    cout << foo.serialize(root) << " level_order is { ";
#endif
    vector<vector<int>> result = foo.level_order(root);
    for (vector<int> a : result)
    {
        cout << "{";
        for (int i : a)
            cout << i << ",";
        cout << "} ";
    }
    cout << "} " << endl;
}
static void leet_98_is_valid_bst()
{
    // Solution foo;
    // vector<int> preorder = {5, 4, 1, 2, 6, 7, 8};
    // vector<int> inorder = {1, 4, 2, 5, 7, 6, 8};
    // TreeNode *root = foo.build_tree(preorder, inorder);
    // foo.print_preorder(root);cout << endl;
    // foo.print_inorder(root);cout << endl;
    // foo.print_postorder(root);cout << endl;
    // cout << "leet 98: binary tree size(" << inorder.size() << ") {";
    // foo.level_order_print(root, inorder.size() - 1);
    // bool result = foo.is_valid_bst(root);
    // cout << "} is \"" << boolalpha << result << "\" binary search tree(bst)" << endl;

    Solution foo;
    string data = "[5,1,4,null,null,3,6]";
    TreeNode *root = foo.deserialize(data);
    bool result = foo.is_valid_bst(root);
    cout << "leet 98: binary tree " << data << " inorder(";
    foo.print_inorder(root);
    cout << ") is \"" << boolalpha << result << "\" binary search tree(bst)" << endl;
}
static void leet_96_num_trees()
{
    Solution foo;
    int n = 3;
    int result = foo.num_trees(n);
    cout << "leet 96: 1~" << n << " binary search tree have " << result << " combinations" << endl;
}
static void leet_95_generate_trees()
{
    Solution foo;
    int n = 3;
    vector<TreeNode *> all = foo.generate_trees(n);
    cout << "leet 95: 1~" << n << " generate_trees binary search tree is { ";
    // for (auto &root : all)
    // {
    //     cout << "{";
    //     // vector<vector<int>> result = foo.level_order(root);
    //     // for (vector<int> a : result)
    //     //     for (int i : a)
    //     //         if (i == 0)
    //     //             cout << "null,";
    //     //         else
    //     //             cout << i << ",";
    //     foo.level_order_print(root, n-1);
    //     cout << "} ";
    // }
    for (auto &root : all)
        cout << foo.serialize(root) << " ";
    cout << "} " << endl;
}
// void create_binary_tree()
// {
//     Solution foo;
//     vector<int> preorder = {5, 4, 1, 2, 6, 7, 8};
//     vector<int> inorder = {1, 4, 2, 5, 7, 6, 8};
//     TreeNode *root = foo.build_tree(preorder, inorder);
//     int i = 0;
//     printf("inorder {");
//     for (i = 0; i < inorder.size(); i++)
//         printf("%d%s", inorder[i], i == inorder.size() - 1 ? "}\n" : ", ");
// }
static void leet_94_inorder_traversal1()
{
    Solution foo;
    vector<int> preorder = {5, 4, 1, 2, 6, 7, 8};
    vector<int> inorder = {1, 4, 2, 5, 7, 6, 8};
    TreeNode *root = foo.build_tree(preorder, inorder);
    vector<int> result = foo.inorder_traversal(root);
    cout << "leet 94: create by preorder and inorder { ";
    for (int i : inorder)
        cout << i << " ";
    cout << "} inorder_traversal is { ";
    for (int i : result)
        cout << i << " ";
    cout << "} " << endl;
}
static void leet_94_inorder_traversal2()
{
    Solution foo;
    // vector<int> preorder = {5, 4, 1, 2, 6, 7, 8};
    vector<int> inorder = {1, 4, 2, 5, 7, 6, 8};
    vector<int> postorder = {1, 2, 4, 7, 8, 6, 5};
    TreeNode *root = foo.build_tree_in_post(inorder, postorder);
    vector<int> result = foo.inorder_traversal(root);
    cout << "leet 94: create by inorder and postorder { ";
    for (int i : inorder)
        cout << i << " ";
    cout << "} inorder_traversal is { ";
    for (int i : result)
        cout << i << " ";
    cout << "} " << endl;
}
static void leet_94_inorder_traversal3()
{
    Solution foo;
    vector<int> preorder = {5, 4, 1, 2, 6, 7, 8};
    // vector<int> inorder = {1, 4, 2, 5, 7, 6, 8};
    vector<int> postorder = {1, 2, 4, 7, 8, 6, 5};
    TreeNode *root = foo.build_tree_pre_post(preorder, postorder);
    vector<int> result = foo.inorder_traversal(root);
    cout << "leet 94: create by preorder and postorder { ";
    for (int i : preorder)
        cout << i << " ";
    cout << "} inorder_traversal is { ";
    for (int i : result)
        cout << i << " ";
    cout << "} " << endl;
}

int main(int argc, char *argv[])
{
    leet_1942_serialize_deserialize();          // string
    leet_102_level_order();
    leet_98_is_valid_bst();                     // dfs(backtrack)
    leet_96_num_trees();                        // dfs(backtrack), catalan number
    leet_95_generate_trees();                   // dfs(backtrack), binary search tree
    leet_94_inorder_traversal3();               // create tree by pre-in-post-order
    leet_94_inorder_traversal2();
    leet_94_inorder_traversal1();
    return 0;
}

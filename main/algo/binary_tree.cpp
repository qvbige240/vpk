
#include <string>
#include <vector>
#include <iostream>

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
    vector<int> inorder_traversal(TreeNode *root)
    {
        vector<int> result;
        inorder(root, result);
        return result;
    }
};

void create_binary_tree()
{
    Solution foo;
    vector<int> preorder = {5, 4, 1, 2, 6, 7, 8};
    vector<int> inorder = {1, 4, 2, 5, 7, 6, 8};
    TreeNode *root = foo.build_tree(preorder, inorder);
    int i = 0;
    printf("inorder {");
    for (i = 0; i < inorder.size(); i++)
        printf("%d%s", inorder[i], i == inorder.size() - 1 ? "}\n" : ", ");
}
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
    leet_94_inorder_traversal3();
    leet_94_inorder_traversal2();
    leet_94_inorder_traversal1();
    return 0;
}

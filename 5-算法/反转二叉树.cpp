/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *  int val;
 *  TreeNode *left;
 *  TreeNode *right;
 *  TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */

// 思路：递归

class Solution {
  public:
  TreeNode* invertTree(TreeNode* root) {
    if (root == NULL)
      return NULL;
    
    TreeNode* temNode = root->left;
    root->left = invertTree(root->right);
    root->right = invertTree(tmpNode);
    return root;
  }
}
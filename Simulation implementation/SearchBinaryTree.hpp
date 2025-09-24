#pragma once
#include <iostream>
using namespace std;
namespace BSTNode_k
{

    template <class k>
    struct BSTNode
    {
        k _key;
        BSTNode<k> *_left;
        BSTNode<k> *_right;
        BSTNode(const k &key = k())
            : _key(key), _left(nullptr), _right(nullptr)
        {}
    };

    template <class k>
    class BSTree
    {
        typedef BSTNode<k> Node;

    public:
        BSTree()
            : _root(nullptr)
        {}

        BSTree(const BSTree &tree)
        {
            _root = _Copy(tree._root);
        }

        // 析构函数无法传参，只能调用子函数进行递归销毁
        ~BSTree()
        {
            // 后序遍历销毁
            _destory(_root);
            _root = nullptr;
        }

        void Inorder()
        {
            _Inorder(_root);
            cout << endl;
        }

        bool Insert(const k &key)
        {
            // 树为空
            if (_root == nullptr)
            {
                _root = new Node(key);
                return true;
            }
            // 树不为空
            Node *cur = _root;
            Node *parent = nullptr;
            while (cur)
            {
                if (cur->_key < key)
                {
                    parent = cur;
                    cur = cur->_right;
                }
                else if (cur->_key > key)
                {
                    parent = cur;
                    cur = cur->_left;
                }
                else
                    return false;
            }
            Node *newNode = new Node(key);
            if (ket < parent->_key)
            {
                parent->_left = newNode;
            }
            else
            {
                parent->_right = newNode;
            }
            return true;
        }

        Node *Find(const k &key)
        {
            Node *cur = _root;
            while (cur)
            {
                // 找到返回
                if (data == cur->_data)
                    return cur;
                // 大于cur，向右找
                else if (data > cur->_data)
                    cur = cur->_right;
                // 小于cur，向左找
                else
                    cur = cur->_left;
            }
            // 没找到return nullptr;
            return nullptr;
        }

        bool Earse(const k &key)
        {
            // 树为空 false
            if (_root == nullptr)
                return false;
            // 树不为空 找删除节点
            Node *cur = _root;
            Node *parent = nullptr;
            while (cur)
            {
                if (cur->_key < key)
                {
                    parent = cur;
                    cur = cur->_right;
                }
                else if (cur->_key > key)
                {
                    parent = cur;
                    cur = cur->_left;
                }
                else
                    break; // cur是要删除的节点
            }
            // cur为空，树中没有要删除的节点
            if (cur == nullptr)
                return false;
            // cur不为空，要删除的节点存在
            // cur最多有一个孩子
            if (cur->_left == nulptr)
            {
                // 只有右孩子
                // cur是根
                if (parent == nullptr)
                {
                    _root = cur->_right
                }
                else
                {
                    if (parent->_left == cur)
                    {
                        parent->_left = cur->_right;
                    }
                    else
                        parent->_right = cur->_right;
                }
                delete cur;
                return true;
            }
            else if (cur->_right == nullptr)
            {
                // 只有左孩子
                // cur是根
                if (parent == nullptr)
                {
                    _root = cur->_left;
                }
                else
                {
                    if (parent->_left == cur)
                    {
                        parent->_left = cur->_left;
                    }
                    else
                        parent->_right = cur->_left;
                }
                delete cur;
                return true;
            }
            // cur有两个孩子
            // 找到cur右子树最小节点
            // 将minNode的值赋给cur
            // 删除minNode
            else
            {
                Node *minNode = cur->_right;
                Node *minParent = cur;
                while (minNode->_left)
                {
                    // 右子树最左节点
                    minParent = minNode;
                    minNode = minNode->_left;
                }
                cur->_key = minNode->_key;
                // 删除minNode
                if (minParent->_left == minNode)
                {
                    minParent->_left = minNode->_right;
                }
                else
                {
                    minParent->_right = minNode->_right;
                }
                delete minNode;
                return true;
            }
        }

    private:
        Node *_root = nullptr;

        void _destory(Node *root)
        {
            if (root == nullptr)
            {
                return;
            }
            _destory(root->_left);
            _destory(root->_right);
            delete root;
            root = nullptr;
        }

        Node *_Copy(const Node *root)
        {
            if (root == nullptr)
                return nullptr;
            Node *newRoot = new Node(root->_data);
            newRoot->_left = _Copy(root->_left);
            newRoot->_right = _Copy(root->_right);

            return newRoot;
        }

        void _Inorder(Node *root)
        {
            if (root == nullptr)
                return;
            _Inorder(root->left);
            cout << root->_key << " ";
            _Inorder(root->right);
        }
    };
}
namespace BSTree_kv
{
    template <class k, class v>
    struct BSTNode
    {
        k _key;
        v _value;
        BSTNode<k, v> *_left;
        BSTNode<k, v> *_right;
        BSTNode(const k &key = k(), const v &value = v())
            : _key(key), _value(value), _left(nullptr), _right(nullptr)
        {}
    };

    template <class k, class v>
    class BSTree
    {
        typedef BSTNode<k, v> Node;

    public:
        BSTree()
            : _root(nullptr)
        {}

        BSTree(const BSTree &tree)
        {
            _root = _Copy(tree._root);
        }

        // 析构函数无法传参，只能调用子函数进行递归销毁
        ~BSTree()
        {
            // 后序遍历销毁
            _destory(_root);
            _root = nullptr;
        }

        void Inorder()
        {
            _Inorder(_root);
            cout << endl;
        }

        bool Insert(const k &key, const v &value)
        {
            // 树为空
            if (_root == nullptr)
            {
                _root = new Node(key, value);
                return true;
            }
            // 树不为空
            Node *cur = _root;
            Node *parent = nullptr;
            while (cur)
            {
                if (cur->_key < key)
                {
                    parent = cur;
                    cur = cur->_right;
                }
                else if (cur->_key > key)
                {
                    parent = cur;
                    cur = cur->_left;
                }
                else
                    return false;
            }
            Node *newNode = new Node(key, value);
            if (key < parent->_key)
            {
                parent->_left = newNode;
            }
            else
            {
                parent->_right = newNode;
            }
            return true;
        }

        Node *Find(const k &key)
        {
            Node *cur = _root;
            while (cur)
            {
                // 找到返回
                if (key == cur->_key)
                    return cur;
                // 大于cur，向右找
                else if (key > cur->_key)
                    cur = cur->_right;
                // 小于cur，向左找
                else
                    cur = cur->_left;
            }
            // 没找到return nullptr;
            return nullptr;
        }

        bool Earse(const k &key)
        {
            // 树为空 false
            if (_root == nullptr)
                return false;
            // 树不为空 找删除节点
            Node *cur = _root;
            Node *parent = nullptr;
            while (cur)
            {
                if (cur->_key < key)
                {
                    parent = cur;
                    cur = cur->_right;
                }
                else if (cur->_key > key)
                {
                    parent = cur;
                    cur = cur->_left;
                }
                else
                    break; // cur是要删除的节点
            }
            // cur为空，树中没有要删除的节点
            if (cur == nullptr)
                return false;
            // cur不为空，要删除的节点存在
            // cur最多有一个孩子
            if (cur->_left == nullptr)
            {
                // 只有右孩子
                // cur是根
                if (parent == nullptr)
                {
                    _root = cur->_right;
                }
                else
                {
                    if (parent->_left == cur)
                    {
                        parent->_left = cur->_right;
                    }
                    else
                        parent->_right = cur->_right;
                }
                delete cur;
                return true;
            }
            else if (cur->_right == nullptr)
            {
                // 只有左孩子
                // cur是根
                if (parent == nullptr)
                {
                    _root = cur->_left;
                }
                else
                {
                    if (parent->_left == cur)
                    {
                        parent->_left = cur->_left;
                    }
                    else
                        parent->_right = cur->_left;
                }
                delete cur;
                return true;
            }
            // cur有两个孩子
            // 找到cur右子树最小节点
            // 将minNode的值赋给cur
            // 删除minNode
            else
            {
                Node *minNode = cur->_right;
                Node *minParent = cur;
                while (minNode->_left)
                {
                    // 右子树最左节点
                    minParent = minNode;
                    minNode = minNode->_left;
                }
                cur->_key = minNode->_key;
                cur->_value = minNode->_value;
                // 删除minNode
                if (minParent->_left == minNode)
                {
                    minParent->_left = minNode->_right;
                }
                else
                {
                    minParent->_right = minNode->_right;
                }
                delete minNode;
                return true;
            }
        }

    private:
        Node *_root = nullptr;

        void _destory(Node *root)
        {
            if (root == nullptr)
            {
                return;
            }
            _destory(root->_left);
            _destory(root->_right);
            delete root;
            root = nullptr;
        }

        Node *_Copy(const Node *root)
        {
            if (root == nullptr)
                return nullptr;
            Node *newRoot = new Node(root->_key, root->_value);
            newRoot->_left = _Copy(root->_left);
            newRoot->_right = _Copy(root->_right);

            return newRoot;
        }
        void _Inorder(Node *root)
        {
            if (root == nullptr)
                return;
            _Inorder(root->left);
            cout << root->_key << " " << root->_value << " ";
            _Inorder(root->right);
        }
    };
}
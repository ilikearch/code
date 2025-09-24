#pragma once
#include<iostream>
#include<assert.h>
using namespace std;

enum Colour
{
	RED,
	BLACK
};

template<class k,class v>
struct BRNode{
    pair<k,v> _kv;
    BRNode<k,v>* _left;
    BRNode<k,v>* _right;
    BRNode<k,v>* _parent;
    Colour _col;

    BRNode(const pair<k,v> &kv)
        : _kv(kv),
        _left(nullptr),
        _right(nullptr),
        _parent(nullptr),
        _col(RED)
    {}
};

template<class k,class v>
class BRTree{
    typedef BRNode<k,v> Node;
public:
    BRTree() = default;

	BRTree(const BRTree<K, V>& t)
	{
		_root = Copy(t._root);
	}

	BRTree<K, V>& operator=(BRTree<K, V> t)
	{
		swap(_root, t._root);
		return *this;
	}

	~BRTree()
	{
		Destroy(_root);
		_root = nullptr;
	}

    Node* find(const k& key){
        Node* cur=_root;
        while(cur){
            if(cur->_kv.first<key){
                cur=cur->_right;
            }
            else if(cur->_kv.first>key){
                cur=cur->_left;
            }
            else{
                return cur;
            }
        }
        return nullptr;
    }

    void Inorder (){
        _Inorder(_root);
        cout<<endl;
    }
    
    bool Insert(const pair<K, V>& kv)
	{
		if (_root == nullptr)
		{
			_root = new Node(kv);
            _root->_col=BLACK;
			return true;
		}

		Node* parent = nullptr;
		Node* cur = _root;
		while (cur)
		{
			if (cur->_kv.first < kv.first)
			{
				parent = cur;
				cur = cur->_right;
			}
			else if (cur->_kv.first > kv.first)
			{
				parent = cur;
				cur = cur->_left;
			}
			else
			{
				return false;
			}
		}

		cur = new Node(kv);
        cur->_parent=parent;
		if (parent->_kv.first < kv.first)
		{
			parent->_right = cur;
		}
		else
		{
			parent->_left = cur;
		}

		while (parent && parent->_col == RED)
		{
			Node* grandfather = parent->_parent;
			//    g
			//  p   u
			if (parent == grandfather->_left)
			{
				Node* uncle = grandfather->_right;
				if (uncle && uncle->_col == RED)
				{
					// u存在且为红 -》变色再继续往上处理
					parent->_col = uncle->_col = BLACK;
					grandfather->_col = RED;

					cur = grandfather;
					parent = cur->_parent;
				}
				else
				{
					// u存在且为黑或不存在 -》旋转+变色
					if (cur == parent->_left)
					{
						//    g
						//  p   u
						//c
						//单旋
						RotateR(grandfather);
						parent->_col = BLACK;
						grandfather->_col = RED;
					}
					else
					{
						//    g
						//  p   u
						//    c
						//双旋
						RotateL(parent);
						RotateR(grandfather);

						cur->_col = BLACK;
						grandfather->_col = RED;
					}

					break;
				}
			}
			else
			{
				//    g
				//  u   p
				Node* uncle = grandfather->_left;
				// 叔叔存在且为红，-》变色即可
				if (uncle && uncle->_col == RED)
				{
					parent->_col = uncle->_col = BLACK;
					grandfather->_col = RED;

					// 继续往上处理
					cur = grandfather;
					parent = cur->_parent;
				}
				else // 叔叔不存在，或者存在且为黑
				{
					// 情况二：叔叔不存在或者存在且为黑
					// 旋转+变色
					//      g
					//   u     p
					//            c
					if (cur == parent->_right)
					{
						RotateL(grandfather);
						parent->_col = BLACK;
						grandfather->_col = RED;
					}
					else
					{
						//		g
						//   u     p
						//      c
						RotateR(parent);
						RotateL(grandfather);
						cur->_col = BLACK;
						grandfather->_col = RED;
					}
					break;
				}				
			}
        }
		return true;
	}

    bool IsBalance(){
        return _IsBalance(_root,0);
    }
    int Height()
	{
		return _Height(_root);
	}

	int Size()
	{
		return _Size(_root);
	}


private:
    void _Inorder(Node* root){
        if(root==nullptr)return 0;
        _Inorder(root->_left);
        cout<<root->_kv.first<<":"<<root->_kv.second<<endl;
        _Inorder(root->_right);
    }

    bool _IsBalance(Node*root,int maxblack){
        if(root==nullptr){
            cout<<maxblack+1<<endl;
            return true;
        }
        Node* parent=root->_parent;
        if(_root->_col==RED&&parent&&parent->_col==RED){
            return false;
        }
        if(root->_col==RED){
            return false;
        }
        if(root->_col==BLACK){
            ++maxblack;
        }
        return _IsBalance(root->_left,maxblack)&&_IsBalance(root->_right,maxblack);
    }

    int _Size(Node* root)
	{
		return root == nullptr ? 0 : _Size(root->_left) + _Size(root->_right) + 1;
	}

    int _Height(Node* root){
        if(root==nullptr)return 0;
        int lheight=_Height(root->_left);
        int rheight=_Height(root->_right);
        return max(lheight,rheight)+1;
    }

    void Destroy(Node* root)
	{
		if (root == nullptr)
			return;

		Destroy(root->_left);
		Destroy(root->_right);
		delete root;
	}

	Node* Copy(Node* root)
	{
		if (root == nullptr)
			return nullptr;

		Node* newRoot = new Node(root->_kv.first, root->_kv.second);
		newRoot->_left = Copy(root->_left);
		newRoot->_right = Copy(root->_right);

		return newRoot;
	}

    void RotateR(Node* parent){
        Node*subR=parent->_right;
        Node*subRL=subR->_left;

        parent->_left=subRL;
        if(subRL){
            subRL->_parent=parent;
        }
        Node* parentparent=parent->_parent;

        subR->_left=parent;
        parent->_parent=subR;

        if (parentParent == nullptr)
		{
			_root = subR;
			subR->_parent = nullptr;
		}
        else{
            if(parent == parentParent->_left)
			{
				parentParent->_left = subR;
			}
			else
			{
				parentParent->_right = subR;
			}
			subR->_parent = parentParent;
        }
    }

    void RotateL(Node* parent){
        Node* subR = parent->_right;
		Node* subRL = subR->_left;

		parent->_right = subRL;
		if(subRL)
			subRL->_parent = parent;

		Node* parentParent = parent->_parent;

		subR->_left = parent;
		parent->_parent = subR;
		
		if (parentParent == nullptr)
		{
			_root = subR;
			subR->_parent = nullptr;
		}
		else
		{
			if (parent == parentParent->_left)
			{
				parentParent->_left = subR;
			}
			else
			{
				parentParent->_right = subR;
			}

			subR->_parent = parentParent;
		}

    }

    void RotateRL(Node* parent){
        Node* subR = parent->_right;
		Node* subRL = subR->_left;
		int bf = subRL->_bf;

		RotateR(parent->_right);
		RotateL(parent);
    }

    void RotateLR(Node* parent){
        Node* subL = parent->_left;
		Node* subLR = subL->_right;
		int bf = subLR->_bf;

		RotateL(parent->_left);
		RotateR(parent);
    }
private:
	Node* _root = nullptr;

};
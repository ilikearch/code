#pragma once
#include<iostream>
#include<assert.h>
using namespace std;

template<class k,class v>
struct BRNode{
    pair<k,v> _kv;
    BRNode<k,v>* _left;
    BRNode<k,v>* _right;
    BRNode<k,v>* _parent;
    int _bf; // balance factor

    BRNode(const pair<k,v> &kv)
        : _kv(kv),
        _left(nullptr),
        _right(nullptr),
        _parent(nullptr),
        _bf(0)
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
		if (parent->_kv.first < kv.first)
		{
			parent->_right = cur;
		}
		else
		{
			parent->_left = cur;
		}
		cur->_parent = parent;

		// 更新平衡因子
		while (parent)
		{
			if (cur == parent->_left)
				parent->_bf--;
			else
				parent->_bf++;

			if (parent->_bf == 0)
			{
				break;
			}
			else if (parent->_bf == 1 || parent->_bf == -1)
			{
				// 继续往上更新
				cur = parent;
				parent = parent->_parent;
			}
			else if (parent->_bf == 2 || parent->_bf == -2)
			{
				// 不平衡了，旋转处理
				if (parent->_bf == 2 && cur->_bf == 1)
				{
					RotateL(parent);
				}
				else if (parent->_bf == -2 && cur->_bf == -1)
				{
					RotateR(parent);
				}
				else if (parent->_bf == 2 && cur->_bf == -1)
				{
					RotateRL(parent);
				}
				else
				{
					RotateLR(parent);
				}

				break;
			}
			else
			{
				assert(false);
			}
		}

		return true;
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

    //判断avl树
    bool _IsAVL(Node* root){
        if(root==nullptr)return true;
        int lheight=_Height(root->_left);
        int rheight=_Height(root->_right);

        int diff=rheight-lheight;

        if(diff!=root->_bf||diff>1||diff<-1){
            return false;
        }

        return _IsAVL(root->_left)&&_IsAVL(root->_right);
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
        parent->_bf = subL->_bf = 0;
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

		parent->_bf = subR->_bf = 0;
    }

    void RotateRL(Node* parent){
        Node* subR = parent->_right;
		Node* subRL = subR->_left;
		int bf = subRL->_bf;

		RotateR(parent->_right);
		RotateL(parent);

		if (bf == 0)
		{
			subR->_bf = 0;
			subRL->_bf = 0;
			parent->_bf = 0;
		}
		else if (bf == 1)
		{
			subR->_bf = 0;
			subRL->_bf = 0;
			parent->_bf = -1;
		}
		else if (bf == -1)
		{
			subR->_bf = 1;
			subRL->_bf = 0;
			parent->_bf = 0;
		}
		else
		{
			assert(false);
		}
    }

    void RotateLR(Node* parent){
        Node* subL = parent->_left;
		Node* subLR = subL->_right;
		int bf = subLR->_bf;

		RotateL(parent->_left);
		RotateR(parent);

		if (bf == 0)
		{
			subL->_bf = 0;
			subLR->_bf = 0;
			parent->_bf = 0;
		}
		else if (bf == 1)
		{
			subL->_bf = 0;
			subLR->_bf = -1;
			parent->_bf = 0;
		}
		else if (bf == -1)
		{
			subL->_bf = 0;
			subLR->_bf =0 ;
			parent->_bf = 1;
		}
		else
		{
			assert(false);
		}
    }
private:
	Node* _root = nullptr;

};
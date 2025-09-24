#include <iostream>
#include <vector>

// 1、一个位置值是负数，那他就是树的根，这个负数绝对值就是这颗树数据个数。
// 2、一个位置值是正数，那他就是双亲的下标
class UnionFind {
public:
    // 构造函数，初始化并查集
    UnionFind(size_t n)
        :_ufs(n,-1)
    {}
    

    // 查找操作
    int FindRoot(int x){
        int parent=x;
        while(_ufs[parent]>=0){
            parent=_ufs[parent];
        }
        return parent;
    }
    // 合并两个集合
    void Union(int x1, int x2) {
        int root1=FindRoot(x1);
        int root2=FindRoot(x2);
        if(root1==root2)return;
        //小根树合并到大根树
        if(root1>root2){
            std::swap(root1,root2);
        }
        _ufs[root1]+=_ufs[root2];
        _ufs[root2]=root1;
    }

    // 判断两个元素是否在同一个集合
    bool connected(int x, int y) {
        return FindRoot(x) == FindRoot(y);
    }

    // 获取集合大小 
    int getSize(int x) {
        return -_ufs[FindRoot(x)];
    }

    // 获取集合个数
    size_t SetSize(){
        size_t size=0;
        for(int i=0;i<_ufs.size();i++){
            if(_ufs[i]<0){
                size++;
            }
        }
        return size;
    }
private:
    std::vector<int> _ufs;
};
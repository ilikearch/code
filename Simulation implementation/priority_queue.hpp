#include<bits/stdc++.h>
#include<queue>
#include<vector>

using namespace std;
template<class T>
struct myless
{
    bool operator()(const T&x,const T&y){
        return x<y;
    }
};
template<class T>
struct mygreater{
    bool operator()(const T&x,const T&y){
        return x>y;
    }
};
template<class T,class container=vector<T>,class com=myless<T>>
class priority_queue{

public:
    priority_queue() = default;
    template <class InputIterator>
    priority_queue(InputIterator first, InputIterator last) {
        while (first!= last)
        {
            con.push_back(*first);
            first++;
        }
        int n=con.size();
        for(int i=(n-1-1)/2;i>=0;i--){
            adjustdown(i);
        }
    }
    void adjustup(int child){
        int parent=(child-1)/2;
        while(child>0){
            if(com(con[parent],con[child])){
                swap(con[parent],con[child]);
                child=parent;
                parent=(child-1)/2;
            }
            else break;
        }    
    }
    void adjustdown(int parent){
        int child=2*parent+1;
        while(child<con.size()){
            if(child+1<con.size()&&com(con[child],con[child+1])){
                child++;
            }
            if(com(con[child],con[parent])){
                swap(con[parent],con[child]);
                parent=child;
                child=2*parent+1;
            }
            else break;
        }
    }
    bool empty() const {
        return con.empty();
    }
    size_t size() const {
        return con.size();
    }
    const T& top() const {
        return con[0];
    }
    void push(const T& value) {
        con.push_back(value);
        adjustup(con.size()-1);
    }
    void pop() {
        swap(con[0],con[con.size()-1]);
        con.pop_back();
        adjustdown(0);
    }
private:
    container con;
};

#include <vector>
#include <list>
template<class T, class Container = std::vector<T>>
class stack
{
public:
    void push(const T& x)
    {
        _t.push_back(x);
    }
    void pop(){
        _t.pop_back();
    }
    const T& top() const{
        return _t.back();
    }
    bool empty() const{
        return _t.empty();
    }
    size_t size(){
        return _t.size();
    }
private:
    Container _t;
};
template<class T, class Container = std::list<T>>
class queue
{
public:
    void push(const T& x)
    {
        _t.push_back(x);
    }
    void pop(){
        _t.pop_front();
    }
    const T& top() const{
        return _t.front();
    }
    bool empty() const{
        return _t.empty();
    }
    size_t size(){
        return _t.size();
    }
    void 
private:
    Container _t;
};
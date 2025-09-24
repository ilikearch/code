#include <assert.h>
//封装节点以实现链表迭代器
template<class T>
struct Listnode
{
    Listnode<T>* _prev;
    Listnode<T>* _next;
    T _data;
    ListNode(const T& data = T())//构造函数，默认构造函数，参数为T类型，默认值为T() T可能为自定义类型
			:_next(nullptr)
			,_prev(nullptr)
			,_data(data)
		{}
};

//封装迭代器
template<class T, class Ref/*=T&*/, class Ptr/*=T*/>

struct ListIterator
{
    typedef ListIterator<T, Ref, Ptr> self;
    typedef Listnode<T> node;
    node* _node;//成员变量，指向当前迭代器指向的节点
    ListIterator(node*node) 
                : _node(node)
    {}
    //前置递增运算符
    self& operator++()
    {
        _node = _node->_next;
        return *this;
    }
    //后置递增运算符
    self operator++(int)
    {
        self tmp = *this;
        _node = _node->_next;
        return tmp;
    }
    //前置递减运算符
    self& operator--()
    {
        _node = _node->_prev;
        return *this;
    }
    //后置递减运算符
    self operator--(int){
        self tmp=*this;
        _node = _node->_prev;
        return tmp;
    }
    //解引用运算符
    Ref operator*() const
    {
        return _node->_data;
    }
    //指针运算符
    Ptr operator->() const
    {
        return &_node->_data;
    }
    //相等运算符
    bool operator==(const self& other) const
    {
        return _node == other._node;
    }
    //不等运算符
    bool operator!=(const self& other) const
    {
        return _node!= other._node;
    }
};

//封装带头双向链表
template<class T>
class List{
    typedef Listnode<T> Node;
private:
    Node* _head;
public:
    //模板实例化 声明迭代器类型
    typedef ListIterator<T, T&, T*> iterator;
    typedef ListIterator<T, const T&, const T*> const_iterator;
    
    List()
    {
        _head = new Node();
        _head->_next = _head->_prev = _head;
    }
    iterator begin()
    {
        return iterator(_head->_next);
    }
    iterator end()
    {
        return iterator(_head);
    }
    const_iterator begin() const{
        return const_iterator(_head->_next);
    }
    const_iterator end() const{ 
        return const_iterator(_head);
    }
    //迭代器失效erase 后 pos失效了，pos指向节点被释放了
    iterator erase(iterator pos)
    {
        assert(pos!= end());
        Node* cur=pos._node;
        Node* next=cur->_next;
        Node* prev=cur->_prev;
        prev->_next=next;
        next->_prev=prev;
        delete cur;
        return iterator(next);
    }
    //插入元素
    iterator insert(iterator pos, const T& data)
    {
        Node *new_node = new Node(data);
        Node *cur=pos._node;
        Node *prev=cur->_prev;
        prev->_next=new_node;
        new_node->_next=cur;
        new_node->_prev=prev;
        cur->_prev=new_node;        
        return iterator(new_node);
    }
    //尾插法
    void push_back(const T& data)
    {
        insert(end(), data);
    }
    //头插法
    void push_front(const T& data)
    {
        insert(begin(), data);
    }
    //弹出尾节点
    void pop_back()
    {
        erase(end()--);
    }
    //弹出头节点
    void pop_front()
    {
        erase(begin());
    }
};
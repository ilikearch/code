#pragma once
#include<string>
#include<iostream>
using namespace std;

template<class K>
struct HashFunc
{
    size_t operator()(const K&key){
        return (size_t)key;
    }
};
//特化
template<>
struct HashFunc<string>
{
    size_t operator()(const string&key){
        size_t hash=0;
        for(auto e:key){
            hash*=31;
            hash+=e;
        }
    }
};

namespace open_address//开放地址
{
    enum State{
        EXIST,
		EMPTY,
		DELETE
    };

    template<class K, class V>
	struct HashData
	{
		pair<K, V> _kv;
		State _state = EMPTY;
	};
    template<class K, class V, class Hash = HashFunc<K>>
    class HashTable
    {
    public:
        HashTable(){
            _table.resize(10);
        }

        bool Insert(const pair<K, V>& kv)
		{
			if (Find(kv.first))
				return false;

			// 扩容
			if (_n * 10 / _tables.size() >= 7)
			{
				//vector<HashData<K, V>> newTables(_tables.size() * 2);
				//// 遍历旧表， 将所有数据映射到新表
				//// ...
				//_tables.swap(newTables);

				HashTable<K, V, Hash> newHT;
				newHT._tables.resize(_tables.size() * 2);
				for (size_t i = 0; i < _tables.size(); i++)
				{
					if (_tables[i]._state == EXIST)
					{
						newHT.Insert(_tables[i]._kv);
					}
				}

				_tables.swap(newHT._tables);
			}

			Hash hs;
			size_t hashi = hs(kv.first) % _tables.size();
			while (_tables[hashi]._state == EXIST)
			{
				++hashi;
				hashi %= _tables.size();
			}

			_tables[hashi]._kv = kv;
			_tables[hashi]._state = EXIST;
			++_n;

			return true;
		}

		HashData<K, V>* Find(const K& key)
		{
			Hash hs;
			size_t hashi = hs(key) % _tables.size();
			while (_tables[hashi]._state != EMPTY)
			{
				if (_tables[hashi]._state == EXIST
					&& _tables[hashi]._kv.first == key)
				{
					return &_tables[hashi];
				}

				++hashi;
				hashi %= _tables.size();
			}

			return nullptr;
		}

		bool Erase(const K& key)
		{
			HashData<K, V>* ret = Find(key);
			if (ret == nullptr)
			{
				return false;
			}
			else
			{
				ret->_state = DELETE;
				return true;
			}
		}

    private:
		vector<HashData<K, V>> _tables;
		size_t _n = 0;  // 表中存储数据个数
    };
};

namespace hash_bucket//哈希桶
{
    template<class K,class V>
    struct HashNode
    {
        pair<K,V> _kv;
        HashNode<K, V>* _next;
        HashNode(const pair<K, V>& kv)
			:_kv(kv)
			,_next(nullptr)
		{}
    };
    template<class K, class V, class Hash = HashFunc<K>>
    class HashTable
    {
        typedef HashNode<K, V> Node;
    public:
        HashTable(){
            _tables.resize(10,nullptr)
        }
        ~HashTable()
		{
			// 依次把每个桶释放
			for (size_t i = 0; i < _tables.size(); i++)
			{
				Node* cur = _tables[i];
				while (cur)
				{
					Node* next = cur->_next;
					delete cur;
					cur = next;
				}
				_tables[i] = nullptr;
			}
		}

        bool Insert(const pair<K, V>& kv)
		{
			Hash hs;
			size_t hashi = hs(kv.first) % _tables.size();

			// 负载因子==1扩容
			if (_n == _tables.size())
			{
				/*HashTable<K, V> newHT;
				newHT._tables.resize(_tables.size() * 2);
				for (size_t i = 0; i < _tables.size(); i++)
				{
					Node* cur = _tables[i];
					while(cur)
					{
						newHT.Insert(cur->_kv);
						cur = cur->_next;
					}
				}

				_tables.swap(newHT._tables);*/

				vector<Node*> newtables(_tables.size() * 2, nullptr);
				for (size_t i = 0; i < _tables.size(); i++)
				{
					Node* cur = _tables[i];
					while (cur)
					{
						Node* next = cur->_next;

						// 旧表中节点，挪动新表重新映射的位置
						size_t hashi = hs(cur->_kv.first) % newtables.size();
						// 头插到新表
						cur->_next = newtables[hashi];
						newtables[hashi] = cur;

						cur = next;
					}

					_tables[i] = nullptr;
				}

				_tables.swap(newtables);
			}

			// 头插
			Node* newnode = new Node(kv);
			newnode->_next = _tables[hashi];
			_tables[hashi] = newnode;
			++_n;

			return true;
		}

		Node* Find(const K& key)
		{
			Hash hs;
			size_t hashi=hs(key)%_tables.size();
            Node cur=_tables[hashi];

            while(cur){
                if(cur->_kv.first==key){
                    return cur;
                }
                cur=cur->_next;
            }

			return nullptr;
		}

		bool Erase(const K& key)
		{
			Hash hs;
			size_t hashi = hs(key) % _tables .size();
			Node* prev = nullptr;
			Node* cur = _tables[hashi];
			while(cur){
                if(cur->_kv.first==key){
                    if(prev=nullptr){
                        _tables[hashi]=cur->_next;
                    }
                    else{
                        prev->_next=cur->_next;
                    }
                    delete cur;
                    --_n;
                    return true;
                }
                prev=cur;
                cur=cur->_next;
            }

			return false;
		}

    private:
		//vector<list<pair<K, V>>> _tables;  // 指针数组
		//struct Bucket
		//{
		//	list<pair<K, V>> _lt;
		//	map<K, V> _m;
		//	size_t _bucketsize;   // >8 map  <=8 list
		//};
		//vector<Bucket> _tables;

		vector<Node*> _tables;  // 指针数组
		size_t _n = 0;			// 表中存储数据个数
    };
};
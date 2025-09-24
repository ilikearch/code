#pragma once
#include<string>
#include<vector>
#include<iostream>
using namespace std;

template<class K>
struct HashFunc
{
	size_t operator()(const K& key)
	{
		return (size_t)key;
	}
};

// 特化
template<>
struct HashFunc<string>
{
	size_t operator()(const string& key)
	{
		size_t hash = 0;
		for (auto e : key)
		{
			hash *= 31;
			hash += e;
		}

		return hash;
	}
};

namespace open_address
{
	enum State
	{
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
		HashTable()
		{
			_tables.resize(10);
		}

		// 15:48
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

	struct StringHashFunc
	{
		size_t operator()(const string& s)
		{
			size_t hash = 0;
			for (auto e : s)
			{
				hash *= 31;
				hash += e;
			}

			return hash;
		}
	};
}

namespace hash_bucket
{
	template<class T>
	struct HashNode
	{
		T _data;
		HashNode<T>* _next;

		HashNode(const T& data)
			:_data(data)
			,_next(nullptr)
		{}
	};

	// 前置声明
	template<class K, class T, class KeyOfT, class Hash>
	class HashTable;

	template<class K, class T, class Ptr, class Ref, class KeyOfT, class Hash>
	struct HTIterator
	{
		typedef HashNode<T> Node;
		typedef HTIterator<K, T, Ptr, Ref, KeyOfT, Hash> Self;

		Node* _node;
		const HashTable<K, T, KeyOfT, Hash>* _pht;

		HTIterator(Node* node, const HashTable<K, T, KeyOfT, Hash>* pht)
			:_node(node)
			,_pht(pht)
		{}

		Ref operator*()
		{
			return _node->_data;
		}

		Ptr operator->()
		{
			return &_node->_data;
		}

		bool operator!=(const Self& s)
		{
			return _node != s._node;
		}

		Self& operator++()
		{
			if (_node->_next)
			{
				// 当前桶还有节点
				_node = _node->_next;
			}
			else
			{
				// 当前桶走完了，找下一个不为空的桶
				KeyOfT kot;
				Hash hs;
				size_t hashi = hs(kot(_node->_data)) % _pht->_tables.size();
				++hashi;
				while (hashi < _pht->_tables.size())
				{
					if (_pht->_tables[hashi])
					{
						break;
					}

					++hashi;
				}

				if (hashi == _pht->_tables.size())
				{
					_node = nullptr; // end()
				}
				else
				{
					_node = _pht->_tables[hashi];
				}
			}

			return *this;
		}
	};

	template<class K, class T, class KeyOfT, class Hash>
	class HashTable
	{
		// 友元声明
		template<class K, class T, class Ptr, class Ref, class KeyOfT, class Hash>
		friend struct HTIterator;

		typedef HashNode<T> Node;
	public:
		typedef HTIterator<K, T, T*, T&, KeyOfT, Hash> Iterator;
		typedef HTIterator<K, T, const T*, const T&, KeyOfT, Hash> ConstIterator;

		Iterator Begin()
		{
			if (_n == 0)
				return End();

			for (size_t i = 0; i < _tables.size(); i++)
			{
				Node* cur = _tables[i];
				if (cur)
				{
					return Iterator(cur, this);
				}
			}

			return End();
		}

		Iterator End()
		{
			return Iterator(nullptr, this);
		}

		ConstIterator Begin() const
		{
			if (_n == 0)
				return End();

			for (size_t i = 0; i < _tables.size(); i++)
			{
				Node* cur = _tables[i];
				if (cur)
				{
					return ConstIterator(cur, this);
				}
			}

			return End();
		}

		ConstIterator End() const
		{
			return ConstIterator(nullptr, this);
		}

		HashTable()
		{
			_tables.resize(10, nullptr);
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

		pair<Iterator, bool> Insert(const T& data)
		{
			KeyOfT kot;
			Iterator it = Find(kot(data));
			if (it != End())
				return make_pair(it, false);

			Hash hs;
			size_t hashi = hs(kot(data)) % _tables.size();

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
						size_t hashi = hs(kot(cur->_data)) % newtables.size();
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
			Node* newnode = new Node(data);
			newnode->_next = _tables[hashi];
			_tables[hashi] = newnode;
			++_n;

			return make_pair(Iterator(newnode, this), true);
		}

		Iterator Find(const K& key)
		{
			KeyOfT kot;
			Hash hs;
			size_t hashi = hs(key) % _tables.size();
			Node* cur = _tables[hashi];
			while (cur)
			{
				if (kot(cur->_data) == key)
				{
					return Iterator(cur, this);
				}

				cur = cur->_next;
			}

			return End();
		}

		bool Erase(const K& key)
		{
			KeyOfT kot;
			Hash hs;

			size_t hashi = hs(key) % _tables .size();
			Node* prev = nullptr;
			Node* cur = _tables[hashi];
			while (cur)
			{
				if (kot(cur->_data) == key)
				{
					if (prev == nullptr)
					{
						_tables[hashi] = cur->_next;
					}
					else
					{
						prev->_next = cur->_next;
					}

					delete cur;
					--_n;
					return true;
				}

				prev = cur;
				cur = cur->_next;
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
}
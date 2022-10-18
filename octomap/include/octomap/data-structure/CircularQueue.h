#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H
#include<bits/stdc++.h>
template<class K, class V>
class CircularQueue {
  private:
    int _front, _back, _count;
    K *_keys;
    V *_values;
    int _maxitems;
  public:
    CircularQueue(int maxitems = 256) { 
      _front = 0; // the frone position is not for storage
      _back = 0;
      _count = 0;
    }
    void init(int maxitems = 256) {
      _front = 0;
      _back = 0;
      _count = 0;
      _maxitems = maxitems;
      _keys = new K[_maxitems];
      _values = new V[_maxitems];
    }
    ~CircularQueue() {
      delete[] _keys;
      delete[] _values;
    }
    inline int count();
    inline int front();
    inline int back();
    inline int maxitems();
    int find(const K &key);
    void push(const K &key, const V &value);
    void pop();
    std::pair<K,V> last();
    std::pair<K,V> getPair(int i);
    void clear();
    void update(int pos, const V &value);
    inline V& getValue(int pos);
    inline K& getKey(int pos);
    inline bool isFull();
};

template<class K, class V>
inline int CircularQueue<K, V>::count() 
{
  return _count;
}

template<class K, class V>
inline int CircularQueue<K, V>::front() 
{
  return _front;
}

template<class K, class V>
inline int CircularQueue<K, V>::back() 
{
  return _back;
}

template<class K, class V>
inline int CircularQueue<K, V>::maxitems() 
{
  return _maxitems;
}

template<class K, class V>
int CircularQueue<K, V>::find(const K &key)
{
  if (_count == 0) {
    return -1;
  }
  for (int i = (_front + 1) % (_maxitems + 1); i != (_back + 1) % (_maxitems + 1); i = (i + 1) % (_maxitems + 1)) {
    if (_keys[i] == key) {
      return i;
    }
  }
  return -1;
}

template<class K, class V>
void CircularQueue<K, V>::push(const K &key, const V &value)
{
  if (_count == _maxitems) {
    // queue is full
    return;
  }
  _keys[_front] = key;
  _values[_front] = value;
  _front--;
  if (_front < 0) {
    _front = _maxitems;
  }
  _count++;
}


template<class K, class V>
void CircularQueue<K, V>::pop() { // pop the last element
  if(_count <= 0) {
    return;
  }
  else {
    _back--;
    if (_back < 0) {
      _back = _maxitems;
    }
    _count--;
  }
}

template<class K, class V>
std::pair<K,V> CircularQueue<K, V>::last() { // return the last element
  assert(_count > 0);
  return std::pair<K,V>(_keys[_back], _values[_back]);
}

template<class K, class V>
std::pair<K,V> CircularQueue<K, V>::getPair(int i) { // return the first element
  assert(_count > 0);
  return std::pair<K,V>(_keys[i], _values[i]);
}


template<class K, class V>
void CircularQueue<K, V>::clear() 
{
  _front = 0;
  _back = 0;
  _count = 0;
}

template<class K, class V>
void CircularQueue<K, V>::update(int pos, const V &value) 
{
  _values[pos] += value;
}

template<class K, class V>
inline V& CircularQueue<K, V>::getValue(int pos) 
{
  return _values[pos];
}

template<class K, class V>
inline K& CircularQueue<K, V>::getKey(int pos) 
{
  return _keys[pos];
}

template<class K, class V>
inline bool CircularQueue<K, V>::isFull() 
{
  return _count == _maxitems;
}

#endif
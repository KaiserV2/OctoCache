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
      _maxitems = maxitems;
      _keys = new K[maxitems + 1];
      _values = new V[maxitems + 1];
    }
    ~CircularQueue() {
      delete[] _keys;
      delete[] _values;
    }
    inline int count();
    inline int front();
    inline int back();
    int find(const K &key);
    void push(const K &key, const V &value);
    void pop();
    std::pair<K,V> last();
    void clear();
    void update(int pos, const V &value);
    V get(int pos);
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
int CircularQueue<K, V>::find(const K &key)
{
  for (int i = (_front + 1) % (_maxitems + 1); i != _back + 1; i = (i + 1) % (_maxitems + 1)) {
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
  std::cout << "put at position: " << _front << std::endl;
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
  if(_count <= 0) {
    return NULL;
  }
  return std::pair<K,V>(_keys[_back], _values[_back]);
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
V CircularQueue<K, V>::get(int pos) 
{
  return _values[pos];
}

#endif
#ifndef AQUEUE_H
#define AQUEUE_H

#include <iostream>

template <class T>
class AQueue {
 private:
  //void operator=(const AQueue&);
  //AQueue (const AQueue&);

  int maxSize;     // max size of queue
  int front_elem;  // index of front element
  int rear_elem;   // index of rear element
  T* arr;          // contains queue elements

 public:
  AQueue(int s = 10);
  ~AQueue();

  void clear();
  void enqueue(const T& item);
  T dequeue();
  void pop();
  const T& front() const;
  const T& back() const;
  int length() const;
  bool empty() const;

};

#endif

#include "aqueue.h"

template <typename T>
AQueue<T>::AQueue(int size) {
  maxSize = size + 1;
  rear_elem = 0; front_elem = 1;
  arr = new T [maxSize];
}

template <typename T>
AQueue<T>::~AQueue() {
  delete [] arr;
}

template <typename T>
void AQueue<T>::clear() {
  rear_elem = 0; 
  front_elem = 1; // reinitialize
}

template <typename T>
void AQueue<T>::enqueue(const T& item) {
  if (length () == maxSize - 1) {
    pop ();
  }
  // assumes queue is not full
  rear_elem = (rear_elem + 1) % maxSize; // circular increment
  arr [rear_elem] = item;
}

// take element out
template <typename T>
T AQueue<T>::dequeue() {
  if (empty ())
    return 0;
  // asserts queue is not empty
  T item = arr [front_elem];
  front_elem = (front_elem + 1) % maxSize;
  return item;
}
// remove element
template <typename T>
void AQueue<T>::pop() {
  front_elem = (front_elem + 1) % maxSize;
}

template <typename T>
const T& AQueue<T>::front() const {
  return arr[front_elem];
}

template <typename T>
const T& AQueue<T>::back() const {
  return arr[rear_elem];
}

template <typename T>
int AQueue<T>::length() const { // return length
  return ((rear_elem + maxSize) - front_elem + 1) % maxSize;
}

template <typename T>
bool AQueue<T>::empty() const {
  if (this->length() == 0) {
    return true; // is empty
  }
  return false; // non empty
}

template class AQueue<int64_t>;

#include "stack.h"

template <typename T>
T Stack<T>::peek() {
  if (isEmpty()) {
    throw std::invalid_argument("Stack Underflow");
  }
  return head->value;
}

template <typename T>
T Stack<T>::pop() {
  auto value = peek();
  head = head->next;
  sizeNum--;
  return value;
}

template <typename T>
void Stack<T>::push(T value) {
  sizeNum++;
  head = Node<T>(value, head);
}

template <typename T>
int Stack<T>::size() {
  return sizeNum;
}

template <typename T>
bool Stack<T>::isEmpty() {
  return sizeNum == 0;
}
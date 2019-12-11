//
// Created by kpfro on 12/10/2019.
//

#ifndef PROGRAM_STACK_H
#define PROGRAM_STACK_H

#include <memory>
#include <map>

template <typename T>
struct Node {
  T value;
  std::unique_ptr<Node<T>> next;
  Node(T value, std::unique_ptr<Node<T>> next) {
    this->value = value;
    this->next = next;
  }
};

template <typename T>
class Stack {
protected:
  std::unique_ptr<Node<T>> head;
  int sizeNum = 0;
public:
  T peek();
  T pop();
  void push(T value);
  int size();
  bool isEmpty();
};
//
//export enum ARType {
//  PROGRAM = 'PROGRAM',
//  PROCEDURE = 'PROCEDURE'
//}
//
//    export const ReturnValue = Symbol('return');
//
//export class ActivationRecord {
//  constructor(
//public readonly name: string,
//public readonly type: ARType,
//public readonly nestingLevel: number,
//public readonly parent?: ActivationRecord,
//public readonly members = new Map<string | symbol, string | boolean | ProcedureDecl>()
//  ) { }
//
//  get(key: string | symbol): any {
//    if (!this.members.has(key) && this.parent !== undefined) {
//      return this.parent.get(key);
//    }
//    return this.members.get(key);
//  }
//
//  set(key: string | symbol, value: any) {
//    return this.members.set(key, value);
//  }
//
//  toString() {
//    const memberVals = Array.from(this.members.entries())
//                           .map(([name, value]) => `\t${name.toString()} : ${value}`)
//    .join(',\n');
//    return `${this.name} (type=${this.type}, nestingLevel=${this.nestingLevel}):\n${memberVals}`;
//  }
//}



#endif //PROGRAM_STACK_H

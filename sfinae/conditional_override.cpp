//
// conditional_override.cpp
// Revl
//
// Created by Naookie Sato on 03/28/2019
// Copyright © 2018 Revl. All rights reserved.
//

#include <iostream>
#include <type_traits>
class Base {
 public:
  static constexpr bool is_sub = false;
  virtual void f() = 0;
};

class Sub : public Base {
 public:
  static constexpr bool is_sub = true;
  void f() override { std::cout << "sub impl" << std::endl; }
};

/*
template <class T, class U = typename std::enable_if<T::is_sub>::type>
class MaybeSub : public T {
 public:
};

template <class T, class U = void>
class MaybeSub : public T {
 public:
  void f() override { std::cout << "maybe sub impl" << std::endl; }
};
*/

template <class T, class U = void>
class MaybeSub : public T {
 public:
  //void f() override { std::cout << "maybe sub impl" << std::endl; }
};

template <class T>
class MaybeSub<T, typename std::enable_if<!T::is_sub>::type> : public MaybeSub<T, bool> {
 public:
  void f() override { std::cout << "maybe sub impl" << std::endl; }
};

int main() {
  MaybeSub<Base> not_sub;
  not_sub.f();
  MaybeSub<Sub> sub;
  sub.f();
  return 0;
}

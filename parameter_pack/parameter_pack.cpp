//
// parameter_pack.cpp
// Revl
//
// Created by Naookie Sato on 04/01/2019
// Copyright © 2018 Revl. All rights reserved.
//

#include <iostream>
#include <tuple>
#include <type_traits>
#include <list>

struct Base {};
struct A : public Base {};
struct B : public Base {};

template<int N, class BaseT, class U, class... T>
struct HasType {
  static constexpr bool value() {
    return std::is_base_of<BaseT, U>() || HasType<N - 1, BaseT, T...>::value();
  }
};


template<class BaseT, class U>
struct HasType<1, BaseT, U> {
  static constexpr bool value() {
    return std::is_base_of<BaseT, U>();
  }
};

template<class BaseT, class... T>
constexpr bool CheckHasType() {
    return HasType<sizeof...(T), BaseT, T...>::value();
}  

template<class U>
void AddToList(std::list<Base>* collection, U to_add) {
  collection->push_back(to_add);
}

template<class U, class... T>
void AddToList(std::list<Base>* collection, U to_add, T... rest) {
  static_assert(std::is_base_of<Base, U>(), "Must be subclasses of Base");
  collection->push_back(to_add);
  AddToList(collection, rest...);
}

template<class HasT, class... T>
std::list<Base> IntoList(T... types) {
  static_assert(HasType<HasT, T...>(), "Must have at least 1 type");
  std::list<Base> result;
  AddToList<T...>(&result, types...);
  return result; 
}

int main() {
  auto r1 = IntoList<A>(A(), B(), B(), B(), A());
  auto r2 = IntoList<A>(B(), B(), B(), B(), B());
  std::cout << "Hello World!\n";
}

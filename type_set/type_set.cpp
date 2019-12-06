//
// type_set.cpp
// Naookie Sato
//
// Created by Naookie Sato on 12/06/2019
// Copyright © 2018 Naookie Sato. All rights reserved.
//

#include <iostream>
#include <string>
#include <type_traits>
#include <memory>
#include <utility>

// Data rep
struct Base {
  std::string name_;
  Base(std::string name) : name_(name) {}
  std::string getName() const { return name_; }
};

// Message one rep
struct A : public Base { 
  constexpr static const char * root = "A"; 
  A() : Base(A::root) {}
};

// Message two rep
struct B : public Base {
  constexpr static const char * root = "B";
  B(): Base(B::root) {}
};

template <class T>
static void validateImpl(const Base &b, std::string s) { std::cout << s << std::endl; }

template <int N, class T, class ...Ts>
struct TWithRoot
{
    // This would return a verdict
  static constexpr void value(const Base &b, const std::string &s)
  {
    if (s == T::root)
    {
      validateImpl<T>(b, s);
    }
    else
    {
      TWithRoot<N-1, Ts...>::value(b, s);
    }
  }
};

template <class T>
struct TWithRoot<1, T>
{
    // This would return a verdict
  static constexpr void value(const Base &b, const std::string &s) {
    if (s == T::root) {
      validateImpl<T>(b, s);
    } else 
    {
      std::cout << "ERROR!" << std::endl;
    }
  }
};

template <class ...Ts>
static void validate(const Base &b, std::string s)
{
  TWithRoot<sizeof...(Ts),Ts...>::value(b, s);
}

int main(int argc, char **argv)
{
  std::unique_ptr<Base> a = std::make_unique<A>();
  std::unique_ptr<Base> b = std::make_unique<B>();
  std::unique_ptr<Base> base = std::make_unique<Base>("Base");
  validate<A>(*a, a->getName());
  validate<A>(*b, b->getName());

  validate<A, B>(*a, a->getName());
  validate<A, B>(*base, "B");
  return 0;
}

//
// enable_if.cpp
// Revl
//
// Created by Naookie Sato on 03/22/2019
// Copyright © 2018 Revl. All rights reserved.
//

#include <type_traits>
#include <iostream>

class NotBase {
 public:
  NotBase() = default;
  virtual ~NotBase() = default;
  NotBase(const NotBase& other) = delete;
  NotBase& operator=(const NotBase& other) = delete;
};

class Base {
 public:
  static constexpr int level = 0;
  Base() = default;
  virtual ~Base() = default;
  Base(const Base& other) = delete;
  Base& operator=(const Base& other) = delete;
};

template <class T> struct is_base : std::false_type {};
template <> struct is_base<Base> : std::true_type {};

class Sub1 : public Base {
 public:
  static constexpr int level = 1;
  Sub1() = default;
  virtual ~Sub1() = default;
  Sub1(const Sub1& other) = delete;
  Sub1& operator=(const Sub1& other) = delete;
};

template <class T> struct is_sub1 : std::false_type {};
template <> struct is_sub1<Sub1> : std::true_type {};

class Sub2 : public Base {
 public:
  static constexpr int level = 1;
  Sub2() = default;
  virtual ~Sub2() = default;
  Sub2(const Sub2& other) = delete;
  Sub2& operator=(const Sub2& other) = delete;
};

template <class T> struct is_sub2 : std::false_type {};
template <> struct is_sub2<Sub2> : std::true_type {};

template <class T> struct is_base_or_sub { static const bool value =
    is_base<T>::value || is_sub1<T>::value || is_sub2<T>::value; };

template <class T>
class Trait1 : public T {
  static_assert(T::level <= 1, "Must be Base, Sub1, or Sub2");
 public:
  static constexpr int level = 2;
  Trait1() = default;
  virtual ~Trait1() = default;
  Trait1(const Trait1& other) = delete;
  Trait1& operator=(const Trait1& other) = delete;
};

template <class T> struct is_trait1 : std::false_type {};
template <class T> struct is_trait1<Trait1<T>> : std::true_type {};

template <class T>
class Trait2 : public T {
 public:
  static constexpr int level = 3;
  static_assert(T::level < level, "base class must have level less than or equal to ");
  Trait2() = default;
  virtual ~Trait2() = default;
  Trait2(const Trait2& other) = delete;
  Trait2& operator=(const Trait2& other) = delete;
};

template <class T> struct is_trait2 : std::false_type {};
template <class T> struct is_trait2<Trait2<T>> : std::true_type {};

template <class T>
class Trait3 : public T {
  static_assert(T::level <= 3, "Must be Base, Sub1, Sub2, or Trait1 or Trait2");
 public:
  static constexpr int level = 4;
  Trait3() = default;
  virtual ~Trait3() = default;
  Trait3(const Trait3& other) = delete;
  Trait3& operator=(const Trait3& other) = delete;
};

int main(int argc, char* argv[]) {
  Base base;
  Sub1 sub1;
  Sub2 sub2;
  Trait1<Base> trait1_from_base;
  Trait1<Sub1> trait1_from_sub1;
  Trait1<Sub1> trait1_from_sub2;
  //Trait1<NotBase> trait1_illegal;
  //Trait1<Trait2<Base>> trait1_illegal2;
  //Trait1<Trait3<Base>> trait1_illegal3;
  Trait2<Base> trait2_from_base;
  Trait2<Sub1> trait2_from_sub1;
  Trait2<Sub2> trait2_from_sub2;
  Trait2<Trait1<Base>> trait2_from_trait1_base;
  Trait2<Trait1<Sub1>> trait2_from_trait1_sub1;
  Trait2<Trait1<Sub2>> trait2_from_trait1_sub2;
  //Trait2<NotBase> trait2_illegal;
  //Trait2<Trait3<Base>> trait2_illegal;
  Trait3<Base> trait3_from_base;
  Trait3<Sub1> trait3_from_sub1;
  Trait3<Sub2> trait3_from_sub2;
  Trait3<Trait1<Base>> trait3_from_trait1_base;
  Trait3<Trait1<Sub1>> trait3_from_trait1_sub1;
  Trait3<Trait1<Sub2>> trait3_from_trait1_sub2;
  Trait3<Trait2<Base>> trait3_from_trait2_base;
  Trait3<Trait2<Sub1>> trait3_from_trait2_sub1;
  Trait3<Trait2<Sub2>> trait3_from_trait2_sub2;
  Trait3<Trait2<Trait1<Base>>> trait3_from_trait2_trait1_base;
  Trait3<Trait2<Trait1<Sub1>>> trait3_from_trait2_trait1_sub1;
  Trait3<Trait2<Trait1<Sub2>>> trait3_from_trait2_trait1_sub2;
  //Trait3<NotBase> trait3_illegal;
  std::cout << 1'000 << std::endl;
  return 0;
}

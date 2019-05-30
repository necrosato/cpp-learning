// Double dispatch
// Naookie Sato

#include <iostream>

struct Base;
struct Sub1;
struct Sub2;

static void DoWorkImpl(const Sub1& sub1_1, const Sub1& sub1_2) {
  std::cout << "Doing work with Sub1 and Sub1" << std::endl;
}
static void DoWorkImpl(const Sub1& sub1, const Sub2& sub2) {
  std::cout << "Doing work with Sub1 and Sub2" << std::endl;
}
static void DoWorkImpl(const Sub2& sub2_1, const Sub2& sub2_2) {
  std::cout << "Doing work with Sub2 and Sub2" << std::endl;
}

struct Base {
  virtual void DoWork(const Base& other) const = 0;
  virtual void DoWork(const Sub1& other) const = 0;
  virtual void DoWork(const Sub2& other) const = 0;
};

struct Sub1 : public Base {
  virtual void DoWork(const Base& other) const { other.DoWork(*this); }
  virtual void DoWork(const Sub1& other) const { DoWorkImpl(*this, other); }
  virtual void DoWork(const Sub2& other) const { DoWorkImpl(*this, other); }
};

struct Sub2 : public Base {
  virtual void DoWork(const Base& other) const { other.DoWork(*this); }
  virtual void DoWork(const Sub1& other) const { DoWorkImpl(other, *this); }
  virtual void DoWork(const Sub2& other) const { DoWorkImpl(*this, other); }
};


static void DoWork(const Base& b1, const Base& b2) {
  b1.DoWork(b2);
}

int main(int argc, char* argv[]) {
  Sub1 s1_1;
  Sub1 s1_2;
  Sub2 s2_1;
  Sub2 s2_2;
  DoWork(s1_1, s1_2);
  DoWork(s1_1, s2_1);
  DoWork(s2_1, s1_1);
  DoWork(s2_1, s2_2);
  return 0;
}

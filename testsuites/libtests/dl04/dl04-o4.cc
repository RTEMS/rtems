class Foo {
public:
  Foo() {};
  ~Foo() {};

  virtual void f1() {};
  virtual void f2() {};
  virtual void f3() {};
  virtual void f4() {};
  virtual void f5() {};
  virtual void f6() {};
  virtual void f7() {};
};

class Bar : public Foo {
};

void baz(void)
{
  Bar b;

  b.f1();
}

extern "C" {
  void func(void)
  {
    baz();
  }
}

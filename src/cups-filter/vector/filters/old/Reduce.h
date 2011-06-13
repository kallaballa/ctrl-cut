#ifndef REDUCE_H_
#define REDUCE_H_

class Reduce
{
public:
  Reduce(float epsilon = 0) : epsilon(epsilon) {}
  virtual ~Reduce() {}

  void filter(class Cut *cut);

private:
  float epsilon;
};

#endif

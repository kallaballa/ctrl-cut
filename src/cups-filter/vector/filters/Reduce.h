#ifndef REDUCE_H_
#define REDUCE_H_

class Reduce
{
public:
  Reduce() {}
  virtual ~Reduce() {}

  void filter(class Cut *cut);
};

#endif

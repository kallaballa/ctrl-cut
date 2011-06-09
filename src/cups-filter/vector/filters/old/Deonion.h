#ifndef DEONION_H_
#define DEONION_H_

class Deonion {
public:
  Deonion()
  {
  }
  virtual ~Deonion();
  void filter(class Cut *cut);
};

#endif

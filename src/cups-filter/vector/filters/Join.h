#ifndef JOIN_H_
#define JOIN_H_

class Join {
public:
  Join(){
  }

  virtual ~Join();
  void filter(class Cut *cut);
};

#endif

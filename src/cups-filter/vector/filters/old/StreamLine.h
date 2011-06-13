#ifndef STREAMLINE_H_
#define STREAMLINE_H_

class StreamLine {
public:
  StreamLine()
  {
  }
  virtual ~StreamLine();
  void filter(class Cut *cut);
};

#endif

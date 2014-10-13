/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include <iostream>
#include <QByteArray>

/**
 * This class implements the streambuf interface to write data to a QByteArray.
 * This class can only be used for writing but not for reading purposes.
 * @author Werner Mayer
 */
class ByteArrayOStreambuf : public std::streambuf
{
public:
  explicit ByteArrayOStreambuf(QByteArray &ba);
  ~ByteArrayOStreambuf();
  
protected:
  virtual int_type overflow(int_type v);
  virtual std::streamsize xsputn (const char* s, std::streamsize num);
  virtual pos_type seekoff(off_type off,
                           std::ios_base::seekdir way,
                           std::ios_base::openmode which =
                           std::ios::in | std::ios::out);
  virtual pos_type seekpos(pos_type sp,
                           std::ios_base::openmode which =
                           std::ios::in | std::ios::out);
  
private:
  class QBuffer *_buffer;
};

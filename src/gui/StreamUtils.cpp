#include "StreamUtils.h"
#include <QBuffer>

ByteArrayOStreambuf::ByteArrayOStreambuf(QByteArray& ba) : _buffer(new QBuffer(&ba))
{
  _buffer->open(QIODevice::WriteOnly);
}

ByteArrayOStreambuf::~ByteArrayOStreambuf()
{
  _buffer->close();
  delete _buffer;
}

std::streambuf::int_type
ByteArrayOStreambuf::overflow(std::streambuf::int_type c)
{
  if (c != EOF) {
    char z = c;
    if (_buffer->write (&z, 1) != 1) {
      return EOF;
    }
  }
  return c;
}

std::streamsize ByteArrayOStreambuf::xsputn (const char* s, std::streamsize num)
{
  return _buffer->write(s,num);
}

std::streambuf::pos_type
ByteArrayOStreambuf::seekoff(std::streambuf::off_type off,
                             std::ios_base::seekdir way,
                             std::ios_base::openmode /*mode*/)
{
  off_type begpos = 0;
  off_type endpos = 0;
  off_type curpos = _buffer->pos();
  switch (way) {
  case std::ios_base::beg:
    begpos = 0;
    endpos = off;
    break;
  case std::ios_base::cur:
    begpos = curpos;
    endpos = begpos + off;
    break;
  case std::ios_base::end:
    begpos = _buffer->size();
    endpos = begpos;
    break;
  default:
    return pos_type(off_type(-1));
  }

  if (endpos != curpos) {
    if (!_buffer->seek(endpos))
      endpos = -1;
  }

  return pos_type(endpos);
}

std::streambuf::pos_type
ByteArrayOStreambuf::seekpos(std::streambuf::pos_type pos,
                             std::ios_base::openmode /*mode*/)
{
  return seekoff(pos, std::ios_base::beg);
}



#ifndef MMAPMATRIX_H_
#define MMAPMATRIX_H_

#include "Image.hpp"
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/file_mapping.hpp>

namespace interproc = boost::interprocess;

template<class T>
class MMapMatrix : public Image<T> {
public:
  std::string filename;
  interproc::file_mapping* m_file;
  interproc::mapped_region m_region;
  std::size_t size;

  MMapMatrix(std::string filename, uint32_t width, uint32_t height, uint8_t components = 3, uint64_t region_off = 0) :
    Image<T>(width, height, components), filename(filename) {
    LOG_DEBUG(region_off);
    this->m_file = new interproc::file_mapping(filename.c_str(), interproc::read_write);
    this->m_region = interproc::mapped_region(*this->m_file, interproc::read_write, 
                                   region_off, width * height * this->bytes_per_pixel);
    this->addr = m_region.get_address();
    this->size = m_region.get_size();
    LOG_DEBUG(this->size);
  }

  MMapMatrix(interproc::file_mapping* m_file, uint32_t width, uint32_t height, uint8_t components = 3) :
    Image<T>(width, height, components), filename(filename), m_file(m_file) {
    this->m_region = interproc::mapped_region(*this->m_file, interproc::read_write, 
                                   0, width * height * this->bytes_per_pixel);
    this->addr = m_region.get_address();
    this->size = m_region.get_size();
  }

  MMapMatrix(const MMapMatrix& other);
};


typedef MMapMatrix<uint8_t> MappedImage;
#endif /* MMAPMATRIX_H_ */


#include "RTypes.h"

using namespace boost::interprocess;

MMapImage::MMapImage(string filename, size_t width, size_t height, size_t bpp, offset_t x, offset_t y) {
	this->filename = filename;
	this->m_file = new file_mapping(filename.c_str(), read_only);
	this->m_region = mapped_region(this->m_file, read_only, x * y * bpp, width * height * bpp);
	this->addr = m_region.get_address();
	this->size = m_region.get_size();
	this->bpp = bpp;
	this->x = x;
	this->y = y;
	this->w = width;
	this->h = height;
}

MMapImage::MMapImage(file_mapping* m_file, string filename, size_t width, size_t height, size_t bpp, offset_t x, offset_t y) {
	this->filename = filename;
	this->m_file = m_file;
	this->m_region = mapped_region(this->m_file, read_only, x * y * bpp, width * height * bpp);
	this->addr = m_region.get_address();
	this->size = m_region.get_size();
	this->bpp = bpp;
	this->x = x;
	this->y = y;
	this->w = width;
	this->h = height;
}

size_t MMapImage::width() {
	return this->w;
}

size_t MMapImage::height() {
	return this->h;
}

offset_t MMapImage::point2offset(Point2D p) {
	return p.y * w + p.x;
}

const unsigned char* MMapImage::point2addr(Point2D p) {
  return (static_cast<unsigned char*>(addr))+point2offset(p);
}

unsigned char MMapImage::pixel(Point2D p){
	return *point2addr(p);
}

MMapImage* MMapImage::tile(offset_t x, offset_t y, size_t width, size_t height) {
	return new MMapImage(this->m_file, this->filename, width, height, this->bpp, x, y);
}

/*
int main(int argc, char *argv[])
{
	MMapImage* img = new MMapImage("/tmp/bla", 10, 10, 8, 0, 0);
	std::cerr << img->pixel({0,0}) << std::endl;
	std::cerr << img->pixel({1,0}) << std::endl;
	std::cerr << img->pixel({2,0}) << std::endl;
	std::cerr << img->pixel({3,0}) << std::endl;
	std::cerr << img->pixel({4,0}) << std::endl;
	std::cerr << img->pixel({5,0}) << std::endl;
	std::cerr << img->pixel({6,0}) << std::endl;
	std::cerr << img->pixel({7,0}) << std::endl;
	std::cerr << img->pixel({8,0}) << std::endl;
	std::cerr << img->pixel({9,0}) << std::endl;
	std::cerr << img->pixel({0,1}) << std::endl;
	std::cerr << img->pixel({1,1}) << std::endl;
	std::cerr << img->pixel({2,1}) << std::endl;
	std::cerr << img->pixel({3,1}) << std::endl;
	std::cerr << img->pixel({4,1}) << std::endl;
	std::cerr << img->pixel({5,1}) << std::endl;
	std::cerr << img->pixel({6,1}) << std::endl;
	std::cerr << img->pixel({7,1}) << std::endl;
	std::cerr << img->pixel({8,1}) << std::endl;
	std::cerr << img->pixel({9,1}) << std::endl;
}
*/

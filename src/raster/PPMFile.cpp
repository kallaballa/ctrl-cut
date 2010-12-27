#include "PPMFile.h"

#ifndef DEBUG
#define DEBUG
#endif

#ifdef DEBUG
  #define DBG(c,val) debug(__LINE__,c, val)
#else
  #define DBG(c,val)
#endif

#define ERR(c,val) error(__LINE__,c, val)

PPMFile::PPMFile() {

}

MMapImage PPMFile::load(string filename) {
	ifstream in;

	in.open(filename.c_str());

	if (!in) {
	    cerr << "Unable to open file datafile.txt";
	    exit(1);   // call system to stop
	}

	int magic, width, height, maxval;

/* if(!check("magic", magic = readMagic(in)))
    return;

  if(!check("width", width = readNum(in, 7)))
    return;

  if(!check("height", height  = readNum(in, 7)))
    return;

  if(!check("maxval", maxval  = readNum(in, 5)))
     return;

  if(maxval != 255) {
    ERR("maxval not supported", maxval);
    return;
  }

	return new MMapImage(filename, width, height, 0, 0, 0);*/
}

void PPMFile::echo(char * c, int val) {
  std::cerr << c;
  std::cerr << '=';
  std::cerr << val << std::endl;
}

void PPMFile::error(int line, char * c, int val) {
	std::cerr << "[E] ";
	std::cerr << line;
	std::cerr << ": ";
  echo(c, val);
}

void PPMFile::debug(int line, char * c, int val) {
	std::cerr << "[D] ";
	std::cerr << line;
	std::cerr << ": ";
    echo(c, val);
}

void PPMFile::debug(char * c, int val[]) {
	std::cerr << "[D] ";
	std::cerr << c;
	std::cerr << " ";
    for(int i = 0; i < sizeof(val); i++) {
    	std::cerr << val[i];
    	std::cerr << " ";
    }
    std::cerr << std::endl;
}

//bRead == v
bool PPMFile::skip(ifstream in, char * name, int v) {
  int r;
  in >> r;
  if(r != v) {
    ERR(name, v);
    return false;
  }
  return true;
}

bool PPMFile::skipSpace(ifstream in) {
  int r;
  in >> r;
  if(!isspace(r)){
    ERR("whitespace expected", r);
    return false;
  }
  return true;
}

//consume \s*, return first none whitespace
int PPMFile::consumeSpace(ifstream in) {
  int r;
  while(in >> r) {
  	if(!isspace(r))
  		break;
  	DBG("consumed", r);
  }

  DBG("not consumed", r);
  return r;
}

//val > -1
bool PPMFile::check(char * name, int val) {
  if(val < 0) {
    ERR(name, val);
    return false;
  }
  DBG(name, val);
  return true;
}

//P\n+\s
int PPMFile::readMagic(ifstream& in)  {
	int r;
	while (in >> r) {
  	if(r == 'P')
			break;
	}
  return readNum(in, 4);
}

int PPMFile::readNum(ifstream& in, int maxdigits) {
  char num[maxdigits];
  int r;

  if(isdigit(num[0] = consumeSpace(in))) {
    bool skipWs = true;
    for(int i = 1; i < maxdigits; i++) {
    	in >> r;
    	if(!isdigit(r)) {
        if(isspace(r)) {
          skipWs = false;
          num[i] = '\0';
          break;
        } else {
          ERR("digit or whitespace expected", num[i]);
          return -1;
        }
      }
      else
        num[i] = r;
    }

    if(skipWs)
      skipSpace(in);
  } else {
    ERR("digit expected", num[0]);
    return -1;
  }
  return atoi(num);
}


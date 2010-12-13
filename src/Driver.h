#ifndef DRIVER_H_
#define DRIVER_H_

#include "vector/Cut.h"
#include <iostream>
#include <fstream>
#include <sstream>

class Driver {
private:


public:
    Driver();
    virtual ~Driver();
    void filter(Cut *cut);
    void process(Cut *cut);
};

#endif /* DRIVER_H_ */

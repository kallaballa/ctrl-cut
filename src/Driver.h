#ifndef DRIVER_H_
#define DRIVER_H_

#include "vector/Cut.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "LaserJob.h"

class Driver {
private:


public:
    Driver();
    virtual ~Driver();
    void filter(LaserJob *job);
    void process(LaserJob *job);
};

#endif /* DRIVER_H_ */

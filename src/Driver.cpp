#include "Driver.h"
#include "LaserJob.h"

Driver::Driver() {
	// TODO Auto-generated constructor stub

}

Driver::~Driver() {
	// TODO Auto-generated destructor stub
}

void Driver::filter(Cut *cut) {
	/*list<Cut*> cuts = job->getCuts();
	for (list<Cut*>::iterator it = cuts.begin(); it != cuts.end(); it++) {

	}*/

}

void Driver::process(Cut *cut) {
	//  filter(job);
	stringstream ss;
	//job->serializeTo(ss);
	std::cerr << ss.str().size();
	std::cout << ss.rdbuf();
}

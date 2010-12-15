#include "Driver.h"
#include "LaserJob.h"
#include "vector/filters/Filters.h"


Driver::Driver() {
	// TODO Auto-generated constructor stub

}

Driver::~Driver() {
	// TODO Auto-generated destructor stub
}

void Driver::filter(LaserJob *job) {
	Explode explode;
	Join join;
	Deonion deonion;

	list<Cut*> cuts = job->getCuts();
	for (list<Cut*>::iterator it = cuts.begin(); it != cuts.end(); it++) {
		Cut *cut = *it;
		cut->xml(std::cout);
		explode.filter(cut);
		cut->xml(std::cout);
		join.filter(cut);
		cut->xml(std::cout);
		deonion.filter(cut);
		cut->xml(std::cout);
	}
}

void Driver::process(LaserJob *job) {
	filter(job);
	stringstream ss;
	job->serializeTo(ss);
	std::cerr << ss.str().size();
	std::cout << ss.rdbuf();
}

/*
 * RasterFilter.h
 *
 *  Created on: 10.11.2009
 *      Author: amir
 */

#ifndef FILTER_H_
#define FILTER_H_

//raster filter ids
#define TP_FILTER_ID "TilePartitionerFilter"
#define TA_FILTER_ID "TileAssemblerFilter"

//vector filter ids
#define SI_FILTER_ID "SplitIntersectionsFilter"
#define FP_FILTER_ID "FindPolylinesFilter"
#define SER_PL_FILTER_ID "SerializePolylinesFilter"
#define DO_FILTER_ID "DeonionFilter"
#define SER_OS_FILTER_ID "SerializeOnionSkinsFilter"

#include <map>
#include <list>
#include <string>
#include <iostream>
#include "LaserPass.h"

using std::map;
using std::list;
using std::string;
using std::cerr;
using std::endl;

class ConditionBase {
public:
	template<class T>
	bool eval(const T actualValue);
};

template<class T>
class Condition: public ConditionBase {
public:
	T expectedValue;
	Condition(const T expectedValue) {
		this->expectedValue = expectedValue;
	}
	bool eval(const T actualValue) {
		return this->expectedValue == actualValue;
	}
};

template<class T>
class BasicCondition: public Condition<T> {

public:
	enum Comparator {
		eq, ne, lt, le, gt, ge, cmp, ncmp
	};
	Comparator comp;

	BasicCondition(const T expectedValue, const Comparator comp) :
		Condition<T> (expectedValue) {
		this->comp = comp;
	}

	bool eval(const T actualValue) {
		switch (comp) {
		case eq:
			return this->expectedValue == actualValue;
		case ne:
			return this->expectedValue != actualValue;
		case lt:
			return this->expectedValue > actualValue;
		case le:
			return this->expectedValue >= actualValue;
		case gt:
			return this->expectedValue < actualValue;
		case ge:
			return this->expectedValue <= actualValue;
		case cmp:
			return ((string) this->expectedValue).compare((string) actualValue)
					== 0;
		case ncmp:
			return ((string) this->expectedValue).compare((string) actualValue)
					!= 0;
		}
		//should never happen since Comparator is an enum
		return false;
	}
};



template<class T>
class ChainCondition: public Condition<T> {
public:
	enum Junctor {
		_and, _or
	};

	struct CCPair {
		Condition<T>* condition;
		Junctor* junctor;
	};

	list<CCPair*> conditionList;

	ChainCondition() : Condition<T>(NULL) {
	}

	ChainCondition* connect(Condition<T>* condition, Junctor* junctor) {
		CCPair ccp = { condition, junctor };
		conditionList.push_back(&ccp);
		return this;
	}

	bool eval(T actualValue) {
		if(conditionList.size() == 0)
			return false;

		bool eval = (*conditionList.begin())->condition->eval(actualValue);
		conditionList.erase(conditionList.begin());
/*
		for(list<CCPair*>::iterator it = conditionList.begin(); it != conditionList.end(); it++) {
			CCPair* ccp = *it;
			if(ccp->junctor == _and)
				eval = eval && ccp->condition->eval(actualValue);
			else
				eval = eval || ccp->condition->eval(actualValue);
		}*/
		return eval;
	}
};

typedef string FilterID;
typedef map<string, ConditionBase*> FilterConditionMap;
typedef set<FilterID > FilterList;

class Filter {
public:
	FilterList provides;
	FilterList preFilters;
	FilterList postFilters;
	FilterConditionMap conditions;
};

//TODO find out how to do interface class declarations without getting "variable not used" warning
class RasterFilter: public Filter {
public:

	RasterFilter(){}
	virtual ~RasterFilter(){}
	void filter(RasterPass* p){ p = p; };
};

class VectorFilter: public Filter {
public:

	VectorFilter(){}
	virtual ~VectorFilter(){}
	void filter(VectorPass* p){ p = p; };
};

#endif /* FILTER_H_ */

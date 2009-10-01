/*
 * Point.h
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#ifndef POINT_HPP_
#define POINT_HPP_

#include <string>
using namespace std;

class Point {
private:
	int x;
	int y;
	string key;
public:
	Point(int x, int y);
	int getX();
	int getY();

	virtual ~Point();
	bool equals(Point *other);
	string getKey();
};




#endif /* POINT_HPP_ */

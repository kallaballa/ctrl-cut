/*
 * Point.h
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#ifndef POINT_HPP_
#define POINT_HPP_

class Point {
public:
	int x;
	int y;


	Point(int x, int y);
	void set(int x, int y);
	virtual ~Point();
	bool equals(Point *other);
};




#endif /* POINT_HPP_ */

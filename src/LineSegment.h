/*
 * Line.h
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */

#ifndef LINE_H_
#define LINE_H_

#include <set>
#include <string>
#include <cmath>

using namespace std;
const float pi = atan2(0, -1);

class LineSegment {
public:
	class LSPoint {
	private:
		int x;
		int y;
		string key;
		void updateKey();
		std::set<LineSegment*> connectors;
	public:
		LSPoint(int x, int y);
		virtual ~LSPoint();
		int getX();
		int getY();
		void setX(int x);
		void setY(int y);
		void setX(int x, bool updateKey);
		void setY(int y, bool updateKey);
		bool equals(LSPoint *other);
		string getKey();
		void addConnector(LineSegment *ls);
		void removeConnector(LineSegment *ls);
		std::set<LineSegment*> getConnectors();
	};
	enum IntersectResult { PARALLEL, COINCIDENT, NOT_INTERSECTING, INTERSECTING };
	LineSegment(LSPoint *start,LSPoint *end, int power);
	virtual ~LineSegment();
	void setStart(LSPoint* start);
	void setEnd(LSPoint* end);
	LSPoint* getStart();
	LSPoint* getEnd();
	void setPower(int power);
	int getPower();
	float getSlope(bool swap=false);
	void swapPoints();
	LSPoint* intersects(LineSegment *otherLine);

private:
	LSPoint *start;
	LSPoint *end;
	int power;
};




#endif /* LINE_H_ */

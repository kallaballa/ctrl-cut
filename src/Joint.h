/*
 * LSPoint.h
 *
 *  Created on: 17.10.2009
 *      Author: amir
 */

#ifndef LSPOINT_H_
#define LSPOINT_H_

#include <set>
#include <string>
#include <cmath>

using namespace std;

class LineSegment;
class Joint {
    private:
        int x;
        int y;
        string key;
        void updateKey();
        std::set<LineSegment*> connectors;
    public:
        Joint(int x, int y);
        virtual ~Joint();
        int getX();
        int getY();
        void setX(int x, bool updateKey=true);
        void setY(int y, bool updateKey=true);
        bool equals(Joint *other);
        string getKey();
        void addConnector(LineSegment *ls);
        void removeConnector(LineSegment *ls);
        std::set<LineSegment*> getConnectors();
    };

#endif /* LSPOINT_H_ */

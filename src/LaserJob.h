/*
 * LaserJob.h
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#ifndef LASERJOB_H_
#define LASERJOB_H_

#include <list>
#include <map>
#include <string>
#include "Laser.h"
#include "LaserPass.h"
#include "LineSegment.h"
#include "OnionSkin.h"
#include "pjl.h"
#include "stdint.h"

using std::string;

/**
 * Convert a big endian value stored in the array starting at the given pointer
 * position to its little endian value.
 *
 * @param position the starting location for the conversion. Each successive
 * unsigned byte is upto nbytes is considered part of the value.
 * @param nbytes the number of successive bytes to convert.
 *
 * @return An integer containing the little endian value of the successive
 * bytes.
 */
static int big_to_little_endian(uint8_t *position, int nbytes) {
    int i;
    int result = 0;

    for (i = 0; i < nbytes; i++) {
        result += *(position + i) << (8 * i);
    }
    return result;
}

struct laser_config {
    /** Variable to track auto-focus. */
    int focus;

    /** Height of the image (y-axis). By default this is the bed's height. */
    int height;

    /** Variable to track the resolution of the print. */
    int resolution;

    /** Variable to track the mode for rasterization. One of color 'c', or
     * grey-scale 'g', mono 'm', or none 'n'
     */
    char raster_mode;

    /** Variable to track the raster speed. */
    int raster_speed;

    /** Variable to track the raster power. */
    int raster_power;

    /** Variable to track whether or not a rasterization should be repeated. */
    int raster_repeat;

    /** FIXME -- pixel size of screen, 0= threshold */
    int screen;

    /** Variable to track the vector speed. */
    int vector_speed;

    /** Variable to track the vector power. */
    int vector_power;

    /** Variable to track the vector frequency. FIXME */
    int vector_freq;

    /** Width of the image (x-axis). By default this is the bed's width. */
    int width;

    /** X re-center (0 = not). */
    int x_center;

    /** Track whether or not to repeat X. */
    int x_repeat;

    /** Y re-center (0 = not). */
    int y_center;

    /** Track whether or not to repeat X. */
    int y_repeat;

    /** The relative x position inside the laser bed to start operating from **/
    int basex;

    /** The relative y position inside the laser bed to start operating from **/
    int basey;

    /** Variable to track whether or not the X axis should be flipped. */
    char flip;
};

typedef list<LaserPass*> LaserPassList;

class LaserJob {
public:
    /** Job name for the print. */
    string name;

    /** User name that submitted the print job. */
    string user;

    /** Title for the job print. */
    string title;


    LaserPassList passes;
    laser_config *lconf;

    LaserJob(laser_config *lconf,
             const string &user, const string &name, const string &title);
    virtual ~LaserJob();

    void optimize();
    void addPass(LaserPass* pass);
    LaserPassList &getPasses() { return passes; }

    void serializeTo(ostream &out);
};

#endif /* LASERJOB_H_ */

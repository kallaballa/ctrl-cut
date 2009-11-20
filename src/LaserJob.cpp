/*
 * LaserJob.cpp
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#define PJL_PARAM
#include "LaserJob.h"
#include "CuttingOptimizer.h"

#include <iostream>
#include "boost/format.hpp"

using boost::format;

LaserJob::LaserJob(laser_config *lconf,
                   const string &user, const  string &name, const string &title) {
    this->lconf = lconf;
    this->user = user;
    this->name = name;
    this->title = title;
}

LaserJob::~LaserJob() {
    // TODO Auto-generated destructor stub
}

void LaserJob::addPass(LaserPass* pass) {
    pass->lconf = this->lconf;
    this->passes.push_back(pass);
}

/**
 *
 */
void LaserJob::serializeTo(ostream &out) {
    int i;
    /* Print the printer job language header. */

    out << format(PJL_HEADER) % this->title.c_str();
    /* Set autofocus on or off. */
    out << format(PCL_AUTOFOCUS) % this->lconf->focus;
    /* FIXME unknown purpose. */
    out << PCL_UNKNOWN_BLAFOO;
    /* FIXME unknown purpose. */
    out << PCL_UNKNOWN_BLAFOO2;
    /* Left (long-edge) offset registration.  Adjusts the position of the
     * logical page across the width of the page.
     */
    out << format(PCL_OFF_X) % 0;
    /* Top (short-edge) offset registration.  Adjusts the position of the
     * logical page across the length of the page.
     */
    out << format(PCL_OFF_Y) % 0;
    /* Resolution of the print. */
    out << format(PCL_PRINT_RESOLUTION) % this->lconf->resolution;
    /* X position = 0 */
    out << format(PCL_POS_X) % 0;
    /* Y position = 0 */
    out << format(PCL_POS_Y) % 0;
    /* PCL resolution. */
    out << format(PCL_RESOLUTION) % this->lconf->resolution;

    /* If raster power is enabled and raster mode is not 'n' then add that
     * information to the print job.
     */

     if (this->lconf->raster_power && this->lconf->raster_mode != 'n') {
    /* We're going to perform a raster print. */
         /* We're going to perform a vector print. */
         LaserPassList &passes = this->getPasses();

         LaserPassList::iterator it;
         RasterPass *rpass;
         for (it = passes.begin(); it != passes.end(); it++) {
             rpass = dynamic_cast<RasterPass*> (*it);
             if (rpass) {
                 rpass->serializeTo(out);
             }
         }
    }

    /* If vector power is > 0 then add vector information to the print job. */
    if (this->lconf->vector_power) {
        out << format(R_ORIENTATION) % 0;
        out << format(R_POWER) % 50;
        out << format(R_SPEED) % 50;
        out << PCL_UNKNOWN_BLAFOO3;
        out << format(R_HEIGHT) % (this->lconf->height * this->lconf->y_repeat);
        out << format(R_WIDTH) % (this->lconf->width * this->lconf->x_repeat);

        /* seems to be obsolete, but windows driver does it*/
        //out << R_COMPRESSION;
        out << PCL_SECTION_END;

        /* We're going to perform a vector print. */
        LaserPassList &passes = this->getPasses();

        LaserPassList::iterator it;
        VectorPass *vpass;
        for (it = passes.begin(); it != passes.end(); it++) {
            vpass = dynamic_cast<VectorPass*> (*it);
            if (vpass) {
                vpass->serializeTo(out);
            }
        }

    }

    out << PCL_RESET;
    out << PJL_FOOTER;

    /* Pad out the remainder of the file with 0 characters. */
    for (i = 0; i < 4096; i++) {
        out << '\0';
    }
}


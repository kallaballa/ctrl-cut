#define PJL_PARAM

#include "LaserJob.h"

using boost::format;

LaserJob::LaserJob(LaserConfig *lconf, const string &user, const string &name,
		const string &title) {
	this->lconf = lconf;
	this->user = user;
	this->name = name;
	this->title = title;
}

LaserJob::~LaserJob() {
	// TODO Auto-generated destructor stub
}

void LaserJob::addCut(Cut* cut) {
	this->cuts.push_back(cut);
}

void LaserJob::addRaster(Raster* raster) {
	this->rasters.push_back(raster);
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

	//if (this->lconf->raster_power && this->lconf->raster_mode != 'n') {
		/* We're going to perform a raster print. */
	/*	for (list<Raster*>::iterator it = this->rasters.begin(); it != this->rasters.end(); it++) {
			Raster *raster = *it;
			if (raster) {
				PclRenderer r(this->lconf);
				r.renderRaster(raster, out);
			}
		}*/
	//}

	/* If vector power is > 0 then add vector information to the print job. */
	if (this->lconf->vector_power) {
		out << format(R_ORIENTATION) % 0;
		out << format(R_POWER) % 50;
		out << format(R_SPEED) % 50;
		out << PCL_UNKNOWN_BLAFOO3;
		// New version; always use bed size until the Windows driver tells us smth. else..
		out << format(R_HEIGHT) % (BED_HEIGHT);
		out << format(R_WIDTH) % (BED_WIDTH);
		// FIXME: Old version, kept for reference
		//     out << format(R_HEIGHT) % (this->lconf->height * this->lconf->y_repeat);
		//     out << format(R_WIDTH) % (this->lconf->width * this->lconf->x_repeat);

		/* seems to be obsolete, but windows driver does it*/
		out << format(R_COMPRESSION) % 2;

		// FIXME: This is to emulate the LT bug in the Epilog drivers:
		// Check if any clipping has been done in any of the passes, and
		// inject the stray "LT" string. This has no function, just for bug compatibility
		// of the output files. See corresponding FIXME in LaserJob.cpp.
		for (list<Cut*>::iterator it = this->cuts.begin(); it != this->cuts.end(); it++) {
			Cut *cut = *it;
			if (cut && cut->wasClipped())
				out << "LT";
		}

		out << PCL_SECTION_END;

		/* We're going to perform a vector print. */
		for (list<Cut*>::iterator it = this->cuts.begin(); it != this->cuts.end(); it++) {
			Cut *cut = *it;
			if (cut) {
				Renderer r(this->lconf);
				r.renderCut(cut, out);
			}
		}

	}

	out << PCL_RESET;
	out << PJL_FOOTER;

	/* Pad out the remainder of the file with spaces and 'footer'. */
	for (i = 0; i < 4092; i++) {
		out << ' ';
	}
	out << "Mini]";
}

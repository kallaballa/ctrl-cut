/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef EVENTFILTER_H_
#define EVENTFILTER_H_

#include <QObject>
#include <QFileOpenEvent>
#include "MainWindow.h"

class EventFilter : public QObject
{
	Q_OBJECT
	
public:
	EventFilter(QObject *parent) : QObject(parent) {}
protected:
	bool eventFilter(QObject *obj, QEvent *event) {
		// Handle Apple event for opening files
		if (event->type() == QEvent::FileOpen) {
			QFileOpenEvent *foe = static_cast<QFileOpenEvent *>(event);
			MainWindow::instance()->openFile(foe->file());
			return true;
		} else {
			// standard event processing
			return QObject::eventFilter(obj, event);
		}
	}
};

#endif

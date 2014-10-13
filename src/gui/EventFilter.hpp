/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef EVENTFILTER_H_
#define EVENTFILTER_H_

#include <QObject>
#include <QFileOpenEvent>
#include "MainWindow.hpp"

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

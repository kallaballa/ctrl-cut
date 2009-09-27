package org.wooden.kallaballa.gui;

import java.awt.event.MouseEvent;
import java.awt.event.MouseMotionListener;

public class IdleListener implements MouseMotionListener {

	public void mouseDragged(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	public void mouseMoved(MouseEvent e) {
		IdleTimer.getInstance().announceMove();
	}
}

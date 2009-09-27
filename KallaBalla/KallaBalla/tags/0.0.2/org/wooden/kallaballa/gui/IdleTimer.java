package org.wooden.kallaballa.gui;

import org.wooden.kallaballa.KallaBalla;

public class IdleTimer extends Thread {
	private long lastMove;
	private long idleThreshold = 1000 * 60 * 5;
	private static IdleTimer instance;

	public IdleTimer() {
		this.instance = this;
		announceMove();
	}

	public static IdleTimer getInstance() {
		return instance;
	}

	public void run() {

		while (true) {
			try {
				Thread.sleep(1000);
				System.out.println("idle: " + isIdle());
				if (isIdle()) {
					ResultViewer.getInstance().animateResults(
							KallaBalla.instance().getAlgorithms(),true);
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

	}

	public synchronized boolean isIdle() {
		return (System.currentTimeMillis() - idleThreshold) > lastMove;
	}

	public synchronized void announceMove() {
		System.out.println("announce");
		lastMove = System.currentTimeMillis();
	}
}
package org.wooden.kallaballa.gui;
import java.awt.*;
import javax.swing.*;

import org.wooden.component.ComponentTool;

public class ProgressDialog extends JDialog {

	private JProgressBar progressBar;

	public ProgressDialog(Window owner) {
		super(owner);
		setLayout(new BorderLayout());
		setModalityType(ModalityType.APPLICATION_MODAL);
		setTitle("Please wait...");
		
		progressBar = new JProgressBar(0, 100);
		progressBar.setValue(0);
		progressBar.setStringPainted(true);

		JPanel panel = new JPanel();
		panel.setBorder(BorderFactory.createEmptyBorder(20, 20, 20, 20));
		panel.add(progressBar);

		add(panel, BorderLayout.PAGE_START);
		pack();
		ComponentTool.centerOnScreen(this);
	}

	public synchronized void start() {
//				setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
//				setVisible(true);
	}

	public synchronized void setProgress(int progress) {
//		progressBar.setValue(progress);
	}

	public synchronized void done() {
//		setCursor(null);
//		setVisible(false);
//		progressBar.setValue(0);
	}

	public static void main(String[] args) {
	}
}

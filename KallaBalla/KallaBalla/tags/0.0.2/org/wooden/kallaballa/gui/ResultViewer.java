package org.wooden.kallaballa.gui;

import java.awt.AlphaComposite;
import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.image.BufferedImage;

import javax.swing.BorderFactory;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.Border;

import org.wooden.component.ComponentTool;
import org.wooden.kallaballa.KallaBalla;
import org.wooden.kallaballa.algorithms.Algorithm;
import org.wooden.kallaballa.util.JAITool;

public class ResultViewer extends JFrame implements ComponentListener {
	private BufferedImage outterImage;
	private BufferedImage innerImage;
	private BufferedImage screenBuffer;
	private boolean animating = false;
	private JPanel canvas;
	private static ResultViewer instance;
	
	public ResultViewer(BufferedImage outterImager) {
		this.outterImage = outterImager;
		this.instance = this;
		initLayout();
		GraphicsDevice[] devices = GraphicsEnvironment
				.getLocalGraphicsEnvironment().getScreenDevices();
		if (devices.length > 1)
			ComponentTool.setFullScreen(devices[1], this);
		addComponentListener(this);
		addMouseMotionListener(new IdleListener());
		// setSize(1024, 768);
		// setExtendedState(MAXIMIZED_BOTH);

	}

	public static ResultViewer getInstance()
	{
		return instance;
	}
	
	public void initLayout() {
		setDefaultCloseOperation(HIDE_ON_CLOSE);

		canvas = new JPanel(false) {
			public void paint(Graphics g){ 
				g.drawImage(screenBuffer, 0, 0, this);
			}
		};
		canvas.addMouseMotionListener(new IdleListener());
		setContentPane(canvas);
	}

	public void setResultImage(BufferedImage innerImage) {
		this.innerImage = innerImage;
		updateScreen();
	}

	public BufferedImage createScreenBuffer() {
		return new BufferedImage(getWidth(), getHeight(),
				BufferedImage.TYPE_INT_ARGB);
	}

	public void clear() {
		clear(screenBuffer.getGraphics(), Color.BLACK);
	}

	public void clear(Graphics g, Color c) {
		g.setColor(Color.BLACK);
		g.fillRect(0, 0, getWidth(), getHeight());
	}

	public void drawBackground() {
		drawBackground(screenBuffer.getGraphics());
	}

	public void drawBackground(Graphics g) {
		int h = getHeight();
		int w = getWidth();

		BufferedImage scaledOutter = JAITool.scaleY(outterImage, h)
				.getAsBufferedImage();
		int offsetX = (w - scaledOutter.getWidth()) / 2;
		int offsetY = (h - scaledOutter.getHeight()) / 2;
		g.drawImage(scaledOutter, offsetX, offsetY, this);
	}

	public void drawResult() {
		Graphics scrGraphics = screenBuffer.getGraphics();
		int h = getHeight();
		int w = getWidth();

		if (innerImage != null) {
			BufferedImage scaledInner = JAITool.scaleY(innerImage, (h / 2))
					.getAsBufferedImage();
			int offsetX = (w - scaledInner.getWidth()) / 2;
			int offsetY = (h - scaledInner.getHeight()) / 2;

			scrGraphics.drawImage(scaledInner, offsetX, offsetY, this);
		}
	}

	public void updateScreen() {
		if (!isVisible() || animating)
			return;
		int w = getWidth();
		int h = getHeight();
		if (screenBuffer == null || w != screenBuffer.getWidth()
				|| h != screenBuffer.getHeight())
			screenBuffer = createScreenBuffer();

		clear();
		drawBackground();
		drawResult();
		canvas.repaint();
		this.repaint();
	}

	public void animateResults(Algorithm[] algorithms, boolean idleSensitive) {
		animating = true;
		Graphics2D canvasGraphics = (Graphics2D) canvas.getGraphics();
		Graphics2D bufferGraphics = (Graphics2D) screenBuffer.getGraphics();
		String[] transDesc = new String[] { "Warp intensity & saturation",
				"Shift curves", "Lookup and interpolate hue" };
		BufferedImage bg;
		BufferedImage overlay;
		int h = getHeight();
		int w = getWidth();
		int alphasteps = 66;
		AlphaComposite opaqueComposite = AlphaComposite.getInstance(
				AlphaComposite.SRC_OVER, 1);
		BufferedImage scaledOutter = JAITool.scaleY(outterImage, h)
				.getAsBufferedImage();
		int offsetX = (w - scaledOutter.getWidth()) / 2;
		int offsetY = (h - scaledOutter.getHeight()) / 2;

		BufferedImage[] layers = new BufferedImage[algorithms.length - 1];
		for (int i = 0; i < layers.length; i++) {
			if(idleSensitive && !IdleTimer.getInstance().isIdle())
				break;
			
			layers[i] = JAITool.scaleY(
					KallaBalla.instance().createBufferedImage(
							algorithms[i + 1].getCachedResult()), (h / 2))
					.getAsBufferedImage();
		}

		drawBackground(bufferGraphics);

		for (int i = 0; i < layers.length - 1; i++) {
			if(idleSensitive && !IdleTimer.getInstance().isIdle())
				break;
			bg = layers[i];
			overlay = layers[i + 1];

			for (int j = 0; j < alphasteps ; j++) {
				if(idleSensitive && !IdleTimer.getInstance().isIdle())
					break;
				AlphaComposite overlayComposite = AlphaComposite
						.getInstance(AlphaComposite.SRC_OVER,
								((1 / (float) alphasteps) * j));
				AlphaComposite titleComposite = AlphaComposite.getInstance(
						AlphaComposite.SRC_OVER,
						(1 - ((1 / (float) alphasteps) * j)));

				bufferGraphics.setComposite(opaqueComposite);
				bufferGraphics.drawImage(bg, (getWidth() - bg.getWidth()) / 2,
						(getHeight() - bg.getHeight()) / 2, this);
				bufferGraphics.setComposite(overlayComposite);
				bufferGraphics.drawImage(overlay, (getWidth() - overlay
						.getWidth()) / 2,
						(getHeight() - overlay.getHeight()) / 2, this);

				System.out.println(overlayComposite.getAlpha());
				System.out.println(titleComposite.getAlpha());
				bufferGraphics.setComposite(opaqueComposite);
				bufferGraphics.setColor(Color.LIGHT_GRAY);
				bufferGraphics.fillRect(0, 0, getWidth(), 70);
				bufferGraphics.fillRect(0, getHeight() - 70, getWidth(), 70);
				bufferGraphics.setColor(Color.WHITE);
				bufferGraphics
						.fillRect(offsetX, 0, scaledOutter.getWidth(), 70);

				bufferGraphics.setComposite(titleComposite);
				bufferGraphics.setColor(Color.BLACK);
				bufferGraphics.setFont(new Font("SansSerif", Font.PLAIN, 30));
				bufferGraphics.drawString(transDesc[i], 150, 50);

				if (j == 0) {
					bufferGraphics.setComposite(opaqueComposite);
					bufferGraphics.setColor(Color.RED);
					bufferGraphics.drawRect(
							(getWidth() - overlay.getWidth()) / 2,
							(getHeight() - overlay.getHeight()) / 2, overlay
									.getWidth() - 1, overlay.getHeight() - 1);
					Graph.getInstance().cbGraphSelect.setSelectedIndex(i + 1);
					Border border = BorderFactory.createLineBorder(Color.RED);
					if (i == (algorithms.length - 2))
						Graph.getInstance().graphPanels[1].setBorder(border);
					else {
						Graph.getInstance().graphPanels[0].setBorder(border);
						Graph.getInstance().graphPanels[2].setBorder(border);
					}

				}
				canvasGraphics.drawImage(screenBuffer, 0, 0, this);
				if (j == 0) {
					//Graph.getInstance().drawIHSGraph(algorithms[i + 1], true);
					
					
					try {
						Thread.sleep(5000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					if (i == (algorithms.length - 2))
						Graph.getInstance().graphPanels[1].setBorder(null);
					else {
						Graph.getInstance().graphPanels[0].setBorder(null);
						Graph.getInstance().graphPanels[2].setBorder(null);
					}
				}

			}
		}
		Graph.getInstance().cbGraphSelect.setSelectedIndex(algorithms.length - 1);
		bufferGraphics.dispose();
		canvasGraphics.dispose();
		animating = false;
		updateScreen();
	}

	public void componentResized(ComponentEvent e) {
		updateScreen();
	}

	public void componentMoved(ComponentEvent e) {
	}

	public void componentShown(ComponentEvent e) {
	}

	public void componentHidden(ComponentEvent e) {
	}
}

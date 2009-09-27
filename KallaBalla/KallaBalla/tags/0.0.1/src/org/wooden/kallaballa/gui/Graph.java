package org.wooden.kallaballa.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.GraphicsDevice;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Robot;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.util.Vector;

import javax.media.jai.JAI;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.xy.DefaultTableXYDataset;
import org.jfree.data.xy.XYSeries;
import org.wooden.component.ComponentTool;
import org.wooden.kallaballa.KallaBalla;
import org.wooden.kallaballa.algorithms.Algorithm;
import org.wooden.kallaballa.ihs.IHSLookup;
import org.wooden.kallaballa.ihs.IHSPixel;

public class Graph extends JFrame {
	private IHSLookup lookupTable;
	public JList cbGraphSelect;
	private JTextArea txtStats;
	private ResultViewer viewer;
	private static Graph instance;
//	private JButton btnRender = new JButton("Render");
	private JButton btnAnimate = new JButton("Animate");
	//private JButton btnShow = new JButton("Show");
	//private JButton btnSave = new JButton("Save");
	private ProgressDialog progressDialog = null;// new ProgressDialog(this);
	private final KallaBalla kallaBalla;

	private int[] glookupI;
	private int[] glookupH;
	private int[] glookupS;

	private int[] gRevLookupI;
	private int[] gRevLookupH;
	private int[] gRevLookupS;

	private int[] gResultI;
	private int[] gResultH;
	private int[] gResultS;

	private DefaultTableXYDataset datasetI = new DefaultTableXYDataset();
	private DefaultTableXYDataset datasetH = new DefaultTableXYDataset();
	private DefaultTableXYDataset datasetS = new DefaultTableXYDataset();

	private IHSPixel[] result;
	public JPanel[] graphPanels;
	public Graph(KallaBalla ie, GraphicsDevice device) {
		super(device.getDefaultConfiguration());
		this.instance = this;
		initLayout();
		ComponentTool.setFullScreen(device, this);
		viewer = new ResultViewer(JAI.create("fileload", "8.jpg")
				.getAsBufferedImage());
		this.kallaBalla = ie;
		this.lookupTable = ie.getIHSLookup();
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		addMouseMotionListener(new IdleListener());
	}

	public static Graph getInstance()
	{
		return instance;
	}
	public static void setFullScreen(GraphicsDevice device, JFrame frame) {
		boolean isFullScreen = device.isFullScreenSupported();
		frame.setUndecorated(isFullScreen);
		frame.setResizable(!isFullScreen);
		if (isFullScreen && !frame.isDisplayable()) {
			// Full-screen mode
			device.setFullScreenWindow(frame);
			frame.validate();
		} else {
			// Windowed mode
			frame.pack();
			frame.setVisible(true);
		}
	}

	private JScrollPane createGraphPane() {
		JPanel container = new JPanel(false);

		container.setBackground(Color.WHITE);
		GridBagConstraints cGraph = new GridBagConstraints();
		GridBagLayout graphLayout;
		cGraph.fill = GridBagConstraints.HORIZONTAL;
		cGraph.gridwidth = GridBagConstraints.REMAINDER;

		container.setLayout(graphLayout = new GridBagLayout());

		JPanel graphI = createGraphPanel("", "Intensity", "Pixels", datasetI);
		JPanel graphH = createGraphPanel("", "Hue", "Pixels", datasetH);
		JPanel graphS = createGraphPanel("", "Saturation", "Pixels", datasetS);
		graphI.setPreferredSize(new Dimension(900, 245));
		graphH.setPreferredSize(new Dimension(900, 245));
		graphS.setPreferredSize(new Dimension(900, 245));
		graphI.addMouseMotionListener(new IdleListener());
		graphH.addMouseMotionListener(new IdleListener());
		graphS.addMouseMotionListener(new IdleListener());
		graphLayout.setConstraints(graphI, cGraph);
		container.add(graphI);
		graphLayout.setConstraints(graphH, cGraph);
		container.add(graphH);
		graphLayout.setConstraints(graphS, cGraph);
		container.add(graphS);
		JScrollPane pane = new JScrollPane(container,
				JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
				JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);

		pane.setBorder(BorderFactory
				.createTitledBorder("Combined IHS Graphs"));
		pane.setBackground(Color.WHITE);
		graphPanels = new JPanel[]{graphI, graphH, graphS};
		return pane;

	}

	private void initLayout() {
		GridBagLayout gridbag = new GridBagLayout();
		GridBagConstraints c = new GridBagConstraints();
		setLayout(gridbag);
		setBackground(Color.WHITE);

		c.fill = GridBagConstraints.BOTH;
		c.weightx = 8.0;
		c.weighty = 1.0;
		c.gridheight = 3;

		JScrollPane pane = createGraphPane();
		pane.addMouseMotionListener(new IdleListener());
		gridbag.setConstraints(pane, c);
		getContentPane().add(pane);

		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1.0;
		c.weighty = 0.0;
		c.gridheight = 1;
		c.gridwidth = GridBagConstraints.REMAINDER;
		this.cbGraphSelect = new JList(KallaBalla.instance().getAlgorithms());
		cbGraphSelect.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
		cbGraphSelect.setBorder(BorderFactory.createTitledBorder("Equalizer Passes"));
		cbGraphSelect.setBackground(Color.WHITE);
		cbGraphSelect.addMouseMotionListener(new IdleListener());
		cbGraphSelect.addListSelectionListener(new ListSelectionListener(){
			public void valueChanged(ListSelectionEvent event)
			{
						Algorithm alg = (Algorithm) cbGraphSelect
								.getSelectedValue();
						txtStats.setText(alg.getDescription());
						drawIHSGraph(alg, true);
			}
		});
		gridbag.setConstraints(cbGraphSelect, c);
		getContentPane().add(cbGraphSelect);

//		c.fill = GridBagConstraints.HORIZONTAL;
//		c.weightx = 1.0;
//		c.weighty = 0.0;
//		c.gridheight = 1;
//		c.gridwidth = GridBagConstraints.REMAINDER;
//		btnRender.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent e) {
//				new Thread() {
//					public void run() {
//						Algorithm alg = (Algorithm) cbGraphSelect
//								.getSelectedValue();
//						txtStats.setText(alg.getDescription());
//						drawIHSGraph(alg, true);
//					}
//				}.start();
//
//			}
//		});
//		gridbag.setConstraints(btnRender, c);
//		getContentPane().add(btnRender);

		c.fill = GridBagConstraints.BOTH;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.gridheight = 1;
		c.gridwidth = GridBagConstraints.REMAINDER;
		this.txtStats = new JTextArea();
		txtStats.addMouseMotionListener(new IdleListener());
//		GridBagLayout gbCont = new GridBagLayout();
		JPanel txtContainer = new JPanel(null);
		txtContainer.addMouseMotionListener(new IdleListener());
//		txtContainer.setSize(new Dimension(315,658));
//		txtContainer.setPreferredSize(new Dimension(315,658));
//		txtContainer.setMaximumSize(new Dimension(315,658));
//		txtContainer.setBackground(Color.RED);
//		gbCont.setConstraints(txtStats, c);
//		c.fill = GridBagConstraints.VERTICAL;
		txtStats.setSize(new Dimension(215,658));
		txtStats.setPreferredSize(new Dimension(215,658));
		txtStats.setWrapStyleWord(true);
		txtStats.setLocation(0, 0);
		//txtStats.setMaximumSize(new Dimension(315,658));
//		txtStats.setMinimumSize(new Dimension(315,658));
		txtStats.setEditable(false);
		txtStats.setAutoscrolls(true);
		//txtStats.setColumns(40);
		txtStats.setBorder(BorderFactory.createTitledBorder("Description"));
		txtStats.setLineWrap(true);
		//txtStats.setMaximumSize(new Dimension(315,658));
		txtContainer.add(txtStats, BorderLayout.CENTER);
		gridbag.setConstraints(txtContainer, c);
		getContentPane().add(txtContainer);

		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 1.0;
		c.weighty = 0.0;
		c.gridheight = 1;
		c.gridwidth = GridBagConstraints.REMAINDER;
		btnAnimate.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				new Thread()
				{
					public void run()
					{viewer.animateResults(kallaBalla.getAlgorithms(),false);}
				}.start();
				
			}
		});
		btnAnimate.addMouseMotionListener(new IdleListener());
		gridbag.setConstraints(btnAnimate, c);
		getContentPane().add(btnAnimate);

//		c.fill = GridBagConstraints.HORIZONTAL;
//		c.weightx = 1.0;
//		c.weighty = 0.0;
//		c.gridheight = 1;
//		c.gridwidth = GridBagConstraints.REMAINDER;
//		btnShow.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent e) {
//				viewer.setVisible(true);
//			}
//		});
//		gridbag.setConstraints(btnShow, c);
//		getContentPane().add(btnShow);
//
//		c.fill = GridBagConstraints.HORIZONTAL;
//		c.weightx = 1.0;
//		c.weighty = 0.0;
//		c.gridheight = 1;
//		c.gridwidth = GridBagConstraints.REMAINDER;
//		btnSave.addActionListener(new ActionListener() {
//			public void actionPerformed(ActionEvent e) {
//				Graph.this.kallaBalla.saveResult("result.png", result);
//			}
//		});
//		gridbag.setConstraints(btnSave, c);
//		getContentPane().add(btnSave);

	}

	public static XYSeries createXYSeries(String id,
			int[] graphData) {
		XYSeries series = new XYSeries(id, false, false);
		for (int i = 0; i < graphData.length; i++) {
				series.add(i, graphData[i]);
		}
		return series;
	}

	public JPanel createGraphPanel(String title, String category, String value,
			DefaultTableXYDataset dataset) {
		JFreeChart jfc = ChartFactory.createXYLineChart(title, category, value,
				dataset, PlotOrientation.VERTICAL, true, true, false);

		XYPlot pp = (XYPlot) jfc.getPlot();

		pp.setNoDataMessage("No Data Available");
		return new ChartPanel(jfc);
	}

	public void drawIHSGraph(Algorithm algorithm, boolean query) {
		// query = false;
		// if (query) {
		datasetI.removeAllSeries();
		datasetH.removeAllSeries();
		datasetS.removeAllSeries();
		// }
		int[][] gLookup = algorithm.getCachedLookupGraph();
		int[][] gRevLookup = algorithm.getCachedRevLookupGraph();
		int[][] gResult = algorithm.getCachedResultGraph();

		if (query || glookupI == null || glookupH == null || glookupS == null) {
			if (gLookup != null) {
				glookupI = gLookup[0];
				glookupH = gLookup[1];
				glookupS = gLookup[2];
			} else {
				glookupI = lookupTable.createIHSGraph(lookupTable
						.getReferencePixels(), IHSLookup.GROUP_I);
				glookupH = lookupTable.createIHSGraph(lookupTable
						.getReferencePixels(), IHSLookup.GROUP_H);
				glookupS = lookupTable.createIHSGraph(lookupTable
						.getReferencePixels(), IHSLookup.GROUP_S);
				algorithm.cacheLookupGraph(glookupI, glookupH, glookupS);
			}

		}

		if (query || gRevLookupI == null || gRevLookupH == null
				|| gRevLookupS == null) {
			if (gRevLookup != null) {

				gRevLookupI = gRevLookup[0];
				gRevLookupH = gRevLookup[1];
				gRevLookupS = gRevLookup[2];
			} else {
				gRevLookupI = lookupTable.createIHSGraph(lookupTable
						.getTargetPixels(), IHSLookup.GROUP_I);
				gRevLookupH = lookupTable.createIHSGraph(lookupTable
						.getTargetPixels(), IHSLookup.GROUP_H);
				gRevLookupS = lookupTable.createIHSGraph(lookupTable
						.getTargetPixels(), IHSLookup.GROUP_S);
				algorithm.cacheRevLookupGraph(gRevLookupI, gRevLookupH,
						gRevLookupS);

			}
		}

		if ((query || gResultI == null || gResultH == null || gResultS == null)) {
			if (gResult != null) {
				gResultI = gResult[0];
				gResultH = gResult[1];
				gResultS = gResult[2];
				result = algorithm.getCachedResult();
			} else {
				result = algorithm.work(progressDialog);

				if (result != null) {
					gResultI = lookupTable.createIHSGraph(result,
							IHSLookup.GROUP_I);
					gResultH = lookupTable.createIHSGraph(result,
							IHSLookup.GROUP_H);
					gResultS = lookupTable.createIHSGraph(result,
							IHSLookup.GROUP_S);
					algorithm.cacheResultGraph(gResultI, gResultH, gResultS);
				}
			}

			if (result == null) {
				gResultI = null;
				gResultH = null;
				gResultS = null;
			}
		}

		drawDataDistribution("Sample Image Intensity", glookupI, datasetI, Color.GREEN);
		drawDataDistribution("Sample Image Hue", glookupH, datasetH, Color.GREEN);
		drawDataDistribution("Sample Image Saturation", glookupS, datasetS, Color.GREEN);
		drawDataDistribution("Target Image Intensity", gRevLookupI, datasetI, Color.RED);
		drawDataDistribution("Target Image Hue", gRevLookupH, datasetH, Color.RED);
		drawDataDistribution("Target Image Saturation", gRevLookupS, datasetS, Color.RED);
		if (gResultI != null && gResultH != null && gResultS != null) {
			drawDataDistribution("Result Image Intensity", gResultI, datasetI, Color.BLUE);
			drawDataDistribution("Result Image Hue", gResultH, datasetH, Color.BLUE);
			drawDataDistribution("Result Image Saturation", gResultS, datasetS, Color.BLUE);
		}

		if (result != null) {
			viewer.setResultImage(kallaBalla.createBufferedImage(result));
			algorithm.cacheResult(result);
		} else {
			viewer.setResultImage(null);
			algorithm.cacheResult(null);
		}
		this.validate();
	}

	public void drawDataDistribution(String id, int[] data,
			DefaultTableXYDataset dataset, Color fgcolor) {
		dataset.addSeries(createXYSeries(id, data));
	}
}

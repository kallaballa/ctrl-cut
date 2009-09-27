import java.awt.Font;

import javax.swing.JFrame;
import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.plot.XYPlot;
import org.jfree.data.general.*;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.xy.DefaultTableXYDataset;
import org.jfree.data.xy.XYSeries;

public class JfcExample extends JFrame
{
	private static final long serialVersionUID = 1L;
	
	private DefaultTableXYDataset dataset;
	private JFreeChart jfc;

	public JfcExample()
	{
		dataset = new DefaultTableXYDataset();
	}
	
	public void addSeries(XYSeries series)
	{
		dataset.addSeries(series);
	}
	
	public void setChar(String title)
	{
		jfc = ChartFactory.createTimeSeriesChart("something", "cat", "val", dataset,false, false,false);
		
		XYPlot pp = (XYPlot) jfc.getPlot();
//		pp.setSectionOutlinesVisible(false);
//		pp.setLabelFont(new Font("SansSerif", Font.PLAIN, 12));
		pp.setNoDataMessage("Nessun Dato Inserito");
//		pp.setCircular(false);
//		pp.setLabelGap(0.02);
	}
	
	private JPanel createPanel()
	{
		return new ChartPanel(jfc);
	}
	
	public void Show()
	{
		setContentPane(createPanel());
		setVisible(true);
	}
	
	public static void main(String[] args)
	{
		JfcExample j = new JfcExample();
		j.setTitle("Example Chart...");
		j.setSize(640, 430);
		XYSeries series = new XYSeries("a",false,false);
		series.add(1, 1);
		series.add(10, 10);
		series.add(2, 4);
		series.add(20, 40);
//		XYSeries series1 = new XYSeries("b",false,false);
//		series1.add(1, 1);
//		series1.add(20, 40);
//		series1.add(2, 4);
//		series1.add(10, 10);


		j.addSeries(series);
//		j.addSeries(series1);
//		j.setValue("UNO", new Double(20.0));
//		j.setValue("DUE", new Double(10.0));
//		j.setValue("TRE", new Double(20.0));
//		j.setValue("QUATTRO", new Double(30.0));
//		j.setValue("CINQUE", new Double(20.0));
		
		j.setChar("Example Chart...");
		
		j.Show();
	}
}

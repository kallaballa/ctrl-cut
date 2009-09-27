package org.wooden.kallaballa;

import java.awt.GraphicsEnvironment;
import java.awt.image.BufferedImage;
import java.awt.image.RenderedImage;

import java.awt.image.WritableRaster;

import java.io.*;

import javax.media.jai.JAI;

import javax.media.jai.RenderedOp;
import javax.media.jai.TiledImage;
import javax.media.jai.operator.ColorConvertDescriptor;
import javax.media.jai.operator.FileStoreDescriptor;

import org.wooden.kallaballa.algorithms.Algorithm;
import org.wooden.kallaballa.gui.Graph;
import org.wooden.kallaballa.gui.IdleTimer;
import org.wooden.kallaballa.gui.ProgressDialog;
import org.wooden.kallaballa.ihs.IHSLookup;
import org.wooden.kallaballa.ihs.IHSPixel;
import org.wooden.kallaballa.util.IHSTool;
import org.wooden.kallaballa.util.ImageTool;
import org.wooden.kallaballa.util.PixelAnalyzer;

public class KallaBalla {
	private static KallaBalla instance;
	private IHSPixel[] codedRefImg;
	private IHSPixel[] codedTargetImg;
	private IHSPixel[] codedResultImg;
	private IHSLookup lookupTable;

	private RenderedImage refImg;
	private RenderedImage targetImg;
	private Algorithm[] algorithms;
	
	
	public KallaBalla(String fsample, String fdiff) throws IOException {
		instance = this;
		refImg = JAI.create("fileload", fsample);
		targetImg = JAI.create("fileload", fdiff);

		codedRefImg = IHSTool.createIHSPixels(refImg);
		codedTargetImg = IHSTool.createIHSPixels(targetImg);
		codedResultImg = new IHSPixel[codedTargetImg.length];

		lookupTable = new IHSLookup(codedRefImg, codedTargetImg);
		algorithms = createAlgorithms();
		new IdleTimer().start();
		new Graph(this, GraphicsEnvironment.getLocalGraphicsEnvironment()
				.getDefaultScreenDevice());
	}

	public static KallaBalla instance() {
		return instance;
	}

	public IHSLookup getIHSLookup() {
		return lookupTable;
	}

	public Algorithm[] getAlgorithms() {
		return algorithms;
	}

	private Algorithm[] createAlgorithms() {
		String[] descriptions = new String[] {
				"The sample image, demonstrating the desired color space to represent.",
				"The target image taken from the next zoom level (therefor with higher resolution), which will be converted to match the color space of the sample image, placed over the matching area of the sample image. No further processing done yet.",
				"Searches the distance between extremes (the distance between the lowest point of the curve to the left and right) of the intensity and saturation curves. Subsequently warps the target image's intensity and saturation curves to match the width of the corresponding sample image's curve extremes. Now both images share roughly the same relative range of intensity and saturation values.",
				"The warped intensity and saturation curves are shifted to overlap the curves of the sample image, as exact as possible. Now both images share almost exactly the same range of intensity and saturation values.",
				"Finally, a lookup table, mapping the distinct hue values of the sample image's pixels, is mapped to the corresponding hue values of the target image's pixels. Then it iterates over every target pixel's hue value and tries to reverse lookup a hue of the sample image. If the reverse lookup cant find an exact match, the closest match is seeked and adjusted to reflect the difference between the target image's pixel and the closest match on the sample image's pixel"};
		Algorithm none = new Algorithm("None", descriptions[0]) {
			private int[][] gRevLookup = new int[3][0];
			private int[][] gResult = new int[3][0];
			
			public IHSPixel[] work(ProgressDialog progress) {
				return null;
			}

			public int[][] getCachedRevLookupGraph()
			{
				return gRevLookup;
			}

			public int[][] getCachedResultGraph()
			{
				return gResult;
			}
		};

		Algorithm overlay = new Algorithm("Overlay", descriptions[1]) {
			public IHSPixel[] work(ProgressDialog progress) {
				return codedTargetImg;
			}
		};

		Algorithm warpAndNormalize = new Algorithm("Overlay & Warp", descriptions[2]) {
			public IHSPixel[] work(ProgressDialog progress) {
				return normalize(warp(progress));
			}
		};

		Algorithm warpAndNormalizeAndShift = new Algorithm("Overlay & Warp & Shift",descriptions[3]) {
			public IHSPixel[] work(ProgressDialog progress) {
				return normalize(warpAndShift(progress));
			}
		};

		Algorithm warpAndNormalizeAndShiftAndLookup = new Algorithm(
				"Overlay & Warp & Shift & Lookup", descriptions[4]) {
			public IHSPixel[] work(ProgressDialog progress) {
				return normalize(warpAndShiftAndLookup(progress));
			}
		};
		return new Algorithm[] { none, overlay, warpAndNormalize,
				warpAndNormalizeAndShift, warpAndNormalizeAndShiftAndLookup };
	}

	public IHSPixel[] normalize(IHSPixel[] ihspixels) {
		WritableRaster writeRaster = targetImg.getData()
				.createCompatibleWritableRaster();
		int width = writeRaster.getWidth();
		int height = writeRaster.getHeight();
		int nbands = writeRaster.getNumBands();

		int[] pixels = IHSTool.decodePixels(ihspixels, width, height, nbands);

		writeRaster.setPixels(0, 0, width, height, pixels);

		TiledImage tile = new TiledImage(ImageTool.rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		RenderedOp result = ColorConvertDescriptor.create(tile, targetImg
				.getColorModel(), null);
		return IHSTool.createIHSPixels(result);
	}

	public void saveResult(String filename, IHSPixel[] codedResultImg) {
		WritableRaster writeRaster = targetImg.getData()
				.createCompatibleWritableRaster();
		int width = writeRaster.getWidth();
		int height = writeRaster.getHeight();
		int nbands = writeRaster.getNumBands();

		int[] pixels = IHSTool.decodePixels(codedResultImg, width, height, nbands);

		writeRaster.setPixels(0, 0, width, height, pixels);

		TiledImage tile = new TiledImage(ImageTool.rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		RenderedOp result = ColorConvertDescriptor.create(tile, targetImg
				.getColorModel(), null);

		FileStoreDescriptor.create(result, filename, "png", null, null, null);
	}

	public BufferedImage createBufferedImage(IHSPixel[] codedResultImg) {
		WritableRaster writeRaster = targetImg.getData()
				.createCompatibleWritableRaster();
		int width = writeRaster.getWidth();
		int height = writeRaster.getHeight();
		int nbands = writeRaster.getNumBands();

		int[] pixels = IHSTool.decodePixels(codedResultImg, width, height, nbands);

		writeRaster.setPixels(0, 0, width, height, pixels);

		TiledImage tile = new TiledImage(ImageTool.rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		return ColorConvertDescriptor.create(tile, targetImg.getColorModel(),
				null).getAsBufferedImage();
	}




	public IHSPixel[] warp(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = PixelAnalyzer.findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = PixelAnalyzer.findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianTargetI = PixelAnalyzer.calcMedian(boundsTargetI);

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = PixelAnalyzer.findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = PixelAnalyzer.findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianTargetS = PixelAnalyzer.calcMedian(boundsTargetS);

		if (progressDialog != null)
			progressDialog.start();

		double strechI = (double) widthRefI / (double) widthTargetI;
		double strechS = (double) widthRefS / (double) widthTargetS;

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			int[] interpolated = new int[3];

			interpolated[0] = medianTargetI
					+ (int) Math
							.round((codedTargetImg[i].getI() - medianTargetI)
									* strechI);
			interpolated[1] = codedTargetImg[i].getH();
			interpolated[2] = medianTargetS
					+ (int) Math
							.round((codedTargetImg[i].getS() - medianTargetS)
									* strechS);

			codedResultImg[i] = new IHSPixel(interpolated);
		}

		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] warpAndShift(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = PixelAnalyzer.findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = PixelAnalyzer.findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = PixelAnalyzer.calcMedian(boundsRefI);
		int medianTargetI = PixelAnalyzer.calcMedian(boundsTargetI);
		int offsetI = medianRefI - medianTargetI;

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = PixelAnalyzer.findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = PixelAnalyzer.findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = PixelAnalyzer.calcMedian(boundsRefS);
		int medianTargetS = PixelAnalyzer.calcMedian(boundsTargetS);
		int offsetS = medianRefS - medianTargetS;

		if (progressDialog != null)
			progressDialog.start();

		double strechI = (double) widthRefI / (double) widthTargetI;
		double strechS = (double) widthRefS / (double) widthTargetS;

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			int[] interpolated = new int[3];

			interpolated[0] = offsetI
					+ medianTargetI
					+ (int) Math
							.round((codedTargetImg[i].getI() - medianTargetI)
									* strechI);
			interpolated[1] = codedTargetImg[i].getH();
			interpolated[2] = 15
					+ offsetS
					+ medianTargetS
					+ (int) Math
							.round((codedTargetImg[i].getS() - medianTargetS)
									* strechS);

			codedResultImg[i] = new IHSPixel(interpolated);
		}

		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] warpAndShiftAndLookup(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = PixelAnalyzer.findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = PixelAnalyzer.findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = PixelAnalyzer.calcMedian(boundsRefI);
		int medianTargetI = PixelAnalyzer.calcMedian(boundsTargetI);
		int offsetI = medianRefI - medianTargetI;

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = PixelAnalyzer.findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = PixelAnalyzer.findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = PixelAnalyzer.calcMedian(boundsRefS);
		int medianTargetS = PixelAnalyzer.calcMedian(boundsTargetS);
		int offsetS = medianRefS - medianTargetS;

		if (progressDialog != null)
			progressDialog.start();

		double strechI = (double) widthRefI / (double) widthTargetI;
		double strechS = (double) widthRefS / (double) widthTargetS;

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			int[] interpolated = new int[3];

			IHSPixel match = lookupTable.findClosestMatch(codedTargetImg[i],
					true, IHSLookup.GROUP_H);
			IHSPixel revMatch = lookupTable.reverseLookup(match);
			int diffH = codedTargetImg[i].getH() - match.getH();

			interpolated[0] = offsetI
					+ medianTargetI
					+ (int) Math
							.round((codedTargetImg[i].getI() - medianTargetI)
									* strechI);
			interpolated[1] = revMatch.getH() - diffH;
			interpolated[2] = 15
					+ offsetS
					+ medianTargetS
					+ (int) Math
							.round((codedTargetImg[i].getS() - medianTargetS)
									* strechS);

			// if((Math.abs(interpolated[0] - codedRefImg[i].getI()) +
			// Math.abs(interpolated[1] - codedRefImg[i].getH()) +
			// Math.abs(interpolated[2] - codedRefImg[i].getS())) > 192)
			// {
			// interpolated[0] = 128;
			// interpolated[1] = 128;
			// interpolated[2] = 128;
			// }
			codedResultImg[i] = new IHSPixel(interpolated);
		}

		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] distortOld(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = PixelAnalyzer.findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = PixelAnalyzer.findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = PixelAnalyzer.calcMedian(boundsRefI);
		int medianTargetI = PixelAnalyzer.calcMedian(boundsTargetI);
		int offsetI = medianRefI - medianTargetI;

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = PixelAnalyzer.findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = PixelAnalyzer.findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = PixelAnalyzer.calcMedian(boundsRefS);
		int medianTargetS = PixelAnalyzer.calcMedian(boundsTargetS);
		int offsetS = medianRefS - medianTargetS;

		if (progressDialog != null)
			progressDialog.start();

		double strechI = (double) widthTargetI / (double) widthRefI;
		double strechS = (double) widthTargetS / (double) widthRefS;

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			int[] interpolated = new int[3];

			if (codedTargetImg[i].getI() > medianTargetI)
				strechI = (double) widthRefI / (double) widthTargetI;

			if (codedTargetImg[i].getS() > medianTargetS)
				strechS = (double) widthRefS / (double) widthTargetS;

			interpolated[0] = (int) Math.round((codedTargetImg[i].getI())
					* strechI);
			interpolated[1] = codedRefImg[i].getH();
			interpolated[2] = (int) Math.round((codedTargetImg[i].getS())
					* strechS);
			// interpolated[0]-=offsetI;
			interpolated[2] += offsetS;

			if (interpolated[0] > 255)
				interpolated[0] = 255;
			else if (interpolated[0] < 0)
				interpolated[0] = 0;

			if (interpolated[2] > 255)
				interpolated[2] = 255;
			else if (interpolated[2] < 0)
				interpolated[2] = 0;
			codedResultImg[i] = new IHSPixel(interpolated);
		}

		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}


	public IHSPixel[] averageoff(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = PixelAnalyzer.findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = PixelAnalyzer.findBounds(targetGraphI, codedTargetImg.length);
		int offsetI = PixelAnalyzer.calcMedian(boundsRefI) - PixelAnalyzer.calcMedian(boundsTargetI);

		int[] refGraphH = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_H);
		int[] targetGraphH = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_H);
		int[] boundsRefH = PixelAnalyzer.findBounds(refGraphH, codedRefImg.length);
		int[] boundsTargetH = PixelAnalyzer.findBounds(targetGraphH, codedTargetImg.length);
		int offsetH = PixelAnalyzer.calcMedian(boundsRefH) - PixelAnalyzer.calcMedian(boundsTargetH);

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = PixelAnalyzer.findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = PixelAnalyzer.findBounds(targetGraphS, codedTargetImg.length);
		int offsetS = PixelAnalyzer.calcMedian(boundsRefS) - PixelAnalyzer.calcMedian(boundsTargetS);

		if (progressDialog != null)
			progressDialog.start();

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			int[] interpolated = new int[3];

			interpolated[0] = (codedTargetImg[i].getI() + codedRefImg[i].getI())
					/ 2 - offsetI;
			interpolated[1] = (codedTargetImg[i].getH() + codedRefImg[i].getH()) / 2;
			interpolated[2] = (codedTargetImg[i].getS() + codedRefImg[i].getS())
					/ 2 - offsetS;
			codedResultImg[i] = new IHSPixel(interpolated);
		}
		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] average(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;
		if (progressDialog != null)
			progressDialog.start();
		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			int[] interpolated = new int[3];

			interpolated[0] = (codedTargetImg[i].getI() + codedRefImg[i].getI()) / 2;
			interpolated[1] = (codedTargetImg[i].getH() + codedRefImg[i].getH()) / 2;
			interpolated[2] = (codedTargetImg[i].getS() + codedRefImg[i].getS()) / 2;
			codedResultImg[i] = new IHSPixel(interpolated);
		}
		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public static void main(String[] args) {
		try {
			new KallaBalla(args[0], args[1]);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}

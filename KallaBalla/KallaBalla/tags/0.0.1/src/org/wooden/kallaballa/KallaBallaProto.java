package org.wooden.kallaballa;

import java.awt.GraphicsEnvironment;
import java.awt.Transparency;

import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.RenderedImage;

import java.awt.image.WritableRaster;
import java.awt.image.renderable.ParameterBlock;

import java.io.*;

import javax.media.jai.IHSColorSpace;
import javax.media.jai.JAI;

import javax.media.jai.InterpolationBicubic;
import javax.media.jai.ParameterBlockJAI;
import javax.media.jai.RasterFactory;
import javax.media.jai.RenderedOp;
import javax.media.jai.TiledImage;
import javax.media.jai.operator.ColorConvertDescriptor;
import javax.media.jai.operator.FileStoreDescriptor;

import org.wooden.kallaballa.gui.Graph;
import org.wooden.kallaballa.gui.IdleTimer;
import org.wooden.kallaballa.gui.ProgressDialog;
import org.wooden.kallaballa.ihs.IHSLookup;
import org.wooden.kallaballa.ihs.IHSPixel;

public class KallaBallaProto {
	private static KallaBallaProto instance;
	private IHSPixel[] codedRefImg;
	private IHSPixel[] codedTargetImg;
	private IHSPixel[] codedResultImg;
	private IHSLookup lookupTable;

	private RenderedImage refImg;
	private RenderedImage targetImg;
	private Algorithm[] algorithms;
	
	
	public KallaBallaProto(String fsample, String fdiff) throws IOException {
		instance = this;
		refImg = JAI.create("fileload", fsample);
		targetImg = JAI.create("fileload", fdiff);

		codedRefImg = createIHSPixels(refImg);
		codedTargetImg = createIHSPixels(targetImg);
		codedResultImg = new IHSPixel[codedTargetImg.length];

		lookupTable = new IHSLookup(codedRefImg, codedTargetImg);
		algorithms = createAlgorithms();
		new IdleTimer().start();
		new Graph(this, GraphicsEnvironment.getLocalGraphicsEnvironment()
				.getDefaultScreenDevice());
	}

	public static KallaBallaProto instance() {
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

		int[] pixels = decodePixels(ihspixels, width, height, nbands);

		writeRaster.setPixels(0, 0, width, height, pixels);

		TiledImage tile = new TiledImage(rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		RenderedOp result = ColorConvertDescriptor.create(tile, targetImg
				.getColorModel(), null);
		return createIHSPixels(result);
	}

	public void saveResult(String filename, IHSPixel[] codedResultImg) {
		WritableRaster writeRaster = targetImg.getData()
				.createCompatibleWritableRaster();
		int width = writeRaster.getWidth();
		int height = writeRaster.getHeight();
		int nbands = writeRaster.getNumBands();

		int[] pixels = decodePixels(codedResultImg, width, height, nbands);

		writeRaster.setPixels(0, 0, width, height, pixels);

		TiledImage tile = new TiledImage(rgb2ihs(targetImg), width, height);
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

		int[] pixels = decodePixels(codedResultImg, width, height, nbands);

		writeRaster.setPixels(0, 0, width, height, pixels);

		TiledImage tile = new TiledImage(rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		return ColorConvertDescriptor.create(tile, targetImg.getColorModel(),
				null).getAsBufferedImage();
	}

	public IHSPixel[] simpleDiffConversion(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;
		if (progressDialog != null)
			progressDialog.start();
		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));
			// IHSPixel value = lookupTable.reverseLookup(codedTargetImg[i]);

			// if(false && value != null)
			// codedResultImg[i] = value;
			// else
			// {
			IHSPixel match = lookupTable.findClosestMatch(codedTargetImg[i],
					true, IHSLookup.GROUP_H);
			IHSPixel revMatch = lookupTable.reverseLookup(match);

			int[] interpolated = new int[3];

			int diffI = codedTargetImg[i].getI() - codedRefImg[i].getI();
			int diffH = codedTargetImg[i].getH() - codedRefImg[i].getH();
			int diffS = codedTargetImg[i].getS() - codedRefImg[i].getS();

			interpolated[0] = revMatch.getI() - diffI;
			interpolated[1] = revMatch.getH() - diffH;
			interpolated[2] = revMatch.getS() - diffS;
			// System.out.println(interpolated[0] + " " +codedRefImg[i].getI() +
			// " " + (int)Math.round((((double)codedRefImg[i].getI() /
			// (double)100) * (2.5 * (double)diffI))) + " " + diffS);
			if (interpolated[0] > 255)
				interpolated[0] = 255;
			else if (interpolated[0] < 0)
				interpolated[0] = 0;

			if (interpolated[1] > 255)
				interpolated[1] = interpolated[1] - 255;
			else if (interpolated[1] < 0)
				interpolated[1] = 255 - interpolated[1];

			if (interpolated[2] > 255)
				interpolated[2] = 255;
			else if (interpolated[2] < 0)
				interpolated[2] = 0;

			// if(Math.abs(interpolated[0] - codedRefImg[i].getH()) > 32 ||
			// Math.abs(interpolated[2] - codedRefImg[i].getS()) > 32)
			// {
			// interpolated[0] = 128;
			// interpolated[1] = 128;
			// interpolated[2] = 128;
			// interpolated[0] = (codedTargetImg[i].getI() +
			// codedRefImg[i].getI()) / 2; //+ diffI;
			// interpolated[1] = codedTargetImg[i].getH(); //+ diffH;
			// interpolated[2] = (codedTargetImg[i].getS() +
			// codedRefImg[i].getS()) / 2; //+ diffS;
			// }
			//				
			// if(interpolated[0] > 255 || interpolated[0] < 0 ||
			// interpolated[1] > 255 || interpolated[1] < 0 || interpolated[2] >
			// 255 || interpolated[2] < 0)
			// {
			// interpolated[0] = 128;
			// interpolated[1] = 128;
			// interpolated[2] = 128;
			// }

			codedResultImg[i] = new IHSPixel(interpolated);
			// }
		}
		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	private int[] findBounds(int[] graph, int total) {
		int cumm = 0;
		int lowbound = 0;
		int highbound = 0;
		int threshold = (total / 100) * 5;
		for (int i = 0; i < graph.length; i++) {
			cumm += graph[i];
			if (cumm > threshold) {
				lowbound = i;
				break;
			}
		}
		cumm = 0;
		for (int i = graph.length - 1; i >= 0; i--) {
			cumm += graph[i];
			if (cumm > threshold) {
				highbound = i;
				break;
			}

		}

		return new int[] { lowbound, highbound };
	}

	public int calcMedian(int[] refBounds) {
		return Math.abs(refBounds[0] - refBounds[1]) / 2;
	}

	public IHSPixel[] averageoffAndLookup(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int offsetI = calcMedian(boundsRefI) - calcMedian(boundsTargetI);

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int offsetS = calcMedian(boundsRefS) - calcMedian(boundsTargetS);

		if (progressDialog != null)
			progressDialog.start();

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			IHSPixel match = lookupTable.findClosestMatch(codedTargetImg[i],
					true, IHSLookup.GROUP_H);
			IHSPixel revMatch = lookupTable.reverseLookup(match);

			int[] interpolated = new int[3];

			int diffH = codedTargetImg[i].getH() - match.getH();

			interpolated[0] = (codedTargetImg[i].getI() + codedRefImg[i].getI())
					/ 2 - offsetI;
			interpolated[1] = revMatch.getH() - diffH;
			interpolated[2] = (codedTargetImg[i].getS() + codedRefImg[i].getS())
					/ 2 - offsetS;
			codedResultImg[i] = new IHSPixel(interpolated);
		}
		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] negSigmoidOffAndLookup(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int offsetI = 0;// findMedian(refGraphI, codedRefImg.length) -
		// findMedian(targetGraphI, codedTargetImg.length);

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int offsetS = 0;// findMedian(refGraphS, codedRefImg.length) -
		// findMedian(targetGraphS, codedTargetImg.length);

		if (progressDialog != null)
			progressDialog.start();

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			IHSPixel match = lookupTable.findClosestMatch(codedTargetImg[i],
					true, IHSLookup.GROUP_H);
			IHSPixel revMatch = lookupTable.reverseLookup(match);

			int[] interpolated = new int[3];

			int diffH = codedTargetImg[i].getH() - match.getH();

			double avI = ((codedTargetImg[i].getI() + codedRefImg[i].getI()) / 2);
			double avS = ((codedTargetImg[i].getS() + codedRefImg[i].getS()) / 2);
			int maxI = Math
					.max(codedRefImg[i].getI(), codedTargetImg[i].getI());
			int maxS = Math
					.max(codedRefImg[i].getS(), codedTargetImg[i].getS());
			double diffI = refGraphI[codedRefImg[i].getI()]
					- targetGraphI[codedTargetImg[i].getI()];
			double diffS = refGraphS[codedRefImg[i].getS()]
					- targetGraphS[codedTargetImg[i].getS()];
			int preferFactorI = codedRefImg[i].getI() < codedTargetImg[i]
					.getI() ? -1 : 1;
			int preferFactorS = codedRefImg[i].getS() < codedTargetImg[i]
					.getS() ? -1 : 1;

			// if(refGraphI[codedRefImg[i].getI()] >
			// targetGraphI[codedTargetImg[i].getI()])
			// diffI = -diffI;
			// if(refGraphS[codedRefImg[i].getS()] >
			// targetGraphS[codedTargetImg[i].getS()])
			// diffS = -diffS;

			double factorI;
			if (diffI == 0)
				factorI = 0;
			else
				factorI = -1 / (1 / diffI);
			double factorS;
			if (diffS == 0)
				factorS = 0;
			else
				factorS = -1 / (1 / diffS);
			// if(diffI < 0)
			// factorI*=-1;
			double preferI = ((maxI - avI) * factorI); // * preferFactorI;
			double preferS = ((maxS - avS) * factorS); // * preferFactorS;

			interpolated[0] = (int) Math.round(avI - offsetI + preferI);
			interpolated[1] = codedRefImg[i].getH();
			interpolated[2] = (int) Math.round(avS - offsetS + preferS);
			codedResultImg[i] = new IHSPixel(interpolated);
		}
		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] sigmoidOffAndLookup(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int offsetI = calcMedian(boundsRefI) - calcMedian(boundsTargetI);

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int offsetS = calcMedian(boundsRefS) - calcMedian(boundsTargetS);

		if (progressDialog != null)
			progressDialog.start();

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));

			IHSPixel match = lookupTable.findClosestMatch(codedTargetImg[i],
					true, IHSLookup.GROUP_H);
			IHSPixel revMatch = lookupTable.reverseLookup(match);

			int[] interpolated = new int[3];

			int diffH = codedTargetImg[i].getH() - match.getH();

			double ifactor = -sigmoid(refGraphI[codedRefImg[i].getI()]
					- targetGraphI[codedTargetImg[i].getI()]);
			double sfactor = sigmoid(refGraphS[codedRefImg[i].getS()]
					- targetGraphS[codedTargetImg[i].getS()]);

			interpolated[0] = (int) Math
					.round((codedTargetImg[i].getI() + codedRefImg[i].getI() + (codedRefImg[i]
							.getI() * ifactor))
							/ 2 - offsetI);
			interpolated[1] = codedRefImg[i].getH();
			interpolated[2] = (int) Math
					.round((codedTargetImg[i].getS() + codedRefImg[i].getS() + (codedRefImg[i]
							.getS() * sfactor))
							/ 2 - offsetS);
			codedResultImg[i] = new IHSPixel(interpolated);
		}
		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] sigmoidAvOffAndLookup(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = calcMedian(boundsRefI);
		int medianTargetI = calcMedian(boundsTargetI);
		int offsetI = 0; // medianRefI - medianTargetI;

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = calcMedian(boundsRefS);
		int medianTargetS = calcMedian(boundsTargetS);
		int offsetS = 0; // medianRefS - medianTargetS;

		if (progressDialog != null)
			progressDialog.start();

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));
			//
			// IHSPixel match = lookupTable.findClosestMatch(codedTargetImg[i],
			// true, IHSLookup.GROUP_H);
			// IHSPixel revMatch = lookupTable.reverseLookup(match);

			int[] interpolated = new int[3];

			// int diffH = codedTargetImg[i].getH() - match.getH();
			double avI = ((codedTargetImg[i].getI() + codedRefImg[i].getI()) / 2);
			double avS = ((codedTargetImg[i].getS() + codedRefImg[i].getS()) / 2);
			int maxI = Math
					.max(codedRefImg[i].getI(), codedTargetImg[i].getI());
			int maxS = Math
					.max(codedRefImg[i].getS(), codedTargetImg[i].getS());
			int minI = Math
					.min(codedRefImg[i].getI(), codedTargetImg[i].getI());
			int minS = Math
					.min(codedRefImg[i].getS(), codedTargetImg[i].getS());

			double ifactor = -sigmoid(refGraphI[codedRefImg[i].getI()]
					- targetGraphI[codedTargetImg[i].getI()]);
			double sfactor = sigmoid(refGraphS[codedRefImg[i].getS()]
					- targetGraphS[codedTargetImg[i].getS()]);

			interpolated[0] = (int) Math.round((codedTargetImg[i].getI()
					+ codedRefImg[i].getI() + ((avI - maxI) * ifactor))
					/ 2 - offsetI);
			interpolated[1] = codedRefImg[i].getH();
			interpolated[2] = (int) Math.round((codedTargetImg[i].getS()
					+ codedRefImg[i].getS() + ((avS - maxS) * sfactor))
					/ 2 - offsetS);
			codedResultImg[i] = new IHSPixel(interpolated);
		}

		codedResultImg = normalize(codedResultImg);
		int[] resultGraphS = lookupTable.createIHSGraph(codedResultImg,
				IHSLookup.GROUP_S);
		int[] boundsResultS = findBounds(resultGraphS, codedResultImg.length);
		int widthResultS = Math.abs(boundsResultS[0] - boundsResultS[1]);
		int medianResultS = calcMedian(boundsResultS);

		int[] resultGraphI = lookupTable.createIHSGraph(codedResultImg,
				IHSLookup.GROUP_I);
		int[] boundsResultI = findBounds(resultGraphI, codedResultImg.length);
		int widthResultI = Math.abs(boundsResultI[0] - boundsResultI[1]);
		int medianResultI = calcMedian(boundsResultI);

		double strechI = (double) widthTargetI / (double) widthResultI;
		double strechS = (double) widthTargetS / (double) widthResultS;

		int[] streched = new int[3];
		for (int i = 0; i < codedResultImg.length; i++) {
			if (codedResultImg[i].getI() < medianResultI)
				strechI = (double) widthTargetI / (double) widthResultI;
			else
				strechI = (double) widthResultI / (double) widthTargetI;

			if (codedResultImg[i].getS() < medianResultS)
				strechS = (double) widthTargetS / (double) widthResultS;
			else
				strechS = (double) widthResultS / (double) widthTargetS;

			streched = new int[3];
			streched[0] = (int) Math
					.round((codedResultImg[i].getI() * strechI));
			streched[1] = codedTargetImg[i].getH();
			streched[2] = (int) Math
					.round((codedResultImg[i].getS() * strechS));
			codedResultImg[i] = new IHSPixel(streched);
		}

		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public IHSPixel[] warp(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianTargetI = calcMedian(boundsTargetI);

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianTargetS = calcMedian(boundsTargetS);

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
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = calcMedian(boundsRefI);
		int medianTargetI = calcMedian(boundsTargetI);
		int offsetI = medianRefI - medianTargetI;

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = calcMedian(boundsRefS);
		int medianTargetS = calcMedian(boundsTargetS);
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
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = calcMedian(boundsRefI);
		int medianTargetI = calcMedian(boundsTargetI);
		int offsetI = medianRefI - medianTargetI;

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = calcMedian(boundsRefS);
		int medianTargetS = calcMedian(boundsTargetS);
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
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = calcMedian(boundsRefI);
		int medianTargetI = calcMedian(boundsTargetI);
		int offsetI = medianRefI - medianTargetI;

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = calcMedian(boundsRefS);
		int medianTargetS = calcMedian(boundsTargetS);
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

	public static double sigmoid(double activation) {
		double e = 2.7183;
		double p = 1;
		double a = activation;
		return (1 / (1 + Math.pow(e, ((a * -1) / p))));
	}

	public IHSPixel[] averageoff(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;

		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] boundsRefI = findBounds(refGraphI, codedRefImg.length);
		int[] boundsTargetI = findBounds(targetGraphI, codedTargetImg.length);
		int offsetI = calcMedian(boundsRefI) - calcMedian(boundsTargetI);

		int[] refGraphH = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_H);
		int[] targetGraphH = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_H);
		int[] boundsRefH = findBounds(refGraphH, codedRefImg.length);
		int[] boundsTargetH = findBounds(targetGraphH, codedTargetImg.length);
		int offsetH = calcMedian(boundsRefH) - calcMedian(boundsTargetH);

		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);
		int[] boundsRefS = findBounds(refGraphS, codedRefImg.length);
		int[] boundsTargetS = findBounds(targetGraphS, codedTargetImg.length);
		int offsetS = calcMedian(boundsRefS) - calcMedian(boundsTargetS);

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

	public int findByFrequency(int ref, int[] refGraph, int refTotal,
			int[] targetGraph, int targetTotal) {
		int match = 0;
		double refFreq = refTotal / refGraph[ref];
		double targetFreq;
		double diff = Double.MAX_VALUE;
		double currDiff;

		for (int i = 0; i < targetGraph.length; i++) {
			if (targetGraph[i] == 0)
				continue;
			targetFreq = targetTotal / targetGraph[i];
			currDiff = Math.abs(refFreq - targetFreq);

			if (diff > currDiff) {
				diff = currDiff;
				match = i;
			}
		}

		return match;
	}

	public IHSPixel[] blafoo(ProgressDialog progressDialog) {
		double onePercent = codedTargetImg.length / 100;
		if (progressDialog != null)
			progressDialog.start();

		int[] refGraphH = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_H);
		int[] targetGraphH = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_H);
		int[] refGraphI = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_I);
		int[] targetGraphI = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_I);
		int[] refGraphS = lookupTable.createIHSGraph(codedRefImg,
				IHSLookup.GROUP_S);
		int[] targetGraphS = lookupTable.createIHSGraph(codedTargetImg,
				IHSLookup.GROUP_S);

		int[] interpolated = new int[3];

		for (int i = 0; i < codedTargetImg.length; i++) {
			if (progressDialog != null && (i % onePercent) == 0)
				progressDialog.setProgress((int) Math.round(i / onePercent));
			interpolated[0] = findByFrequency(codedRefImg[i].getI(), refGraphI,
					codedRefImg.length, targetGraphI, codedTargetImg.length);
			interpolated[1] = findByFrequency(codedRefImg[i].getH(), refGraphH,
					codedRefImg.length, targetGraphH, codedTargetImg.length);
			interpolated[2] = findByFrequency(codedRefImg[i].getS(), refGraphS,
					codedRefImg.length, targetGraphS, codedTargetImg.length);

			codedResultImg[i] = new IHSPixel(interpolated);
			// }
		}
		if (progressDialog != null)
			progressDialog.done();
		return codedResultImg;
	}

	public static RenderedOp scaleX(RenderedImage src, float width) {
		float scaleFactor;
		scaleFactor = width / (float) src.getWidth();
		return scale(src, scaleFactor);
	}

	public static RenderedOp scaleY(RenderedImage src, float height) {
		float scaleFactor;
		scaleFactor = height / (float) src.getHeight();
		return scale(src, scaleFactor);
	}

	public static RenderedOp scale(RenderedImage src, float amount) {
		ParameterBlockJAI pb = new ParameterBlockJAI("scale");
		pb.addSource(src);
		pb.setParameter("xScale", amount); // x Scale Factor
		pb.setParameter("yScale", amount); // y Scale Factor
		pb.setParameter("xTrans", 0.0F); // x Translate amount
		pb.setParameter("yTrans", 0.0F); // y Translate amount
		pb.setParameter("interpolation", new InterpolationBicubic(4));
		return JAI.create("scale", pb, null);
	}

	public static RenderedOp cropTranslate(RenderedOp src, int x, int y,
			int width, int height) {
		ParameterBlock pb = new ParameterBlock();
		pb.addSource(src);
		pb.add((float) x);
		pb.add((float) y);
		pb.add((float) width);
		pb.add((float) height);

		RenderedOp cropped = JAI.create("crop", pb);
		pb = new ParameterBlock();
		pb.addSource(cropped);
		pb.add((float) -x);
		pb.add((float) -y);

		return JAI.create("translate", pb, null);
	}

	public static IHSPixel[] createIHSPixels(RenderedImage src) {
		return createIHSPixels(src, 0, 0, src.getWidth(), src.getHeight());
	}

	public static IHSPixel[] createIHSPixels(RenderedImage src, int x, int y,
			int width, int height) {
		RenderedOp result = rgb2ihs(src);
		Raster raster = result.getData();
		int nbands = raster.getNumBands();
		int[] pixels = raster.getPixels(x, y, width, height, (int[]) null);

		return encodePixels(pixels, width, height, nbands);
	}

	public static RenderedOp rgb2ihs(RenderedImage src) {

		// Create an IHS color model to transform to
		IHSColorSpace ihsCS = IHSColorSpace.getInstance();
		ColorModel ihsCM = RasterFactory.createComponentColorModel(
				DataBuffer.TYPE_BYTE, ihsCS, false, false, Transparency.OPAQUE);

		RenderedOp result = ColorConvertDescriptor.create(src, ihsCM, null);
		return result;
	}

	public static int[] decodePixels(IHSPixel[] codedPixels, int width,
			int height, int nbands) {
		int[] pixels = new int[codedPixels.length * nbands];
		int pi = 0;
		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				int offset = h * width * nbands + w * nbands;
				pixels[offset + 2] = codedPixels[pi].getS();
				pixels[offset + 1] = codedPixels[pi].getH();
				pixels[offset] = codedPixels[pi].getI();
				pi++;
			}
		}
		return pixels;
	}

	public static IHSPixel[] encodePixels(int[] pixels, int width, int height,
			int nbands) {
		IHSPixel[] codedPixels = new IHSPixel[(int) (pixels.length / nbands)];
		int pi = 0;

		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				int offset = h * width * nbands + w * nbands;

				codedPixels[pi] = new IHSPixel(new int[] { pixels[offset],
						pixels[offset + 1], pixels[offset + 2] });

				pi++;
			}
		}
		return codedPixels;
	}

	public static void main(String[] args) {
		try {
			new KallaBallaProto(args[0], args[1]);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}

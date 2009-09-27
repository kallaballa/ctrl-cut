package org.wooden.kallaballa.algorithms;

import org.wooden.kallaballa.ihs.IHSLookup;
import org.wooden.kallaballa.ihs.IHSPixel;
import org.wooden.kallaballa.util.IHSTool;
import org.wooden.kallaballa.util.PixelAnalyzer;

public class Warp extends Algorithm {

	public Warp(String title, String description) {
		super(title, description);
	}

	public IHSPixel[] work(IHSPixel[] refPixels, IHSPixel[] targetPixels, IHSPixel[] resultPixels) {
		if(resultPixels == null)
			resultPixels = new IHSPixel[refPixels.length];

		int[] refGraphI = IHSTool.createIHSGraph(refPixels,
				IHSLookup.GROUP_I);
		int[] targetGraphI = IHSTool.createIHSGraph(targetPixels,
				IHSLookup.GROUP_I);
		int[] boundsRefI = PixelAnalyzer.findBounds(refGraphI, refPixels.length);
		int[] boundsTargetI = PixelAnalyzer.findBounds(targetGraphI, targetPixels.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianTargetI = PixelAnalyzer.calcMedian(boundsTargetI);

		int[] refGraphS = IHSTool.createIHSGraph(refPixels,
				IHSLookup.GROUP_S);
		int[] targetGraphS = IHSTool.createIHSGraph(targetPixels,
				IHSLookup.GROUP_S);
		int[] boundsRefS = PixelAnalyzer.findBounds(refGraphS, refPixels.length);
		int[] boundsTargetS = PixelAnalyzer.findBounds(targetGraphS, targetPixels.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianTargetS = PixelAnalyzer.calcMedian(boundsTargetS);

		double strechI = (double) widthRefI / (double) widthTargetI;
		double strechS = (double) widthRefS / (double) widthTargetS;

		for (int i = 0; i < targetPixels.length; i++) {
			int[] interpolated = new int[3];

			interpolated[0] = medianTargetI
					+ (int) Math
							.round((targetPixels[i].getI() - medianTargetI)
									* strechI);
			interpolated[1] = targetPixels[i].getH();
			interpolated[2] = medianTargetS
					+ (int) Math
							.round((targetPixels[i].getS() - medianTargetS)
									* strechS);

			resultPixels[i] = new IHSPixel(interpolated);
		}

		return resultPixels;
	}
}

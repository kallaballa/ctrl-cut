package org.wooden.kallaballa.algorithms;

import org.wooden.kallaballa.KallaBalla;
import org.wooden.kallaballa.ihs.IHSLookup;
import org.wooden.kallaballa.ihs.IHSPixel;
import org.wooden.kallaballa.util.IHSTool;
import org.wooden.kallaballa.util.PixelAnalyzer;

public class WarpShiftLookup extends Algorithm {

	public WarpShiftLookup(String title, String description) {
		super(title, description);
	}

	public IHSPixel[] work(IHSPixel[] refPixels, IHSPixel[] targetPixels, IHSPixel[] resultPixels) {
		if(resultPixels == null)
			resultPixels = new IHSPixel[refPixels.length];

		IHSLookup lookup = KallaBalla.instance().getIHSLookup();

		int[] refGraphI = IHSTool.createIHSGraph(refPixels,
				IHSLookup.GROUP_I);
		int[] targetGraphI = IHSTool.createIHSGraph(targetPixels,
				IHSLookup.GROUP_I);
		int[] boundsRefI = PixelAnalyzer.findBounds(refGraphI, refPixels.length);
		int[] boundsTargetI = PixelAnalyzer.findBounds(targetGraphI, targetPixels.length);
		int widthRefI = Math.abs(boundsRefI[0] - boundsRefI[1]);
		int widthTargetI = Math.abs(boundsTargetI[0] - boundsTargetI[1]);
		int medianRefI = PixelAnalyzer.calcMedian(boundsRefI);
		int medianTargetI = PixelAnalyzer.calcMedian(boundsTargetI);
		int offsetI = medianRefI - medianTargetI;

		int[] refGraphS = IHSTool.createIHSGraph(refPixels,
				IHSLookup.GROUP_S);
		int[] targetGraphS = IHSTool.createIHSGraph(targetPixels,
				IHSLookup.GROUP_S);
		int[] boundsRefS = PixelAnalyzer.findBounds(refGraphS, refPixels.length);
		int[] boundsTargetS = PixelAnalyzer.findBounds(targetGraphS, targetPixels.length);
		int widthRefS = Math.abs(boundsRefS[0] - boundsRefS[1]);
		int widthTargetS = Math.abs(boundsTargetS[0] - boundsTargetS[1]);
		int medianRefS = PixelAnalyzer.calcMedian(boundsRefS);
		int medianTargetS = PixelAnalyzer.calcMedian(boundsTargetS);
		int offsetS = medianRefS - medianTargetS;


		double strechI = (double) widthRefI / (double) widthTargetI;
		double strechS = (double) widthRefS / (double) widthTargetS;

		for (int i = 0; i < targetPixels.length; i++) {

			int[] interpolated = new int[3];

			IHSPixel match = lookup.findClosestMatch(targetPixels[i], true, IHSLookup.GROUP_H);
			IHSPixel revMatch = lookup.reverseLookup(match);
			
			int diffH = targetPixels[i].getH() - match.getH();

			interpolated[0] = offsetI
					+ medianTargetI
					+ (int) Math
							.round((targetPixels[i].getI() - medianTargetI)
									* strechI);
			interpolated[1] = revMatch.getH() - diffH;
			interpolated[2] = 15
					+ offsetS
					+ medianTargetS
					+ (int) Math
							.round((targetPixels[i].getS() - medianTargetS)
									* strechS);

			resultPixels[i] = new IHSPixel(interpolated);
		}

		return resultPixels;
	}
}

package org.wooden.kallaballa.algorithms;

import org.wooden.kallaballa.ihs.IHSPixel;

public class Average extends Algorithm {

	public Average(String title, String description) {
		super(title, description);
	}

	public IHSPixel[] work(IHSPixel[] refPixels, IHSPixel[] targetPixels, IHSPixel[] resultPixels) {
		if(resultPixels == null)
			resultPixels = new IHSPixel[refPixels.length];

		for (int i = 0; i < targetPixels.length; i++) {
			int[] interpolated = new int[3];

			interpolated[0] = (targetPixels[i].getI() + refPixels[i].getI()) / 2;
			interpolated[1] = (targetPixels[i].getH() + refPixels[i].getH()) / 2;
			interpolated[2] = (targetPixels[i].getS() + refPixels[i].getS()) / 2;
			refPixels[i] = new IHSPixel(interpolated);
		}
		return resultPixels;
	}
}

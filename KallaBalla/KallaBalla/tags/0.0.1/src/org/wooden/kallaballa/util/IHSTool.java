package org.wooden.kallaballa.util;

import java.awt.image.Raster;
import java.awt.image.RenderedImage;

import javax.media.jai.RenderedOp;

import org.wooden.kallaballa.ihs.IHSPixel;

public class IHSTool {
	public static IHSPixel[] createIHSPixels(RenderedImage src) {
		return createIHSPixels(src, 0, 0, src.getWidth(), src.getHeight());
	}

	public static IHSPixel[] createIHSPixels(RenderedImage src, int x, int y,
			int width, int height) {
		RenderedOp result = ImageTool.rgb2ihs(src);
		Raster raster = result.getData();
		int nbands = raster.getNumBands();
		int[] pixels = raster.getPixels(x, y, width, height, (int[]) null);

		return encodePixels(pixels, width, height, nbands);
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
}

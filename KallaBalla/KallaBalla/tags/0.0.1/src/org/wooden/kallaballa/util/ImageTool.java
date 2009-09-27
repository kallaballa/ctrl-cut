package org.wooden.kallaballa.util;

import java.awt.Transparency;
import java.awt.image.ColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.RenderedImage;
import java.awt.image.renderable.ParameterBlock;

import javax.media.jai.IHSColorSpace;
import javax.media.jai.InterpolationBicubic;
import javax.media.jai.JAI;
import javax.media.jai.ParameterBlockJAI;
import javax.media.jai.RasterFactory;
import javax.media.jai.RenderedOp;
import javax.media.jai.operator.ColorConvertDescriptor;

public class ImageTool {
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
	public static RenderedOp rgb2ihs(RenderedImage src) {

		// Create an IHS color model to transform to
		IHSColorSpace ihsCS = IHSColorSpace.getInstance();
		ColorModel ihsCM = RasterFactory.createComponentColorModel(
				DataBuffer.TYPE_BYTE, ihsCS, false, false, Transparency.OPAQUE);

		RenderedOp result = ColorConvertDescriptor.create(src, ihsCM, null);
		return result;
	}
}

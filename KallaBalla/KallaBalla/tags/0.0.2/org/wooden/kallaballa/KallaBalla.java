package org.wooden.kallaballa;

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
import org.wooden.kallaballa.algorithms.Average;
import org.wooden.kallaballa.algorithms.Warp;
import org.wooden.kallaballa.algorithms.WarpShiftLookup;
import org.wooden.kallaballa.ihs.IHSLookup;
import org.wooden.kallaballa.ihs.IHSPixel;
import org.wooden.kallaballa.util.IHSTool;
import org.wooden.kallaballa.util.JAITool;

public class KallaBalla {
	private static KallaBalla instance;

	private IHSLookup lookupTable;
	private RenderedImage refImg;
	private RenderedImage targetImg;
	private Algorithm[] algorithms;
	
	
	public KallaBalla(String fsample, String fdiff) throws IOException {
		instance = this;
		refImg = JAI.create("fileload", fsample);
		targetImg = JAI.create("fileload", fdiff);

		lookupTable = new IHSLookup(IHSTool.createIHSPixels(refImg), IHSTool.createIHSPixels(targetImg));
		algorithms = createAlgorithms();
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
		return new Algorithm[] { new Average("a","a"), new Warp("w","w"), new WarpShiftLookup("wsl", "wsl")};
	}

	public IHSPixel[] normalize(IHSPixel[] ihspixels) {
		WritableRaster writeRaster = targetImg.getData()
				.createCompatibleWritableRaster();
		int width = writeRaster.getWidth();
		int height = writeRaster.getHeight();
		int nbands = writeRaster.getNumBands();

		int[] pixels = IHSTool.decodePixels(ihspixels, width, height, nbands);

		writeRaster.setPixels(0, 0, width, height, pixels);

		TiledImage tile = new TiledImage(JAITool.rgb2ihs(targetImg), width, height);
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

		TiledImage tile = new TiledImage(JAITool.rgb2ihs(targetImg), width, height);
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

		TiledImage tile = new TiledImage(JAITool.rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		return ColorConvertDescriptor.create(tile, targetImg.getColorModel(),
				null).getAsBufferedImage();
	}

	public static void main(String[] args) {
		try {
			new KallaBalla(args[0], args[1]);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}

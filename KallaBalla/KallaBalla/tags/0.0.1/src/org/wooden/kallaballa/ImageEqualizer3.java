package org.wooden.kallaballa;
import java.awt.Transparency;

import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.RenderedImage;

import java.awt.image.WritableRaster;
import java.awt.image.renderable.ParameterBlock;

import java.io.*;
import java.util.Arrays;
import java.util.HashMap;

import javax.media.jai.IHSColorSpace;
import javax.media.jai.JAI;

import javax.media.jai.InterpolationNearest;
import javax.media.jai.ParameterBlockJAI;
import javax.media.jai.RasterFactory;
import javax.media.jai.RenderedOp;
import javax.media.jai.TiledImage;
import javax.media.jai.operator.ColorConvertDescriptor;
import javax.media.jai.operator.FileStoreDescriptor;

import java.util.Hashtable;

public class ImageEqualizer3 {
	
	public ImageEqualizer3(String fsample, String fdiff) throws IOException {
//		RenderedOp refImg = scale(cropTranslate(JAI.create("fileload", fsample),512,512,1024,1024), 2);

		RenderedImage refImg = JAI.create("fileload", fsample);
		RenderedImage targetImg = JAI.create("fileload", fdiff);
//		FileStoreDescriptor.create(refImg, "output.png", "png", null, null,	null);
//		System.exit(0);
		
		int[] codedRefImg = getCodedIHS(refImg); 
		int[] codedTargetImg = getCodedIHS(targetImg);
		
		
		HashMap<Integer, Integer> lookup = new HashMap<Integer, Integer>();

		for (int i = 0; i < codedRefImg.length; i++) {
			Integer key = new Integer(codedRefImg[i]);
			if(!lookup.containsKey(key))
				lookup.put(key, codedTargetImg[i]);
		}
		
		
		int[][] hueLookup = new int[256][1];
		for (int i = 0; i < hueLookup.length; i++) {
			hueLookup[i][0] = -1;
		}
		Integer[] keys = lookup.keySet().toArray(new Integer[0]);
		for (int i = 0; i < keys.length; i++) {
			Integer h = decodeH(lookup.get(keys[i]));
			if(hueLookup[h][0] == -1)
				hueLookup[h][0] = keys[i];
		}
		
		System.out.println(codedRefImg.length + " " + lookup.size());
		
		double onePercent = codedTargetImg.length / 100;
		double progress;
		for (int i = 0; i < codedTargetImg.length; i++) {
			progress = i % onePercent;
			if(progress == 0)
				System.out.println(i / onePercent);

				
				Integer match = findClosestMatch(codedTargetImg[i], hueLookup);
				int[] decodedMatch = decodePixel(match,null);

				codedTargetImg[i] = match;

		}
		
		
		WritableRaster writeRaster = targetImg.getData()
				.createCompatibleWritableRaster();
		int width = writeRaster.getWidth();
		int height = writeRaster.getHeight();
		int nbands = writeRaster.getNumBands();

		int[] pixels = decodePixels(codedTargetImg, width, height, nbands);
		
		writeRaster.setPixels(0, 0, width, height, pixels);


		TiledImage tile = new TiledImage(rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		RenderedOp result = ColorConvertDescriptor.create(tile, targetImg.getColorModel(), null);

		FileStoreDescriptor.create(result, fdiff+ ".png", "png", null, null,	null);

	}

    public static RenderedOp scale(RenderedOp src, float amount) {
        ParameterBlockJAI pb = new ParameterBlockJAI("scale");
        pb.addSource(src);
        pb.setParameter("xScale", amount); //x Scale Factor
        pb.setParameter("yScale", amount); //y Scale Factor
        pb.setParameter("xTrans", 0.0F);      //x Translate amount
        pb.setParameter("yTrans", 0.0F);      //y Translate amount
        pb.setParameter("interpolation", new InterpolationNearest());
        return JAI.create("scale", pb, null);
    }
    
    public static RenderedOp cropTranslate(RenderedOp src, int x, int y, int width, int height) {
    	ParameterBlock pb = new ParameterBlock();
        pb.addSource(src);
        pb.add((float)x);
        pb.add((float)y);
        pb.add((float)width);
        pb.add((float)height);
        
        RenderedOp cropped = JAI.create("crop",pb);
        pb = new ParameterBlock();
        pb.addSource(cropped);
        pb.add((float)-x);
        pb.add((float)-y);

        return JAI.create("translate",pb,null);
    }
    


	public BufferedImage fromRenderedToBuffered(RenderedImage img) {
		if (img instanceof BufferedImage) {
			return (BufferedImage) img;
		}

		ColorModel cm = img.getColorModel();
		int w = img.getWidth();
		int h = img.getHeight();
		WritableRaster raster = cm.createCompatibleWritableRaster(w, h);
		boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
		Hashtable props = new Hashtable();
		String[] keys = img.getPropertyNames();

		if (keys != null) {
			for (int i = 0; i < keys.length; i++) {
				props.put(keys[i], img.getProperty(keys[i]));
			}
		}
		BufferedImage ret = new BufferedImage(cm, raster, isAlphaPremultiplied,
				props);
		img.copyData(raster);
		return ret;
	}

	public Integer findClosestMatch(int ihs, int[][] hues) {
		if(ihs == 7865866)
			System.out.println();
		int h = decodeH(ihs);
		int ascMatch=255;
		int descMatch=-255;
		
		if(hues[h][0] != -1)
			return hues[h][0];
			
		for (int i = h + 1; i < hues.length; i++) {
			if(hues[i][0] != -1)
			{
				ascMatch = i;
				break;
			}
		}

		for (int i = h - 1; i > 0; i--) {
			if(hues[i][0] != -1)
			{
				descMatch = i;
				break;
			}
		}

		if(descMatch == -255 && ascMatch == 255)
			throw new IllegalStateException("Not a single match found");
		
		if((ascMatch - h) <= (h - descMatch))
			return hues[ascMatch][0];
		else
			return hues[ascMatch][0];
	}
	
	
	
	public static int[] getCodedIHS(RenderedImage src)
	{
		return getCodedIHS(src, 0, 0, src.getWidth(),src.getHeight());
	}
	
	public static int[] getCodedIHS(RenderedImage src, int x, int y, int width, int height)
	{
		RenderedOp result = rgb2ihs(src);
		Raster raster = result.getData();
		int nbands = raster.getNumBands();
		int[] pixels = raster.getPixels(x, y, width, height, (int[]) null);
	
		return encodePixels(pixels, width, height, nbands);
	}

	public static RenderedOp rgb2ihs(RenderedImage src)
	{

		// Create an IHS color model to transform to
		IHSColorSpace ihsCS = IHSColorSpace.getInstance();
		ColorModel ihsCM = RasterFactory.createComponentColorModel(
				DataBuffer.TYPE_BYTE, ihsCS, false, false, Transparency.OPAQUE);

		RenderedOp result = ColorConvertDescriptor.create(src, ihsCM, null);
		return result;
	}

	
	public static int decodeI(int codedPixel)
	{
		return (codedPixel & 0x000000ff);
	}

	public static int decodeH(int codedPixel)
	{
		return (codedPixel & 0x0000ff00) >>> 8;
	}
	public static int decodeS(int codedPixel)
	{
		return (codedPixel & 0x00ff0000) >>> 16;
	}

	public static int[] decodePixel(int codedPixel, int[] pixels)
	{
		if(pixels == null)
			pixels = new int[3];
		
		pixels[2] = (codedPixel & 0x000000ff);
		pixels[1] = (codedPixel & 0x0000ff00) >>> 8;
		pixels[0] = (codedPixel & 0x00ff0000) >>> 16;
		
		return pixels;
	}
	
	public static int encodePixel(int[] pixel)
	{
		int codedPixel;
		
		codedPixel = pixel[2];
		codedPixel = codedPixel | (pixel[1] << 8);
		codedPixel = codedPixel | (pixel[0] << 16);
		
		return codedPixel;
	}
	
	public static int[] decodePixels(int[] codedPixels, int width, int height, int nbands)
	{
		int[] pixels = new int[codedPixels.length * nbands];
		int pi = 0;
		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				int offset = h * width * nbands + w * nbands;
				pixels[offset + 2] = (codedPixels[pi] & 0x000000ff);
				pixels[offset + 1] = (codedPixels[pi] & 0x0000ff00) >>> 8;
				pixels[offset] = (codedPixels[pi] & 0x00ff0000) >>> 16;	
				pi++;
			}
		}
		return pixels;
	}
	
	public static int[] encodePixels(int[] pixels, int width, int height, int nbands)
	{
		int[] codedPixels = new int[(int) (pixels.length / nbands)];
		int pi = 0;

		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				int offset = h * width * nbands + w * nbands;
				
				codedPixels[pi] = pixels[offset + 2];
				codedPixels[pi] = codedPixels[pi] | (pixels[offset + 1] << 8);
				codedPixels[pi] = codedPixels[pi] | (pixels[offset] << 16);
				
				pi++;
			}
		}
		return codedPixels;
	}

	public static void main(String[] args) {
		try {
			new ImageEqualizer3(args[0],args[1]);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}

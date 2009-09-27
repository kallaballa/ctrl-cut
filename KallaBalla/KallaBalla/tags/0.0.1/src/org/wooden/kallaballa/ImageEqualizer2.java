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
import java.util.Vector;

import javax.media.jai.IHSColorSpace;
import javax.media.jai.JAI;

import javax.media.jai.InterpolationNearest;
import javax.media.jai.ParameterBlockJAI;
import javax.media.jai.RasterFactory;
import javax.media.jai.RenderedOp;
import javax.media.jai.TiledImage;
import javax.media.jai.operator.ColorConvertDescriptor;
import javax.media.jai.operator.FileStoreDescriptor;

import org.wooden.kallaballa.ihs.IHSPixel;

import java.util.Hashtable;

public class ImageEqualizer2 {
	
	public ImageEqualizer2(String fsample, String fdiff) throws IOException {
//		RenderedOp refImg = scale(cropTranslate(JAI.create("fileload", fsample),512,512,1024,1024), 2);

		RenderedImage refImg = JAI.create("fileload", fsample);
		RenderedImage targetImg = JAI.create("fileload", fdiff);
//		FileStoreDescriptor.create(refImg, "output.png", "png", null, null,	null);
//		System.exit(0);
		
		IHSPixel[] codedRefImg = createIHSPixels(refImg); 
		IHSPixel[] codedTargetImg = createIHSPixels(targetImg);
		IHSPixel[] codedResultImg = new IHSPixel[codedTargetImg.length];
		
		Hashtable<IHSPixel, IHSPixel> lookup = new Hashtable<IHSPixel, IHSPixel>();

		for (int i = 0; i < codedRefImg.length; i++) {
			if(!lookup.containsKey(codedRefImg[i]))
				lookup.put(codedRefImg[i], codedTargetImg[i]);
		}

		
		System.out.println(lookup.size());
		Hashtable<IHSPixel, IHSPixel> revLookup = new Hashtable<IHSPixel, IHSPixel>();
		IHSPixel[] lookupKeys = lookup.keySet().toArray(new IHSPixel[0]);
		
		for (int i = 0; i < lookupKeys.length; i++) {
			IHSPixel value = lookup.get(lookupKeys[i]);
			if(!revLookup.containsKey(value)) {
				revLookup.put(value, lookupKeys[i]);
			}
		}
		
		IHSPixel[] revKeys = revLookup.keySet().toArray(new IHSPixel[0]);
		Vector[] keysByHue = new Vector[256];
		Arrays.fill(keysByHue, null);
		Vector<IHSPixel> keys;
		
		
		for (int i = 0; i < revKeys.length; i++) {
			int index = revKeys[i].getH();
			
			keys = keysByHue[index];
			
			if(keys == null) {
				keys = new Vector<IHSPixel>();
				keysByHue[index] = keys;
			}
			
			keys.add(revKeys[i]);
		}
		
		System.out.println(codedRefImg.length + " " + lookup.size() + " " + revLookup.size() + " " + revKeys.length);
		
		
		double onePercent = codedTargetImg.length / 100;
		double progress;
		for (int i = 0; i < codedTargetImg.length; i++) {
			progress = i % onePercent;
			if(progress == 0)
				System.out.println(i / onePercent);
			IHSPixel value = revLookup.get(codedTargetImg[i]);
//			
//			if(value != null)
//				codedResultImg[i] = value;
//			else
//			{
				IHSPixel match = findClosestMatch(codedTargetImg[i], keysByHue);
				IHSPixel revMatch = revLookup.get(match);
				if(revMatch == null)
					System.out.println();
				int[] interpolated = new int[3];
				
				int diffI = codedTargetImg[i].getI() - match.getI(); 
				int diffH = codedTargetImg[i].getH() - match.getH();
				int diffS = codedTargetImg[i].getS() - match.getS();
				
				interpolated[0] = revMatch.getI() + diffI;  
				interpolated[1] = revMatch.getH() + diffH;
				interpolated[2] = revMatch.getS() + diffS;
				
				if(interpolated[0] > 255)
					interpolated[0] = 255;
				else if(interpolated[0] < 0)
					interpolated[0] = 0;

				if(interpolated[1] > 255)
					interpolated[1] = interpolated[1] - 255;
				else if(interpolated[1] < 0)
					interpolated[1] = 255 + interpolated[1];

				if(interpolated[2] > 255)
					interpolated[2] = 255;
				else if(interpolated[2] < 0)
					interpolated[2] = 0;

				if(Math.abs(interpolated[0] - codedRefImg[i].getI()) > 64)
				{
//					interpolated[0] = 128;
//					interpolated[1] = 128;
//					interpolated[2] = 128;
					interpolated[0] = codedRefImg[i].getI();
					interpolated[1] = codedRefImg[i].getH();
					interpolated[2] = codedRefImg[i].getS();
				}
				
//				if(interpolated[0] > 255 || interpolated[0] < 0 || interpolated[1] > 255 || interpolated[1] < 0 || interpolated[2] > 255 || interpolated[2] < 0)
//				{
//					interpolated[0] = 128;
//					interpolated[1] = 128;
//					interpolated[2] = 128;
//				}
					
				codedResultImg[i] = new IHSPixel(interpolated);
//			}
		}
		
		
		WritableRaster writeRaster = targetImg.getData()
				.createCompatibleWritableRaster();
		int width = writeRaster.getWidth();
		int height = writeRaster.getHeight();
		int nbands = writeRaster.getNumBands();

		int[] pixels = decodePixels(codedResultImg, width, height, nbands);
		
		writeRaster.setPixels(0, 0, width, height, pixels);


		TiledImage tile = new TiledImage(rgb2ihs(targetImg), width, height);
		tile.setData(writeRaster);
		RenderedOp result = ColorConvertDescriptor.create(tile, targetImg.getColorModel(), null);

		System.out.println(fdiff+ ".png");
		FileStoreDescriptor.create(result, fdiff+ ".png", "png", null, null, null);

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
    
	
//	public IHSPixel findClosestMatch(IHSPixel ihs, IHSPixel[] arrIHS) {
//
//		int diff = 255 * 3;
//		int currDiff = 0;
//		IHSPixel match = null;
//		
//		for (int i = 0; i < arrIHS.length; i++) {
//			int diffI = Math.abs(ihs.getI() - arrIHS[i].getI());
//			int diffH = Math.abs(ihs.getH() - arrIHS[i].getH());
//			int diffS = Math.abs(ihs.getS() - arrIHS[i].getS());
//
//			currDiff = diffI + diffH + diffS;
//			
//			if(diff > currDiff){
//				diff = currDiff;
//				match = arrIHS[i];
////				if(currDiff < 10)
////					return match;
//			}
//		}
//
//		return match;
//	}
	
	public IHSPixel findClosestMatch(IHSPixel ihs, Vector[] keysByHue) {

		int diff = 255 * 3;
		int currDiff = 0;
		
		int h = ihs.getH();
		Vector<IHSPixel> ascMatch = null;
		Vector<IHSPixel> descMatch  = null;
		IHSPixel[] arrIHS = null;
		IHSPixel match = null; 
		
		for (int i = 0; h + i < 256 || h - i > 0; i++) {
			if(h + i < 256)
				ascMatch = keysByHue[h + i]; 
			if(h - i > 0)
				descMatch = keysByHue[h - i]; 
		
			if(ascMatch != null)
			{
				arrIHS = ascMatch.toArray(new IHSPixel[0]);
				break;
			}
			if(descMatch != null)
			{
				arrIHS = descMatch.toArray(new IHSPixel[0]);
				break;
			}

		}

		
		for (int i = 0; i < arrIHS.length; i++) {
			int diffI = Math.abs(ihs.getI() - arrIHS[i].getI());
			int diffH = Math.abs(ihs.getH() - arrIHS[i].getH());
			int diffS = Math.abs(ihs.getS() - arrIHS[i].getS());

			currDiff = diffI + diffH + diffS;
			
			if(diff > currDiff){
				diff = currDiff;
				match = arrIHS[i];
//				if(currDiff < 10)
//					return match;
			}
		}

		return match;
	}
	
	
	public static IHSPixel[] createIHSPixels(RenderedImage src)
	{
		return createIHSPixels(src, 0, 0, src.getWidth(),src.getHeight());
	}
	
	public static IHSPixel[] createIHSPixels(RenderedImage src, int x, int y, int width, int height)
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
	
	public static int[] decodePixels(IHSPixel[] codedPixels, int width, int height, int nbands)
	{
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
	
	public static IHSPixel[] encodePixels(int[] pixels, int width, int height, int nbands)
	{
		IHSPixel[] codedPixels = new IHSPixel[(int) (pixels.length / nbands)];
		int pi = 0;

		for (int h = 0; h < height; h++) {
			for (int w = 0; w < width; w++) {
				int offset = h * width * nbands + w * nbands;
				
				codedPixels[pi] = new IHSPixel(new int[]{pixels[offset], pixels[offset + 1], pixels[offset + 2]});
				
				pi++;
			}
		}
		return codedPixels;
	}

	public static void main(String[] args) {
		try {
			new ImageEqualizer2(args[0],args[1]);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}

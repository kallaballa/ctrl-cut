package org.wooden.kallaballa;

import java.awt.Transparency;

import java.awt.image.ColorModel;
import java.awt.image.DataBuffer;
import java.awt.image.Raster;
import java.awt.image.RenderedImage;

import java.awt.image.WritableRaster;
import java.awt.image.renderable.ParameterBlock;

import java.io.*;

import javax.media.jai.IHSColorSpace;
import javax.media.jai.JAI;

import javax.media.jai.InterpolationNearest;
import javax.media.jai.ParameterBlockJAI;
import javax.media.jai.RasterFactory;
import javax.media.jai.RenderedOp;
import javax.media.jai.TiledImage;
import javax.media.jai.operator.ColorConvertDescriptor;
import javax.media.jai.operator.FileStoreDescriptor;

import org.wooden.kallaballa.ihs.IHSLookup;
import org.wooden.kallaballa.ihs.IHSPixel;

public class ImageEqualizer4 {
	
	public ImageEqualizer4(String fsample, String fdiff) throws IOException {
//		RenderedOp refImg = scale(cropTranslate(JAI.create("fileload", fsample),512,512,1024,1024), 2);

		RenderedImage refImg = JAI.create("fileload", fsample);
		RenderedImage targetImg = JAI.create("fileload", fdiff);
//		FileStoreDescriptor.create(refImg, "output.png", "png", null, null,	null);
//		System.exit(0);
		
		IHSPixel[] codedRefImg = createIHSPixels(refImg); 
		IHSPixel[] codedTargetImg = createIHSPixels(targetImg);
		IHSPixel[] codedResultImg = new IHSPixel[codedTargetImg.length];
		IHSLookup lookupTable = new IHSLookup(codedRefImg,codedTargetImg); 
				
		
		double onePercent = codedTargetImg.length / 100;
		double progress;
		for (int i = 0; i < codedTargetImg.length; i++) {
			progress = i % onePercent;
			if(progress == 0)
				System.out.println(i / onePercent);
//			IHSPixel value = lookupTable.reverseLookup(codedTargetImg[i]);
			
//			if(false && value != null)
//				codedResultImg[i] = value;
//			else
//			{
				IHSPixel match = lookupTable.findClosestMatch(codedTargetImg[i], true, IHSLookup.GROUP_H);
				IHSPixel revMatch = lookupTable.reverseLookup(match);

				int[] interpolated = new int[3];
				
				int diffI = match.getI() - codedTargetImg[i].getI(); 
				int diffH = match.getH() - codedTargetImg[i].getH();
				int diffS = match.getS() - codedTargetImg[i].getS();
				
				interpolated[0] = codedRefImg[i].getI() + 2 * (int)Math.round((((double)codedRefImg[i].getI() / (double)100) * (2.5 * (double)diffI)));//(codedTargetImg[i].getI() * 2 + codedRefImg[i].getI()) / 3;  
				interpolated[1] = codedRefImg[i].getH() + 2 * (int)Math.round((((double)codedRefImg[i].getH() / (double)100) * (2.5 * (double)diffH)));
				interpolated[2] = codedRefImg[i].getS() + 2 * (int)Math.round((((double)codedRefImg[i].getS() / (double)100) * (2.5 * (double)diffS)));//(codedTargetImg[i].getS() * 2 + codedRefImg[i].getS()) / 3;
				//System.out.println(interpolated[0] + " " +codedRefImg[i].getI() + " " + (int)Math.round((((double)codedRefImg[i].getI() / (double)100) * (2.5 * (double)diffI))) + " " + diffS);
				if(interpolated[0] > 255)
					interpolated[0] = 255;
				else if(interpolated[0] < 0)
					interpolated[0] = 0;

				if(interpolated[1] > 255)
					interpolated[1] = interpolated[1] - 255;
				else if(interpolated[1] < 0)
					interpolated[1] = 255 - interpolated[1];

				if(interpolated[2] > 255)
					interpolated[2] = 255;
				else if(interpolated[2] < 0)
					interpolated[2] = 0;

//				if(Math.abs(interpolated[0] - codedRefImg[i].getH()) > 32 || Math.abs(interpolated[2] - codedRefImg[i].getS()) > 32)
//				{
//					interpolated[0] = 128;
//					interpolated[1] = 128;
//					interpolated[2] = 128;
//					interpolated[0] = (codedTargetImg[i].getI() + codedRefImg[i].getI()) / 2; //+ diffI;
//					interpolated[1] = codedTargetImg[i].getH(); //+ diffH;
//					interpolated[2] = (codedTargetImg[i].getS() + codedRefImg[i].getS()) / 2; //+ diffS;
//				}
//				
//				if(interpolated[0] > 255 || interpolated[0] < 0 || interpolated[1] > 255 || interpolated[1] < 0 || interpolated[2] > 255 || interpolated[2] < 0)
//				{
//					interpolated[0] = 128;
//					interpolated[1] = 128;
//					interpolated[2] = 128;
//				}
					
				codedResultImg[i] = new IHSPixel(interpolated);
//			}
		}
		
//		for (int j = 0; j < codedResultImg.length; j++) {
//			int i = codedResultImg[j].getI() - 20;
//			int h = codedResultImg[j].getH();
//			int s = codedResultImg[j].getS() - 20;
//			codedResultImg[j] = new IHSPixel(new int[]{i,h,s});
//		}
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
			new ImageEqualizer4(args[0],args[1]);
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}

package org.wooden.kallaballa;

import java.awt.image.BufferedImage;
import java.io.*;
import java.util.HashMap;
import java.util.Iterator;

import org.wooden.image.ImageToolkit;

public class ImageEqualizer {
	private BufferedImage sample;
	private BufferedImage diff;

	public ImageEqualizer(File fsample, File fdiff) throws IOException {
		sample = ImageToolkit.readJpegImage(new FileInputStream(fsample));
		diff = ImageToolkit.readJpegImage(new FileInputStream(fdiff));

		int[][] sampleHSV = getHSV(sample);
		int[][] diffHSV = getHSV(diff);

		HashMap<int[],int[]> lookup = new HashMap<int[],int[]>();
		double percent = sampleHSV.length / 100;
		for (int i = 0; i < sampleHSV.length; i++) {
			if(i % percent == 0)
				System.out.println("Progress: " + i / percent);
			if(!contains(lookup, sampleHSV[i]))
				lookup.put(sampleHSV[i], diffHSV[i]);
		}
		System.out.println(lookup.size());

		
	}

	private static boolean contains(HashMap<int[],int[]> lookup, int[] hsv)
	{
		Iterator<int[]> keys = lookup.keySet().iterator();

		while(keys.hasNext()) {
			if(equals(keys.next(), hsv))
				return true;
		}
		return false;
	}
	private static boolean equals(int[] hsv1, int[] hsv2)
	{
		for (int i = 0; i < hsv2.length; i++) {
			if(hsv1[i] != hsv2[i])
				return false;
		}
		return true;
	}
	private static int[][] getHSV(BufferedImage img) {
		int height = img.getHeight();
		int width = img.getWidth();
		int[] rgb = new int[height * width];
		int[][] hsv = new int[height * width][3];
		int[] singleRGB = new int[4];
		int[] singleHSV = new int[3];
		rgb = img.getRGB(0, 0, width, height, rgb, 0, width);
		
		for (int i = 0; i < rgb.length; i++) {
			ImageToolkit.decodeColorInfo(rgb[i], singleRGB);
			rgb2hsv(singleRGB[0], singleRGB[1], singleRGB[2], singleHSV);

			for (int j = 0; j < singleHSV.length; j++) {
				hsv[i][j] = singleHSV[j];
			}
		}

		return hsv;
	}

	private static void rgb2hsv(int r, int g, int b, int hsv[]) {

		int min; // Min. value of RGB
		int max; // Max. value of RGB
		int delMax; // Delta RGB value

		if (r > g) {
			min = g;
			max = r;
		} else {
			min = r;
			max = g;
		}
		if (b > max)
			max = b;
		if (b < min)
			min = b;

		delMax = max - min;

		float H = 0, S;
		float V = max;

		if (delMax == 0) {
			H = 0;
			S = 0;
		} else {
			S = delMax / 255f;
			if (r == max)
				H = ((g - b) / (float) delMax) * 60;
			else if (g == max)
				H = (2 + (b - r) / (float) delMax) * 60;
			else if (b == max)
				H = (4 + (r - g) / (float) delMax) * 60;
		}

		hsv[0] = (int) (H);
		hsv[1] = (int) (S * 100);
		hsv[2] = (int) (V * 100);
	}

	public static void main(String[] args) {
		try {
			new ImageEqualizer(new File("8-scaled.jpg"), new File("9.jpg"));
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}
}

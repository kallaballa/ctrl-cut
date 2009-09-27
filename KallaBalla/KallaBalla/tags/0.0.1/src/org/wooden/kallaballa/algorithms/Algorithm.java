package org.wooden.kallaballa.algorithms;

import java.util.Vector;

import org.wooden.kallaballa.gui.ProgressDialog;
import org.wooden.kallaballa.ihs.IHSPixel;

public abstract class Algorithm {
	private String title, description;
	private IHSPixel[] cachedResult;
	private int[][] gLookup;
	private int[][] gRevLookup;
	private int[][] gResult;
	
	public Algorithm(String title, String description) {
		this.title = title;
		this.description = description;
	}

	public abstract IHSPixel[] work(ProgressDialog progress);

	public void cacheResult(IHSPixel[] cachedResult) {
		this.cachedResult = cachedResult;
	}

	public IHSPixel[] getCachedResult()
	{
		return this.cachedResult;
	}
	public String getTitle() {
		return title;
	}

	public String getDescription() {
		return description;
	}

	public String toString() {
		return getTitle();
	}
	
	public void cacheLookupGraph(int[] gLookupI, int[] gLookupH, int[] gLookupS)
	{
		gLookup = new int[][]{gLookupI, gLookupH, gLookupS};
	}

	public void cacheRevLookupGraph(int[] gRevLookupI, int[] gRevLookupH, int[] gRevLookupS)
	{
		gRevLookup = new int[][]{gRevLookupI, gRevLookupH, gRevLookupS};
	}
	public void cacheResultGraph(int[] gResultI, int[] gResultH, int[] gResultS)
	{
		gResult = new int[][]{gResultI, gResultH, gResultS};
	}

	public int[][] getCachedLookupGraph()
	{
		return gLookup;
	}

	public int[][] getCachedRevLookupGraph()
	{
		return gRevLookup;
	}

	public int[][] getCachedResultGraph()
	{
		return gResult;
	}

}

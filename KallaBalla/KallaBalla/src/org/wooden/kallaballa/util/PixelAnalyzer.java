package org.wooden.kallaballa.util;

public class PixelAnalyzer {
	public static int[] findBounds(int[] graph, int total) {
		int cumm = 0;
		int lowbound = 0;
		int highbound = 0;
		int threshold = (total / 100) * 5;
		for (int i = 0; i < graph.length; i++) {
			cumm += graph[i];
			if (cumm > threshold) {
				lowbound = i;
				break;
			}
		}
		cumm = 0;
		for (int i = graph.length - 1; i >= 0; i--) {
			cumm += graph[i];
			if (cumm > threshold) {
				highbound = i;
				break;
			}

		}

		return new int[] { lowbound, highbound };
	}

	public static int calcMedian(int[] bounds) {
		return Math.abs(bounds[0] - bounds[1]) / 2;
	}
	
	public static int findByFrequency(int ref, int[] refGraph, int refTotal,
			int[] targetGraph, int targetTotal) {
		int match = 0;
		double refFreq = refTotal / refGraph[ref];
		double targetFreq;
		double diff = Double.MAX_VALUE;
		double currDiff;

		for (int i = 0; i < targetGraph.length; i++) {
			if (targetGraph[i] == 0)
				continue;
			targetFreq = targetTotal / targetGraph[i];
			currDiff = Math.abs(refFreq - targetFreq);

			if (diff > currDiff) {
				diff = currDiff;
				match = i;
			}
		}

		return match;
	}
}

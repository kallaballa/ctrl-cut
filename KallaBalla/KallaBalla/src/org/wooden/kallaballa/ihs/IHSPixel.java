package org.wooden.kallaballa.ihs;
public class IHSPixel implements Comparable<IHSPixel> {
	private int ihs;
	private int[] decoded;

	public IHSPixel(int ihs) {
		setPixel(ihs);
	}

	public IHSPixel(int[] decoded) {
		setPixel(decoded);
	}

	public int getPixel() {
		return ihs;
	}

	public void setPixel(int pixel) {
		this.ihs = pixel;
		this.decoded = decodePixel(pixel, decoded);
	}

	public void setPixel(int[] decoded) {
		this.decoded = decoded;
		this.ihs = encodePixel(decoded);
	}

	public int[] getDecodedPixel() {
		return decoded;
	}

	public int getI() {
		return decoded[0];
	}

	public int getH() {
		return decoded[1];
	}

	public int getS() {
		return decoded[2];
	}

	public static int[] decodePixel(int codedPixel, int[] pixels) {
		if (pixels == null)
			pixels = new int[3];

		pixels[2] = (codedPixel & 0x000000ff);
		pixels[1] = (codedPixel & 0x0000ff00) >>> 8;
		pixels[0] = (codedPixel & 0x00ff0000) >>> 16;

		return pixels;
	}

	public static int encodePixel(int[] pixel) {
		int codedPixel;

		codedPixel = pixel[2];
		codedPixel = codedPixel | (pixel[1] << 8);
		codedPixel = codedPixel | (pixel[0] << 16);

		return codedPixel;
	}

	public int hashCode() {
		return ihs;
	}

	public boolean equals(Object obj) {
		if (obj instanceof IHSPixel) {
			return getPixel() == ((IHSPixel) obj).getPixel();
		}
		return false;
	}

	public int compareTo(IHSPixel anotherPixel) {
		int thisVal = ihs;
		int anotherVal = anotherPixel.getPixel();

		return (thisVal < anotherVal ? -1 : (thisVal == anotherVal ? 0 : 1));
	}
}

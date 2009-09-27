package org.wooden.kallaballa.ihs;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Vector;

public class IHSLookup {
	public final static int GROUP_I = 0;
	public final static int GROUP_H = 1;
	public final static int GROUP_S = 2;
	private HashMap<IHSPixel, IHSPixel> lookupTable = new HashMap<IHSPixel, IHSPixel>();
	private HashMap<IHSPixel, IHSPixel> revLookupTable = new HashMap<IHSPixel, IHSPixel>();
	private IHSPixel[] lookupKeys;
	private IHSPixel[] revLookupKeys;
	private Vector<IHSPixel>[][] groupedLookupKeys = new Vector[3][256];
	private Vector<IHSPixel>[][] groupedRevLookupKeys = new Vector[3][256];
	private IHSPixel[] reference;
	private IHSPixel[] target;
	public IHSLookup(IHSPixel[] reference, IHSPixel[] target) {
		this.reference = reference;
		this.target = target;
		
		Arrays.fill(groupedLookupKeys, null);
		Arrays.fill(groupedRevLookupKeys, null);
		for (int i = 0; i < reference.length; i++) {
			if (!lookupTable.containsKey(reference[i]))
				lookupTable.put(reference[i], target[i]);
		}

		IHSPixel[] lookupKeys = lookupTable.keySet().toArray(new IHSPixel[0]);
		for (int i = 0; i < lookupKeys.length; i++) {
			IHSPixel value = lookupTable.get(lookupKeys[i]);
			if (!revLookupTable.containsKey(value)) {
				revLookupTable.put(value, lookupKeys[i]);
			}
		}
	}
	
	public IHSPixel[] getReferencePixels()
	{
		return reference;
	}

	public IHSPixel[] getTargetPixels()
	{
		return target;
	}

	public IHSPixel lookup(IHSPixel ihs) {
		return lookupTable.get(ihs);
	}

	public IHSPixel reverseLookup(IHSPixel ihs) {
		return revLookupTable.get(ihs);
	}

	public IHSPixel[] getLookupKeys() {
		return getLookupKeys(true);
	}

	public IHSPixel[] getReverseLookupKeys() {
		return getReverseLookupKeys(true);
	}

	private IHSPixel[] getReverseLookupKeys(boolean cache) {
		IHSPixel[] revKeys;
		if (revLookupKeys == null) {
			revKeys = revLookupTable.keySet().toArray(new IHSPixel[0]);
			if (cache)
				revLookupKeys = revKeys;
		} else
			revKeys = revLookupKeys;

		return revKeys;
	}

	private IHSPixel[] getLookupKeys(boolean cache) {
		IHSPixel[] keys;
		if (lookupKeys == null) {
			keys = lookupTable.keySet().toArray(new IHSPixel[0]);
			if (cache)
				lookupKeys = keys;
		} else
			keys = revLookupKeys;

		return keys;
	}

	public Vector<IHSPixel>[] getGroupedReverseLookupKeys(int groupedBy) {
		Vector<IHSPixel>[] groupedKeys;
		if (groupedRevLookupKeys[groupedBy] != null) {
			groupedKeys = groupedRevLookupKeys[groupedBy];
		} else {
			IHSPixel[] revKeys = getReverseLookupKeys(false);
			groupedKeys = new Vector[256];
			Arrays.fill(groupedKeys, null);
			Vector<IHSPixel> keys;

			for (int i = 0; i < revKeys.length; i++) {
				int index = -1;
				switch (groupedBy) {
				case GROUP_I:
					index = revKeys[i].getI();
					break;
				case GROUP_H:
					index = revKeys[i].getH();
					break;
				case GROUP_S:
					index = revKeys[i].getS();
					break;

				default:
					throw new IllegalArgumentException(
							"Unknown group directive: " + groupedBy);
				}

				keys = groupedKeys[index];

				if (keys == null) {
					keys = new Vector<IHSPixel>();
					groupedKeys[index] = keys;
				}

				keys.add(revKeys[i]);
			}
			groupedRevLookupKeys[groupedBy] = groupedKeys;
		}
		return groupedKeys;
	}


	public Vector<IHSPixel>[] getGroupedLookupKeys(int groupedBy) {
		Vector<IHSPixel>[] groupedKeys;
		if (groupedLookupKeys[groupedBy] != null) {
			groupedKeys = groupedLookupKeys[groupedBy];
		} else {
			IHSPixel[] lookupKeys = getLookupKeys(false);
			groupedKeys = new Vector[256];
			Arrays.fill(groupedKeys, null);
			Vector<IHSPixel> keys;

			for (int i = 0; i < lookupKeys.length; i++) {
				int index = -1;
				switch (groupedBy) {
				case GROUP_I:
					index = lookupKeys[i].getI();
					break;
				case GROUP_H:
					index = lookupKeys[i].getH();
					break;
				case GROUP_S:
					index = lookupKeys[i].getS();
					break;

				default:
					throw new IllegalArgumentException(
							"Unknown group directive: " + groupedBy);
				}

				keys = groupedKeys[index];

				if (keys == null) {
					keys = new Vector<IHSPixel>();
					groupedKeys[index] = keys;
				}

				keys.add(lookupKeys[i]);
			}
			groupedLookupKeys[groupedBy] = groupedKeys;
		}
		return groupedKeys;
	}


	public IHSPixel findClosestMatch(IHSPixel ihs, boolean reverse,
			int groupeDirective) {
		Vector<IHSPixel>[] groupedKeys;
		if (reverse)
			groupedKeys = getGroupedReverseLookupKeys(groupeDirective);
		else
			groupedKeys = getGroupedLookupKeys(groupeDirective);

		int diff = 255 * 3;
		int currDiff = 0;

		int offset = ihs.getH();
		switch (groupeDirective) {
		case GROUP_I:
			offset = ihs.getI();
			break;
		case GROUP_H:
			offset = ihs.getH();
			break;
		case GROUP_S:
			offset = ihs.getS();
			break;

		default:
			throw new IllegalArgumentException("Unknown group directive: "
					+ groupeDirective);
		}
		Vector<IHSPixel> ascMatch = null;
		Vector<IHSPixel> descMatch = null;
		IHSPixel[] arrIHS = null;
		IHSPixel match = null;

		for (int i = 0; offset + i < 256 || offset - i > 0; i++) {
			if (offset + i < 256)
				ascMatch = groupedKeys[offset + i];
			if (offset - i > 0)
				descMatch = groupedKeys[offset - i];

			if (ascMatch != null) {
				arrIHS = ascMatch.toArray(new IHSPixel[0]);
				break;
			}
			if (descMatch != null) {
				arrIHS = descMatch.toArray(new IHSPixel[0]);
				break;
			}

		}

		for (int i = 0; i < arrIHS.length; i++) {
			int diffI = Math.abs(ihs.getI() - arrIHS[i].getI());
			int diffH = Math.abs(ihs.getH() - arrIHS[i].getH());
			int diffS = Math.abs(ihs.getS() - arrIHS[i].getS());

			currDiff = diffI + diffH + diffS;

			if (diff > currDiff) {
				diff = currDiff;
				match = arrIHS[i];
				// if(currDiff < 10)
				// return match;
			}
		}

		return match;
	}
}

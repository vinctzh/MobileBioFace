package com.beyare.ndkopencv2;

public class libJNICV {

	static{
		System.loadLibrary("JNICV");
	}
	
	public static native void JNICV(int[] buf, int[] outbuf, int w, int h);
	
}

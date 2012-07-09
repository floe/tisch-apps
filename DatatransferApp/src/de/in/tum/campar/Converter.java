package de.in.tum.campar;
import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.CharBuffer;
import java.nio.charset.Charset;

public class Converter {
	public static byte[] StringToByteArray(String string) {
		return toByteArray(string.toCharArray());
	}
	
	public static String ByteArrayToHexString(byte[] bytes) {
		BigInteger bi = new BigInteger(1, bytes);
		return String.format("%0" + (bytes.length << 1) + "X", bi);
	}
	
	public static byte[] toByteArray(char[] array) {
		return toByteArray(array, Charset.defaultCharset());
	}

	public static byte[] toByteArray(char[] array, Charset charset) {
		CharBuffer cbuf = CharBuffer.wrap(array);
		ByteBuffer bbuf = charset.encode(cbuf);
		return bbuf.array();
	}
	
	public static String ByteArrayToString(byte[] bytes) {
		return new String(bytes);
	}
	
	public static int toInt( byte[] bytes ) {
	    int result = 0;
	    for (int i=0; i<4; i++) {
	      result = ( result << 8 ) - Byte.MIN_VALUE + (int) bytes[i];
	    }
	    return result;
	  }
}

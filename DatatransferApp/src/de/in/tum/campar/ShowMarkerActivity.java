package de.in.tum.campar;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.widget.ImageView;

public class ShowMarkerActivity extends Activity {

	private Bitmap markerImage;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.show_marker);
		ImageView image = (ImageView) findViewById(R.id.showMarker);

		byte[] markerID = getIntent().getByteArrayExtra("markerID");
		Log.d("ShowMarker",
				"markerID " + Converter.ByteArrayToHexString(markerID));

		createMarkerImage(markerID);
		image.setImageBitmap(markerImage);

	}

	private void createMarkerImage(byte[] markerID) {

		Paint paint = new Paint();
		Display display = getWindowManager().getDefaultDisplay();
		int display_height = display.getHeight();
		int display_width = display.getWidth();

		int size;
		if (display_height > display_width)
			size = display_width;
		else
			size = display_height;

		markerImage = Bitmap.createBitmap(size, size, Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(markerImage);
		canvas.drawARGB(255, 255, 255, 255);
		int block_size = (int) size / 8;

		paint.setColor(Color.BLACK);
		paint.setStrokeWidth(0);

		Rect rect = null;
		// draw border
		for (int i = 0; i < 20; i++) {

			// top
			if (i >= 0 && i < 6)
				rect = new Rect(block_size + i * block_size, block_size, 2
						* block_size + i * block_size, 2 * block_size);

			// left
			if (i == 6 || i == 7 || i == 8 || i == 9)
				rect = new Rect(block_size, block_size + (i - 5) * block_size,
						2 * block_size, 2 * block_size + (i - 5) * block_size);

			// right
			if (i == 10 || i == 11 || i == 12 || i == 13)
				rect = new Rect(block_size + 5 * block_size, block_size
						+ (i - 9) * block_size,
						2 * block_size + 5 * block_size, 2 * block_size
								+ (i - 9) * block_size);

			// bottom
			if (i >= 14 && i < 20)
				rect = new Rect(block_size + (i - 14) * block_size, block_size
						+ 5 * block_size, 2 * block_size + (i - 14)
						* block_size, 2 * block_size + 5 * block_size);

			canvas.drawRect(rect, paint);

		}

	}
}

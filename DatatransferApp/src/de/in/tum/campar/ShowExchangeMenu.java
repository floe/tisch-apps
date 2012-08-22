package de.in.tum.campar;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.lang.reflect.Array;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.Intent;
import android.content.res.TypedArray;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.Gallery;
import android.widget.ImageView;
import android.widget.Toast;

public class ShowExchangeMenu extends Activity {

	public class ImageAdapater extends BaseAdapter {

		int mGalleryItemBackground;
		ArrayList<Bitmap> bitmapList;
		private Context mContext;

		public Integer[] mImageIds = { R.drawable.img00042,
				R.drawable.img00052, R.drawable.img00054 };

		// public ImageAdapater(Context c, ArrayList<Bitmap> bitmapList) {
		public ImageAdapater(Context c) {
			mContext = c;
			TypedArray attr = mContext
					.obtainStyledAttributes(R.styleable.HelloGallery);
			mGalleryItemBackground = attr.getResourceId(
					R.styleable.HelloGallery_android_galleryItemBackground, 0);
			attr.recycle();
			// this.bitmapList = bitmapList;
		}

		public int getCount() {
			return mImageIds.length;
			// return bitmapList.size();
		}

		public Object getItem(int position) {
			return mImageIds[position];
			// return bitmapList.get(position);
		}

		public long getItemId(int position) {
			return position;
		}

		public View getView(int position, View convertView, ViewGroup parent) {
			ImageView imageView = new ImageView(mContext);
			imageView.setImageResource(mImageIds[position]);

			// imageView.setImageBitmap(bitmapList.get(position));

			imageView.setLayoutParams(new Gallery.LayoutParams(200, 150));
			imageView.setScaleType(ImageView.ScaleType.FIT_XY);
			imageView.setBackgroundResource(mGalleryItemBackground);

			return imageView;
		}

	};

	byte[] mMarkerID;
	String ipTISCH;
	int portTISCH;
	private TcpClientService mTcpClientService;
	private ImageView imgView;
	ArrayList<Bitmap> bitmapList;
	private Bitmap yourSelectedImage = null;
	private String filePath;
	public static final String TAG = "ShowExchangeMenu";
	private ProgressDialog progressDialog;

	// -----------------------------------------------------------------------
	// manage Activity Lifecycle functions
	// -----------------------------------------------------------------------
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.showexchangemenu);

		Log.d("ShowExchangeMenu", "show exchange menu");

		setDisplayOrientation();
		setupButtonsAndListeners();

		mMarkerID = getIntent().getByteArrayExtra("markerID");
		ipTISCH = getIntent().getStringExtra("ipTISCH");
		portTISCH = getIntent().getIntExtra("portTISCH", 8080);

		mTcpClientService = TcpClientService.getInstance();
		mTcpClientService.setConnectionHandler(handleConnection);

		imgView = (ImageView) findViewById(R.id.selectedImage);
	}

	private Bitmap decodeFile(File f) {
		try {
			// Decode image size
			BitmapFactory.Options o = new BitmapFactory.Options();
			o.inJustDecodeBounds = true;
			BitmapFactory.decodeStream(new FileInputStream(f), null, o);

			// The new size we want to scale to
			final int REQUIRED_SIZE = 150;

			// Find the correct scale value. It should be the power of 2.
			int width_tmp = o.outWidth, height_tmp = o.outHeight;
			int scale = 1;
			while (true) {
				if (width_tmp / 2 < REQUIRED_SIZE
						|| height_tmp / 2 < REQUIRED_SIZE)
					break;
				width_tmp /= 2;
				height_tmp /= 2;
				scale *= 2;
			}

			// Decode with inSampleSize
			BitmapFactory.Options o2 = new BitmapFactory.Options();
			o2.inSampleSize = scale;
			return BitmapFactory.decodeStream(new FileInputStream(f), null, o2);

		} catch (FileNotFoundException e) {
		}
		return null;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.startSerice:
			startService(new Intent(ShowExchangeMenu.this,
					TcpClientService.class));
			break;
		case R.id.stopService:
			stopService(new Intent(ShowExchangeMenu.this,
					TcpClientService.class));
			break;
		case R.id.exit:
			stopService(new Intent(ShowExchangeMenu.this,
					TcpClientService.class));
			// Todo: stop threads
			closeConnection();
			finish();
			break;
		}
		return true;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu, menu);
		return true;
	}

	// -----------------------------------------------------------------------
	// UI Interaction - Display and BackgroundService
	// -----------------------------------------------------------------------
	private void setDisplayOrientation() {
		int display_orientation = getResources().getConfiguration().orientation;
		if (display_orientation == 1)
			setContentView(R.layout.showexchangemenu);
		else
			setContentView(R.layout.showexchangemenu);
	}

	private void setupButtonsAndListeners() {
		Button selectImage = (Button) findViewById(R.id.selectImage);
		selectImage.setOnClickListener(selectImageFromGalary);

		Button sendImage = (Button) findViewById(R.id.sendImage);
		sendImage.setOnClickListener(sendImageListener);
		sendImage.setVisibility(View.GONE);

		Button quit = (Button) findViewById(R.id.buttonClose);
		quit.setOnClickListener(closeConnectionToServer);
	}

	private OnClickListener selectImageFromGalary = new OnClickListener() {

		public void onClick(View v) {
			Intent intent = new Intent(
					Intent.ACTION_PICK,
					android.provider.MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
			startActivityForResult(intent, 100);

		}
	};

	private OnClickListener sendImageListener = new OnClickListener() {
		public void onClick(View v) {

			sendImage();
		}
	};

	private OnClickListener closeConnectionToServer = new OnClickListener() {

		public void onClick(View v) {
			closeConnection();
		}
	};

	private void closeConnection() {
		byte[] msg = new byte[] { (byte) 0x00, (byte) 0x00, (byte) 0x00,
				(byte) 0x03, mMarkerID[0], mMarkerID[1], mMarkerID[2],
				mMarkerID[3], };
		// | contentType = (int) 3 | markerID 4 byte |
		mTcpClientService.sendMessage(ipTISCH, portTISCH, msg);
		finish();
	}

	@SuppressLint("ParserError")
	private void sendImage() {

		// int bytesAvailable, bytesRead, bufferSize;
		// byte[] buffer;
		// int maxBufferSize = 1 * 1024 * 1024;
		// int sentBytes = 0;
		// long fileSize;
		//
		//
		//
		// Socket target;
		// try {
		// target = new Socket(ipTISCH, portTISCH);
		// File f = new File(filePath);
		// fileSize = f.length();
		// DataOutputStream dos = new DataOutputStream(
		// target.getOutputStream());
		// int size = (int) (header.length + fileSize);
		// dos.writeInt(size);
		// dos.flush();
		// dos.write(header);
		//
		// FileInputStream fis = new FileInputStream(f);
		// bytesAvailable = fis.available();
		// bufferSize = Math.min(bytesAvailable, maxBufferSize);
		// buffer = new byte[bufferSize];
		// bytesRead = fis.read(buffer, 0, bufferSize);
		// while (bytesRead > 0) {
		// dos.write(buffer, 0, bufferSize);
		// bytesAvailable = fis.available();
		// bufferSize = Math.min(bytesAvailable, maxBufferSize);
		// bytesRead = fis.read(buffer, 0, bufferSize);
		// }
		// dos.flush();
		// dos.close();
		// fis.close();
		//
		// } catch (UnknownHostException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// } catch (IOException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// }
		byte[] header = new byte[] { (byte) 0x00, (byte) 0x00, (byte) 0x00,
				(byte) 0x14, mMarkerID[0], mMarkerID[1], mMarkerID[2],
				mMarkerID[3] };

		progressDialog = ProgressDialog.show(this, "Please Wait",
				"sending image ...", true, false);

		mTcpClientService.sendFile(ipTISCH, portTISCH, new File(filePath),
				header);
		
	}

	public Handler handleConnection = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			if (progressDialog.isShowing())
				progressDialog.dismiss();

			Bundle myBundle = msg.getData();

			switch (msg.what) {
			case 20: // image received
				byte[] imageData = myBundle.getByteArray("image");
				Bitmap bmp = BitmapFactory.decodeByteArray(imageData, 0,
						imageData.length);
				break;

			}
		}

	};

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);

		switch (requestCode) {
		case 100:
			if (resultCode == RESULT_OK) {
				Uri selectedImage = data.getData();
				String[] filePathColumn = { MediaStore.Images.Media.DATA };

				Cursor cursor = getContentResolver().query(selectedImage,
						filePathColumn, null, null, null);
				cursor.moveToFirst();

				int columnIndex = cursor.getColumnIndex(filePathColumn[0]);
				filePath = cursor.getString(columnIndex);
				cursor.close();

				if(yourSelectedImage != null)
					yourSelectedImage.recycle();
				
				yourSelectedImage = BitmapFactory.decodeFile(filePath);
				imgView.setImageBitmap(yourSelectedImage);

				findViewById(R.id.sendImage).setVisibility(View.VISIBLE);

			}
		}
	}

}

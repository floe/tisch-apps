package de.in.tum.campar;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Array;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.GregorianCalendar;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.content.res.TypedArray;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
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
	private Bitmap bitmap = null;
	private String filePath;
	private Matrix matrix;
	private ContentResolver cr;
	private BitmapFactory.Options options;
	private Uri imageUri;
	private boolean connectionClosed = false;
	public static final String TAG = "ShowExchangeMenu";
	private ProgressDialog progressDialog;

	// -----------------------------------------------------------------------
	// manage Activity Lifecycle functions
	// -----------------------------------------------------------------------
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		if (savedInstanceState != null) {
			this.filePath = savedInstanceState.getString("filePath");
			this.imageUri = Uri.parse(savedInstanceState.getString("imageUri"));
		}

		setContentView(R.layout.showexchangemenu_port);

		Log.d("ShowExchangeMenu", "show exchange menu");

		setDisplayOrientation();

		TcpClientService.setMainActivity(this);
		TcpClientService.setMainContext(getApplicationContext());
		Log.d("ShowExchangeMenu", "main Activity is ShowExtendedMenu");

		mMarkerID = getIntent().getByteArrayExtra("markerID");
		ipTISCH = getIntent().getStringExtra("ipTISCH");
		portTISCH = getIntent().getIntExtra("portTISCH", 8080);

		mTcpClientService = TcpClientService.getInstance();
		Log.d("ShowExchangeMenu", "update connection handler");
		mTcpClientService.setConnectionHandler(handleConnection);

		cr = getApplicationContext().getContentResolver();
		options = new BitmapFactory.Options();
		options.inDither = false;
		options.inPurgeable = true;
		options.inInputShareable = true;
		options.inTempStorage = new byte[32 * 1024];
		// options.inJustDecodeBounds = true; // only get info, no mem
		// is alloced
		options.inSampleSize = 4;
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

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		setDisplayOrientation();
	}

	@Override
	protected void onSaveInstanceState(Bundle outState) {
		super.onSaveInstanceState(outState);
		outState.putString("filePath", filePath);
		if(imageUri != null)
			outState.putString("imageUri", imageUri.toString());
	}

	@Override
	protected void onDestroy() {
		Log.d("ShowExchangeMenu", "onDestroy()");
		if (!connectionClosed)
			closeConnection();
		super.onDestroy();
	}

	// -----------------------------------------------------------------------
	// UI Interaction - Display and BackgroundService
	// -----------------------------------------------------------------------
	private void setDisplayOrientation() {
		int display_orientation = getResources().getConfiguration().orientation;
		if (display_orientation == 1) {
			setContentView(R.layout.showexchangemenu_port);
		} else {
			setContentView(R.layout.showexchangemenu_land);
		}
		imgView = (ImageView) findViewById(R.id.selectedImage);
		setupButtonsAndListeners();

		if (bitmap != null && imageUri != null) {

			try {
				bitmap = BitmapFactory.decodeStream(
						cr.openInputStream(imageUri), null, options);
			} catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			displaySelectedImage();
		}
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
			intent.setType("image/*");

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
		connectionClosed = true;
		finish();
	}

	private void sendImage() {

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
			if (progressDialog != null && progressDialog.isShowing())
				progressDialog.dismiss();

			Bundle myBundle = msg.getData();

			Log.d("ShowExchangeMenu", "handleConnection; msg.what: " + msg.what);

			switch (msg.what) {
			case 20: // image received
				byte[] imageData = myBundle.getByteArray("image");
				Log.d("handle image", "received bytes: " + imageData.length);

				for (int i = 0; i < imageData.length; i += 100000) {
					Log.d("handle image", "imgD: " + imageData[i]);
				}

				Bitmap bmp = BitmapFactory.decodeByteArray(imageData, 0,
						imageData.length);
				Log.d("handle image",
						"bmp w*h: " + bmp.getWidth() + "*" + bmp.getHeight());

				ByteArrayOutputStream bytes = new ByteArrayOutputStream();
				bmp.compress(Bitmap.CompressFormat.JPEG, 40, bytes);

				// you can create a new file name "test.jpg" in sdcard
				// folder.
				String imageName = "IMG001_DTA_";
				SimpleDateFormat formatter = new SimpleDateFormat(
						"yyyy-MM-dd_HH-mm-ss");
				Date currentTime = new Date();
				imageName += formatter.format(currentTime) + ".jpg";

				// GregorianCalendar now = new GregorianCalendar();
				// DateFormat df =
				// DateFormat.getDateInstance(DateFormat.MEDIUM);
				// imageName += df.format(now.getTime());
				// df = DateFormat.getTimeInstance(DateFormat.SHORT);
				// imageName += df.format(now.getTime());
				// imageName += "jpg";

				String filename = Environment.getExternalStorageDirectory()
						+ File.separator + "DCIM" + File.separator + "Camera"
						+ File.separator + imageName;

				File f;
				f = new File(filename);
				try {
					f.createNewFile();
					// write the bytes in file
					FileOutputStream fo = new FileOutputStream(f);
					fo.write(bytes.toByteArray());
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				sendBroadcast(new Intent(Intent.ACTION_MEDIA_MOUNTED,
						Uri.parse("file://"
								+ Environment.getExternalStorageDirectory())));
				Log.d("handle image", "new image saved to: " + filename);
				break;

			}
		} // handleMessage

	};

	@Override
	protected void onActivityResult(int requestCode, int resultCode,
			Intent intent) {
		super.onActivityResult(requestCode, resultCode, intent);

		switch (requestCode) {
		case 100:
			if (resultCode == RESULT_OK) {

				imageUri = intent.getData();
				String[] cols = { MediaStore.Images.Media.ORIENTATION,
						MediaStore.Images.Media.DATA };
				Cursor cur = managedQuery(imageUri, cols, null, null, null);
				int orientation = -1;
				if (cur != null && cur.moveToFirst()) {
					orientation = cur.getInt(cur.getColumnIndex(cols[0]));
				}
				Log.d("orientation", "" + orientation);
				matrix = new Matrix();
				matrix.postRotate(orientation);

				// filePath is necessary to send selected file!
				filePath = cur.getString(cur.getColumnIndex(cols[1]));
				cur.close();

				displaySelectedImage();

			}
		}
	} // onActivityResult

	private void displaySelectedImage() {
		if (bitmap != null) {
			bitmap.recycle();
		}

		// bitmap = BitmapFactory.decodeFile(filePath);
		try {
			bitmap = BitmapFactory.decodeStream(cr.openInputStream(imageUri),
					null, options);
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		// rotate image according to retrieved orientation
		bitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(),
				bitmap.getHeight(), matrix, true);

		imgView.setImageBitmap(bitmap);

		findViewById(R.id.sendImage).setVisibility(View.VISIBLE);
	}
}

package de.in.tum.campar;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.Gallery;
import android.widget.ImageView;
import android.widget.Toast;

public class ShowExchangeMenu extends Activity {

	public class ImageAdapater extends BaseAdapter {

		int mGalleryItemBackground;
		private Context mContext;
		private Integer[] mImageIds = { R.drawable.img00042,
				R.drawable.img00052, R.drawable.img00054 };

		public ImageAdapater(Context c) {
			mContext = c;
			TypedArray attr = mContext
					.obtainStyledAttributes(R.styleable.HelloGallery);
			mGalleryItemBackground = attr.getResourceId(
					R.styleable.HelloGallery_android_galleryItemBackground, 0);
			attr.recycle();
		}

		public int getCount() {
			return mImageIds.length;
		}

		public Object getItem(int position) {
			return position;
		}

		public long getItemId(int position) {
			return position;
		}

		public View getView(int position, View convertView, ViewGroup parent) {
			ImageView imageView = new ImageView(mContext);
			imageView.setImageResource(mImageIds[position]);
			imageView.setLayoutParams(new Gallery.LayoutParams(600, 450));
			imageView.setScaleType(ImageView.ScaleType.FIT_XY);
			imageView.setBackgroundResource(mGalleryItemBackground);

			return imageView;
		}

	};

	byte[] mMarkerID;
	String ipTISCH;
	int portTISCH;
	private TcpClientService mTcpClientService;

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

		Gallery gallery = (Gallery) findViewById(R.id.gallery);
		gallery.setAdapter(new ImageAdapater(this));
		gallery.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView parent, View v, int position,
					long id) {
				Toast.makeText(ShowExchangeMenu.this, "" + position,
						Toast.LENGTH_SHORT).show();
				mTcpClientService.sendMessage(
						getIntent().getStringExtra("ipTISCH"), getIntent()
								.getIntExtra("portTISCH", 8080),
						new byte[] { (byte) (position + 1) });
			}
		});

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
		Button quit = (Button) findViewById(R.id.buttonClose);
		quit.setOnClickListener(closeConnectionToServer);
	}

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

	public Handler handleConnection = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			Bundle myBundle = msg.getData();
			Intent intent;

			switch (msg.what) {
			case 20: // image received
				byte[] imageData = myBundle.getByteArray("image");
				Bitmap bmp = BitmapFactory.decodeByteArray(imageData, 0,
						imageData.length);
				// ImageView image = new ImageView(this);
				// image.setImageBitmap(bmp);
				break;

			}
		}

	};

}

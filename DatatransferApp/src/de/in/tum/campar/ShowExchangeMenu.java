package de.in.tum.campar;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.TypedArray;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.AdapterView.OnItemLongClickListener;
import android.widget.BaseAdapter;
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
	private TcpClientService mTcpClientService;

	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		setContentView(R.layout.showexchangemenu);

		Log.d("ShowExchangeMenu", "show exchange menu");

		mMarkerID = getIntent().getByteArrayExtra("markerID");
		mTcpClientService = TcpClientService.getInstance();
		mTcpClientService.setHandleTischRequests(handleTCPResponses);
		
		Gallery gallery = (Gallery) findViewById(R.id.gallery);
		gallery.setAdapter(new ImageAdapater(this));
		gallery.setOnItemClickListener(new OnItemClickListener() {

			public void onItemClick(AdapterView parent, View v, int position,
					long id) {
				Toast.makeText(ShowExchangeMenu.this, "" + position, Toast.LENGTH_SHORT).show();
	            mTcpClientService.sendMessage(getIntent().getStringExtra("ipTISCH"), getIntent().getIntExtra("portTISCH", 8080), new byte[] { (byte) (position+1) }, 2);
			}
		});
				
	}

	public Handler handleTCPResponses = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			Bundle myBundle;
			Intent intent;

			switch (msg.what) {
			case 2:
				break;

			}
		}

	};

}

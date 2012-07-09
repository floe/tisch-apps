package de.in.tum.campar;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;

public class DatatransferAppActivity extends Activity {

	private Activity mMainActivity;
	private Context mMainContext;
	private ComponentName mStartedService;
	private TcpClientService mTcpClientService = null;
	private Bitmap markerImage;
	public String ipTISCH;
	public int portTISCH;

	private ProgressDialog progressDialog;

	// -----------------------------------------------------------------------
	// manage Activity Lifecycle functions
	// -----------------------------------------------------------------------
	@Override
	public void onCreate(Bundle savedInstanceState) {
		Log.d("DatatransferApp", "onCreate()");
		// -> onStart
		super.onCreate(savedInstanceState);

		mMainActivity = this;
		mMainContext = getApplicationContext();

		setDisplayOrientation();

		activateService();

		new Thread() {
			// wait until client is up, then set response handler
			public void run() {
				while ((mTcpClientService = TcpClientService.getInstance()) == null) {
					try {
						sleep(100);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
				mTcpClientService.setConnectionHandler(handleConnection);
				Log.d("DatatransferApp", "onCreate ResponseHandler set");
			}
		}.start();

		
	}

	@Override
	protected void onStart() {
		Log.d("DatatransferApp", "onStart()");
		// -> onResume
		super.onStart();
		setupButtonsAndListeners();
	}

	@Override
	protected void onResume() {
		Log.d("DatatransferApp", "onResume()");
		// -> onPause
		super.onResume();
		if(mTcpClientService != null)
			mTcpClientService.setConnectionHandler(handleConnection);
	}

	// -----------------------------------------------------------------------
	// Activity is now running
	// -----------------------------------------------------------------------

	@Override
	protected void onPause() {
		Log.d("DatatransferApp", "onPause()");
		// -> onResume
		// -> onCreate
		// -> onStop
		super.onPause();
	}

	@Override
	protected void onStop() {
		Log.d("DatatransferApp", "onStop()");
		// -> onRestart
		// -> onDestroy
		super.onStop();
	}

	@Override
	protected void onRestart() {
		Log.d("DatatransferApp", "onRestart()");
		// -> onStart
		super.onRestart();
	}

	@Override
	protected void onDestroy() {
		Log.d("DatatransferApp", "onDestroy()");
		super.onDestroy();
	}

	// -----------------------------------------------------------------------
	// Activity is now killed
	// -----------------------------------------------------------------------
	// ***********************************************************************
	// -----------------------------------------------------------------------
	// UI Interaction - Application Menu
	// -----------------------------------------------------------------------

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.startSerice:
			startService(new Intent(DatatransferAppActivity.this,
					TcpClientService.class));
			break;
		case R.id.stopService:
			stopService(new Intent(DatatransferAppActivity.this,
					TcpClientService.class));
			break;
		case R.id.exit:
			stopService(new Intent(DatatransferAppActivity.this,
					TcpClientService.class));
			// Todo: stop threads
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
			setContentView(R.layout.main_portrait);
		else
			setContentView(R.layout.main_land);
	}

	private void setupButtonsAndListeners() {
		Button connect = (Button) findViewById(R.id.buttonConnect);

		connect.setOnClickListener(connectToServer);
	}

	private void activateService() {
		Log.d("DataTransferApp", "start TCP Client Service");
		Intent tcpService = new Intent(mMainContext, TcpClientService.class);
		TcpClientService.setMainActivity(mMainActivity);
		TcpClientService.setMainContext(mMainContext);
		mStartedService = startService(tcpService);
		Log.d("DataTransferApp", "TCP Client Service started");
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

		for (int x = 0; x != markerID.length; x++) {

			byte tmp = (byte) (markerID[x] >> 4);
			// System.out.println((x + 1) + "tes Element aus dem Array::");
			for (int y = 0; y != 4; y++) // 8 stellen hat ein Byte
			{
				// Log.d("","  Bit an der " + (y + 1)
				// + " Stelle (entspricht "
				// + (int) java.lang.Math.pow(2, y) + ") ist: "
				// + (tmp & 1));

				if ((tmp & 1) == 1) {
					rect = new Rect((5 - y) * block_size, x * 2 * block_size
							+ 2 * block_size, (6 - y) * block_size, x * 2
							* block_size + 3 * block_size);
					canvas.drawRect(rect, paint);
				}
				tmp >>= 1;
			}

			tmp = markerID[x];

			for (int y = 0; y != 4; y++) // 8 stellen hat ein Byte
			{
				// Log.d("","  Bit an der " + (y + 1)
				// + " Stelle (entspricht "
				// + (int) java.lang.Math.pow(2, y) + ") ist: "
				// + (tmp & 1));
				if ((tmp & 1) == 1) {
					rect = new Rect((5 - y) * block_size, x * 2 * block_size
							+ 3 * block_size, (6 - y) * block_size, x * 2
							* block_size + 4 * block_size);
					canvas.drawRect(rect, paint);
				}
				tmp >>= 1;
			}

		}

	}
	
	public Handler handleConnection = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			Bundle myBundle = msg.getData();
			Intent intent;

			byte[] markerID = myBundle.getByteArray("markerID");

			switch (msg.what) {
			case 0: // response contains markerID
				// show marker
				setContentView(R.layout.show_marker);
				ImageView image = (ImageView) findViewById(R.id.showMarker);
				// markerID[0], markerID[1] should be 0 since marker consists
				// only of 16bit
				createMarkerImage(new byte[] { markerID[2], markerID[3] });
				image.setImageBitmap(markerImage);
				
				break;

			case 1: // marker was found
				setContentView(R.layout.main_portrait);
				intent = new Intent(getBaseContext(), ShowExchangeMenu.class);
				intent.putExtra("markerID", markerID);
				intent.putExtra("ipTISCH", ipTISCH);
				intent.putExtra("portTISCH", portTISCH);
				
				// switch to Gallery
				startActivity(intent);
				break;

			default:
				Log.d("handleTCPResponses", "error occured");

			}
		}

	};

	private OnClickListener connectToServer = new OnClickListener() {

		public void onClick(View v) {
			EditText inputIP = (EditText) findViewById(R.id.ipAddress);
			ipTISCH = inputIP.getText().toString();

			EditText inputPort = (EditText) findViewById(R.id.port);
			portTISCH = Integer.parseInt(inputPort.getText().toString());

			Log.d("connectToServer", "ip: " + ipTISCH);
			Log.d("connectToServer", "port: " + portTISCH);

			if (mTcpClientService == null)
				Log.d("connectToServer", "mTcpClientService null");

			// requestMarkerID
			byte[] msg = new byte[] { (byte) 0x00, (byte) 0x00, (byte) 0x00,
					(byte) 0x00, (byte) 0x00 };
			// | contentType = (int) 0 | message = 0x00 |
			mTcpClientService.sendMessage(ipTISCH, portTISCH, msg);

		}

	};

}
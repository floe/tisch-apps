package de.in.tum.campar;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

public class DatatransferAppActivity extends Activity {

	private Activity mMainActivity;
	private Context mMainContext;
	private ComponentName mStartedService;
	private TcpClientService mTcpClientService;
	private String ipTISCH;
	private int portTISCH;

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

				}
				mTcpClientService.setHandleTischRequests(handleTCPRequests);
				mTcpClientService.setHandleTischResponses(handleTCPResponses);
				Log.d("DatatransferApp", "onCreate ResponseHandler set");
			}
		}.start();

		setupButtonsAndListeners();
	}

	@Override
	protected void onStart() {
		Log.d("DatatransferApp", "onStart()");
		// -> onResume
		super.onStart();
	}

	@Override
	protected void onResume() {
		Log.d("DatatransferApp", "onResume()");
		// -> onPause
		super.onResume();

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

	public Handler handleTCPRequests = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			Bundle myBundle;
			Intent intent;

			switch (msg.what) {
			case 1: // marker was found

				myBundle = msg.getData();
				byte[] markerFound = myBundle.getByteArray("TischMSG");
				Log.d("handleMarkerID",
						"received markerID "
								+ Converter.ByteArrayToHexString(markerFound));

				intent = new Intent(getBaseContext(), ShowExchangeMenu.class);
				intent.putExtra("markerID", markerFound);

				startActivity(intent);
				break;
			}
		}
	};

	public Handler handleTCPResponses = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			Bundle myBundle;
			Intent intent;

			switch (msg.what) {
			case -1: // error occured while getting an ID
				myBundle = msg.getData();
				Log.d("handleTCPResponses", "error occured");
				Log.d("handleMarkerID",
						"received markerID "
								+ Converter.ByteArrayToHexString(myBundle
										.getByteArray("response")));
				break;

			case 0: // response contains markerID
				myBundle = msg.getData();
				byte[] markerIDtmp = myBundle.getByteArray("response");
				byte[] markerID = { markerIDtmp[2], markerIDtmp[3] };
				Log.d("handleMarkerID",
						"received markerID "
								+ Converter.ByteArrayToHexString(markerID));

				intent = new Intent(getBaseContext(), ShowMarkerActivity.class);
				intent.putExtra("markerID", markerID);

				startActivity(intent);
				// byte[] message = new byte[] { (byte) 0x01 };
				// int msgType = 1;
				// mTcpClientService.sendMessage(ipTISCH, portTISCH, message,
				// msgType); // waitForMarkerFound
				break;

			// case 1: // marker was found
			//
			// myBundle = msg.getData();
			// byte[] markerFound = myBundle.getByteArray("response");
			// Log.d("handleMarkerID",
			// "received markerID "
			// + Converter.ByteArrayToHexString(markerFound));
			//
			// intent = new Intent(getBaseContext(), ShowExchangeMenu.class);
			// intent.putExtra("markerID", markerFound);
			//
			//
			// startActivity(intent);
			// break;
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
			byte[] msg = new byte[] { (byte) 0x00 };
			int msgType = 0;
			mTcpClientService.sendMessage(ipTISCH, portTISCH, msg, msgType);

		}

	};

}
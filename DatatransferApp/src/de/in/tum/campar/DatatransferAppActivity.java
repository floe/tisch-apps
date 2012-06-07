package de.in.tum.campar;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;

import android.app.Activity;
import android.app.ProgressDialog;
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
import android.widget.TextView;

public class DatatransferAppActivity extends Activity {

	private TcpClient client;

	private ProgressDialog progressDialog;

	// -----------------------------------------------------------------------
	// manage Activity Lifecycle functions
	// -----------------------------------------------------------------------
	@Override
	public void onCreate(Bundle savedInstanceState) {
		// -> onStart
		super.onCreate(savedInstanceState);

		setDisplayOrientation();

		setupButtonsAndListeners();
	}

	@Override
	protected void onStart() {
		// -> onResume
		super.onStart();
	}

	@Override
	protected void onResume() {
		// -> onPause
		super.onResume();
	}

	// -----------------------------------------------------------------------
	// Activity is now running
	// -----------------------------------------------------------------------

	@Override
	protected void onPause() {
		// -> onResume
		// -> onCreate
		// -> onStop
		super.onPause();
	}

	@Override
	protected void onStop() {
		// -> onRestart
		// -> onDestroy
		super.onStop();
	}

	@Override
	protected void onRestart() {
		// -> onStart
		super.onRestart();
	}

	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
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
		case R.id.exit:
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

	private Handler handleMarkerID = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);

			switch (msg.what) {
			case 0: // everthing ok
				Bundle myBundle = msg.getData();
				byte[] markerID = myBundle.getByteArray("markerID");
				Log.d("handleMarkerID",
						"received markerID "
								+ Converter.ByteArrayToHexString(markerID));

				Intent intent = new Intent(getBaseContext(),
						ShowMarkerActivity.class);
				intent.putExtra("markerID", markerID);
				startActivity(intent);

				break;
			case 1: // error occured while getting an ID
				Log.d("handleMarkerID", "error occured getting a markerID");
				break;
			}
		}

	};

	private OnClickListener connectToServer = new OnClickListener() {

		public void onClick(View v) {
			EditText inputIP = (EditText) findViewById(R.id.ipAddress);
			String ip = inputIP.getText().toString();

			EditText inputPort = (EditText) findViewById(R.id.port);
			int port = Integer.parseInt(inputPort.getText().toString());

			Log.d("connectToServer", "ip: " + ip);
			Log.d("connectToServer", "port: " + port);

			client = new TcpClient(ip, port);
			Thread clientThread = new Thread(client);
			clientThread.start();
		}

	};

	public class TcpClient implements Runnable {

		int port;
		String target;
		Socket clientSocket;
		private DataOutputStream dos;
		private DataInputStream dis;
		private byte[] errorMsg = new byte[] { (byte) 0x65, (byte) 0x72,
				(byte) 0x72, (byte) 0x6f, (byte) 0x72 };

		private Message msg;
		private Bundle mBundle;

		public TcpClient(String target, int port) {
			this.port = port;
			this.target = target;
		}

		private void initConnection() throws UnknownHostException, IOException {
			msg = Message.obtain();
			msg.what = 0;
			mBundle = new Bundle();
			clientSocket = new Socket(target, port);
			dos = new DataOutputStream(clientSocket.getOutputStream());
			dis = new DataInputStream(clientSocket.getInputStream());

		}

		private void requestID() throws IOException {
			// =========================================================
			// to server
			// =========================================================
			// send request to server
			byte[] toServer;
			int out_len, start;

			// requestID
			toServer = new byte[] { (byte) 0x72, (byte) 0x65, (byte) 0x71,
					(byte) 0x75, (byte) 0x65, (byte) 0x73, (byte) 0x74,
					(byte) 0x49, (byte) 0x44 };
			out_len = toServer.length;
			start = 0;

			// send header
			dos.writeInt(out_len);

			// send payload
			if (out_len > 0) {
				dos.write(toServer, start, out_len);
			}

			// =========================================================
			// from server
			// =========================================================
			// receiver server response
			// read header
			int in_len = dis.readInt();
			byte[] receive_buffer = new byte[in_len];

			// read payload
			dis.readFully(receive_buffer);

			boolean error = true;
			if (in_len > 0) {
				Log.d("from Server",
						Converter.ByteArrayToHexString(receive_buffer));
				error = Arrays.equals(receive_buffer, errorMsg);
			}

			if (!error) {
				mBundle.putByteArray("markerID", receive_buffer);
				msg.setData(mBundle);
				handleMarkerID.sendMessage(msg);
			} else {
				handleMarkerID.sendEmptyMessage(1);
			}

		}

		private void closeConnection() throws IOException {

			clientSocket.close();

		}

		public void run() {

			try {

				initConnection();

				requestID();

				closeConnection();

			} catch (UnknownHostException e) {

				e.printStackTrace();
			} catch (IOException e) {

				e.printStackTrace();
			}

		}

	}
}
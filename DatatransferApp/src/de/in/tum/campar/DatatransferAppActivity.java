package de.in.tum.campar;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.TextView;

public class DatatransferAppActivity extends Activity {

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

	private OnClickListener connectToServer = new OnClickListener() {

		public void onClick(View v) {
			TextView tv_ip = (TextView) findViewById(R.id.ipAddress);
			String ip = tv_ip.toString();

			TextView tv_port = (TextView) findViewById(R.id.port);
			int port = Integer.parseInt(tv_port.toString());

			TcpClient client = new TcpClient(ip, port);
			client.run();
		}

	};

	public class TcpClient implements Runnable {

		int port;
		String target;
		Socket clientSocket;

		public TcpClient(String target, int port) {
			this.port = port;
			this.target = target;
		}

		public void run() {
			try {
				clientSocket = new Socket(target, port);
				// =========================================================
				// to server
				// =========================================================
				// send request to server
				DataOutputStream dos = new DataOutputStream(
						clientSocket.getOutputStream());
				byte[] toServer;
				int out_len, start;
				toServer = new byte[] { (byte) 0x38, (byte) 0x39 };
				out_len = toServer.length;
				start = 0;
				dos.writeInt(out_len);
				if (out_len > 0) {
					dos.write(toServer, start, out_len);
				}

				// =========================================================
				// from server
				// =========================================================
				// receiver server response
				InputStream is = clientSocket.getInputStream();
				byte[] receive_buffer = new byte[4096];
				int in_len = -1;
				in_len = is.read(receive_buffer);
				if (in_len > 0) {
					byte[] dataFromServer = Arrays.copyOfRange(receive_buffer,
							0, in_len);
				}

				clientSocket.close();

			} catch (UnknownHostException e) {

			} catch (IOException e) {

			} finally {
				if (clientSocket != null) {
					try {
						clientSocket.close();

					} catch (IOException e) {

					}
				}
			}

		}

	}
}
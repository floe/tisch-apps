package de.in.tum.campar;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Arrays;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

public class TcpClientService extends Service {

	public static Activity mMainActivity;
	public static Context mContext;
	private static TcpClientService sInstance;
	private Handler handleTischResponses;
	private Handler handleTischRequests;
	public TcpServer tcpServer;

	private NotificationManager nNM;
	private int NOTIFICATION = R.string.tcpServiceStarted;
	private final IBinder mBinder = new LocalBinder();

	public class LocalBinder extends Binder {
		TcpClientService getService() {
			return TcpClientService.this;
		}
	}

	@Override
	public IBinder onBind(Intent intent) {
		return mBinder;
	}

	public static TcpClientService getInstance() {

		return sInstance;
	}

	// TCP Server
	Thread mTCPServerThread = null;

	// ------------------------------------------------------------------------
	// manage Service Lifecycle function
	// ------------------------------------------------------------------------
	@Override
	public void onCreate() {
		Log.d("TcpClientService", "onCreate()");
		super.onCreate();
		sInstance = this;
		nNM = (NotificationManager) getSystemService(NOTIFICATION_SERVICE);

		showNotification();

		if (mTCPServerThread == null) {
			initServer();
		} else {
			Log.d("TcpClientService",
					"onCreate tcpServer thread already running");
		}

		Toast.makeText(this, "Service created", Toast.LENGTH_SHORT).show();

	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.d("TcpClientService", "onStartCommand()");
		super.onStartCommand(intent, flags, startId);

		if (mTCPServerThread == null) {
			initServer();
		} else {
			Log.d("TcpClientService",
					"onStartCommand tcpServer thread already running");
		}

		return START_STICKY;
	}

	@Override
	public void onDestroy() {
		Log.d("TcpClientService", "onDestroy()");

		tcpServer.stopServer();
		mTCPServerThread = null;

		nNM.cancel(NOTIFICATION);
		super.onDestroy();
	}

	// ------------------------------------------------------------------------
	// set service in connection with calling Activity
	// ------------------------------------------------------------------------
	public static void setMainActivity(Activity activity) {
		mMainActivity = activity;
	}

	public static void setMainContext(Context context) {
		mContext = context;
	}

	// ------------------------------------------------------------------------
	// show a notification while the service is running
	// ------------------------------------------------------------------------
	private void showNotification() {
		CharSequence text = getText(R.string.tcpServiceStarted);
		Notification notification = new Notification(R.drawable.ic_launcher,
				text, System.currentTimeMillis());

		notification.flags = Notification.FLAG_NO_CLEAR;

		PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
				new Intent(this, mMainActivity.getClass()), 0);
		notification.setLatestEventInfo(this,
				getText(R.string.tcpServiceStarted), text, contentIntent);

		nNM.notify(NOTIFICATION, notification);
	}

	// ------------------------------------------------------------------------
	//
	// ------------------------------------------------------------------------
	public void setHandleTischResponses(Handler mHandler) {
		handleTischResponses = mHandler;
	}

	public void setHandleTischRequests(Handler mHandler) {
		handleTischRequests = mHandler;
	}

	private void initServer() {

		tcpServer = new TcpServer(8080);
		mTCPServerThread = new Thread(tcpServer);
		mTCPServerThread.start();
		Log.d("TcpClientService", "initServer tcpServer thread started");

	}

	// ------------------------------------------------------------------------
	// API to Activity for TCP communication
	// ------------------------------------------------------------------------
	public void sendMessage(String ip, int port, byte[] message, int messageType) {
		Log.d("sendMessage", "IP: " + ip + " port: " + port);

		SentToTISCH sendToTisch = new SentToTISCH(ip, port);

		sendToTisch.setMessage(message, messageType);

		Thread clientThread = new Thread(sendToTisch);
		clientThread.start();

	}

	// ========================================================================
	// TCP Stuff
	// ========================================================================

	// ------------------------------------------------------------------------
	// server thread to handle messages from TISCH
	// ------------------------------------------------------------------------
	public class TcpServer implements Runnable {

		private ServerSocket serverSocket = null;
		private int IN_LISTEN_PORT = 0;
		private boolean runTCPServer;

		public TcpServer() {
			this.IN_LISTEN_PORT = 8080;
			initServerSocket();
			this.runTCPServer = true;
		}

		public TcpServer(int listenPort) {
			this.IN_LISTEN_PORT = listenPort;
			initServerSocket();
			this.runTCPServer = true;

		}

		public int getServerPort() {
			return this.IN_LISTEN_PORT;
		}

		public void stopServer() {
			this.runTCPServer = false;
		}

		private void initServerSocket() {
			try {
				serverSocket = new ServerSocket(this.IN_LISTEN_PORT);
				Toast.makeText(mContext, "server socket created",
						Toast.LENGTH_SHORT).show();
			} catch (IOException e) {
				e.printStackTrace();
				Log.d("TcpServer",
						"initServerSocket() problem creating ServerSocket on Port "
								+ IN_LISTEN_PORT);
			}

		}

		public void run() {
			while (runTCPServer) {
				try {
					Log.d("TcpServer", "waiting for connection ...");

					// wait for requests from TISCH
					Socket requestSocket = serverSocket.accept();

					// handle accepted request in separate thread
					TcpRequest tcpRequest = new TcpRequest(requestSocket);

					Log.d("TcpServer", "start request thread");

					// prepare and start separate thread
					Thread requestThread = new Thread(tcpRequest);
					requestThread.start();

				} catch (SocketException e) {
					Log.d("TcpServer", "SocketException");
					runTCPServer = false;
					Log.d("TcpServer", "stopped");

				} catch (IOException e) {
					Log.d("TcpServer", "IOException");
					e.printStackTrace();
				}

			} // while(runTCPServer) {
		}
	}

	// -----------------------------------------------------------------------
	// Request Handler
	// -----------------------------------------------------------------------
	public class TcpRequest implements Runnable {

		Socket clientSocket;
		private DataOutputStream dos;
		private DataInputStream dis;
		byte[] toTISCH;
		private Message msg;
		private Bundle mBundle;

		public TcpRequest(Socket socket) {
			this.clientSocket = socket;
		}

		public void run() {
			Log.d("TcpRequest", "running");

			try {
				initConnection();

				receiveMessage();

				closeConnection();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		} // run()

		private void initConnection() throws IOException {
			msg = Message.obtain();
			mBundle = new Bundle();
			dos = new DataOutputStream(clientSocket.getOutputStream());
			dis = new DataInputStream(clientSocket.getInputStream());

		}

		private void receiveMessage() throws IOException {

			// =========================================================
			// read from TISCH
			// =========================================================
			// read header
			int in_len = dis.readInt();

			// prepare buffer
			byte[] receive_buffer = new byte[in_len];

			// read payload
			dis.readFully(receive_buffer);

			mBundle.putByteArray("TischMSG", receive_buffer);
			msg.setData(mBundle);
			msg.what = 1;
			handleTischRequests.sendMessage(msg);

			// =========================================================
			// send ack to TISCH
			// =========================================================
			// prepare header
			int out_len, start;
			toTISCH = new byte[] { (byte) 0x00 };
			out_len = toTISCH.length;
			start = 0;

			// send header
			dos.writeInt(out_len);
			dos.flush();

			// send payload
			if (out_len > 0) {
				dos.write(toTISCH, start, out_len);
				dos.flush();
			}

		} // receiveMessage()

		private void closeConnection() throws IOException {
			Log.d("TcpRequest", "clientSocket.closed()");
			clientSocket.close();

		}

	}

	// ------------------------------------------------------------------------
	// handle send request to TISCH
	// ------------------------------------------------------------------------
	private class SentToTISCH implements Runnable {

		int port = 0;
		String target = null;
		Socket clientSocket;
		private DataOutputStream dos;
		private DataInputStream dis;
		private byte[] errorMsg = new byte[] { (byte) 0xFF };
		// private byte[] errorMsg = new byte[] { (byte) 0x65, (byte) 0x72,
		// (byte) 0x72, (byte) 0x6f, (byte) 0x72 };

		byte[] toServer;

		private Message msg;
		private Bundle mBundle;

		public SentToTISCH(String ip, int port) {
			this.port = port;
			this.target = ip;
		}

		public void setMessage(byte[] message, int msgType) {
			msg = Message.obtain();
			msg.what = msgType;
			toServer = message;
		}

		// ------------------------------------------------------------------------
		//
		// ------------------------------------------------------------------------
		public void run() {

			try {

				initConnection();

				sendMessage();

				closeConnection();

			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}

		}

		// ------------------------------------------------------------------------
		//
		// ------------------------------------------------------------------------
		private void initConnection() throws UnknownHostException, IOException {
			mBundle = new Bundle();
			clientSocket = new Socket(target, port);
			dos = new DataOutputStream(clientSocket.getOutputStream());
			dis = new DataInputStream(clientSocket.getInputStream());

		}

		private void sendMessage() throws IOException {
			// =========================================================
			// send message to server
			// =========================================================
			// calc header
			int out_len, start;
			out_len = toServer.length;
			start = 0;

			// send header
			dos.writeInt(out_len);
			dos.flush();

			// send payload
			if (out_len > 0) {
				dos.write(toServer, start, out_len);
				dos.flush();
			}

			// =========================================================
			// from server
			// =========================================================
			// read header
			int in_len = dis.readInt();

			// prepare buffer
			byte[] receive_buffer = new byte[in_len];

			// read payload
			dis.readFully(receive_buffer);

			Log.d("from Server", Converter.ByteArrayToHexString(receive_buffer));

			mBundle.putByteArray("response", receive_buffer);
			msg.setData(mBundle);
			handleTischResponses.sendMessage(msg);

			// // verify read data
			// boolean error = true;
			// if (in_len > 0) {
			// Log.d("from Server",
			// Converter.ByteArrayToHexString(receive_buffer));
			// error = Arrays.equals(receive_buffer, errorMsg);
			// }
			//
			// if (!error) {
			// mBundle.putByteArray("response", receive_buffer);
			// msg.setData(mBundle);
			// handleTischResponses.sendMessage(msg);
			// } else {
			// handleTischResponses.sendEmptyMessage(-1);
			// }

		}

		private void closeConnection() throws IOException {
			clientSocket.close();

		}

	}
}

package de.in.tum.campar;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Arrays;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.location.GpsStatus.NmeaListener;
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
	private TcpClient mTcpClient = null;
	private Handler handleTCPResponses;

	private NotificationManager nNM;
	private int NOTIFICATION = R.string.tcpClientStarted;
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

		if (mTcpClient == null) {
			createTcpClient();
		}

		Toast.makeText(this, "TcpClient created", Toast.LENGTH_SHORT).show();

	}

	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		Log.d("TcpClientService", "onStartCommand()");
		if(mTcpClient == null) {
			createTcpClient();
		}
			
		return START_STICKY;
	}

	@Override
	public void onDestroy() {
		Log.d("TcpClientService", "onDestroy()");
		mTcpClient = null;
		
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
		CharSequence text = getText(R.string.tcpClientStarted);
		Notification notification = new Notification(R.drawable.ic_launcher,
				text, System.currentTimeMillis());

		notification.flags = Notification.FLAG_NO_CLEAR;

		PendingIntent contentIntent = PendingIntent.getActivity(this, 0,
				new Intent(this, mMainActivity.getClass()), 0);
		notification.setLatestEventInfo(this,
				getText(R.string.tcpClientStarted), text, contentIntent);

		nNM.notify(NOTIFICATION, notification);
	}

	// ------------------------------------------------------------------------
	//
	// ------------------------------------------------------------------------
	private void createTcpClient() {
		mTcpClient = new TcpClient();

	}

	public void setResponseHandler(Handler mHandler) {
		handleTCPResponses = mHandler;
	}

	// ------------------------------------------------------------------------
	// API to Activity for TCP communication
	// ------------------------------------------------------------------------
	public void establishConnection(String ip, int port) {
		Log.d("establishConnection", "IP: " + ip + " port: "+ port);
		mTcpClient.setTarget(ip, port);

	}

	public void requestMarkerID() {
		// requestID
//		byte[] message = new byte[] { (byte) 0x72, (byte) 0x65, (byte) 0x71,
//				(byte) 0x75, (byte) 0x65, (byte) 0x73, (byte) 0x74,
//				(byte) 0x49, (byte) 0x44 };
		byte[] message = new byte[] { (byte) 0x00 };

		mTcpClient.setMessage(message, 0);
		mTcpClient.connectionOpen = true;
		mTcpClient.waitForNewMessage = false;
		Thread clientThread = new Thread(mTcpClient);
		clientThread.start();

	}

	public void waitForMarkerFound() {
		// waitForID
//		byte[] message = new byte[] { (byte) 0x77, (byte) 0x61, (byte) 0x69,
//				(byte) 0x74, (byte) 0x46, (byte) 0x6f, (byte) 0x72,
//				(byte) 0x49, (byte) 0x44 };
		byte[] message = new byte[] { (byte) 0x01 };
		mTcpClient.setMessage(message, 1);

//		Thread clientThread = new Thread(mTcpClient);
//		clientThread.start();
	}

	// ------------------------------------------------------------------------
	//
	// ------------------------------------------------------------------------
	private class TcpClient implements Runnable {

		int port = 0;
		String target = null;
		Socket clientSocket;
		public boolean connectionOpen;
		public boolean waitForNewMessage;
		private DataOutputStream dos;
		private DataInputStream dis;
		private byte[] errorMsg = new byte[] { (byte) 0xFF };
//		private byte[] errorMsg = new byte[] { (byte) 0x65, (byte) 0x72,
//				(byte) 0x72, (byte) 0x6f, (byte) 0x72 };

		byte[] toServer;

		private Message msg;
		private Bundle mBundle;

		public TcpClient() {

		}

		public void setTarget(String ip, int port) {
			this.port = port;
			this.target = ip;
		}

		public void setMessage(byte[] message, int msgType) {
			msg = Message.obtain();
			msg.what = msgType;
			toServer = message;
			waitForNewMessage = false;
		}

		// ------------------------------------------------------------------------
		//
		// ------------------------------------------------------------------------
		public void run() {

			try {

				initConnection();

				while(connectionOpen) {
					while(waitForNewMessage) {}
					sendMessage();
				}
				

				closeConnection();

			} catch (UnknownHostException e) {

				e.printStackTrace();
			} catch (IOException e) {

				e.printStackTrace();
			}

		}

		private void initConnection() throws UnknownHostException, IOException {
			mBundle = new Bundle();
			clientSocket = new Socket(target, port);
			dos = new DataOutputStream(clientSocket.getOutputStream());
			dis = new DataInputStream(clientSocket.getInputStream());

		}

		private void sendMessage() throws IOException {
			// =========================================================
			// to server
			// =========================================================
			// send request to server
			int out_len, start;

			// requestID

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
			// receiver server response
			// read header
			int in_len = dis.readInt();
			Log.d("from Server", "inLen: " + in_len);
			byte[] receive_buffer = new byte[in_len];
			Log.d("from Server", "recv buffer created");
			// read payload
			dis.readFully(receive_buffer);
			Log.d("from Server", "readFully payload");
			boolean error = true;
			if (in_len > 0) {
				Log.d("from Server",
						Converter.ByteArrayToHexString(receive_buffer));
				error = Arrays.equals(receive_buffer, errorMsg);
			}

			if (!error) {
				mBundle.putByteArray("response", receive_buffer);
				msg.setData(mBundle);
				handleTCPResponses.sendMessage(msg);
			} else {
				handleTCPResponses.sendEmptyMessage(-1);
			}
			
			waitForNewMessage = true;

		}

		private void closeConnection() throws IOException {

			clientSocket.close();

		}

	}
}

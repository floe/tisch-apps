package de.in.tum.campar;

import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Arrays;

import android.app.Activity;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.ProgressDialog;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap.CompressFormat;
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
	private Handler ConnectionHandler;
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
	public void setConnectionHandler(Handler mHandler) {
		ConnectionHandler = mHandler;
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
	public void sendMessage(String ip, int port, byte[] message) {
		Log.d("sendMessage", "IP: " + ip + " port: " + port);

		SentToTISCH sendToTisch = new SentToTISCH(ip, port, 0);

		sendToTisch.setMessage(message);

		Thread clientThread = new Thread(sendToTisch);
		clientThread.start();

	}

	public void sendFile(String ip, int port, File file, byte[] header) {
		Log.d("sendFile", "IP: " + ip + " port: " + port);

		SentToTISCH sendToTisch = new SentToTISCH(ip, port, 1);
		sendToTisch.setFile(file, header);

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
			try {
				// necessary to free the socket for later restart of app!
				serverSocket.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
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
			dis = new DataInputStream(clientSocket.getInputStream());
			Log.d("TcpRequest", "initConnection()");
		}

		private void receiveMessage() throws IOException {
			Log.d("TcpRequest", "receiveMessage()");
			// =========================================================
			// read from TISCH
			// =========================================================
			// read header
			// for (int i = 0; i < 13; i++) {
			// byte b = dis.readByte();
			// Log.d("TcpRequest", "i:" + i + " byte: " + b);
			// }
			// return;
			int in_len = dis.readInt();
			Log.d("TcpRequest", "in_len: " + in_len);

			if (in_len > 0) {
				// prepare buffer
				byte[] receive_buffer = new byte[in_len];

				// read payload
				dis.readFully(receive_buffer);
				int contentType = ((0xFF & receive_buffer[0]) << 24)
						| ((0xFF & receive_buffer[1]) << 16)
						| ((0xFF & receive_buffer[2]) << 8)
						| (0xFF & receive_buffer[3]);
				
				msg.what = contentType;
				byte content[];
				
				switch (contentType) {
				case 0: // markerID, handler in DatatransferAppActivitiy
					Log.d("TcpRequest", "case: "+ contentType);
				case 1: // marker found
					Log.d("TcpRequest", "marker found, case: " + contentType);
					
					content = new byte[in_len - 4]; // 4 byte as header, markerID as 32 bit int
					// copy subarray to new array
					// from receive_buffer, pos 8
					// to content, pos 0
					// copy in_len-8 elements
					System.arraycopy(receive_buffer, 4, content, 0, in_len - 4);

					mBundle.putByteArray("markerID", content);
					break;

				case 20: // image, handler in ShowExchangeMenu
					Log.d("TcpRequest", "received image, case: " + contentType);
					
					content = new byte[in_len - 8]; // 2 x 4 byte as header, (contentType, markerID) each as 32 bit int
					// copy subarray to new array
					// from receive_buffer, pos 8
					// to content, pos 0
					// copy in_len-8 elements
					System.arraycopy(receive_buffer, 8, content, 0, in_len - 8);
					
					Log.d("TcpRequest", "content has size: " + content.length);
					
					int targetMarkerID = ((0xFF & receive_buffer[4]) << 24)
							| ((0xFF & receive_buffer[5]) << 16)
							| ((0xFF & receive_buffer[6]) << 8)
							| (0xFF & receive_buffer[7]);
					
					Log.d("TcpRequest", "targetMarkerID is: " + targetMarkerID);
					
					mBundle.putInt("targetMarkerID", targetMarkerID);
					mBundle.putByteArray("image", content);
					
					for(int i = 0; i < content.length; i += 100000) {
						Log.d("TcpRequest", "imgD: " + content[i]);
					}
					
					Log.d("TcpRequest", "mBundle ready, jump back");
					break;

				default:

				}

				msg.setData(mBundle);
				ConnectionHandler.sendMessage(msg);
			}// if(in_len > 0)

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
		private int type;
		private File file;

		byte[] toServer;

		public SentToTISCH(String ip, int port, int type) {
			this.port = port;
			this.target = ip;
			this.type = type;
		}

		public void setMessage(byte[] message) {
			toServer = message;
		}

		public void setFile(File file, byte[] header) {
			this.file = file;
			this.toServer = header;
		}

		// ------------------------------------------------------------------------
		//
		// ------------------------------------------------------------------------
		public void run() {

			try {

				initConnection();

				switch (type) {
				case 0:
					sendMessage();
					break;
				case 1:
					sendFile();
					ConnectionHandler.sendEmptyMessage(0);
					break;
				default:
					break;
				}

				closeConnection();

			} catch (UnknownHostException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}

		}

		
		private Bitmap codec2(Bitmap src, Bitmap.CompressFormat format,
				int quality) {
			ByteArrayOutputStream os = new ByteArrayOutputStream();
			src.compress(format, quality, os);
	 
			byte[] array = os.toByteArray();
			return BitmapFactory.decodeByteArray(array, 0, array.length);
		}
		
		private File codec(Bitmap src, Bitmap.CompressFormat format,
				int quality) {
			
			File tmp = new File("/mnt/sdcard/tmpImage.png");
			try {
				OutputStream ostream = new FileOutputStream(tmp);
				src.compress(format, quality, ostream);
				ostream.close();
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			return tmp;
			
		}

		// ------------------------------------------------------------------------
		//
		// ------------------------------------------------------------------------
		private void initConnection() throws UnknownHostException, IOException {
			clientSocket = new Socket(target, port);
			dos = new DataOutputStream(clientSocket.getOutputStream());

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

		}

		private void sendFile() throws IOException {
			int bytesAvailable, bytesRead, bufferSize;
			int maxBufferSize = 1 * 1024 * 1024;
			byte[] buffer;
			long fileSize = file.length();
			int size = (int) (toServer.length + fileSize);
			dos.writeInt(size);
			dos.flush();

			dos.write(toServer, 0, toServer.length);

//			Bitmap bmp = BitmapFactory.decodeFile(file.getAbsolutePath());
//			File image = codec(bmp, Bitmap.CompressFormat.PNG, 0);
//			
			FileInputStream fis = new FileInputStream(file);
			bytesAvailable = fis.available();
			bufferSize = Math.min(bytesAvailable, maxBufferSize);
			buffer = new byte[bufferSize];
			bytesRead = fis.read(buffer, 0, bufferSize);
			while (bytesRead > 0) {
				dos.write(buffer, 0, bufferSize);
				//
				bytesAvailable = fis.available();
				bufferSize = Math.min(bytesAvailable, maxBufferSize);
				bytesRead = fis.read(buffer, 0, bufferSize);
			}
			dos.flush();
			fis.close();
			
//			image.delete();

		}

		private void closeConnection() throws IOException {

			dos.close();

			clientSocket.close();

		}

	}
}

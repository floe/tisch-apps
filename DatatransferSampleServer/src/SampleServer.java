import java.io.*;
import java.net.*;
import java.util.Arrays;
import java.util.Random;

public class SampleServer {

	ServerSocket serverSocket = null;
	int serverPort = 8080;
	Thread mThread = null;
	private byte[][] idList = new byte[][] {
			new byte[] { (byte) 0x12, (byte) 0x28 }, // 1228
			new byte[] { (byte) 0x1c, (byte) 0x44 }, // 1c44
			new byte[] { (byte) 0x0b, (byte) 0x44 }, // 0b44
			new byte[] { (byte) 0x06, (byte) 0x90 }, // 0690
			new byte[] { (byte) 0x00, (byte) 0x5a }, // 005a
			new byte[] { (byte) 0x02, (byte) 0x72 } // 0272
	};

	public SampleServer() throws IOException {
		System.out.println("create ServerSocket ...");
		serverSocket = new ServerSocket(serverPort);

		if (mThread == null) {
			System.out.println("create Serverthread ...");
			TcpServer tcpServer = new TcpServer(serverPort);
			mThread = new Thread(tcpServer);
			mThread.start();

		}

	}

	public static void main(String args[]) throws IOException {
		new SampleServer();
	}

	// -------------------------------------------------------------
	// Server
	// -------------------------------------------------------------
	public class TcpServer implements Runnable {
		private int TCP_PORT;
		private boolean runTcpServer;

		public TcpServer(int port) {
			this.TCP_PORT = port;
			runTcpServer = true;
		}

		@Override
		public void run() {
			while (runTcpServer) {
				System.out
						.println("TCP Server running, wait for connections ...");
				try {
					Socket requestSocket = serverSocket.accept();
					TcpRequest tcpRequest = new TcpRequest(requestSocket,
							TCP_PORT);
					Thread requestThread = new Thread(tcpRequest);
					requestThread.start();
				} catch (SocketException e) {
					runTcpServer = false;
				} catch (IOException e) {
					e.printStackTrace();
				}
			}

			if (serverSocket != null) {
				try {
					serverSocket.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}

	}

	// -------------------------------------------------------------
	// Request
	// -------------------------------------------------------------
	public class TcpRequest implements Runnable {

		int protocolStep;
		Socket clientSocket;
		int TCP_PORT;
		private DataInputStream dis;
		private DataOutputStream dos;
		byte[][] request = new byte[][] { //
		new byte[] { // requestID
				(byte) 0x72, (byte) 0x65, (byte) 0x71, (byte) 0x75,
						(byte) 0x65, (byte) 0x73, (byte) 0x74, (byte) 0x49,
						(byte) 0x44 }, //
				new byte[] { // waitForID
				(byte) 0x77, (byte) 0x61, (byte) 0x69, (byte) 0x74,
						(byte) 0x46, (byte) 0x6f, (byte) 0x72, (byte) 0x49,
						(byte) 0x44 //
				} //
		};

		public TcpRequest(Socket socket, int port) {
			this.clientSocket = socket;
			this.TCP_PORT = port;
		}

		@Override
		public void run() {
			System.out.println("Client connected, handle connection ...");

			try {
				initConnection();

				handleRequest();

				closeConnection();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		}

		private void initConnection() throws IOException {
			dis = new DataInputStream(clientSocket.getInputStream());
			dos = new DataOutputStream(clientSocket.getOutputStream());

		}

		private void handleRequest() throws IOException {
			// =========================================================
			// from client
			// =========================================================
			int in_len = dis.readInt();
			byte[] receive_buffer = new byte[in_len];

			dis.readFully(receive_buffer);
			boolean validRequest = false;

			if (in_len > 0) {
				System.out.println(Converter
						.ByteArrayToHexString(receive_buffer));

				for (int i = 0; i < request.length; i++) {
					validRequest = Arrays.equals(receive_buffer, request[i]);

					if (validRequest) {
						System.out.println("i: " + i);
						protocolStep = i;
						break;
					}

				}

			}
			// =========================================================
			// to client
			// =========================================================
			byte[] toClient = null;
			int out_len = 0;
			int start = 0;

			if (validRequest) {
				switch (protocolStep) {
				case 0:
					int rnd = new Random().nextInt(idList.length);
					toClient = idList[rnd];
					out_len = toClient.length;
					break;
				case 1:
					toClient = new byte[] {(byte) 0x61, (byte) 0x69};
					out_len = toClient.length;
					Random random = new Random();
					int myRandomNumber = random.nextInt(10000 - 5000) + 5000;
					try {
						Thread.sleep(myRandomNumber);
					} catch (InterruptedException ex) {
						Thread.currentThread().interrupt();
					}

					break;
				}

			} else {
				// error
				toClient = new byte[] { (byte) 0x65, (byte) 0x72, (byte) 0x72,
						(byte) 0x6f, (byte) 0x72 };
				out_len = toClient.length;

			}

			// send header
			dos.writeInt(out_len);

			// send payload
			if (out_len > 0) {
				dos.write(toClient, start, out_len);
			}

		}

		private void closeConnection() throws IOException {
			clientSocket.close();

		}

	}
}

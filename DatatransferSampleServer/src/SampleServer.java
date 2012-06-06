import java.io.*;
import java.net.*;
import java.util.Arrays;

public class SampleServer {

	ServerSocket serverSocket = null;
	int serverPort = 8080;
	Thread mThread = null;

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
		SampleServer server = new SampleServer();
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
				System.out.println("TCP Server running, wait for connections ...");
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

		Socket clientSocket;
		int TCP_PORT;

		public TcpRequest(Socket socket, int port) {
			this.clientSocket = socket;
			this.TCP_PORT = port;
		}

		@Override
		public void run() {
			System.out.println("Client connected, handle connection ...");
			handleBytes();
		}

		public void handleBytes() {
			try {
				// =========================================================
				// from client
				// =========================================================
				InputStream is = clientSocket.getInputStream();
				byte[] receive_buffer = new byte[4096];
				int in_len = -1;

				in_len = is.read(receive_buffer);

				if (in_len > 0) {
					byte[] dataFromClient = Arrays.copyOfRange(receive_buffer,
							0, in_len);
				}
				// =========================================================
				// to client
				// =========================================================
				DataOutputStream dos = new DataOutputStream(
						clientSocket.getOutputStream());
				byte[] toClient;
				int out_len, start;

				toClient = new byte[] { (byte) 0x40, (byte) 0x41 };
				out_len = toClient.length;
				start = 0;

				dos.writeInt(out_len);
				if (out_len > 0) {
					dos.write(toClient, start, out_len);
				}
				
				clientSocket.close();

			} catch (IOException e) {

			} finally {
				if (clientSocket != null) {
					try {
						clientSocket.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			}
		}

	}
}

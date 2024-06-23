import java.io.*;
import java.net.Socket;

public class SocketClient {
    private String host;
    private int port;
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;

    public SocketClient(String host, int port) {
        this.host = host;
        this.port = port;
    }

    public void connect() throws IOException {
        socket = new Socket(host, port);
        out = new PrintWriter(socket.getOutputStream(), true);
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    }

    public String sendMessage(String message) throws IOException {
        out.println(message);
        return in.readLine();
    }

    public void close() throws IOException {
        in.close();
        out.close();
        socket.close();
    }

    public static void main(String[] args) {
        SocketClient client = new SocketClient("localhost", 9999);
        try {
            client.connect();
            String response = client.sendMessage("2 binario1.bin");
            System.out.println("Response from server: " + response);
            client.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

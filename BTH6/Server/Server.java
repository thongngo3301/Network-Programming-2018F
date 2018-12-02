/**
 * Name: Ngo Doan Thong
 * MSSV: 16022451
 * Description:
 *    - Server open port 9090 to listen to connections from Clients
 *    - Server receive file name from Clients
 *    - Server send file back to Clients
 *    - When receive terminate character (QUIT), Server close connection
 **/

import java.net.*;
import java.io.*;

class ServerMultiThread extends Thread {
    private static final int _bufferSize = 1024;
    private static Socket connClientSocket;
    private static int fileCounter = 0;

    public ServerMultiThread(Socket _connSock) {
        connClientSocket = _connSock;
    }

    public void run() {
        InputStream is = null;
        InputStreamReader isr = null;
        OutputStream os = null;
        BufferedReader br = null;
        DataOutputStream dos = null;
        try {
            // Get info client
            InetAddress clientIpAddress = connClientSocket.getInetAddress();
            int clientPort = connClientSocket.getPort();
            System.out.println("Client address: " + clientIpAddress.toString().replace("/", "") + ":" + clientPort);

            // Input utils
            is = connClientSocket.getInputStream();
            isr = new InputStreamReader(is);
            br = new BufferedReader(isr);

            // Output utils
            os = connClientSocket.getOutputStream();
            dos = new DataOutputStream(os);

            while (true) {
                // Get requested file name
                String fileName = br.readLine();

                // Handle file name errors
                if (fileName == null || fileName.length() == 0) {
                    break;
                }

                fileName = fileName.trim();

                // Get file
                File file;
                FileInputStream fis;
                long fileSize;
                try {
                    file = new File(fileName);
                    fis = new FileInputStream(file);
                    fileSize = file.length();
                } catch (IOException ex) {
                    System.out.println("Cannot find file '" + fileName + "'!");
                    dos.writeLong(0);
                    continue;
                }

                System.out.println("Requested file name: '" + fileName + "'. File size: " + fileSize + " bytes.");

                // Handle file errors
                if (fileSize <= 0 ) {
                    System.out.println("Cannot find file '" + fileName + "'!");
                    dos.writeLong(0);
                    continue;
                }
                dos.writeLong(fileSize);

                // Send file
                byte[] buffer = new byte[_bufferSize];
                int nBytes;
                while ((nBytes = fis.read(buffer)) != -1) {
                    os.write(buffer, 0, nBytes);
                }
                System.out.println("Sent file successfully!");
                os.flush();
                fis.close();
                increaseFileCounter();
            }

        } catch (IOException ex) {
            ex.printStackTrace();
        } finally {
            try {
                is.close();
                isr.close();
                os.close();
                br.close();
                dos.close();
                connClientSocket.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }

    }

    private synchronized void increaseFileCounter() {
        fileCounter++;
        System.out.println("Total files sent to Clients: " + fileCounter);
    }
}

class Server {
    private final static int _port = 9090;

    public static void main(String[] args) {
        try {
            ServerSocket serverSocket = new ServerSocket(_port);
            System.out.println("Server is listening at port " + _port + ". Waiting for connections...");

            while (true) {
                Socket connClientSocket = serverSocket.accept();
                Thread multipleThread = new ServerMultiThread(connClientSocket);
                multipleThread.start();
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
}
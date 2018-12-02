/**
 * Name: Ngo Doan Thong
 * MSSV: 16022451
 * Description:
 *    - User type server's IP address and port from keyboard
 *    - User type message from keyboard
 *    - Client read and send file name to Server
 *    - Client receive requested file from Server
 *    - User type 'QUIT' to close connection
 **/

import java.net.*;
import java.io.*;
import java.util.Scanner;

class Client {
    private static final String _terminateStr = "QUIT";
    private static final int _bufferSize = 1024;
    private static Scanner sc = new Scanner(System.in);
    private static Socket serverSocket;
    private static String ipAddress;
    private static int port;


    public static void main(String[] args) {
        InputStream is = null;
        DataInputStream dis = null;
        OutputStream os = null;
        OutputStreamWriter osw = null;
        BufferedWriter bw = null;
        try {
            System.out.print("IP Address: ");
            ipAddress = sc.nextLine();
            System.out.print("Port: ");
            port = sc.nextInt();
            sc.nextLine();
            InetAddress address = InetAddress.getByName(ipAddress);
            serverSocket = new Socket(address, port);

            // Input utils
            is = serverSocket.getInputStream();
            dis = new DataInputStream(is);

            // Output utils
            os = serverSocket.getOutputStream();
            osw = new OutputStreamWriter(os);
            bw = new BufferedWriter(osw);

            while (true) {
                // Enter file name
                System.out.print("Enter file name to download, send 'QUIT' to stop: ");
                String fileName;
                if (sc.hasNextLine()) {
                    fileName = sc.nextLine();
                } else {
                    serverSocket.close();
                    break;
                }

                // Handle file name errors
                if (fileName == null || _terminateStr.equals(fileName) || fileName.length() == 0) {
                    serverSocket.close();
                    break;
                }

                String msg = fileName + "\n";
                bw.write(msg);
                bw.flush();

                // Receive file size
                long fileSize = dis.readLong();
                if (fileSize == 0) {
                    System.out.println("Cannot download file '" + fileName + "'!");
                    continue;
                }

                // Receive file
                FileOutputStream fos = new FileOutputStream(fileName);
                byte[] buffer = new byte[_bufferSize];
                long totalReadBytes = 0;
                while (totalReadBytes < fileSize) {
                    int nBytes = dis.read(buffer);
                    fos.write(buffer, 0, nBytes);
                    totalReadBytes += nBytes;
                }
                System.out.println("File '" + fileName + "' has been downloaded successfully!");
                fos.close();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        try {
            if (!serverSocket.isClosed()) {
                serverSocket.close();
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
}
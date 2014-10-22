import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Random;

/**
 * Trivial client for the date server.
 */
public class Recebedor {
	private static int CHANCE = 10;

	public static void main(String[] args) throws IOException, InterruptedException {
        ServerSocket servidor = new ServerSocket(9090); //abre a porta
       
        Random rand = new Random();
        Socket socket = servidor.accept(); //aceita conexao do servidor
        
        
        while (true) {
        	BufferedReader input =new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter out =new PrintWriter(socket.getOutputStream(), true);
            String answer = input.readLine();
            //System.out.print("answer = "+answer);
            System.out.println("RECEBEDOR: Recebeu o quadro "+answer);
            //Thread.sleep(100);
            if(answer.equals("fim")){
            	out.println(answer);
            	break;
            }
            if(rand.nextInt(100)>CHANCE){
            	out.println(answer);
            	System.out.println("RECEBEDOR: enviando confirmação");
            }else{
            	System.out.println("RECEBEDOR: falha ao enviar confirmação");
            }
        }
        socket.close();
        servidor.close();

    }
}

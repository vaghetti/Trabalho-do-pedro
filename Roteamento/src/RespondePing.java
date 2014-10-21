import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Random;

//thread que responde os pedidos de ping
public class RespondePing extends Thread{
    private Socket conexao;
    private Random rand;
    private PrintWriter out ;
    public void run(){
        try {
            Thread.sleep(rand.nextInt(300));  //espera um tempo aleatorio que será o ping
            out.println("HELLO"); //responde o pedido
            //System.out.println("RESPONDEDOR DE PING: Pedido de ping de "+conexao.getLocalAddress()+" respondido");
            conexao.close();
        } catch (InterruptedException | IOException e) {
            e.printStackTrace();
        }

    }
    public RespondePing(Socket conexao) throws IOException{
        this.conexao=conexao;
        rand = new Random();
        out =new PrintWriter(this.conexao.getOutputStream(), true);
    }
}

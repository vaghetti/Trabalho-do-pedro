import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

//Thread que fica recebendo requisicoes de ping e disparando Threads para responde-las

public class RespondedorPing extends Thread{
    private ServerSocket servidor; //abre a porta
    public void run(){
        while(true){
            try {
                Socket conexao = servidor.accept();
                new RespondePing(conexao).start();  //dispara Thread para esperar um tempo aleatorio, responder o pedido e fechar a conexao
            } catch (IOException e) {
                e.printStackTrace();
            }		
        }
    }
    public RespondedorPing() throws IOException{
        servidor = new ServerSocket(Principal.portaIP);	 
    }
}

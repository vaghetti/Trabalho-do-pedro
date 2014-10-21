import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;


//Thread que atualiza o ping de um vizinho especifico
public class AtualizaPing extends Thread {
    private Socket vizinho;
    private BufferedReader input;
    private PrintWriter out ;
    private String IPvizinho;
    private long tempo;
    @Override
    public void run(){
        try {
            System.out.println("PINGADOR: Pingando "+IPvizinho);
            out.println("HELLO");
            tempo=System.currentTimeMillis();
            input.readLine(); //recebe o hello de volta
            tempo=System.currentTimeMillis()-tempo;
            synchronized(Principal.LockMatrizEDados){
               // System.out.println("PINGADOOR: calculou pos = "+Principal.mapaIPs.get(IPvizinho)+" para o ip "+IPvizinho);
                Principal.pesos[0][Principal.mapaIPs.get(IPvizinho)][0]=tempo;
                Principal.pesos[0][Principal.mapaIPs.get(IPvizinho)][1]=System.currentTimeMillis();
            }
            //System.out.println("PINGADOR: Calculou ping para "+IPvizinho+" como sendo "+tempo);
            vizinho.close();
        } catch ( NumberFormatException | IOException e) {
            System.out.println("PINGADOR: Erro no pedido de ping");
        }
        
    }


    public AtualizaPing(String IPvizinho) throws IOException{
        vizinho=new Socket(IPvizinho,Principal.portaPing);
        input =new BufferedReader(new InputStreamReader(vizinho.getInputStream()));
        out =new PrintWriter(vizinho.getOutputStream(), true);
        this.IPvizinho=IPvizinho;
    }
}

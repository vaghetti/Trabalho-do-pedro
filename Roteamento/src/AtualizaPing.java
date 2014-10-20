import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;


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
            System.out.println("PINGADR: Calculou ping para "+IPvizinho+" como sendo "+tempo);
            vizinho.close();

        } catch (IOException | NumberFormatException e) {
            //e.printStackTrace();
            System.out.println("PINGADOR: Erro no pedido de ping");
        }
    }


    public AtualizaPing(String IPvizinho) throws IOException{
        vizinho=new Socket(IPvizinho,Principal.portaIP);
        input =new BufferedReader(new InputStreamReader(vizinho.getInputStream()));
        out =new PrintWriter(vizinho.getOutputStream(), true);
        this.IPvizinho=IPvizinho;
    }


    public long getTempo() throws InterruptedException {
        while(this.isAlive()){
            Thread.sleep(100);
        }
        return tempo;
    }
}

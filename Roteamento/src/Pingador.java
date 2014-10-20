import java.io.IOException;


//Thread que fica pedidndo o ping de cada um dos vizinhos
public class Pingador extends Thread {
    String[] IPsVizinhos;
    private final int tempoEspera=5;
    public void run(){
        try {
            while(true){	
                for(int x=0;x<IPsVizinhos.length;x++){
                        new AtualizaPing(IPsVizinhos[x]).start();  //manda o ip do vizinho para ober o novo ping
                }
                Thread.sleep(tempoEspera*1000);
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        } 	
    }
    public Pingador (String[] IPsVizinhos){
        this.IPsVizinhos=IPsVizinhos;
    }
}

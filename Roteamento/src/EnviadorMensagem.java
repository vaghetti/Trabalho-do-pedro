
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Map;
import javax.swing.JOptionPane;


public class EnviadorMensagem extends Thread {
    private String saida;
    public void run(){
        try {
            while(true){
                while(!Principal.fezPrimeiroDijkstra);
                String entrada = JOptionPane.showInputDialog("Informe a sua mensagem no formato IP:MENSAGEM");
                int anterior = Principal.mapaIPs.get(entrada.split(":")[0]);
                synchronized(Dijkstrador.anterior){
                    while(Dijkstrador.anterior[anterior]!=0){
                        anterior = Dijkstrador.anterior[anterior];
                    }
                    for(Map.Entry<String, Integer> entry : Principal.mapaIPs.entrySet()) {
                        String key = entry.getKey();
                        int value = entry.getValue();
                        if(value == anterior){

                                Socket envia = new Socket(key,Principal.portaMensagens);
                                PrintWriter out =new PrintWriter(envia.getOutputStream(), true);
                                out.println(entrada.split(":")[0]+":"+Principal.say()+":"+entrada.split(":")[1]);
                                break;
                        }
                    }
                }
            }
        }catch ( IOException ex) {
            ex.printStackTrace();
        }
    }
    public EnviadorMensagem(){
        
    }
}

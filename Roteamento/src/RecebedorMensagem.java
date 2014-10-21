
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Map;
import javax.lang.model.type.PrimitiveType;
import javax.swing.JOptionPane;

/*
 * PROTOCOLO DE MENSAGEM
 * IPRECEBER:IPENVIOU:MENSAGEM
 */
public class RecebedorMensagem extends Thread {
    private ServerSocket servidor;
    private BufferedReader input;
    private PrintWriter out ;
    private Socket conexao;
    public void run(){
        try {
            while(true){
                Socket conexao = servidor.accept();
                input =new BufferedReader(new InputStreamReader(conexao.getInputStream()));
                String aux=input.readLine();
                String mensagem[] = aux.split(":");
                
                if(mensagem[0].equals(Principal.say())){
                    System.out.println("RECEBEDORMENSAGEM: recebeu a mensagem "+mensagem[2]+" de "+mensagem[1]);
                    JOptionPane.showMessageDialog(null, "Você recebeu a mensagem \n"+ mensagem[2]+"\n de: "+mensagem[1]);
                }else{
                    synchronized(Dijkstrador.anterior){
                        int anterior = Principal.mapaIPs.get(mensagem[0]);
                        Socket repassa = null;
                        System.out.println("RECEBEDORMENSAGEM: anterior = "+anterior);
                        while(Dijkstrador.anterior[anterior]!=0){
                            anterior = Dijkstrador.anterior[anterior];
                            System.out.println("RECEBEDORMENSAGEM: anterior = "+anterior);
                        }
                        for(Map.Entry<String, Integer> entry : Principal.mapaIPs.entrySet()) {
                            String key = entry.getKey();
                            int value = entry.getValue();
                            if(value == anterior){
                                JOptionPane.showMessageDialog(null,"encaminhou mensagem "+mensagem[2]+" de "+mensagem[0]+" para "+mensagem[0] + "atravez de "+key);
                                repassa = new Socket(key,Principal.portaMensagens);
                                break;
                            }
                        }
                        out =new PrintWriter(repassa.getOutputStream(), true);
                        out.println(aux);
                    }
                }
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    public RecebedorMensagem(){
        try {
            servidor=new ServerSocket(Principal.portaMensagens);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
}

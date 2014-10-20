import java.io.IOException;
import java.net.UnknownHostException;

import javax.swing.JOptionPane;


public class Principal {
    static int[][] pesos;
    public static int portaIP = 9090;

    public static void main(String[] args) {
        try {
            System.out.println("Começou o programa!");
            RespondedorPing respondedorPing= new RespondedorPing();
            respondedorPing.start();
            Pingador pingador = new Pingador(args);  //os ips dos vizinhos sao recebidos como argumentos
            //TODO: mostrar o ip na caixinha de dialogo
            if(JOptionPane.showConfirmDialog(null, "iniciar operação?"+say()) == JOptionPane.CANCEL_OPTION){
                return;
            }
            pingador.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
     public static  String say()   {    
        try {    
            java.net.InetAddress i = java.net.InetAddress.getLocalHost();    
            String ip = i.getHostAddress();    
            return ip;   
        }    
        catch(UnknownHostException e){e.printStackTrace();  
        return "NO-IP";}    
      }    

}

import java.io.IOException;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.swing.JOptionPane;


public class Principal {
    public static long[][][] pesos;
    public static int portaPing = 9093;
    public static int portaDados = 9094;
    public static int portaMensagens = 9095;
    public static int Ncomputadores = 5;
    public static Map<String,Integer> mapaIPs;
    public static Integer NComputadoresConhecidos=0;
    public static final Object LockMatrizEDados= new Object();
    public static long inicio;
    

    public static void main(String[] args) {
        inicio = System.currentTimeMillis();
        pesos = new long[Ncomputadores][Ncomputadores][2];
        //primeira coluna = peso
        //segunda = hora em que este peso foi lido
        initMapa(args);
        
        try {
            System.out.println("Começou o programa!");
            RespondedorPing respondedorPing= new RespondedorPing();
            respondedorPing.start();
            RecebedorDados recebedorDados = new RecebedorDados();
            recebedorDados.start();
            Pingador pingador = new Pingador(args);  //os ips dos vizinhos sao recebidos como argumentos
            //TODO: mostrar o ip na caixinha de dialogo
            if(JOptionPane.showConfirmDialog(null, "iniciar operação?"+say()) == JOptionPane.CANCEL_OPTION){
                return;
            }
            pingador.start();
            PropagadorDados propagadorDados = new PropagadorDados(args);
            propagadorDados.start();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    
    private static void initMapa(String[] IPsVizinhos){
        mapaIPs = new HashMap();
        synchronized(LockMatrizEDados){
            mapaIPs.put(say(), 0);
            NComputadoresConhecidos++;
            for(int x=0;x<IPsVizinhos.length;x++){
                mapaIPs.put(IPsVizinhos[x], NComputadoresConhecidos);
                NComputadoresConhecidos++;
            }
            System.out.println("PRINCIPAL: formou o mapa: ");
            for (Map.Entry<String, Integer> entrySet : mapaIPs.entrySet()) {
                String key = entrySet.getKey();
                int value = entrySet.getValue();
                System.out.println("PRINCIPAL: "+key+" val = "+value);
            }
            for (int y = 0; y <Ncomputadores; y++) {
                for (int x = 0; x < Ncomputadores; x++) {
                    pesos[x][y][0]=Long.MAX_VALUE;
                    pesos[x][y][1]=0;
                }
            }
        }
    }
        
    //Gambiarra copiada diretamente do stack overflow para obtensão de IP
     public static  String say(){    
        Enumeration e;
        String retorno="";
        int cont=0;
        try {
            e = NetworkInterface.getNetworkInterfaces();
            while(e.hasMoreElements())
            {
                NetworkInterface n = (NetworkInterface) e.nextElement();
                Enumeration ee = n.getInetAddresses();
                while (ee.hasMoreElements())
                {
                    InetAddress i = (InetAddress) ee.nextElement();
                    cont++;
                    if(cont==2){
                        return i.getHostAddress();
                        
                    }
                }
                    
            }
        } catch (SocketException ex) {
            Logger.getLogger(Principal.class.getName()).log(Level.SEVERE, null, ex);
        }
        return retorno;
      }    

}

package conexaosegura;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ThreadHandlePeer extends Thread{
    private BufferedReader input;
    
    @Override
    public void run(){
        while(true){
            try {
                String recebido = input.readLine();
                System.out.println("THREADHANDLEPEER: Recebeu mensagem: "+recebido);
                if(recebido.startsWith("oiEuSou")){
                    int idPeer = Integer.parseInt(recebido.split(" ")[1]);
                    System.out.println("THREADHANDLEPEER: Abrindo conexao com "+idPeer);
                    String strChaveDesencriptada =Encriptador.cesar(recebido.split(" ")[2], DistribuidorDeChaves.chavesClientes[Cliente.idCliente]);
                    int chaveDesencriptada = Integer.parseInt(strChaveDesencriptada);
                    Cliente.arrayChaves[idPeer]=chaveDesencriptada;
                    
                }
            } catch (IOException ex) {
                Logger.getLogger(ThreadHandlePeer.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public void ThreadHandlePeer(Socket conexao) throws IOException{
        input =new BufferedReader(new InputStreamReader(conexao.getInputStream()));
    }
}

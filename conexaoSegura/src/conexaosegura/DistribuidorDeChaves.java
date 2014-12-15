package conexaosegura;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

public class DistribuidorDeChaves extends Thread{
    public static int[] chavesClientes;
    public void run(){
        try {
            ServerSocket servidor = new ServerSocket(ConexaoSegura.PORTADISTRIBUIDOR);
            while(true){
                Socket conexao = servidor.accept();
                new HandleClientThread(conexao).start();
            }
        } catch (IOException ex) {
            System.out.println("deu treta");
        }
    }
    public DistribuidorDeChaves(){
        chavesClientes = new int[8];
        
        Random geradorDeRandom = new Random();
        chavesClientes[0] = 9;
        chavesClientes[1] = 2;
        chavesClientes[2] = 6;
        chavesClientes[3] = 9;
        chavesClientes[4] = 12;
        chavesClientes[5] = 1;
        chavesClientes[6] = 6;
        chavesClientes[7] = 8;   
    }
}

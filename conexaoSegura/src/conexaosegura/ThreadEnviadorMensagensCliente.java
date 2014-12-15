
package conexaosegura;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class ThreadEnviadorMensagensCliente extends Thread {
    private final int idCliente;
    private Scanner scanner;
    @Override
    public void run(){
        while(true){
            System.out.println("Voce esta conectado a :");
            
            System.out.println("digite mensagem X e um texto ou conecta X IP");
            String entrada = scanner.nextLine();
            if(entrada.startsWith("conecta")){
                try {
                    abreConexao(Integer.parseInt(entrada.split(" ")[1]),entrada.split(" ")[2]);
                } catch (IOException ex) {

                }
            }
        }
    }
   
    private void abreConexao(int idAlvo,String ipAlvo) throws IOException{
        Socket conexaoDistribuidor = new Socket("localhost",ConexaoSegura.PORTADISTRIBUIDOR);
        BufferedReader inputDist =new BufferedReader(new InputStreamReader(conexaoDistribuidor.getInputStream()));
        PrintWriter outDist =new PrintWriter(conexaoDistribuidor.getOutputStream(), true);
        
        outDist.println("novaConexao "+idCliente+" "+idAlvo);
        
        String recebido = inputDist.readLine();
        System.out.println("recebeu mensagem "+recebido);
        String chaveEncriptada = recebido.split(" ")[1];
        String chaveEncriptadaAlvo = recebido.split(" ")[2];
        String chaveDesencriptada = Encriptador.cesar(chaveEncriptada,-DistribuidorDeChaves.chavesClientes[idCliente]);
        int chaveConexao = Integer.parseInt(chaveDesencriptada);
        
        Cliente.arraySockets[idAlvo] = new Socket(ipAlvo,ConexaoSegura.PORTACONEXOES);
        Cliente.arrayChaves[idAlvo]=chaveConexao;
        
        Cliente.mandaMensagemPeer("oiEuSou "+idCliente+" "+chaveEncriptadaAlvo, idAlvo);
    }
    
    public ThreadEnviadorMensagensCliente(int idCliente){
        this.idCliente= idCliente;
        scanner = new Scanner(System.in);
    }
}

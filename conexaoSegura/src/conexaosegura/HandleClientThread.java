
package conexaosegura;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

public class HandleClientThread extends Thread {
    Socket conexao;
    BufferedReader input;
    PrintWriter out;
    Random geradorDeRandom;
    public void run(){
        try {
            String recebido = input.readLine();
            System.out.println("Recebeu mensagem :"+recebido);
            if(recebido.startsWith("novaConexao")){
                int cliente1,cliente2;
                cliente1 = Integer.parseInt(recebido.split(" ")[1]);
                cliente2 = Integer.parseInt(recebido.split(" ")[2]);
                mandaMensagemNovaConexao(cliente1, cliente2);
            }
        } catch (IOException ex) {
            Logger.getLogger(HandleClientThread.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    void mandaMensagemNovaConexao(int cliente1,int cliente2){
        int chave = geradorDeRandom.nextInt(15);
        String mensagem = "suasChavesSao ";
        //encripta a chave que a vai usar para b com a chave de a com o distribuidor
        mensagem = mensagem+Encriptador.cesar(String.valueOf(chave),DistribuidorDeChaves.chavesClientes[cliente1]);
        //encripta a chave que B vai usar para A com a chave de B com o distribuidor (a chave é a mesma)
        mensagem = mensagem+" "+Encriptador.cesar(String.valueOf(chave),DistribuidorDeChaves.chavesClientes[cliente2]);
        System.out.println("Montou a mensagem encriptada :"+ mensagem);
        out.println(mensagem);   
    }
    
    
    public HandleClientThread(Socket conexao) throws IOException{
        this.conexao = conexao;
        input =new BufferedReader(new InputStreamReader(conexao.getInputStream()));
        out =new PrintWriter(conexao.getOutputStream(), true);
        
        geradorDeRandom = new Random();
    }
}



import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class HandleClient extends Thread{
    Socket conexao;
    private PrintWriter out;
    private BufferedReader input;
    
    public HandleClient(Socket conexao){
        try {
            this.conexao= conexao;
            input =new BufferedReader(new InputStreamReader(conexao.getInputStream()));
            out =new PrintWriter(conexao.getOutputStream(), true);
        } catch (IOException ex) {
            Logger.getLogger(HandleClient.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    public void run(){
        while(true){
            try {
                String mensagem = input.readLine();
                String comando = mensagem.split(":")[0];
                if(comando.equals("download")){
                    String nome = mensagem.split(":")[1];
                    synchronized(ServidorArquivos.arquivos){
                        boolean falha=true;
                        for(int x=0;x<ServidorArquivos.arquivos.size();x++){
                            if(ServidorArquivos.arquivos.get(x).getName().equals(nome)){
                                UtilArquivo.enviaArquivo(conexao.getOutputStream(), ServidorArquivos.arquivos.get(x));
                                falha=false;
                                break;
                            }
                        }
                        if(falha){
                            System.out.println("HANDLECLIENT: Erro:não existe um arquivo com o nome "+nome);
                        }
                    }
                }else{
                    if(comando.equals("upload")){
                        File arquivo = UtilArquivo.recebeArquivo(conexao,mensagem.split(":")[1],mensagem.split(":")[2]);
                        synchronized(ServidorArquivos.arquivos){
                            ServidorArquivos.arquivos.add(arquivo);
                        }
                        System.out.println("servidor recebeu o arquivo "+arquivo.getName());
                    }
                }
            } catch (IOException ex) {
                Logger.getLogger(HandleClient.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
}



import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;

public class Cliente {
        
    public static void main(String[] args) {
        try {
            Scanner scanner = new Scanner(System.in);
            Socket conexao=new Socket("localhost",50000);
            BufferedReader input =new BufferedReader(new InputStreamReader(conexao.getInputStream()));
            PrintWriter out =new PrintWriter(conexao.getOutputStream(), true);
            
            while(true){
                System.out.println("digite upload ou download");
                String acao = scanner.nextLine();
                if(acao.equals("download")){
                    System.out.println("Digite o nome do arquivo que quer receber");
                    String nome = scanner.nextLine();
                    out.println("download:"+nome);
                    File arquivo = UtilArquivo.recebeArquivo(conexao);
                }else{
                    if(acao.equals("upload")){
                        System.out.println("Escolha o arquivo");
                        JFileChooser escolhedor = new JFileChooser();
                        if(escolhedor.showOpenDialog(null)==JFileChooser.APPROVE_OPTION){
                            out.println("upload");
                            UtilArquivo.enviaArquivo(conexao.getOutputStream(), escolhedor.getSelectedFile());
                        }else{
                            System.out.println("erro na escolha do arquivo");
                        }
                    }
                }
            }
        } catch (UnknownHostException ex) {
            Logger.getLogger(Cliente.class.getName()).log(Level.SEVERE, null, ex);
        } catch (IOException ex) {
            Logger.getLogger(Cliente.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
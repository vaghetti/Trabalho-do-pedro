
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;

public class UtilArquivo {

    public static void enviaArquivo(PrintWriter out, File arquivo) {
        try {
            out.println(arquivo.getName());
            Scanner arq = new Scanner(new FileReader(arquivo));

            while (arq.hasNextLine()) {
                out.println(arq.nextLine());
            }
            out.println("fim_do_arquivo");
            
            arq.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

	public static void enviaArquivoServidor(PrintWriter out, File arquivo) {
        try {
            Scanner arq = new Scanner(new FileReader(arquivo));

            while (arq.hasNextLine()) {
                out.println(arq.nextLine());
            }
            out.println("fim_do_arquivo");
            
            arq.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static String recebeArquivo(BufferedReader input,boolean servidor) {
        try {
            String nome = input.readLine();
            if(servidor){
                ServidorArquivos.nomesArquivos.add(nome);
            }
            System.out.println("Nome do arquivo = "+nome);
            PrintWriter gravarArq = new PrintWriter(new FileWriter(nome),true);

            String buf = input.readLine(); 

            while(!buf.equals("fim_do_arquivo")){
                    gravarArq.println(buf);
                    buf = input.readLine(); 
            }
            gravarArq.close();
            return nome;
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}

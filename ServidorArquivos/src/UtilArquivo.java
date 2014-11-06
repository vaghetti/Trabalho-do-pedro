

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class UtilArquivo {
    
    public static boolean enviaArquivo(OutputStream out,File arquivo){
        try {
            
            FileInputStream in = new FileInputStream(arquivo);
            
            OutputStreamWriter osw = new OutputStreamWriter(out);  
            BufferedWriter writer = new BufferedWriter(osw);  
            writer.write(arquivo.getName() + "\n");  
            writer.flush();  
            int tamanho = 4096; // buffer de 4KB    
            byte[] buffer = new byte[tamanho];    
            int lidos = -1;    
            while ((lidos = in.read(buffer, 0, tamanho)) != -1) {    
                out.write(buffer, 0, lidos);    
            } 
            return true;
        } catch (IOException ex) {
            Logger.getLogger(UtilArquivo.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }
    public static File recebeArquivo(Socket conexao){
        try {
            
            InputStream in = conexao.getInputStream();  
            InputStreamReader isr = new InputStreamReader(in);  
            BufferedReader reader = new BufferedReader(isr);  
            String fName = reader.readLine();  
            System.out.println("criando arquivo "+fName);  
            File f1 = new File(fName);  
            FileOutputStream out = new FileOutputStream(f1);  
  
            int tamanho = 4096; // buffer de 4KB    
            byte[] buffer = new byte[tamanho];    
            int lidos = -1;    
            while ((lidos = in.read(buffer, 0, tamanho)) != -1) {    
                System.out.println(lidos);  
                out.write(buffer, 0, lidos);    
            }    
            out.flush();
            return f1;
        } catch (IOException e) {
            return null;
        }  
    }
}

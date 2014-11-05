

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class UtilArquivo {
    
    public static boolean enviaArquivo(OutputStream out,File arquivo){
        try {
            byte [] mybytearray  = new byte [(int)arquivo.length()];
            FileInputStream fis = new FileInputStream(arquivo);
            BufferedInputStream bis = new BufferedInputStream(fis);
            bis.read(mybytearray,0,mybytearray.length);
            System.out.println("Sending " + arquivo + "(" + mybytearray.length + " bytes)");
            out.write(mybytearray, 0, mybytearray.length);
            System.out.println("Done.");
            return true;
        } catch (IOException ex) {
            Logger.getLogger(UtilArquivo.class.getName()).log(Level.SEVERE, null, ex);
            return false;
        }
    }
    public static File recebeArquivo(Socket conexao,String nome,String bytes){
        try {
            byte[] mybytearray = bytes.getBytes();
            InputStream is = conexao.getInputStream();
            FileOutputStream fos = new FileOutputStream(nome);
            BufferedOutputStream bos = new BufferedOutputStream(fos);
            bos.write(mybytearray,0,mybytearray.length);
            bos.close();
            return new File(nome);
            
        } catch (IOException ex) {
            Logger.getLogger(UtilArquivo.class.getName()).log(Level.SEVERE, null, ex);
            return null;
        }
    }
}

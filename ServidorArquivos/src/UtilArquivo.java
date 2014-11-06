

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

public class UtilArquivo {
    
    public static void enviaArquivo(BufferedWriter out,File arquivo) {
      try {
    	  BufferedReader leitor = new BufferedReader( new FileReader(arquivo));
    	  char[] buffer = new char[100000000];
    	  leitor.read(buffer);
    	  out.write(buffer);
		} catch (IOException e) {
			e.printStackTrace();
		}
      
    }
    public static File recebeArquivo(BufferedReader input){
    	try {
			File retorno = new File(input.readLine());
			BufferedWriter arq = new BufferedWriter(new FileWriter(retorno));
			char[] charArray= new char[10000000];
			input.read(charArray);
			arq.write(charArray);
			return retorno;
			
		} catch (IOException e) {
			e.printStackTrace();
		}
    	return null;
    }
}

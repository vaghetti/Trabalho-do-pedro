
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;

public class HandleClient extends Thread{
	Socket conexao;
	private BufferedReader input;
    private BufferedWriter out ;
    
    public HandleClient(Socket conexao){
    	try {
    		this.conexao=conexao;
    		input =new BufferedReader(new InputStreamReader(conexao.getInputStream()));
			out =new BufferedWriter(new OutputStreamWriter(conexao.getOutputStream()));
		} catch (IOException e) {
			e.printStackTrace();
		}
    }
    public void run(){
    	try {
			String mensagem = input.readLine();
			if(mensagem.equals("download")){
				String nome = input.readLine();
				synchronized (ServidorArquivos.arquivos) {
					int x;
					for(x=0;x<ServidorArquivos.arquivos.size();x++){
						if(ServidorArquivos.arquivos.get(x).getName().equals(nome)){
							UtilArquivo.enviaArquivo(out, ServidorArquivos.arquivos.get(x));
							break;
						}
					}
					if(x==ServidorArquivos.arquivos.size()){
						System.out.println("Arquivo "+nome +" não encontrado");
					}
				}
			}else{
				if(mensagem.equals("upload")){
					UtilArquivo.recebeArquivo(input);
				}else{
					System.out.println("Recebeu mensagem desconhecida "+mensagem);
				}
			}
			conexao.close();
		} catch (IOException e) {
			e.printStackTrace();
		}    	
    }
}

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;


//Thread que atualiza o ping de um vizinho especifico
public class AtualizaPing extends Thread {
	private Socket vizinho;
	private BufferedReader input;
	private PrintWriter out ;
	private long tempo;
	public void run(){
		try {
			out.println("HELLO");
			tempo=System.currentTimeMillis();
			input.readLine(); //recebe o hello de volta
			tempo=System.currentTimeMillis()-tempo;
			vizinho.close();
			
		} catch (IOException | NumberFormatException e) {
			e.printStackTrace();
		}
	}
	
	
	public AtualizaPing(String IPvizinho) throws IOException{
		vizinho=new Socket(IPvizinho,Principal.portaIP);
		input =new BufferedReader(new InputStreamReader(vizinho.getInputStream()));
        out =new PrintWriter(vizinho.getOutputStream(), true);
	}
	
	
	public long getTempo() throws InterruptedException {
		while(this.isAlive()){
			Thread.sleep(100);
		}
		return tempo;
	}
}

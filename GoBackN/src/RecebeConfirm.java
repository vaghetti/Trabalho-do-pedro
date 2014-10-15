import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketException;




public class RecebeConfirm extends Thread {
	
	 private Socket socket;
     public int[] confirmados;
     
	public void run(){
		try{		   
	        BufferedReader input =new BufferedReader(new InputStreamReader(socket.getInputStream()));
	        //PrintWriter out =new PrintWriter(socket.getOutputStream(), true);
	        while (true) {
	        	try{
	        		String answer = input.readLine();
	        		//out.println(answer);
	        		System.out.println("recebeu confimação de "+answer);
	        		if(!answer.equals("fim")){
	        			synchronized(confirmados){
	        				confirmados[Integer.parseInt(answer)]=1;
	        			}
	        		}else{
	        			break;
	        		}
	        	}catch(SocketException e){
	        		socket.close();
	            	System.out.println("fechando a thread de receber confirmações");
	            	break;
	        	}
	        }
		}catch(IOException e){
			e.printStackTrace();
		}
    }
	
	public RecebeConfirm(Socket socket){
		this.socket= socket;
		confirmados = new int[Enviador.QUADROS];
		for(int x=0;x<Enviador.QUADROS;x++){
			//System.out.println("indexou posicao "+x);
			confirmados[x]=0;
		}
	}
}
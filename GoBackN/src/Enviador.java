import java.io.IOException;
import java.net.Socket;
import java.util.Vector;

import javax.swing.JOptionPane;


public class Enviador {
	
	public static final int QUADROS=64;
	
    public static void main(String[] args) throws IOException, InterruptedException {
    	String IP = JOptionPane.showInputDialog("Digite o ip que deseja se conectar");
    	Socket socket = new Socket(IP,9090);
    	int atual = 0;
    	int tamJanela=8;
    	// Cria e abre o socket do recebedor
    	RecebeConfirm recebedor = new RecebeConfirm(socket);
    	recebedor.start();
    	
    	//da um mutex no recebedor
    	synchronized(recebedor){};
    	
    	//Cria um vectro de enviadores
    	Vector<EnviaDado> enviadores = new Vector<EnviaDado>();
    	
    	//enquando não confirmou o ultimo quadro
    	while(recebedor.confirmados[QUADROS-1]!=1){
    		//envia a janela
	    	for(int x=atual;x<QUADROS && x<atual+tamJanela;x++){
	    		enviadores.add(new EnviaDado(socket,x));  //cria a thread de envio
	    		System.out.println("enviou o quadro "+x);
	    		//da um mutax no vetor de confirmados
	    		synchronized(recebedor.confirmados){
	    			recebedor.confirmados[x]=0;
	    		}
	    		enviadores.get(enviadores.size()-1).start(); //inicia a thread
	    	}
	    	//espera todas as threads terminarem
	    	for(int x=0;x<enviadores.size();x++){
	    		while(enviadores.get(x).getState()!=Thread.State.TERMINATED);
	    	}
	    	// limpa o vector de threads
	    	enviadores.clear();
	    	Thread.sleep(100);
	    	//desliza a janela
	    	int fim = atual+tamJanela;
	    	
	    	//verifica os quadros confrmados
	    	for(int x=atual;x<QUADROS && x<fim;x++){
	    		synchronized(recebedor.confirmados){
	    			if(recebedor.confirmados[x]==1){
	    				System.out.println("tudo ok com o quadro "+x);
		    			atual++;
		    		}else{
		    			System.out.println("erro no envio do quadro "+x+" reenviando a partir dele");
		    			if(tamJanela>4){
		    				tamJanela=tamJanela/2;
		    			}
		    			break;
		    		}
    			}
	    	}
    	}
    	new EnviaDado(socket,-1).start(); //envia o sinal de final para o recebedor
    	
    }
    
}

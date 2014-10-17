import java.io.IOException;
import java.net.Socket;
import java.util.Vector;

import javax.swing.JOptionPane;


public class Enviador {
	
	public static final int QUADROS=16;
	public static int numeroDeConfirmados=0;
	
    public static void main(String[] args) throws IOException, InterruptedException {
    	System.out.println("argumentos recebidos:");
    	for(int x=0;x<args.length;x++){
    		System.out.println(args[x]);
    	}
    	String IP = JOptionPane.showInputDialog("Digite o ip que deseja se conectar");
    	Socket socket = new Socket(IP,9090);
    	int atual = 0;
    	int tamJanela=8;
    	RecebeConfirm recebedor = new RecebeConfirm(socket);
    	recebedor.start();
    	synchronized(recebedor){};
    	Vector<EnviaDado> enviadores = new Vector<EnviaDado>();
    	
    	while(numeroDeConfirmados!=QUADROS){
	    	for(int x=atual;x<QUADROS && x<atual+tamJanela;x++){
	    		synchronized(recebedor.confirmados){
	    			if(recebedor.confirmados[x]==0){  //so reenvia os dados que nao foram confirmados
	    				enviadores.add(new EnviaDado(socket,x));  //cria a thread de envio
	    	    		System.out.println("enviou o quadro "+x);
	    	    		enviadores.get(enviadores.size()-1).start(); //inicia a thread
	    			}
	    			if(recebedor.confirmados[x]==3){
	    				enviadores.add(new EnviaDado(socket,x));  //cria a thread de envio
	    	    		System.out.println("reenviou quadro "+x);
	    	    		enviadores.get(enviadores.size()-1).start(); //inicia a thread
	    			}
	    		}
	    		
	    	}
	    	for(int x=0;x<enviadores.size();x++){
	    		while(enviadores.get(x).getState()!=Thread.State.TERMINATED){ //espera que todos enviadores terminem
	    		}
	    	}
	    	enviadores.clear();
	    	Thread.sleep(100);  //deixa que a thread de receber confirmacoes funcione
	    	int fim = atual+tamJanela;
	    	boolean deuErro=false;
	    	for(int x=atual;x<QUADROS && x<fim;x++){
	    		synchronized(recebedor.confirmados){
	    			if(recebedor.confirmados[x]==1){ 
	    				System.out.println("tudo ok com o quadro "+x);
	    				recebedor.confirmados[x]=2;
	    				if(!deuErro)
	    					atual++;
		    			numeroDeConfirmados++;
		    		}else{
		    			if(recebedor.confirmados[x]!=2){ //so para nao dar a mensagem de erro caso ele ja tenha sido confirmado (confirmados[x]==2)
			    			System.out.println("erro no envio do quadro "+x);
			    			recebedor.confirmados[x]=3;
			    			if(tamJanela>4 && !deuErro){  // !deuErro garante que o tamanho da janela nao seja reduzido duas vezes
			    				tamJanela=tamJanela/2;
			    				deuErro=true;
			    			}
		    			}
		    		}
    			}
	    	}
	    	if(!deuErro){
	    		tamJanela=tamJanela*2; //aumenta a janela se tudo foi enviado sem problemas
	    	}
    	}
    	new EnviaDado(socket,-1).start(); //envia o sinal de final para o recebedor
    	
    }
    
}
import java.io.IOException;

import javax.swing.JOptionPane;


public class Principal {
	static int[][] pesos;
	public static int portaIP = 9090;
	
	public static void main(String[] args) {
		try {
			RespondedorPing respondedorPing= new RespondedorPing();
			respondedorPing.start();
			Pingador pingador = new Pingador(args);  //os ips dos vizinhos sao recebidos como argumentos
			if(JOptionPane.showConfirmDialog(null, "iniciar operação?") == JOptionPane.CANCEL_OPTION){
				return;
			}
			pingador.start();
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}

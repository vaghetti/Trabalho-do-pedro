package conexaosegura;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

public class ThreadRecebedorConexoesCliente extends Thread {
    ServerSocket recebedor;
    @Override
    public void run(){
        while(true){
            try {
                Socket conexao = recebedor.accept();
                new ThreadHandlePeer(conexao,)
            } catch (IOException ex) {
                Logger.getLogger(ThreadRecebedorConexoesCliente.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public ThreadRecebedorConexoesCliente(ServerSocket recebedor){
        this.recebedor= recebedor;
    }
}

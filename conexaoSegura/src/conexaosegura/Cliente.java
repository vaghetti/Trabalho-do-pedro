package conexaosegura;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class Cliente {
    public static Socket conexaoClientes;
    public static Socket conexaoDistribuidorChaves;
    public static ServerSocket recebedorConexaoClientes;
    public static int idCliente;
    public ThreadEnviadorMensagensCliente criador;
    public ThreadRecebedorConexoesCliente recebedor;
    public static int[] arrayChaves;
    public static Socket[] arraySockets;
    
    
    private Cliente(int idCliente) throws IOException{
        this.idCliente= idCliente;
        recebedorConexaoClientes = new ServerSocket(ConexaoSegura.PORTACONEXOES);
        criador = new  ThreadEnviadorMensagensCliente(idCliente);
        criador.start();
        recebedor = new ThreadRecebedorConexoesCliente(recebedorConexaoClientes);
        recebedor.start();
        
        arrayChaves = new int[8];
        for(int x=0;x<8;x++){
            arrayChaves[x]=-1;
        }
    }
    
    public static void mandaMensagemPeer(String mensagem,int idPeer) throws IOException{
        PrintWriter out =new PrintWriter(arraySockets[idPeer].getOutputStream(), true);
        out.println(mensagem);
    }
}

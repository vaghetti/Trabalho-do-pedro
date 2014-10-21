
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

/*
    DEFINIÇÃO DO PROTOCOLO DE TRANSFERENCIA DE DADOS
    numeroDeComputadores@IP0:IP1:IP2:IP3/peso 0-0,tempo0-0:peso0-1,tempo0-1:peso0-2,tempo0-2;peso 1-0,tempo1-0:peso1-1,tempo1-1:peso1-2,tempo1-2;peso 2-0,tempo2-0:peso2-1,tempo2-1:peso2-2,tempo2-2;
*/
public class RecebedorDados extends Thread {
    private ServerSocket servidor;
    private BufferedReader input;
    private Socket conexao;
    public void run(){
        try {
            while(true){
                conexao = servidor.accept();
                input =new BufferedReader(new InputStreamReader(conexao.getInputStream()));
                atualizaDados(input.readLine());
                conexao.close();
            }
        } catch (IOException ex) {
            Logger.getLogger(RecebedorDados.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    public RecebedorDados() throws IOException{
        servidor = new ServerSocket(Principal.portaDados);
    }
    private void atualizaDados(String dados){
        synchronized(Principal.LockMatrizEDados){
            String[] aux = dados.split("@");  
            int numeroDeIps = Integer.parseInt(aux[0]);
            
           // System.out.println("RECEBEDORDADOS: parser recebe string de dados :"+dados);
            aux = aux[1].split("/");
            String vetorIPs[] = aux[0].split(":"); 
            for (String ip : vetorIPs) {
                //System.out.println("RECEBEDORDADOS: parser leu ip :"+ip);
                if(!Principal.mapaIPs.containsKey(ip)){
                    Principal.mapaIPs.put(ip, Principal.NComputadoresConhecidos);
                    Principal.NComputadoresConhecidos++;
                }
            }
            aux = aux[1].split(";");
            String pares[],par[];
            long tempo,peso;
            for (int y = 0; y < numeroDeIps; y++) {
                pares = aux[y].split(":");
                for(int x=0;x<numeroDeIps;x++){
                    par = pares[x].split(",");
                    peso = Long.parseLong(par[0]);
                    tempo = Long.parseLong(par[1]);
                    //System.out.println("RECEBEDORDADOS: parser leu tempo :"+tempo+" e peso "+ peso+" de "+vetorIPs[x]+ " ate "+vetorIPs[y]);
                    if(Principal.pesos[Principal.mapaIPs.get(vetorIPs[x])][Principal.mapaIPs.get(vetorIPs[y])][1] > tempo && tempo!=Long.MAX_VALUE){
                       Principal.pesos[Principal.mapaIPs.get(vetorIPs[x])][Principal.mapaIPs.get(vetorIPs[y])][1] = tempo;
                       Principal.pesos[Principal.mapaIPs.get(vetorIPs[x])][Principal.mapaIPs.get(vetorIPs[y])][0] = peso;
                       System.out.println("RECEBEDORDADOS: atualizou o peso de "+vetorIPs[x]+" ate "+vetorIPs[y]+" para "+peso);
                    }
                }
            }
        }
    }
}
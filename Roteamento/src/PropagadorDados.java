import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

/*
    DEFINIÇÃO DO PROTOCOLO DE TRANSFERENCIA DE DADOS
    numeroDeComputadores@IP0:IP1:IP2:IP3/peso 0-0,tempo0-0:peso0-1,tempo0-1:peso0-2,tempo0-2;peso 1-0,tempo1-0:peso1-1,tempo1-1:peso1-2,tempo1-2;peso 2-0,tempo2-0:peso2-1,tempo2-1:peso2-2,tempo2-2;
*/
public class PropagadorDados extends Thread {
    private String[] IPsVizinhos;
    private static final int espera = 6;
    private PrintWriter out;
    private Socket conexao;
    public void run(){
        try{
            while(true){
                String dados = geraStringDados();
                System.out.println("PROPAGADOR DADOS: Gerou string de dados para compartilhar : ");
                System.out.println("PROPAGADOR DADOS: "+dados);
                for(int x=0;x<IPsVizinhos.length;x++){
                    conexao = new Socket(IPsVizinhos[x],Principal.portaDados);
                    out =new PrintWriter(conexao.getOutputStream(), true);
                    out.println(dados);
                    conexao.close();
                }
                for(int y=0;y<Principal.NComputadoresConhecidos;y++){
                    for (int x = 0; x < Principal.NComputadoresConhecidos; x++) {
                        if(Principal.pesos[x][y][0]==Long.MAX_VALUE){
                            System.out.print("MAX ");
                        }else{
                            System.out.print(Principal.pesos[x][y][0]+" ");
                        }
                    }
                    System.out.println("");
                }
                Thread.sleep(espera*1000);
            }
        }catch(InterruptedException | IOException   e){
            e.printStackTrace();
        }
    }
    public PropagadorDados(String[] IPsVizinhos){
        this.IPsVizinhos = IPsVizinhos;
    }
    
    private String geraStringDados(){
        String dados= new String("");
        synchronized(Principal.LockMatrizEDados){
            dados+=Principal.NComputadoresConhecidos+"@"; //declara os ips associados as posicoes

            for (Map.Entry<String, Integer> entrySet : Principal.mapaIPs.entrySet()) {
                String IP = entrySet.getKey();
                int index = entrySet.getValue();
                dados+=IP+":";
            }
            dados+="/";

            for(int y=0;y<Principal.NComputadoresConhecidos;y++){
                for(int x=0;x<Principal.NComputadoresConhecidos;x++){
                    if(x!=0)
                        dados+=":";
                    if(x==y){
                        dados+="0"+","+Principal.pesos[x][y][1];  //sempre envia peso 0 de um nodo para si mesmo
                    }else{
                        if(Principal.pesos[x][y][0]==Long.MAX_VALUE){
                            dados+="MAX"+","+Principal.pesos[x][y][1];
                        }else{
                            dados+=Principal.pesos[x][y][0]+","+Principal.pesos[x][y][1];
                        }
                    }
                }
                dados+=";";
            }
        }
        return dados;
    }
}
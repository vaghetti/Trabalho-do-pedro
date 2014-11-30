
import java.util.Map;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author aluno
 */
public class Dijkstrador extends Thread{
    private static final int intervalo=10;
    public static int anterior[];
    private static long best[];
    
    public void run(){
        try {
            while(true){
                Thread.sleep(intervalo*1000);
                synchronized(Principal.LockMatrizEDados){
                    if(Principal.NComputadoresConhecidos < Principal.Ncomputadores){
                        System.out.println("DIJKSTRADOR: dijkstrador nao fez nada, tabela ainda nao esta completa nComputadoresConhecidos ="+Principal.NComputadoresConhecidos+" N computadores = "+Principal.Ncomputadores);
                        continue;  //so faz o dijktra se tiver todas informaçoes
                    }
                }
                System.out.println("DIJKSTRADOR: mapa de ips");
                for(Map.Entry<String, Integer> entry : Principal.mapaIPs.entrySet()) {
                    String key = entry.getKey();
                    int value = entry.getValue();
                    System.out.println("DIJKSTRADOR: "+key+" val = "+value);
                }
                System.out.println("DIJKSTRADOR: calculo dijstra, lista de caminhos");
                dijkstra();
                for(int x=0;x<anterior.length;x++){
                    System.out.print(x+":"+anterior[x]+" ");
                }
                System.out.println("\nDIJKSTRADOR: lista de pesos calculados");
                for(int x=0;x<anterior.length;x++){
                    System.out.print(x+":"+best[x]+" ");
                }
                System.out.println("");
            }
        }catch (InterruptedException ex) {
            ex.printStackTrace();
        }
    }
    
    private static void dijkstra() // Added a start point.
    {
        synchronized(Principal.LockMatrizEDados){
            // Dijkstra's Algorithm
            best = new long[Principal.Ncomputadores];
            boolean[] visited = new boolean[Principal.Ncomputadores];
            anterior = new int[Principal.Ncomputadores];
            
            for (int i = 0; i < Principal.Ncomputadores; i++)
            {
                best[i] = Principal.MAX;
                visited[i] = false;
            }

            best[0] = 0; // Changed the 0 to variable start.
            anterior[0]=-1;

            while(true)
            {
                int v = -1;
                for (int j = 0; j < Principal.Ncomputadores; j++)
                {
                    if(!visited[j] && ( v<0 || best[j]<best[v])) 
                        v=j;
                }
                if(v<0 || best[v]==Principal.MAX)
                    break;
                visited[v]=true;
                for(int i=0;i<Principal.Ncomputadores;i++){
                    long w = Principal.pesos[v][i][0];
                    
                    if(best[i]>best[v]+w){
                        best[i]=best[v]+w;
                        anterior[i]=v;
                    }
                }
                
                
            }
        }
    }
    
}

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
                System.out.println("DIJKSTRADOR: calculo dijstra, lista de caminhos");
                dijkstra();
                for(int x=0;x<anterior.length;x++){
                    System.out.print(x+":"+anterior[x]+" ");
                }
                System.out.println("DIJKSTRADOR: lista de pesos calculados");
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
            int max = 10000; // Infinity equivalent.
            for (int i = 0; i < Principal.Ncomputadores; i++)
            {
                best[i] = max;
                visited[i] = false;
            }

            best[0] = 0; // Changed the 0 to variable start.

            for(int i = 0; i < Principal.Ncomputadores; i++)
            {
                long min = max;
                int currentNode = 0;
                for (int j = 0; j < Principal.Ncomputadores; j++)
                {
                    if (!visited[j] && best[j] < min)
                    {
                        currentNode = j;
                        min = best[j];
                    }
                }
                visited[currentNode] = true;
                for (int j = 0; j < Principal.Ncomputadores; j++)
                {
                    if (Principal.pesos[currentNode][j][0] < max && best[currentNode] +   Principal.pesos[currentNode][j][0] < best[j])
                    {
                        best[j] = best[currentNode] + Principal.pesos[currentNode][j][0];
                        anterior[currentNode]  = j;
                    }
                }
            }
        }
    }
    
}


package conexaosegura;

public class ConexaoSegura {
    public static int PORTADISTRIBUIDOR = 50000;
    public static int PORTACONEXOES = 50001;
    public static void main(String[] args) {
        // TODO code application logic here
        if(args[0].equals("servidor")){
            new DistribuidorDeChaves().start();
        }
        
        if(args[0].equals("cliente")){
            int idCliente = Integer.parseInt(args[1]);
            while(true){
                
            }
        }
        
    }
    
}

package conexaosegura;

public class Encriptador {
    public static String cesar(String entrada,int n){
        StringBuilder nova = new StringBuilder(entrada);
        for(int x=0;x<entrada.length();x++){
            nova.setCharAt(x, (char) (nova.charAt(x)+n));
        }
        return nova.toString();
    }  
}

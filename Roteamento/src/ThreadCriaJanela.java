import javax.swing.JOptionPane;

public class ThreadCriaJanela extends Thread{
    String mensagem;
    public void run(){
        JOptionPane.showMessageDialog(null, mensagem);
    }
    public ThreadCriaJanela(String mensagem){
        this.mensagem=mensagem;
    }
    
}


import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;

public class HandleClient extends Thread {

    Socket conexao;
    private BufferedReader input;
    private PrintWriter out;

    public HandleClient(Socket conexao) {
        try {
            this.conexao = conexao;
            input = new BufferedReader(new InputStreamReader(conexao.getInputStream()));
             out = new PrintWriter(conexao.getOutputStream(),true);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void run() {
        try {
            String mensagem = input.readLine();
            if (mensagem.equals("download")) {
                String nome = input.readLine();
                System.out.println("procurando arquivo com o nome "+nome);
                synchronized (ServidorArquivos.nomesArquivos) {
                    int x;
                    for (x = 0; x < ServidorArquivos.nomesArquivos.size(); x++) {
                        if (ServidorArquivos.nomesArquivos.get(x).equals(nome)) {
                            File arquivo = new File(nome);
                            UtilArquivo.enviaArquivo(out, arquivo);
                            break;
                        }
                    }
                    if (x == ServidorArquivos.nomesArquivos.size()) {
                        System.out.println("Arquivo " + nome + " não encontrado");
                    }
                }
            } else {
                if (mensagem.equals("upload")) {
                    UtilArquivo.recebeArquivo(input,true);
                } else {
                    System.out.println("Recebeu mensagem desconhecida " + mensagem);
                }
            }
            conexao.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}

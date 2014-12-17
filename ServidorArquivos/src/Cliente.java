
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

import javax.swing.JFileChooser;

public class Cliente {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        while (true) {
            try {
                Socket conexao = new Socket("localhost", 50000);
                //192.168.208.24
                PrintWriter out = new PrintWriter(conexao.getOutputStream(), true);
                BufferedReader input = new BufferedReader(new InputStreamReader(conexao.getInputStream()));
                String comando = scanner.nextLine();
                if (comando.equals("upload")) {
                    JFileChooser escolhedor = new JFileChooser();
                    if (escolhedor.showOpenDialog(null) == JFileChooser.APPROVE_OPTION) {
                        out.println("upload");
                        out.println(escolhedor.getSelectedFile().getName());
                        UtilArquivo.enviaArquivo(out, escolhedor.getSelectedFile());
                        System.out.println("arquivo " + escolhedor.getSelectedFile().getName() + " enviado ");
                    }
                } else {
                    if (comando.equals("download")) {
                        System.out.println("Digite o nome do arquivo a ser baixado");
                        String nome = scanner.nextLine();
                        out.println("download");
                        out.println(nome);
                        UtilArquivo.recebeArquivo(input,false);
                    } else {
                        System.out.println("comando " + comando + " desconhecido");
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }
}
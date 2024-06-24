import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.net.Socket;

public class InterfaceFIFA extends JFrame {
    private JTextField idCampo, ageCampo, nomeJogadorCampo, nacionalidadeCampo, nomeClubeCampo;
    private JPanel campoResultado;
    private Socket socket;
    private PrintWriter out;
    private BufferedReader in;
    File fileIndice;
    File fileDados;

    public InterfaceFIFA() {
        setTitle("Leitor de Registros FIFA");
        setSize(800, 600);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        initComponents();
    }

    private void initComponents() {
        JPanel painel = new JPanel(new GridLayout(6, 2));
        idCampo = new JTextField();
        ageCampo = new JTextField();
        nomeJogadorCampo = new JTextField();
        nacionalidadeCampo = new JTextField();
        nomeClubeCampo = new JTextField();

        painel.add(new JLabel("id:"));
        painel.add(idCampo);
        painel.add(new JLabel("idade:"));
        painel.add(ageCampo);
        painel.add(new JLabel("nomeJogador:"));
        painel.add(nomeJogadorCampo);
        painel.add(new JLabel("nacionalidade:"));
        painel.add(nacionalidadeCampo);
        painel.add(new JLabel("nomeClube:"));
        painel.add(nomeClubeCampo);

        JButton buscaBotao = new JButton("Buscar");
        buscaBotao.addActionListener(new SearchButtonListener());
        painel.add(buscaBotao);

        JButton listarTodos = new JButton("Listar todos");
        listarTodos.addActionListener(new ListarTodosButtonListener());
        painel.add(listarTodos);

        campoResultado = new JPanel();
        campoResultado.setLayout(new BoxLayout(campoResultado, BoxLayout.Y_AXIS)); // Usar BoxLayout para alinhar os botões verticalmente
        JScrollPane scroll = new JScrollPane(campoResultado);
        scroll.setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);

        add(scroll, BorderLayout.CENTER); // Adicionar o scrollPane em vez de campoResultado diretamente|

        JMenuBar menuBar = new JMenuBar();
        JMenu fileMenu = new JMenu("Arquivo");
        JMenuItem loadItem1 = new JMenuItem("Abrir Arquivo de indice");
        loadItem1.addActionListener(new abrirArquivoIndice());
        fileMenu.add(loadItem1);

        JMenuItem loadItem2 = new JMenuItem("Abrir Arquivo de dados");
        loadItem2.addActionListener(new abrirArquivoDados());
        fileMenu.add(loadItem2);

        JMenuItem connectItem = new JMenuItem("Conectar a um Servidor");
        connectItem.addActionListener(new conectarServidor());
        fileMenu.add(connectItem);
        menuBar.add(fileMenu);

        setJMenuBar(menuBar);
        add(painel, BorderLayout.NORTH);
        add(scroll, BorderLayout.CENTER);
    }

    private class abrirArquivoIndice implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            JFileChooser fileChooser = new JFileChooser();
            int result = fileChooser.showOpenDialog(null);
            if (result == JFileChooser.APPROVE_OPTION) {
                fileIndice = fileChooser.getSelectedFile();
                JOptionPane.showMessageDialog(null, "Arquivo " + fileIndice.getName() + " aberto com sucesso.");
            }
        }
    }

    private class abrirArquivoDados implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            JFileChooser fileChooser = new JFileChooser();
            int result = fileChooser.showOpenDialog(null);
            if (result == JFileChooser.APPROVE_OPTION) {
                fileDados = fileChooser.getSelectedFile();
                JOptionPane.showMessageDialog(null, "Arquivo " + fileDados.getName() + " aberto com sucesso.");
            }
        }
    }

    private class conectarServidor implements ActionListener {
        @Override
        public void actionPerformed(ActionEvent e) {
            String endereco = JOptionPane.showInputDialog("Endereço do servidor:");
            int porta;
            try {
                porta = Integer.parseInt(JOptionPane.showInputDialog("Porta do servidor:"));
            } catch (NumberFormatException ex) {
                JOptionPane.showMessageDialog(null, "Porta inválida.");
                return;
            }
            try {
                socket = new Socket(endereco, porta);
                out = new PrintWriter(socket.getOutputStream(), true);
                in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                JOptionPane.showMessageDialog(null, "Conectado ao servidor " + endereco + ":" + porta);
            } catch (IOException ex) {
                JOptionPane.showMessageDialog(null, "Falha ao conectar em " + endereco + ":" + porta);
                ex.printStackTrace();
            }
        }
    }

    private class ListarTodosButtonListener implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent e) {
            try {
                if (fileDados != null && !fileDados.getName().isEmpty()) {
                    String query = "2 " + fileDados.getName();
                    if (socket != null && out != null) {
                        out.println(query);
                        out.flush();

                        // Cria uma nova Thread para ler a resposta do servidor
                        new Thread(() -> {
                            StringBuilder resultado = new StringBuilder();
                            try {
                                char[] buffer = new char[1024];
                                int charsRead;
                                while ((charsRead = in.read(buffer)) != -1) {
                                    resultado.append(buffer, 0, charsRead);
                                    if (charsRead < buffer.length) break; // Assumir que a leitura foi concluída se menos caracteres do que o buffer foram lidos
                                }
                                System.out.println("Resposta recebida: " + resultado.toString()); // Adicionar log

                                // Utiliza o método SwingUtilities.invokeLater para garantir que a GUI seja atualizada corretamente na thread da GUI
                                SwingUtilities.invokeLater(() -> {
                                    createListarDialog(resultado.toString());
                                });
                            } catch (IOException ex) {
                                SwingUtilities.invokeLater(() -> {
                                    JOptionPane.showMessageDialog(null, "Erro ao ler a resposta: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
                                });
                            }
                        }).start();

                    } else {
                        JOptionPane.showMessageDialog(null, "Nenhum servidor conectado");
                    }
                } else {
                    JOptionPane.showMessageDialog(null, "Nenhum arquivo selecionado\n");
                }
            } catch (Exception ex) {
                JOptionPane.showMessageDialog(null, "Erro: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
            }
        }

        private void createListarDialog(String resultado) {
            JDialog listarDialog = new JDialog();
            listarDialog.setTitle("Listar Jogadores");
            listarDialog.setSize(600, 900);
            listarDialog.setLayout(new BorderLayout());

            JTextArea lisTextField = new JTextArea(resultado);
            JScrollPane scrollPane = new JScrollPane(lisTextField);  // Adiciona a JTextArea dentro de um JScrollPane
            listarDialog.add(scrollPane, BorderLayout.CENTER);
            listarDialog.setVisible(true);  // Torna o diálogo visível
        }
    }

    private class SearchButtonListener implements ActionListener {
        String camposBusca;
        int quantidadeCampos;

        @Override
        public void actionPerformed(ActionEvent e) {
            String id = idCampo.getText().trim();
            String age = ageCampo.getText().trim();
            String nomeJogador = nomeJogadorCampo.getText().trim();
            String nacionalidade = nacionalidadeCampo.getText().trim();
            String nomeClube = nomeClubeCampo.getText().trim();

            if (id.isEmpty() && age.isEmpty() && nomeJogador.isEmpty() && nacionalidade.isEmpty() && nomeClube.isEmpty()) {
                JOptionPane.showMessageDialog(null, "Por favor, preencha pelo menos um campo de busca.");
                return;
            }

            String camposBusca = "";
            int quantidadeCampos = 0;

            if (!id.isEmpty()) {
                camposBusca += "id " + id + " ";
                quantidadeCampos++;
            }

            if (!age.isEmpty()) {
                camposBusca += "idade " + age + " ";
                quantidadeCampos++;
            }

            if (!nomeJogador.isEmpty()) {
                camposBusca += "nomeJogador \"" + nomeJogador + "\" ";
                quantidadeCampos++;
            }

            if (!nacionalidade.isEmpty()) {
                camposBusca += "nacionalidade \"" + nacionalidade + "\" ";
                quantidadeCampos++;
            }

            if (!nomeClube.isEmpty()) {
                camposBusca += "nomeClube \"" + nomeClube + "\" ";
                quantidadeCampos++;
            }

            if (fileDados != null && !fileDados.getName().isEmpty()) {
                buscar(quantidadeCampos, camposBusca);
            } else {
                JOptionPane.showMessageDialog(null, "Nenhum arquivo selecionado\n");
            }
        }

        private void buscar(int quantidadeCampos, String camposBusca) {
            String query = "3 " + fileDados.getName() + " 1\n" + quantidadeCampos + " " + camposBusca;
            if (socket != null && out != null) {
                out.println(query);
                out.flush();
                new Thread(() -> {
                    try {
                        StringBuilder resultado = new StringBuilder();
                        char[] buffer = new char[1024];
                        int charsRead;
                        while ((charsRead = in.read(buffer)) != -1) {
                            resultado.append(buffer, 0, charsRead);
                            if (charsRead < buffer.length) break; // Assumir que a leitura foi concluída se menos caracteres do que o buffer foram lidos
                        }
                        System.out.println("Resposta recebida: " + resultado.toString()); // Adicionar log
                        String[] jogadorResultado = resultado.toString().split("\n\n");
                        SwingUtilities.invokeLater(() -> {
                            if ((resultado.toString().split("\n\n")[1]).equals("Registro inexistente.")) {
                                campoResultado.removeAll(); // Limpar o painel
                            } else {
                                campoResultado.removeAll(); // Limpar o painel antes de adicionar novos botões
                                for (int i = 1; i < jogadorResultado.length; i++) {
                                    String jogadorInfo = jogadorResultado[i];
                                    String nome = jogadorInfo.split("\n")[0].split(": ")[1]; // Supondo que o nome esteja na primeira posição
                                    JButton btnJogador = new JButton(nome);
                                    btnJogador.addActionListener(e1 -> createPlayerDialog(jogadorInfo.split("\n")));
                                    campoResultado.add(btnJogador);
                                }
                            }
                            campoResultado.revalidate(); // Revalidar o painel para refletir as mudanças
                            campoResultado.repaint(); // Redesenhar o painel
                        });
                    } catch (IOException ex) {
                        SwingUtilities.invokeLater(() -> {
                            JOptionPane.showMessageDialog(null, "Erro ao ler a resposta: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
                        });
                    }
                }).start();
            } else {
                JOptionPane.showMessageDialog(null, "Nenhum servidor conectado");
            }
        }

        private void createPlayerDialog(String[] jogadorInfo) {
            JDialog playerDialog = new JDialog();
            playerDialog.setTitle("Editar Jogador");
            playerDialog.setSize(300, 400);
            playerDialog.setLayout(new BorderLayout());

            String id = jogadorInfo[4].split(": ")[1];
            String idade = jogadorInfo[3].split(": ")[1];
            String nomeJogador = jogadorInfo[0].split(": ")[1];
            String nacionalidade = jogadorInfo[1].split(": ")[1];
            String nomeClube = jogadorInfo[2].split(": ")[1];

            JPanel dataPanel = new JPanel(new GridLayout(0, 2));
            JTextField nameField = new JTextField(nomeJogador, 10);
            JTextField nacionalidadeField = new JTextField(nacionalidade, 10);
            JTextField clubField = new JTextField(nomeClube, 10);
            JTextField idadeField = new JTextField(idade, 10);

            dataPanel.add(new JLabel("Nome:"));
            dataPanel.add(nameField);
            dataPanel.add(new JLabel("Nacionalidade:"));
            dataPanel.add(nacionalidadeField);
            dataPanel.add(new JLabel("Clube:"));
            dataPanel.add(clubField);
            dataPanel.add(new JLabel("Idade:"));
            dataPanel.add(idadeField);

            JPanel buttonsPanel = new JPanel();
            JButton saveButton = new JButton("Salvar");
            JButton deleteButton = new JButton("Remover");

            saveButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    EditarJogador(id, idadeField.getText(), nameField.getText(), nacionalidadeField.getText(), clubField.getText());
                    // buscar(quantidadeCampos, camposBusca);
                    JOptionPane.showMessageDialog(null, "Jogador alterado com sucesso");
                }
            });

            deleteButton.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    ExcluirJogador(id);
                    playerDialog.dispose();
                    JOptionPane.showMessageDialog(null, "Jogador removido com sucesso");
                }
            });

            buttonsPanel.add(saveButton);
            buttonsPanel.add(deleteButton);

            playerDialog.add(dataPanel, BorderLayout.CENTER);
            playerDialog.add(buttonsPanel, BorderLayout.SOUTH);

            playerDialog.setLocationRelativeTo(null); // Centraliza o diálogo
            playerDialog.setModal(true); // Define o diálogo como modal
            playerDialog.setVisible(true); // Exibe o diálogo
        }

        private void ExcluirJogador(String id) {
            try {
                if (fileDados != null && fileIndice != null && !fileDados.getName().isEmpty() && !fileIndice.getName().isEmpty()) {
                    String query = "5 " + fileDados.getName() + " " + fileIndice.getName() + " 1\n1 " + "id " + id + "\n";
                    System.out.println(query);
                    if (socket != null && out != null) {
                        out.println(query);
                        out.flush();

                        new Thread(() -> {
                            StringBuilder resultado = new StringBuilder();
                            try {
                                char[] buffer = new char[1024];
                                int charsRead;
                                while ((charsRead = in.read(buffer)) != -1) {
                                    resultado.append(buffer, 0, charsRead);
                                    if (charsRead < buffer.length) break; // Assumir que a leitura foi concluída se menos caracteres do que o buffer foram lidos
                                }
                                System.out.println("Resposta recebida: " + resultado.toString()); // Adicionar log

                                // Utiliza o método SwingUtilities.invokeLater para garantir que a GUI seja atualizada corretamente na thread da GUI
                                SwingUtilities.invokeLater(() -> {
                                    
                                });
                            } catch (IOException ex) {
                                SwingUtilities.invokeLater(() -> {
                                    JOptionPane.showMessageDialog(null, "Erro ao ler a resposta: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
                                });
                            }
                        }).start();
                    } else {
                        JOptionPane.showMessageDialog(null, "Nenhum servidor conectado");
                    }
                } else {
                    JOptionPane.showMessageDialog(null, "Nenhum arquivo selecionado\n");
                }
            } catch (Exception ex) {
                JOptionPane.showMessageDialog(null, "Erro: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
            }
        }

        private void EditarJogador(String id, String idade, String nomeJogador, String nacionalidade, String nomeClube) {
            ExcluirJogador(id);
            try {
                if (fileDados != null && fileIndice != null && !fileDados.getName().isEmpty() && !fileIndice.getName().isEmpty()) {
                    String query = "6 " + fileDados.getName() + " " + fileIndice.getName() + " 1\n" + id + " " + idade + " \"" + nomeJogador + "\" \"" + nacionalidade + "\" \"" + nomeClube + "\"\n";
                    System.out.println(query); // Adicionar log
                    if (socket != null && out != null) {
                        out.println(query);
                        out.flush();

                        new Thread(() -> {
                            StringBuilder resultado = new StringBuilder();
                            try {
                                char[] buffer = new char[1024];
                                int charsRead;
                                while ((charsRead = in.read(buffer)) != -1) {
                                    resultado.append(buffer, 0, charsRead);
                                    if (charsRead < buffer.length) break; // Assumir que a leitura foi concluída se menos caracteres do que o buffer foram lidos
                                }
                                System.out.println("Resposta recebida: " + resultado.toString()); // Adicionar log

                                // Utiliza o método SwingUtilities.invokeLater para garantir que a GUI seja atualizada corretamente na thread da GUI
                                SwingUtilities.invokeLater(() -> {
                                    
                                });
                            } catch (IOException ex) {
                                SwingUtilities.invokeLater(() -> {
                                    JOptionPane.showMessageDialog(null, "Erro ao ler a resposta: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
                                });
                            }
                        }).start();
                    } else {
                        JOptionPane.showMessageDialog(null, "Nenhum servidor conectado");
                    }
                } else {
                    JOptionPane.showMessageDialog(null, "Nenhum arquivo selecionado\n");
                }
            } catch (Exception ex) {
                JOptionPane.showMessageDialog(null, "Erro: " + ex.getMessage(), "Erro", JOptionPane.ERROR_MESSAGE);
            }
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            new InterfaceFIFA().setVisible(true);
        });
    }
}

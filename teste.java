import javax.swing.*;

public class teste {
    public static void main(String[] args) {
        JFrame frame = new JFrame("Test");
        JLabel label = new JLabel("Hello, Swing!");
        frame.getContentPane().add(label);
        frame.setSize(300, 200);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
    }
}


public class Stack {

  public static void main(String[] args) {

    Thread t1 = new Thread(new Runnable() {
        public void run() {
          System.console().readLine();
        }
      });
    try {
      t1.start();
      t1.join();
    } catch (Exception ex) {
      System.out.println(ex);
    }
  }
}

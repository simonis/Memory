import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;

public class Uncommit {

  static class KB {
    int[] kb;
    public KB() {
      kb = new int[256];
    }
  }

  static class MB {
    KB[] mb;
    public MB() {
      mb = new KB[1024];
      for (int i = 0; i < mb.length; i++) {
        mb[i] = new KB();
      }
    }
  }

  static class GB {
    MB[] gb;
    public GB() {
      gb = new MB[1024];
      for (int i = 0; i < gb.length; i++) {
        gb[i] = new MB();
      }
    }
  }

  static void printRSS() throws IOException {
    Files.lines(FileSystems.getDefault().getPath("/proc/self/status")).
      filter(l -> l.startsWith("VmRSS")).
      forEach(System.out::println);
  }

  public static void main(String[] args) throws Exception {

    System.out.println("Calling System.gc()");
    System.gc();
    printRSS();
    System.out.println("\nPress <Enter>");
    System.console().readLine();

    int size = Integer.parseInt(args.length > 0 ? args[0] : "512");
    MB mb[] = new MB[size];
    for (int i = 0; i < size; i++) {
      mb[i] = new MB();
    }
    System.out.println("Successfully allocated " + size + "MB memory");
    printRSS();
    System.out.println("\nPress <Enter>");
    System.console().readLine();
    for (int i = 0; i < size; i++) {
      mb[i] = null;
    }
    System.out.println("Successfully unlinked " + size + "MB memory");
    printRSS();
    System.out.println("\nPress <Enter>");
    System.console().readLine();
    int gcs = 5;
    if (args.length > 1) {
      gcs = Integer.parseInt(args[1]);
    }
    for (int i = 1; i <= gcs; i++) {
      System.gc();
      System.out.println("Performed " + i + ". System.gc()\n");
      printRSS();
      System.out.println("Press <Enter>");
      System.console().readLine();
      }
  }
}

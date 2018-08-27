import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.channels.FileChannel.MapMode;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.concurrent.CountDownLatch;


public class NIO {

  public static void main(String[] args) throws Exception {

    Path p = Paths.get(args.length > 0 ? args[0] : "file.txt");
    FileChannel fc = FileChannel.open(p, StandardOpenOption.READ);
    MappedByteBuffer mbb = fc.map(MapMode.READ_ONLY, 0, fc.size());

    System.out.println("direct = " + mbb.isDirect());
    System.out.println("mapped = " + mbb.isLoaded());

    System.console().readLine();

    byte[] buf = new byte[4096];
    mbb.get(buf, 0, buf.length);
    System.out.println("Reading first " + buf.length + " bytes from the memory mapped byte buffer");
    System.out.println("mapped = " + mbb.isLoaded());

    System.console().readLine();

    mbb.load();
    System.out.println("Trying to load the whole memory mapped byte buffer into memory");
    System.out.println("mapped = " + mbb.isLoaded());

    System.console().readLine();

    CountDownLatch overflow = new CountDownLatch(1);
    CountDownLatch unwind = new CountDownLatch(1);
    CountDownLatch exit = new CountDownLatch(1);

    int tc = 5;
    for (int i = 0; i < tc; i++) {
      new Thread(null, new Runnable() {
          CountDownLatch o;
          CountDownLatch u;
          {
            this.o = overflow;
            this.u = unwind;
          }
          void overflow() {
            long l0, l1, l2, l3, l4, l5, l6, l7, l8, l9;
            long m0, m1, m2, m3, m4, m5, m6, m7, m8, m9;
            long n0, n1, n2, n3, n4, n5, n6, n7, n8, n9;
            long o0, o1, o2, o3, o4, o5, o6, o7, o8, o9;
            long p0, p1, p2, p3, p4, p5, p6, p7, p8, p9;
            long q0, q1, q2, q3, q4, q5, q6, q7, q8, q9;
            long r0, r1, r2, r3, r4, r5, r6, r7, r8, r9;
            long s0, s1, s2, s3, s4, s5, s6, s7, s8, s9;
            long t0, t1, t2, t3, t4, t5, t6, t7, t8, t9;
            long u0, u1, u2, u3, u4, u5, u6, u7, u8, u9;
            long v0, v1, v2, v3, v4, v5, v6, v7, v8, v9;
            long w0, w1, w2, w3, w4, w5, w6, w7, w8, w9;
            long l00, l11, l22, l33, l44, l55, l66, l77, l88, l99;
            long m00, m11, m22, m33, m44, m55, m66, m77, m88, m99;
            long n00, n11, n22, n33, n44, n55, n66, n77, n88, n99;
            long o00, o11, o22, o33, o44, o55, o66, o77, o88, o99;
            long p00, p11, p22, p33, p44, p55, p66, p77, p88, p99;
            long q00, q11, q22, q33, q44, q55, q66, q77, q88, q99;
            long r00, r11, r22, r33, r44, r55, r66, r77, r88, r99;
            long s00, s11, s22, s33, s44, s55, s66, s77, s88, s99;
            long t00, t11, t22, t33, t44, t55, t66, t77, t88, t99;
            long u00, u11, u22, u33, u44, u55, u66, u77, u88, u99;
            long v00, v11, v22, v33, v44, v55, v66, v77, v88, v99;
            long w00, w11, w22, w33, w44, w55, w66, w77, w88, w99;
            try {
              overflow();
            } catch (StackOverflowError soe) {
              try {
                System.out.println("Catched StackOverflowError");
                u.await();
              } catch (InterruptedException ex) {
                System.out.println(ex);
              }
            }
          }
          public void run() {
            try {
              o.await();
              System.out.println("Running into overlfow");
              overflow();
              System.out.println("Unwinded after overlfow");
              exit.await();
              System.out.println("Exiting");
            } catch (InterruptedException ex) {
              System.out.println(ex);
            }
          }
        }, "OverflowThread" + i, 1_700_000).start();
    }
    System.out.println("Started " + tc + " threads");

    System.console().readLine();

    overflow.countDown();

    System.out.println("Running all " + tc + " threads into an stack overflow");

    System.console().readLine();

    unwind.countDown();

    System.out.println("Unwinding all " + tc + " threads from the previous stack overflow");

    System.console().readLine();

    mbb.load();
    System.out.println("Trying to load the whole memory mapped byte buffer into memory");
    System.out.println("mapped = " + mbb.isLoaded());

    System.console().readLine();

    exit.countDown();

    System.out.println("Exiting all " + tc + " threads");

    System.console().readLine();
  }
}

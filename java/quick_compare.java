import java.io.*; 
import java.util.concurrent.*;

public class quick_compare {
  private Semaphore sema;

  public quick_compare(File f1, File f2, int maxThreads) throws Exception {
    this.sema = new Semaphore(maxThreads);
    final BufferedInputStream fin1 = new BufferedInputStream(new FileInputStream(f1));
    final BufferedInputStream fin2 = new BufferedInputStream(new FileInputStream(f2));
    long length1 = f1.length();
    long length2 = f2.length();
    long minlen = Math.min(length1, length2);
    int chunklen = 1024 * 256;

    if(length1 != length2) 
      System.err.println("files differ in length");

    boolean broken = false;
    Thread read1;

    for(long i = 0; i < minlen; i+=chunklen) {
        final byte[] chunk1 = new byte[chunklen];
        final byte[] chunk2 = new byte[chunklen];

        read1 = new Thread(){
          public void run() {
            try {
              int off = 0;
              int len;        
              while((len = fin1.read(chunk1,off, chunk1.length - off)) > 0)
                off += len;
            } catch (Exception ex) {
              ex.printStackTrace();
              System.exit(2);
            }
          }
        };
        read1.start();

        int off = 0;
        int len;
        while((len = fin2.read(chunk2,off, chunk2.length - off)) > 0)
          off += len;

        if(len < 0)
          broken = true;

        read1.join();

        new CompareThread(i, chunk1, chunk2).start();       

        if(broken)
          break;
    }
  }

  public static void main(String[] args) {
    try{
      File f1 = new File(args[0]);
      File f2 = new File(args[1]);
      int maxThreads;
      if(args.length < 3)
        maxThreads = 3;
      else
        maxThreads = Integer.parseInt(args[2]);
      if(maxThreads <= 0)
        throw new IllegalArgumentException("maxThreads must be >= 0");
      new quick_compare(f1,f2, maxThreads);
    } catch (Exception ex) {
      ex.printStackTrace();
    }
  }

  public class CompareThread extends Thread {
    private byte[] d1;
    private byte[] d2;
    private long off;
    public CompareThread(long off, byte[] d1, byte[] d2) throws InterruptedException {
      sema.acquire();
      if(d1.length != d2.length) 
        throw new IllegalArgumentException("the byte arrays must be the same size");

      this.off = off;
      this.d1 = d1;
      this.d2 = d2;
    }

    public void run() {
      for(int i = 0; i < d1.length; i++) {
        if(d1[i] != d2[i]) {
          System.err.println(d1[i] + "!=" + d2[i] + " at " + (off + i));
          System.exit(1);
        }
      }
      sema.release();
    }
  }       
}

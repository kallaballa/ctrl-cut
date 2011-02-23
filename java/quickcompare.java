import java.io.*; 
import java.util.concurrent.*;

public class quickcompare {
  private Semaphore sema;

  public quickcompare(File f1, File f2, int maxThreads) throws Exception {
    this.sema = new Semaphore(maxThreads);
    final BufferedInputStream fin1 = new BufferedInputStream(new FileInputStream(f1));
    final BufferedInputStream fin2 = new BufferedInputStream(new FileInputStream(f2));
    long length1 = f1.length();
    long length2 = f2.length();
    long minlen = Math.min(length1, length2);
    int chunklen = 1024 * 256;

    if(length1 != length2) 
      System.err.println("files differ in length");

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

        read1.join();

        if(off > 0)
          new CompareThread(i, off, chunk1, chunk2).start();  
        
        if(len < 0)
          break;
    }
    
    while(sema.availablePermits() != maxThreads)
      Thread.sleep(100);
  }

  public static void main(String[] args) {
    try{
      File f1 = new File(args[0]);
      File f2 = new File(args[1]);
      int maxThreads = 3;
      if(args.length > 2)
        maxThreads = Integer.parseInt(args[2]);

      if(maxThreads <= 0)
        throw new IllegalArgumentException("maxThreads must be >= 0");
      new quickcompare(f1,f2, maxThreads);
    } catch (Exception ex) {
      ex.printStackTrace();
      System.exit(2);
    }
  }

  public class CompareThread extends Thread {
    private byte[] d1;
    private byte[] d2;
    private long off;
    private int len;
    public CompareThread(long off, int len,  byte[] d1, byte[] d2) throws InterruptedException {
      sema.acquire();
      if(d1.length != d2.length) 
        throw new IllegalArgumentException("the byte arrays must be the same size");

      this.off = off;
      this.d1 = d1;
      this.d2 = d2;
      this.len = len;
    }

    public void run() {
      for(int i = 0; i < len; i++) {
        if(d1[i] != d2[i]) {
          System.err.println(d1[i] + "!=" + d2[i] + " at " + (off + i));
          System.exit(1);
        }
      }
      sema.release();
    }
  }       
}

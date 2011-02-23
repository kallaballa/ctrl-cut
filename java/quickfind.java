import java.io.*; 
import java.util.concurrent.*;

public class quickfind {
  private Semaphore sema;
  private boolean invert; 
  private BufferedInputStream fin;
  private String rawPattern;
  public quickfind(File f, String pattern, boolean invert, int maxThreads) throws Exception {
    this.sema = new Semaphore(maxThreads);
    this.invert = invert;
    this.fin = new BufferedInputStream(new FileInputStream(f));
    this.rawPattern = pattern;
    long flen = f.length();
    
    int patternLen = rawPattern.length();
    if(rawPattern.length() % 2 > 0) {
      patternLen++;
      rawPattern = "0" + rawPattern;
    }
    
    byte[] binPattern = new byte[patternLen/2];
    for (int i = 0; i < patternLen; i+=2)
      binPattern[i/2] = (byte) Integer.parseInt(rawPattern.substring(i, i + 2), 16);

    int chunklen = 1024 * 256;
    chunklen -= chunklen % binPattern.length;
    
    for (long i = 0; i < flen; i += chunklen) {
      final byte[] chunk = new byte[chunklen];

      int off = 0;
      int len;
      
      while ((len = fin.read(chunk, off, chunk.length - off)) > 0)
        off += len;
      
      if(off > 0)
        new FinderThread(i, off, chunk, binPattern).start();
      
      if(len < 0)
        break;
    }
    
    while(sema.availablePermits() != maxThreads)
      Thread.sleep(100);
  }

  public static void main(String[] args) {
    try{
      File f = new File(args[0]);
      String pattern = args[1];
      boolean invert = false;
      int maxThreads = 3;
      if(args.length > 2)
        invert = Boolean.parseBoolean(args[2]);

      if(args.length > 3)
        maxThreads = Integer.parseInt(args[3]);
      if(maxThreads <= 0)
        throw new IllegalArgumentException("maxThreads must be >= 0");
      new quickfind(f,pattern, invert, maxThreads);
      System.exit(1);
    } catch (Exception ex) {
      ex.printStackTrace();
    }
  }

  public class FinderThread extends Thread {
    private byte[] chunk;
    private byte[] binPattern;
    private long off;
    private int len;

    public FinderThread(long off, int len,  byte[] chunk, byte[] binPattern) throws InterruptedException {
      sema.acquire();
      this.off = off;
      this.len = len - (len % binPattern.length);
      this.chunk = chunk;
      this.binPattern = binPattern;
    }

    public void run() {
      try {
        int chunkoff;
        int patternoff;
        for(chunkoff = 0; chunkoff < len; chunkoff+=binPattern.length) {
          boolean found = false;
          for (patternoff = 0; patternoff < binPattern.length; patternoff++) {
            if(chunk[chunkoff + patternoff] == binPattern[patternoff]) {
              found = true;
            } else {
              found = false;
              break;
            }
          }

          if(found != invert) {
            String op = " does match at ";
            if(!found)
              op = " does not match at ";
            
            System.err.println(rawPattern + op + (off + chunkoff + patternoff));
            System.exit(0);
          }
        }
        sema.release();
      } catch (ArrayIndexOutOfBoundsException e) {
        e.printStackTrace();
      }
    }
  }       
}

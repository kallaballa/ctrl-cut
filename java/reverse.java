import java.io.*;

public class reverse {
  public static void main(String[] args) {
    try{
      File f = new File(args[0]);
      BufferedInputStream in = new BufferedInputStream(new FileInputStream(f));
      BufferedOutputStream out = new BufferedOutputStream(System.out);

      byte[] data = new byte[(int)f.length()];
      int off = 0;
      int len = 0;

      while((len = in.read(data,off, data.length - off)) > 0) 
        off += len;
      
      for(int i = data.length - 1; i >= 0; i--)
        out.write(data[i]);

      out.close();
    } catch (Exception ex) { ex.printStackTrace(); }
  }
}

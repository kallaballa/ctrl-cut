import java.io.*;

public class flipy {
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
      
      int width = 21600 * 3;
      for(int i = data.length - width; i >= 0; i -= width)
        out.write(data, i,width);

      out.close();
    } catch (Exception ex) { ex.printStackTrace(); }
  }
}

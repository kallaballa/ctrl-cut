magick++ {
  message("Configuring Magick++...")


QMAKE_LIBS+="-lMagick++ -lMagickWand -lMagickCore -llcms -ltiff -lfreetype -ljpeg -lfontconfig -lXext -lSM -lICE -lX11 -lXt -lbz2 -lz -lm -lgomp -lpthread -lltdl"
QMAKE_CXXFLAGS+="-I/usr/include/ImageMagick -fmessage-length=0 -O2 -Wall -D_FORTIFY_SOURCE=2 -fstack-protector -funwind-tables -fasynchronous-unwind-tables -g -fstack-protector-all -pthread -L/usr/lib64"
}

libpng {
  SUSE = $$system(cat /proc/version | grep -o SUSE)
  message("Configuring libpng...")
  DEFINES += cimg_use_png
  contains(SUSE, SUSE): {
    message("openSUSE detected")
    PKGCONFIG += libpng16
	} else {
  	PKGCONFIG += libpng
	}
}

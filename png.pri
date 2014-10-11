libpng {
	SUSE = $$system(cat /proc/version | grep -o SUSE)
  message("Configuring libpng...")
  contains(SUSE, SUSE): {
		message("Detected Opensuse")
    PKGCONFIG += libpng16
	} else {
		PKGCONFIG += libpng
	}

  DEFINES += cimg_use_png
}

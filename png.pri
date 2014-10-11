libpng {
	SUSE = $$system(cat /proc/version | grep -o SUSE)
  contains(SUSE, SUSE): {
    message("openSUSE detected")
    PKGCONFIG += libpng16
	} else {
  	PKGCONFIG += libpng
	}
}

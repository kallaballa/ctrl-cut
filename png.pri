libpng {
	SUSE = $$system(ls /etc/*-release | grep -o SuSE)
	message("Configuring png...")
	contains(SUSE, SuSE): {
		message("openSUSE detected")
		QMAKE_CXXFLAGS += -I/usr/include/libpng16
		QMAKE_LIBS += -lpng16
	} else {
		PKGCONFIG += libpng
	}
}

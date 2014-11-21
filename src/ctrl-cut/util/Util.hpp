/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <string>

int lower_case (int c);
void install_signal_handlers();
std::string make_temp_filename(const std::string suffix = "");

#endif /* UTIL_H_ */

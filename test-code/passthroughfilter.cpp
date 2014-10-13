/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include <stdio.h>
#include <stdlib.h>

#include <cups/cups.h>

static void printEnv(const char *env)
{
  const char *envstr = getenv(env);
  if (envstr) {
    fprintf(stderr, "DEBUG: %s = %s\n", env, envstr);
  }
}

/**
 * Main entry point for the program.
 *
 * @param argc The number of command line options passed to the program.
 * @param argv An array of strings where each string represents a command line
 * argument.
 * @return An integer where 0 represents successful termination, any other
 * value represents an error code.
 */
int main(int argc, char *argv[])
{
  // Make sure status messages are not buffered
  setbuf(stderr, NULL);

  if (argc < 6 || argc > 7) {
    fprintf(stderr, "ERROR: Usage: %s job-id user title copies options [file]\n", argv[0]);
    return 1;
  }

  fprintf(stderr, "DEBUG: Start debug output - passthroughfilter\n");

  // Arguments debug output:
  fprintf(stderr, "DEBUG: ");
  for (int i=0;i<argc;i++) {
    // Don't print the option arg - these are parsed and output later
    if (i != 5) {
      fprintf(stderr, "%s ", argv[i]);
    }
  }
  fprintf(stderr, "\n");

  // Env. variable debug output
  printEnv("CHARSET");
  printEnv("CLASS");
  printEnv("CONTENT_TYPE");
  printEnv("CUPS_CACHEDIR");
  printEnv("CUPS_DATADIR");
  printEnv("CUPS_FILETYPE");
  printEnv("CUPS_SERVERROOT");
  printEnv("DEVICE_URI");
  printEnv("FINAL_CONTENT_TYPE");
  printEnv("LANG");
  printEnv("PATH");
  printEnv("PPD");
  printEnv("PRINTER");
  printEnv("RIP_CACHE");
  printEnv("SOFTWARE");
  printEnv("TZ");
  printEnv("USER");

  cups_option_t	*options = NULL;
  int num_options = cupsParseOptions(argv[5], 0, &options);
  for (int i=0;i<num_options;i++) {
    fprintf(stderr, "DEBUG: %s: %s\n", options[i].name, options[i].value);
  }

  // FIXME: Register a signal handler to support cancelling of jobs

  cups_file_t *fp;
  if (argc == 6) {
    fp = cupsFileStdin();
  } else {
    // Try to open the print file...
    if ((fp = cupsFileOpen(argv[6], "r")) == NULL) {
      perror("ERROR: unable to open print file - ");
      return 1;
    }
  }

  char buffer[8192];
  int bytes;
  while ((bytes = cupsFileRead(fp, buffer, sizeof(buffer))) > 0) {
    fwrite(buffer, 1, bytes, stdout);
  }

  fprintf(stderr, "DEBUG: End debug output - passthroughfilter\n");
  return 0;
}

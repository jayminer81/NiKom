#include <exec/types.h>
#include <proto/exec.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "NiKomLib.h"

struct Library *NiKomBase;

void printUsage(void) {
  printf("Usage: One of the following\n");
  printf("  NiKomFido UpdateFido\n");
  printf("  NiKomFido RescanConf <conf number>\n");
  printf("  NiKomFido RescanAllConf\n");
  printf("  NiKomFido RenumberConf <conf number> <new lowest text number>\n");
}

void updateFido(void) {
  printf("Updating Fido mail.\n");
  Matrix2NiKom();
  printf("Updating Fido conferences.\n");
  UpdateAllFidoConf();
}

int rescanConf(int argc, char *argv[]) {
  int conf;
  if(argc != 3) {
    printf("RescanConf takes exactly one parameter, the number of the conf to rescan.\n");
    return 5;
  }

  conf = atoi(argv[2]);
  printf("Rescanning Fido conference %d\n", conf);
  
  ReScanFidoConf(NULL, conf);
}

void rescanAllConf(void) {
  printf("Rescanning all Fido conferences.\n");
  ReScanAllFidoConf();
}

int renumberConf(int argc, char *argv[]) {
  int conf, lowtext, res, error = 10;
  if(argc != 4) {
    printf("RenumberConf takes exactly two parameters.\n");
    printf(" - the number of the conf to renumber.\n");
    printf(" - the new lowest text number.\n");
    return 5;
  }

  conf = atoi(argv[2]);
  lowtext = atoi(argv[3]);
  printf("Renumbering Fido conference %d. New low text: %d\n", conf, lowtext);
  
  res = ReNumberConf(NULL, conf, lowtext);

  switch(res) {
  case 0:
    error = 0;
    break;
  case 1:
    printf("There is no conference with number %d.\n", conf);
    break;
  case 2:
    printf("The new low text number is not lower than to old one.\n");
    break;
  case 3:
    printf("Conference %d is not a Fido conference.\n", conf);
    break;
  case 4:
    printf("Could not read the HWM (High Water Mark).\n");
    break;
  case 5:
    printf("Could not write a new HWM (High Water Mark).\n");
    break;
  default:
    printf("Unexpected error code: %d\n", res);
  }

  return error;
}

void main(int argc, char *argv[]) {
  int ret = 0;

  if(argc < 2) {
    printUsage();
    exit(5);
  }

  if(!(NiKomBase = OpenLibrary("nikom.library",0))) {
    printf("Could not open nikom.library\n");
    exit(10);
  }

  if(stricmp(argv[1], "UpdateFido") == 0) {
    updateFido();
  } else if(stricmp(argv[1], "RescanConf") == 0) {
    ret = rescanConf(argc, argv);
  } else if(stricmp(argv[1], "RescanAllConf") == 0) {
    rescanAllConf();
  } else if(stricmp(argv[1], "RenumberConf") == 0) {
    ret = renumberConf(argc, argv);
  } else {
    printUsage();
    ret = 5;
  }

  CloseLibrary(NiKomBase);
  exit(ret);
}




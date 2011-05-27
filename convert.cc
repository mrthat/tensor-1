
#include "error.h"
#include "file.h"
#include "tensor.h"
#include "utility.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define OPTION_MESSAGE(x,a,b)    (x ? a:b)
#define DEFAULT_ON_OR_OFF(x)     OPTION_MESSAGE(x, "on", "off")

#define DESCRIPTION "A tool for converting between th-order tensor storage strategies."
#define VERSION     "Version 0.01 (" __DATE__ "), " \
  "Copyright (C) 2011, and GPLv3'd, by Ben Burnett\n" \
  "This is free software; see the source for copying conditions.  There is NO\n" \
  "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

#define DEFAULT_VERBOSITY false

int  storage_strategy;
int  operation_code;
char *program_name;
bool verbose;

void
usage() 
{
  message("%s: %s\n\n", program_name, DESCRIPTION);
  message("%s\n", VERSION);
  message("\nUsage:\n");
  message("\t%s [options] <input> [output]\n",
	  program_name);  
  message("\nOptions:\n");
  message("\t-h\tthis screen\n");
  message("\t-o\torientation:\n");
  print_orientations("\t\t- %s\n");
  message("\t-s\tstrategy:\n");
  print_strategies("\t\t- %s\n");
  message("\t-v\ttoggle verbosity (default: %s)\n", 
	  DEFAULT_ON_OR_OFF(DEFAULT_VERBOSITY));
  message("\nExample:\n\n");
  message("\t$ ./convert -s compressed -o column ieee-fig4.in ieee-fig4.out\n");
  message("\tReading ieee-fig4.in ... done [0.000305]\n");
  message("\tConverting from 'coordinate' to 'column-compressed' ... done [0.000010]\n");
  message("\tWriting ieee-fig4.out ... done [0.000031]\n");
  exit(1);
}

tensor_t*
timed_tensor_read(char const *name)
{
  clock_t  t;
  tensor_t *tensor;
  
  message("Reading %s ... ", name);
  t = clock();
  tensor = tensor_read(name);
  message("done [%lf]\n", SECONDS_SINCE(t));

  return tensor;
}

tensor_t*
timed_tensor_convert(tensor_t *source, strategy::type_t strategy, orientation::type_t orientation)
{
  clock_t  t;
  tensor_t *tensor;
  
  message("Converting from '%s' to '%s-%s' ... ", 
	  strategy_to_string(source->strategy), 
	  orientation_to_string(orientation),
	  strategy_to_string(strategy));
  t = clock();
  tensor = tensor_convert(source, strategy, orientation);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  return tensor;
}

int
main(int argc, char *argv[])
{
  int                 c, offset;
  clock_t             t;
  char                *name;
  tensor_t            *tensor, *result;
  FILE                *file;
  strategy::type_t    strategy;
  orientation::type_t orientation;
  
  /* store the our name for future use */
  program_name = basename(argv[0]);
  
  /* just to be safe, set the tensors to null */
  tensor = result = NULL;
  
  /* set the program's defaults */
  verbose = false;  
  
  /* extract any command-line options the user provided */
  opterr = 0; /* we will privide our own error messages */
  while (-1 != (c = getopt(argc, argv, "ho:s:v"))) {
    switch (c) {
    case 'h': usage();   break;
    case 'o': 
      if (isdigit(optarg[0])) {
	orientation = (orientation::type_t) atoi(optarg);
      } else {
	orientation = string_to_orientation(optarg); 
      }
      break;
    case 's': 
      if (isdigit(optarg[0])) {
	strategy = (strategy::type_t) atoi(optarg);
      } else {
	strategy = string_to_strategy(optarg);
      }
      break;
    case 'v': 
      verbose = !verbose;
      break;
    case '?':
      if (isprint(optopt)) {
	die("Unknown option `-%c'.\n", optopt);
      } else {
	die("Unknown option character `\\x%x'.\n", optopt);
      }
      break;
    default:
      abort();
      break;
    }    
  }
  
  /* count the number of remaining arguments */
  if (argc-optind < 1) {
    usage();
  }
  
  offset = optind;
  name   = argv[offset++];
  tensor = timed_tensor_read(name);  
  debug("main: tensor=0x%x\n", tensor);
  
  result = timed_tensor_convert(tensor, strategy, orientation);
  debug("main: result=0x%x\n", result);
  
  if (offset == argc) {
    file = stdout;
    message("Writing stdout ... ");
  } else {
    name = argv[offset++];
    file = fopen_or_die(name, "w+");
    message("Writing %s ... ", name);
  }  
  t = clock();
  tensor_fwrite(file, result);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  tensor_free(result);
  tensor_free(tensor);
  
  return 0;
}


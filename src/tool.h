
#ifndef _TOOL_H_
#define _TOOL_H_

#include "cache.h"
#include "matrix.h"
#include "tensor.h"
#include "types.h"
#include "vector.h"

namespace tool {
  typedef enum {
    unknown,
    tensor,
    convert,
    generate,
    effectuate,
    permute
  } type_t;
}

#define OPTION_MESSAGE(x,a,b)   (x ? a:b)
#define DEFAULT_ON_OR_OFF(x)    OPTION_MESSAGE(x, "on", "off")

#define DEFAULT_HUMAN_READABLE        true
#define DEFAULT_ITERATIONS            1
#define DEFAULT_OPERATION             operation::n_mode_product
#define DEFAULT_ORIENTATION           orientation::row
#define DEFAULT_PERMUTATION_HEURISTIC permutation_heuristic::none
#define DEFAULT_SIMULATE              false
#define DEFAULT_STRATEGY              strategy::compressed
#define DEFAULT_TRACING               false
#define DEFAULT_VERBOSE               false
#define DEFAULT_VERBOSITY             verbosity::low
#define DEFAULT_WRITE_RESULTS         false
#define DEFAULT_EMIT_LATEX            false
#define DEFAULT_SEED                  1234

#define DEFAULT_CACHE_SIZE            (2*1024)
#define DEFAULT_CACHE_LINE_SIZE       32

void convert_tool_main(int argc, char *argv[]);
void generate_tool_main(int argc, char *argv[]);
void effectuate_tool_main(int argc, char *argv[]);
void permute_tool_main(int argc, char *argv[]);

vector_t* timed_vector_read(char const *name);
matrix_t* timed_matrix_read(char const *name);
tensor_t* timed_tensor_read(char const *name);
void timed_tensor_write(int argc, char *argv[], int const offset, tensor_t const *tensor);
void timed_tensor_fwrite(FILE *file, tensor_t const *tensor);

tool::type_t tool_from_string(char const *s);
char const* string_from_tool(tool::type_t tool);

void print_tool_banner();
void print_tools_with_descriptions(char const *format);
void print_tool_options();

#endif /* _UTILITY_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/

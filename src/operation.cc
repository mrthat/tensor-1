
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "matrix.h"
#include "operation.h"
#include "thread.h"
#include "tensor.h"
#include "utility.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

extern uint thread_count;

void
operation_mode_1_product(matrix_t *matrix, tensor_t const *tensor, vector_t const *vector)
{
  debug("operation_mode_1_product(matrix=0x%x, tensor=0x%x, vector=0x%x)\n", matrix, tensor, vector);
  
  compatible(vector, tensor);
  
  switch (tensor->strategy) {
  case strategy::array:
    operation_mode_1_product_array(matrix, tensor, vector);
    break;
  default:
    die("operation_mode_1_product: tensor product for '%s' strategy (using threads) is not currently supported.\n",
	strategy_to_string(tensor->strategy));
    break;
  }
}

matrix_t*
operation_mode_1_product(vector_t const *vector, tensor_t const *tensor)
{
  matrix_t *matrix;
  
  compatible(vector, tensor);
  debug("operation_mode_1_product(tensor=0x%x, vector=0x%x)\n", tensor, vector);
  
  matrix = matrix_malloc(tensor->m, tensor->n, ownership::creator);
  debug("operation_mode_1_product: matrix=0x%x\n", matrix);
 
  operation_mode_1_product(matrix, tensor, vector);
  
  return matrix;
}

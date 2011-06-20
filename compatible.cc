
#include "compatible.h"
#include "error.h"
#include "information.h"

void
compatible(tensor_t const *lhs, tensor_t const *rhs)
{
  debug("compatible(tensor=0x%x, tensor=0x%x)\n", lhs, rhs);
  
  if (lhs->nnz != rhs->nnz) {
    die("Tensors do not have the same number non-zero entries.\n");
  }
  
  if (lhs->l != rhs->l || lhs->m != rhs->m || lhs->n != rhs->n) {
    die("Tensors do not have the same dimensions.\n");
  }
  
  if (rhs->strategy != strategy::coordinate) {
    die("Conversion from '%s' is not currently supported.\n",
	strategy_to_string(rhs->strategy));
  }
}

void
compatible(vector_t const *lhs, tensor_t const *rhs)
{
  bool compatible, supported;
  
  debug("compatible(vector=0x%x, tensor=0x%x)\n", lhs, rhs);
  
  switch (rhs->strategy) {
  case strategy::compressed:
  case strategy::ekmr:
    supported = true;
    break;
  default:
    supported = false;
    break;
  }
  
  if (!supported) {
    die("Tensor strategy '%s' is not currently supported.\n",
	strategy_to_string(rhs->strategy));
  }
  
  switch (rhs->orientation) {
  case orientation::row:
    compatible = (lhs->n == rhs->n);
    break;
  case orientation::column:
    compatible = (lhs->n == rhs->m);
    break;
  case orientation::tube:
    compatible = (lhs->n == rhs->l);
    break;
  default:
    compatible = false;
    break;
  }
  
  if (!compatible) {
    print_information(lhs);
    print_information(rhs);
    die("Tensors and vector do not have matching dimensions.\n");
  }
}
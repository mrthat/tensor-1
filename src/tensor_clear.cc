
#include "error.h"
#include "tensor.h"

void
tensor_storage_clear_coordinate(tensor_t *tensor)
{
  uint i;
  tensor_storage_coordinate_t *storage;
  coordinate_tuple_t   *tuples;
  
  debug("tensor_storage_clear_coordinate(0x%x)\n", tensor);
  
  storage = STORAGE_COORIDINATE(tensor);
  tuples  = storage->tuples;
  
  for (i = 0; i < tensor->nnz; ++i) {
    tuples[i].i     = 0;
    tuples[i].j     = 0;
    tuples[i].k     = 0;
    tuples[i].index = 0;
  }
}

void
tensor_storage_clear_compressed(tensor_t *tensor)
{
  uint i;
  tensor_storage_compressed_t *storage;
  
  debug("tensor_storage_clear_compressed(0x%x)\n", tensor);
  
  storage = STORAGE_COMPRESSED(tensor);
  
  for (i = 0; i < storage->rn; ++i) {
    storage->RO[i] = 0;
  }
  
  for (i = 0; i < tensor->nnz; ++i) {
    storage->CO[i] = 0;
    storage->KO[i] = 0;
  }
}

void
tensor_storage_clear_extended(tensor_t *tensor)
{
  uint i;
  tensor_storage_extended_t *storage;
  
  debug("tensor_storage_clear_ekmr(0x%x)\n", tensor);
  
  storage = STORAGE_EXTENDED(tensor);
  
  for (i = 0; i < storage->rn; ++i) {
    storage->RO[i] = 0;
  }
  
  for (i = 0; i < tensor->nnz; ++i) {
    storage->CK[i] = 0;
  }
}

void
tensor_clear(tensor_t *tensor)
{
  uint i;
  
  debug("tensor_clear(0x%x)\n", tensor);
  tensor_validate(tensor);
  
  for (i = 0; i < tensor->nnz; ++i) {
    tensor->values[i] = 0.0;
  }
  
  switch (tensor->strategy) {
  case strategy::coordinate:
    tensor_storage_clear_coordinate(tensor);
    break;
  case strategy::compressed:
    tensor_storage_clear_compressed(tensor);
    break;
  case strategy::ekmr:
  case strategy::zzekmr:
    tensor_storage_clear_extended(tensor);
    break;
  default:
    die("Tensor storage strategy '%d' is not supported.\n", tensor->strategy);
  }
}


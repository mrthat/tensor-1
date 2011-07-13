
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "matrix.h"
#include "operation.h"
#include "tensor.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

/*
  Computing ($pT$):
  Let $\T \in R^{n\times n\times n}$ be a tensor.
  Let $\M \in R^{n\times n}$ be a matrix.
  Let $p \in R^{n}$ be a vector.
  for i = 1 to l do
    for j = 1 to m do 
      for k = 1 to m do
        M[i][j] += p[k] * T[i][j][k]
      end for
    end for
  end for
*/

extern cache_t *cache;

void
compressed_operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                       i, j, k;
  uint                       size, nnz;
  uint                       start, end;
  uint                       c, r;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C, *T;
  tensor_storage_compressed_t const *storage;
  
  debug("compressed_operation_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  
  storage = STORAGE_COMPRESSED(tensor);
  size    = storage->size;
  R       = storage->RO;
  C       = storage->CO;
  T       = storage->KO;
  
  /*
    Using \emph{compressed row storage} ($\CRS$), this tensor can be
    represented as:
    
           $k$   0   1   2    3   4   5   6    7   8   9   10   11
     $\rowcrs$ & 0 & 4 & 8 & 12
     $\colcrs$ & 1 & 3 & 0 &  2 & 0 & 2 & 1 &  2 & 1 & 2 &  0 &  3
    $\tubecrs$ & 0 & 0 & 1 &  1 & 0 & 0 & 1 &  1 & 0 & 0 &  1 &  1
     $\valcrs$ & 1 & 2 & 7 &  8 & 3 & 4 & 9 & 10 & 5 & 6 & 11 & 12
  */
  
  DEBUG("\n");
  
  for (i = 0, r = 1; r < size; ++r, ++i) {
    start = R[i];
    end   = R[r];
    
    cache_access(cache, &R[i], cache_operation::read);
    cache_access(cache, &R[r], cache_operation::read);
    
    DEBUG("start=%d, end=%d\n", start, end);
    
    for (k = start; k < end; ++k) {
      c = C[k];
      j = T[k];
      
      cache_access(cache, &C[k], cache_operation::read);
      cache_access(cache, &T[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += (p[j=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", i, j, M[i][j], j, p[j], k, V[k], p[j] * V[k]);
      
      M[i][j] += p[c] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[c],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d, T[k]=%d => c=%d, i=%d, j=%d\n", k, C[k], T[k], c, i, j);
    }
  }
}

void
compressed_slice_operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                       i, j, k, ri;
  uint                       size, nnz;
  uint                       start, end, offset;
  uint                       c, r, t, m, n;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C;
  tensor_storage_extended_t const *storage;
  
  debug("compressed_slice_operation_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_EXTENDED(tensor);
  size    = storage->size;
  R       = storage->RO;
  C       = storage->CK;
  
  /*
    Using frontal \emph{compressed slice storage} ($\FCSS$), this
    tensor can be represented as:
    
           $k$   0   1   2    3   4   5   6    7   8   9   10   11  12
     $\rowcss$   0   1   3    4   6   7   9   10  12  13
      $\ctcss$   1   0   2    1   0   2   1    0   2   1    0    2   1
     $\valcss$   1   2   3    4   5   6   7    8   9  10   11   12  13
     
  */
  
  DEBUG("\nsize=%d, offset=%d\n", size, offset);
  
  j = 0;
  
  for (r = 1; r < size; ++r) {
    ri    = R[r-1];
    i     = ri % m;
    //j     = (r-1) / n;
    t     = (r-1) / n;
    start = ri;
    end   = R[r];
    
    
    cache_access(cache, &R[i], cache_operation::read);
    cache_access(cache, &R[r], cache_operation::read);
    
    DEBUG("ri=%d, start=%d, end=%d\n", ri, start, end);
    
    for (k = start; k < end; ++k) {
      c = C[k];
      
      
      //DEBUG("c=%d, i=%d, j=%d, k=%d\n", c, i, j, k);
            
      cache_access(cache, &C[k], cache_operation::read);
      //cache_access(cache, &T[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += ", i, j, M[i][j]);
      DEBUG("(p[t=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", t, p[t], k, V[k], p[t] * V[k]);
      
      M[i][j] += p[t] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[t],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d => c=%d, r=%d, t=%d, i=%d, j=%d\n", k, C[k], c, r, t, i, j);
    }
  }
}

void
ekmr_row_operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                 i, j, k, ck;
  uint                 size, nnz;
  uint                 start, end;
  uint                 c, r, m, n;
  double               **M;
  double const         *p, *V;
  uint const           *R, *CK;
  tensor_storage_extended_t const *storage;
  
  debug("ekmr_row_operation_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_EXTENDED(tensor);
  size    = storage->size;
  R       = storage->RO;
  CK      = storage->CK;
  
  /* 
     Now, using \emph{extended compressed row storage} ($\ECRS$), the
     original tensor can be represented as:
     
           $k$   0   1   2    3   4   5   6    7   8   9   10   11
     $\rowcrs$ & 0 & 4 & 8 & 12
      $\ctcrs$ & 1 & 2 & 5 &  6 & 0 & 3 & 4 &  5 &  1 & 2 & 4 &  7
     $\valcrs$ & 7 & 1 & 8 &  2 & 3 & 9 & 4 & 10 & 11 & 5 & 6 & 12
  */
  
  DEBUG("\n");
  
  for (i = 0, r = 1; r < size; ++r, ++i) {
    start = R[i];
    end   = R[r];
    
    cache_access(cache, &R[i], cache_operation::read);
    cache_access(cache, &R[r], cache_operation::read);
    
    DEBUG("start=%d, end=%d\n", start, end);
    
    for (k = start; k < end; ++k) {
      ck = CK[k];
      c  = ck / n;
      j  = ck % n;
      
      cache_access(cache, &CK[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += (p[j=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", i, j, M[i][j], j, p[j], k, V[k], p[j] * V[k]);
      
      M[i][j] += p[j] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[c],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("CK[k=%2d]=%2d => c=%d, i=%d, j=%d\n", k, CK[k], c, i, j);
    }
  }
}

void
ekmr_operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("ekmr_operation_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  switch (tensor->orientation) {
  case orientation::row:
    ekmr_row_operation_n_mode_product(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' orientation is not currently supported.\n",
	orientation_to_string(tensor->orientation));
    break;
  }
}

void
operation_n_mode_product_inplace(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("operation_n_mode_product_inplace(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  compatible(vector, tensor);
  
  switch (tensor->strategy) {
  case strategy::compressed:
    compressed_operation_n_mode_product(matrix, vector, tensor);
    break;
  case strategy::slice:
    compressed_slice_operation_n_mode_product(matrix, vector, tensor);
    break;
  case strategy::ekmr:
    ekmr_operation_n_mode_product(matrix, vector, tensor);
    break;
  case strategy::zzekmr:
    /* NOTE: the encoding may differ, but the way we calculate
       products remains the same.  How is that for simplicity? */
    ekmr_operation_n_mode_product(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' strategy is not currently supported.\n",
	strategy_to_string(tensor->strategy));
    break;
  }
}

void
calculate_output_matrix_dimentions(tensor_t const *tensor, uint *m, uint *n)
{
  *m = 0;
  *n = 0;
  switch (tensor->orientation) {
  case orientation::row:
  case orientation::column:
  case orientation::tube:
  case orientation::lateral:
  case orientation::horizontal:
  case orientation::frontal:
    /* rows * columns */
    *m = tensor->m;
    *n = tensor->n;
    break;
  default:
    die("calculate_output_matrix_dimentions: unknown or unsupported orientation %s (%d)\n",
	orientation_to_string(tensor->orientation), tensor->orientation);
    break;
  }
}

matrix_t*
operation_n_mode_product(vector_t const *vector, tensor_t const *tensor)
{
  uint     m, n;
  matrix_t *matrix;
  
  debug("operation_n_mode_product(vector=0x%x, tensor=0x%x)\n", vector, tensor);
  
  compatible(vector, tensor);
  calculate_output_matrix_dimentions(tensor, &m, &n);
  
  debug("operation_n_mode_product: m=%d, n=%d\n");
  
  matrix = matrix_malloc(m, n);
  operation_n_mode_product_inplace(matrix, vector, tensor);
  
  return matrix;
}

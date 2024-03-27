#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>


// TODO invent way to store digits in literal digit array
// MAX fibonacci value representable by 64 bit: 93

#define UINTX_T_DEFAULT 40000

int max(int v1, int v2) {
  if (v1 > v2) return v1;
  else return v2;
}

typedef struct _uintx_t {
  int size;
  int count;
  uint8_t* val;
} uintx_t;

typedef struct _matrix_2x2 {
  uint64_t m11, m12;
  uint64_t m21, m22;
} matrix_2x2;

uintx_t* init_uintx_t(int size) {
  uintx_t* v = (uintx_t*)malloc(sizeof(uintx_t));
  uint8_t* val = (uint8_t*)calloc(size, sizeof(uint8_t));
  *v = (uintx_t){
    .size = size,
    .count = 0,
    .val = val,
  };

  return v;
}

uintx_t* init_uintx_t_alt(int size, uint64_t v) {
  uintx_t* res = init_uintx_t(size);
  int ptr = 0;
  while (v != 0) {
    res->val[ptr] = v % 10;
    ptr++;
    v = (v - (v % 10)) / 10;
  }
  res->count = ptr;
  return res;
}

uintx_t* copy_uintx_t(uintx_t* v) {
  uintx_t* copy = (uintx_t*)malloc(sizeof(uintx_t));
  uint8_t* c_val = (uint8_t*)calloc(v->size, sizeof(uint8_t));
  for (int i = 0; i < v->count; i++) {
    c_val[i] = v->val[i];
  }

  copy->size = v->size;
  copy->count = v->count;
  copy->val = c_val;

  return copy;
}

void destroy_uintx_t(uintx_t** v) {
  free((*v)->val);
  free(*v);
  *v = NULL;
}

void print_matrix_2x2(matrix_2x2 v) {
  printf("[MATRIX_2X2]: \n");
  printf("[ %lu %lu ]\n", v.m11, v.m12);
  printf("[ %lu %lu ]\n", v.m21, v.m22);
}

void print_uintx_t(uintx_t* v) {
  printf("[UINTX_T] size=%d, count=%d, value=", v->size, v->count);
  for (int i = v->count - 1; i >= 0; i--) {
    printf("%d", v->val[i]);
  }
  printf("\n");
}

// adds v2 to v1, returns v1
// TODO: check overflow
uintx_t* add_uintx_t(uintx_t* v1, uintx_t* v2) {
  int ptr;
  int carry = 0;
  int swap;
  uintx_t* res = init_uintx_t(UINTX_T_DEFAULT);
  for (ptr = 0; ptr < max(v1->count, v2->count) + 1; ptr++) {
    swap = v1->val[ptr] + v2->val[ptr] + carry;
    res->val[ptr] = swap % 10;
    carry = swap / 10;
  }

  int count = max(v1->count, v2->count); // check new length
  if (res->val[count] != 0) {
    count++;
  }
  res->count = count;
  return res;
}

uintx_t* sub_uintx_t(uintx_t* v1, uintx_t* v2) {
  int ptr;
  int borrowptr;
  uintx_t* res = init_uintx_t(UINTX_T_DEFAULT);
  for (ptr = 0; ptr < max(v1->count, v2->count); ptr++) {
    if (v1->val[ptr] < v2->val[ptr]) {
      borrowptr = ptr+1;
      while (v1->val[borrowptr] == 0 && borrowptr < v1->count) {
	v1->val[borrowptr] = 9;
	borrowptr++;
      }
      v1->val[borrowptr] -= 1;
      v1->val[ptr] += 10;
    }

    res->val[ptr] = v1->val[ptr] - v2->val[ptr];
  }

  res->count = v2->count;

  return res;
}

uintx_t* rshift_uintx_t(uintx_t* v, int amount) {
  for (int i = 0; i < amount; i++) {
    v->val[i] = v->val[i + amount];
    v->val[i + amount] = 0;
  }

  v->count -= amount;
  return v;
}

uintx_t* lshift_uintx_t(uintx_t* v, int amount) {
  for (int i = v->count - 1; i > v->count - 1 + amount; i--) {
    v->val[i] = v->val[i - amount];
    v->val[i - v->count + 1] = 0;
  }

  return v;
}

uintx_t* mask_uintx_t(uintx_t* v, int amount) {
  for (int i = v->count - amount; i < v->count; i++) {
    v->val[i] = 0;
  }

  v->count -= amount;
  return v;
}

matrix_2x2 mul_matrix_2x2(matrix_2x2 v1, matrix_2x2 v2) {
  uint64_t m1 = (v1.m11 + v1.m22) * (v2.m11 + v2.m22);
  uint64_t m2 = (v1.m21 + v1.m22) * v2.m11;
  uint64_t m3 = v1.m11 * (v2.m12 - v2.m22);
  uint64_t m4 = v1.m22 * (v2.m21 - v2.m11);
  uint64_t m5 = (v1.m11 + v1.m12) * v2.m22;
  uint64_t m6 = (v1.m21 - v1.m11) * (v2.m11 + v2.m12);
  uint64_t m7 = (v1.m12 - v1.m22) * (v2.m21 + v2.m22);

  return (matrix_2x2){
    .m11 = m1 + m4 - m5 + m7,
    .m12 = m3 + m5,
    .m21 = m2 + m4,
    .m22 = m1 - m2 + m3 + m6,
  };
}

matrix_2x2* pow_matrix_2x2_rec(matrix_2x2 v, int n, matrix_2x2** memo) {
  matrix_2x2* res = malloc(sizeof(matrix_2x2));
  if (n == 0) {
    *res = (matrix_2x2){ // faster if taken from memo?
      .m11 = 1,
      .m12 = 0,
      .m21 = 0,
      .m22 = 1,
    };
  } else if (n == 1) {
    *res = v;
  } else {
    int n1 = n / 2;
    int n2 = n / 2 + (n%2);

    if (memo[n1] == NULL) memo[n1] = pow_matrix_2x2_rec(v, n1, memo);
    if (memo[n2] == NULL) memo[n2] = pow_matrix_2x2_rec(v, n2, memo);

    *res = mul_matrix_2x2(*memo[n1], *memo[n2]);
  }

  return res;
}

matrix_2x2 pow_matrix_2x2(matrix_2x2 v, int n) {
  matrix_2x2** memo = malloc(sizeof(matrix_2x2) * n);
  matrix_2x2* res = pow_matrix_2x2_rec(v, n, memo);
  matrix_2x2 res_deref = *res;
  for (int i = 0; i < n; ++i) {
    free(memo[i]);
  }
  free(memo);
  free(res);
  return res_deref;
}

// Mul V1 and v2, return v1 
uintx_t* mul_uintx_t(uintx_t* v1, uintx_t* v2) {
  int max_num_length = max(v1->count, v2->count);
  int half_max_num_length = (max_num_length / 2) + (max_num_length % 2);

  uintx_t* a = rshift_uintx_t(copy_uintx_t(v1), half_max_num_length);
  uintx_t* b =  mask_uintx_t(copy_uintx_t(v1), half_max_num_length);
  uintx_t* c = rshift_uintx_t(copy_uintx_t(v2), half_max_num_length);
  uintx_t* d =  mask_uintx_t(copy_uintx_t(v2), half_max_num_length);

  uintx_t* sum_a_b = add_uintx_t(a, b);
  uintx_t* sum_c_d = add_uintx_t(c, d);
  uintx_t* z0 = mul_uintx_t(a, c);
  uintx_t* z1 = mul_uintx_t(sum_a_b, sum_c_d);
  uintx_t* z2 = mul_uintx_t(b, d);

  destroy_uintx_t(&a);
  destroy_uintx_t(&b);
  destroy_uintx_t(&c);
  destroy_uintx_t(&d);
  destroy_uintx_t(&sum_a_b);
  destroy_uintx_t(&sum_c_d);
  
  return v1;
}


uint64_t logarithmic(uint64_t n) {
  return n;
}

uint64_t dynamic_r(uint64_t n, uint64_t* memo) {
  if (n < 2) return n;
  else {
    if (memo[n-1] < 1) memo[n-1] = dynamic_r(n-1, memo);
    if (memo[n-2] < 1) memo[n-2] = dynamic_r(n-2, memo);
    return memo[n-1] + memo[n-2];
  }
}

uint64_t dynamic(uint64_t n) {
  uint64_t* memo = (uint64_t*)calloc(n, sizeof(uint64_t));
  return dynamic_r(n, memo);
}

uintx_t* dynamic_large_r(uint64_t n, uintx_t** memo) {
  if (n < 2) return init_uintx_t_alt(UINTX_T_DEFAULT, n);
  else {
    if (memo[n-1] == NULL) memo[n-1] = dynamic_large_r(n-1, memo);
    if (memo[n-2] == NULL) memo[n-2] = dynamic_large_r(n-2, memo);
    return add_uintx_t(memo[n-1], memo[n-2]);
  }
}

uintx_t* dynamic_large(uint64_t n) {
  uintx_t** memo = (uintx_t**)calloc(n, sizeof(uintx_t*));
  return dynamic_large_r(n, memo);
}

uint64_t recursive(uint64_t n) {
  if (n == 0) {
    return 0;
  } else if (n == 1) {
    return 1;
  } else {
    return recursive(n-1) + recursive(n-2);
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("No amount provided\n");
    printf("usage: %s <nth>-n", argv[0]);
    return 1;
  }

  uint64_t n = (uint64_t)atoi(argv[1]);

  /* uintx_t* v = init_uintx_t_alt(UINTX_T_DEFAULT, 653); */
  /* uintx_t* v2 = init_uintx_t_alt(UINTX_T_DEFAULT, 150); */
  /* uintx_t* subtr = sub_uintx_t(v, v2); */
  /* print_uintx_t(subtr); */


  matrix_2x2 v1 = (matrix_2x2){
    .m11 = 1,
    .m12 = 1,
    .m21 = 1,
    .m22 = 0,
  };

  clock_t s = clock();
  matrix_2x2 res = pow_matrix_2x2(v1, n);
  clock_t e = clock(); 
  print_matrix_2x2(res);
  printf("[MATRIX_2X2] Finished F(%lu) in %lfs\n", n, (double)(e - s) / CLOCKS_PER_SEC);
  
  clock_t s_dyn = clock();
  uint64_t res_dyn = dynamic(n);
  clock_t e_dyn = clock(); 
  printf("[DYNAMIC] Result: %lu\n", res_dyn);
  printf("[DYNAMIC] Finished F(%lu) in %lfs\n", n, (double)(e_dyn - s_dyn) / CLOCKS_PER_SEC);

  
  clock_t dyn_s = clock();
  uintx_t* fib_uintx = dynamic_large(n);
  clock_t dyn_e = clock();

  
  printf("[UINTX_T] finished F(%lu) in time=%lf\n", n, (double)(dyn_e - dyn_s) / CLOCKS_PER_SEC);
  print_uintx_t(fib_uintx);
  
  return 0;
}

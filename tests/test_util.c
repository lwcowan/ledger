
#include "../src/base/util.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void acquire_ref_dtor(void*);

static int allocate_test(void);
static int allocate_zero_test(void);
static int allocate_ref_test(void);
static int acquire_ref_test(void);
static int allocate_ref_zero_test(void);
static int string_test(void);
static int string_length_test(void);
static int null_string_dup_test(void);
static int string_cmp_test(void);
static int trivial_string_cmp_test(void);
static int zero_check_test(void);
static int itoa_test(void);
static int atoi_test(void);
static int string_ndup_test(void);
static int string_ncmp_test(void);
static int trivial_string_ncmp_test(void);

struct test_struct {
  int (*fn)(void);
  char const* name;
};

struct test_struct test_array[] = {
  { allocate_test, "allocate" },
  { allocate_zero_test, "allocate_zero" },
  { allocate_ref_test, "allocate (reference counted)" },
  { acquire_ref_test, "acquire (reference counted)" },
  { allocate_ref_zero_test, "allocate_zero (reference counted)" },
  { string_test, "string duplicate" },
  { string_length_test, "string length" },
  { null_string_dup_test, "null string duplicate" },
  { string_cmp_test, "string compare" },
  { trivial_string_cmp_test, "trivial string compare" },
  { zero_check_test, "zero check" },
  { itoa_test, "integer to string" },
  { atoi_test, "string to integer" },
  { string_ndup_test, "string length-restricted duplicate" },
  { string_ncmp_test, "string length-restricted compare" },
  { trivial_string_ncmp_test, "trivial string length-restricted compare" }
};


int allocate_test(void){
  void* ptr;
  ptr = ledger_util_malloc(15);
  if (ptr == NULL) return 0;
  ledger_util_free(ptr);
  return 1;
}

int allocate_zero_test(void){
  void* ptr;
  ptr = ledger_util_malloc(0);
  if (ptr != NULL){
    ledger_util_free(ptr);
    return 0;
  }
  return 1;
}

int allocate_ref_test(void){
  void* ptr;
  ptr = ledger_util_ref_malloc(15, NULL);
  if (ptr == NULL) return 0;
  ledger_util_ref_free(ptr);
  return 1;
}

static void acquire_ref_dtor(void* ptr){
  int** const num = (int**)ptr;
  (**num) -= 1;
  return;
}
int acquire_ref_test(void){
  int ok = 0;
  int num = 5;
  int** ptr;
  ptr = (int**)ledger_util_ref_malloc(15, &acquire_ref_dtor);
  if (ptr == NULL) return 0;
  else do {
    *ptr = &num;
    if (ledger_util_ref_acquire(ptr) != ptr) break;
    ledger_util_ref_free(ptr);
    if (num != 5) break;
    ledger_util_ref_free(ptr);
    if (num != 4) break;
    ok = 1;
  } while (0);
  if (num != 4) ledger_util_ref_free(ptr);
  return ok;
}

int allocate_ref_zero_test(void){
  void* ptr;
  ptr = ledger_util_ref_malloc(0, NULL);
  if (ptr != NULL){
    ledger_util_ref_free(ptr);
    return 0;
  }
  return 1;
}

int string_test(void){
  int result = 0;
  char const* text = "text text text";
  unsigned char* new_text;
  /* string duplication */do {
    int ok;
    new_text = ledger_util_ustrdup((unsigned char const*)text,&ok);
    if (!ok) break;
    if (new_text == NULL) break;
    if (strcmp((char const*)new_text, text)) break;
    if (strlen(text) != strlen((char const*)new_text)) break;
    result = 1;
  } while (0);
  ledger_util_free(new_text);
  return result;
}

int string_length_test(void){
  unsigned char buffer[] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x00,
      0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2e, 0x00 };
  if (ledger_util_ustrlen(buffer) != 6) return 0;
  return 1;
}

int trivial_string_cmp_test(void){
  unsigned char const static empty[1] = {0};
  unsigned char buffer[] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x00,
      0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2e, 0x00 };
  /* null vs. null */
  if (ledger_util_ustrcmp(NULL, NULL) != 0) return 0;
  /* empty vs. null */
  if (ledger_util_ustrcmp(empty, NULL) == 0) return 0;
  if (ledger_util_ustrcmp(NULL, empty) == 0) return 0;
  /* buffer vs. null */
  if (ledger_util_ustrcmp(buffer, NULL) == 0) return 0;
  if (ledger_util_ustrcmp(NULL, buffer) == 0) return 0;
  /* buffer vs. empty */
  if (ledger_util_ustrcmp(empty, buffer) == 0) return 0;
  if (ledger_util_ustrcmp(buffer, empty) == 0) return 0;
  /* buffer vs. buffer */
  if (ledger_util_ustrcmp(buffer, buffer) != 0) return 0;
  /* empty vs. empty */
  if (ledger_util_ustrcmp(empty, empty) != 0) return 0;
  return 1;
}

int string_cmp_test(void){
  unsigned char buffer[] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x00,
      0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2e, 0x00 };
  unsigned char other_buffer[] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x00,
      0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2e, 0x00 };
  /* equal strings */
  if (ledger_util_ustrcmp(buffer, other_buffer) != 0) return 0;
  if (ledger_util_ustrcmp(other_buffer, buffer) != 0) return 0;
  buffer[3] = 0x6f;
  /* unequal strings */
  if (ledger_util_ustrcmp(buffer, other_buffer) <= 0) return 0;
  if (ledger_util_ustrcmp(other_buffer, buffer) >= 0) return 0;
  other_buffer[3] = 0x6f;
  /* equal strings */
  if (ledger_util_ustrcmp(buffer, other_buffer) != 0) return 0;
  if (ledger_util_ustrcmp(other_buffer, buffer) != 0) return 0;
  /* uneven strings */
  other_buffer[6] = 0x20;
  if (ledger_util_ustrcmp(buffer, other_buffer) >= 0) return 0;
  if (ledger_util_ustrcmp(other_buffer, buffer) <= 0) return 0;
  return 1;
}

int null_string_dup_test(void){
  char const* text = NULL;
  /* string duplication */{
    int ok;
    unsigned char* new_text =
      ledger_util_ustrdup((unsigned char const*)text,&ok);
    if (!ok) return 0;
    if (new_text != NULL) return 0;
  }
  return 1;
}

int zero_check_test(void){
  unsigned char buffer[] = { 1, 0, 2, 0, 4 };
  /* string zero check */{
    if (ledger_util_uiszero(buffer,sizeof(buffer))) return 0;
    buffer[0] = 0;
    if (ledger_util_uiszero(buffer,sizeof(buffer))) return 0;
    buffer[2] = 0;
    if (ledger_util_uiszero(buffer,sizeof(buffer))) return 0;
    buffer[4] = 0;
    if (!ledger_util_uiszero(buffer,sizeof(buffer))) return 0;
    buffer[3] = 3;
    if (ledger_util_uiszero(buffer,sizeof(buffer))) return 0;
    buffer[1] = 9;
    if (ledger_util_uiszero(buffer,sizeof(buffer))) return 0;
  }
  return 1;
}

int itoa_test(void){
  unsigned char buffer[16];
  if (ledger_util_itoa(0, buffer,sizeof(buffer), 0) != 1) return 0;
  if (ledger_util_ustrcmp(buffer,
      (unsigned char const*)"0") != 0)
    return 0;
  if (ledger_util_itoa(20, buffer,sizeof(buffer), 1) != 3) return 0;
  if (ledger_util_ustrcmp(buffer,
      (unsigned char const*)"+20") != 0)
    return 0;
  if (ledger_util_itoa(-5362, buffer, sizeof(buffer), 1) != 5) return 0;
  if (ledger_util_ustrcmp(buffer,
      (unsigned char const*)"-5362") != 0)
    return 0;
  if (ledger_util_itoa(-5362, buffer, sizeof(buffer), 0) != 5) return 0;
  if (ledger_util_ustrcmp(buffer,
      (unsigned char const*)"-5362") != 0)
    return 0;
  return 1;
}

int atoi_test(void){
  if (ledger_util_atoi((unsigned char const*)"0") != 0) return 0;
  if (ledger_util_atoi((unsigned char const*)"+0") != 0) return 0;
  if (ledger_util_atoi((unsigned char const*)"-0") != 0) return 0;
  if (ledger_util_atoi((unsigned char const*)"+20") != 20) return 0;
  if (ledger_util_atoi((unsigned char const*)"-5362") != -5362) return 0;
  return 1;
}

int string_ndup_test(void){
  int result = 0;
  char const* text = "text text text";
  unsigned char* new_text;
  /* string duplication */do {
    int ok;
    new_text = ledger_util_ustrndup((unsigned char const*)text,7u,&ok);
    if (!ok) break;
    if (new_text == NULL) break;
    if (strcmp((char const*)new_text, "text te")) break;
    if (strlen(text) == strlen((char const*)new_text)) break;
    if (7u != strlen((char const*)new_text)) break;
    result = 1;
  } while (0);
  ledger_util_free(new_text);
  return result;
}

int trivial_string_ncmp_test(void){
  unsigned char const static empty[1] = {0};
  unsigned char buffer[] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x00,
      0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2e, 0x00 };
  /* null vs. null */
  if (ledger_util_ustrncmp(NULL, NULL, 4) != 0) return 0;
  /* empty vs. null */
  if (ledger_util_ustrncmp(empty, NULL, 4) == 0) return 0;
  if (ledger_util_ustrncmp(NULL, empty, 4) == 0) return 0;
  if (ledger_util_ustrncmp(empty, NULL, 0) == 0) return 0;
  if (ledger_util_ustrncmp(NULL, empty, 0) == 0) return 0;
  /* buffer vs. null */
  if (ledger_util_ustrncmp(buffer, NULL, 4) == 0) return 0;
  if (ledger_util_ustrncmp(NULL, buffer, 4) == 0) return 0;
  if (ledger_util_ustrncmp(buffer, NULL, 0) == 0) return 0;
  if (ledger_util_ustrncmp(NULL, buffer, 0) == 0) return 0;
  /* buffer vs. empty */
  if (ledger_util_ustrncmp(empty, buffer, 44) == 0) return 0;
  if (ledger_util_ustrncmp(buffer, empty, 44) == 0) return 0;
  if (ledger_util_ustrncmp(empty, buffer, 4) == 0) return 0;
  if (ledger_util_ustrncmp(buffer, empty, 4) == 0) return 0;
  if (ledger_util_ustrncmp(empty, buffer, 0) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, empty, 0) != 0) return 0;
  /* buffer vs. buffer */
  if (ledger_util_ustrncmp(buffer, buffer, 44) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, buffer, 4) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, buffer, 0) != 0) return 0;
  /* empty vs. empty */
  if (ledger_util_ustrncmp(empty, empty, 44) != 0) return 0;
  if (ledger_util_ustrncmp(empty, empty, 4) != 0) return 0;
  if (ledger_util_ustrncmp(empty, empty, 0) != 0) return 0;
  return 1;
}

int string_ncmp_test(void){
  unsigned char buffer[] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x00,
      0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2e, 0x00 };
  unsigned char other_buffer[] = { 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x21, 0x00,
      0x77, 0x6f, 0x72, 0x6c, 0x64, 0x2e, 0x00 };
  /* equal strings */
  if (ledger_util_ustrncmp(buffer, other_buffer, 44) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 44) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 0) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 4) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 4) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 0) != 0) return 0;
  buffer[3] = 0x6f;
  /* unequal strings */
  if (ledger_util_ustrncmp(buffer, other_buffer, 44) <= 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 44) >= 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 4) <= 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 4) >= 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 3) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 3) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 0) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 0) != 0) return 0;
  other_buffer[3] = 0x6f;
  /* equal strings */
  if (ledger_util_ustrncmp(buffer, other_buffer, 44) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 44) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 4) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 4) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 3) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 3) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 0) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 0) != 0) return 0;
  /* uneven strings */
  other_buffer[6] = 0x20;
  if (ledger_util_ustrncmp(buffer, other_buffer, 44) >= 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 44) <= 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 7) >= 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 7) <= 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 6) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 6) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 2) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 2) != 0) return 0;
  if (ledger_util_ustrncmp(buffer, other_buffer, 0) != 0) return 0;
  if (ledger_util_ustrncmp(other_buffer, buffer, 0) != 0) return 0;
  return 1;
}




int main(int argc, char **argv){
  int pass_count = 0;
  int const test_count = sizeof(test_array)/sizeof(test_array[0]);
  int i;
  printf("Running %i tests...\n", test_count);
  for (i = 0; i < test_count; ++i){
    int pass_value;
    printf("\t%s... ", test_array[i].name);
    pass_value = ((*test_array[i].fn)())?1:0;
    printf("%s\n",pass_value==0?"FAILED":"PASSED");
    pass_count += pass_value;
  }
  printf("...%i out of %i tests passed.\n", pass_count, test_count);
  return pass_count==test_count?EXIT_SUCCESS:EXIT_FAILURE;
}

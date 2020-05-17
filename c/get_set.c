#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "wasm.h"

#define own

// dw it's a datastore and I'm definitely not being lazy by using it like this
wasm_val_t im_lazy[] = {0, 0, 0};

// Print a Wasm value
void wasm_val_print(wasm_val_t val)
{
  switch (val.kind)
  {
  case WASM_I32:
  {
    printf("%" PRIu32, val.of.i32);
  }
  break;
  case WASM_I64:
  {
    printf("%" PRIu64, val.of.i64);
  }
  break;
  case WASM_F32:
  {
    printf("%f", val.of.f32);
  }
  break;
  case WASM_F64:
  {
    printf("%g", val.of.f64);
  }
  break;
  case WASM_ANYREF:
  case WASM_FUNCREF:
  {
    if (val.of.ref == NULL)
    {
      printf("null");
    }
    else
    {
      printf("ref(%p)", val.of.ref);
    }
  }
  break;
  }
}

// A function to print the value passed back from wasm.
own wasm_trap_t *print_callback(
    const wasm_val_t args[], wasm_val_t results[])
{
  printf("print: ");
  wasm_val_print(args[0]);
  printf("\n");
  return NULL;
}

// A function to get a value
own wasm_trap_t *get_callback(
    const wasm_val_t args[], wasm_val_t results[])
{
  printf("get: ");
  wasm_val_print(args[0]);
  printf("\n");

  wasm_val_copy(&results[0], &im_lazy[args[0].of.i32]);
  return NULL;
}

// A function to be called from Wasm code.
own wasm_trap_t *set_callback(
    const wasm_val_t args[], wasm_val_t results[])
{
  printf("set: ");
  wasm_val_print(args[0]);
  printf(", ");
  wasm_val_print(args[1]);
  printf("\n");

  im_lazy[args[0].of.i32] = args[1];

  return NULL;
}

int main(int argc, const char *argv[])
{
  // Initialize.
  wasm_engine_t *engine = wasm_engine_new();
  wasm_store_t *store = wasm_store_new(engine);

  // Load binary.
  FILE *file = fopen("../get_set.wasm", "rb");
  if (!file)
  {
    printf("> Error loading module!\n");
    return 1;
  }
  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  fseek(file, 0L, SEEK_SET);
  wasm_byte_vec_t binary;
  wasm_byte_vec_new_uninitialized(&binary, file_size);
  if (fread(binary.data, file_size, 1, file) != 1)
  {
    printf("> Error loading module!\n");
    return 1;
  }
  fclose(file);

  // Compile.
  own wasm_module_t *module = wasm_module_new(store, &binary);
  if (!module)
  {
    printf("> Error compiling module!\n");
    return 1;
  }

  wasm_byte_vec_delete(&binary);

  // Create external print functions.
  // Set
  own wasm_functype_t *set_type = wasm_functype_new_2_0(wasm_valtype_new_i32(), wasm_valtype_new_i32());
  own wasm_func_t *set_func = wasm_func_new(store, set_type, set_callback);

  // Get
  own wasm_functype_t *get_type = wasm_functype_new_1_1(wasm_valtype_new_i32(), wasm_valtype_new_i32());
  own wasm_func_t *get_func = wasm_func_new(store, get_type, get_callback);

  // Print fn
  own wasm_functype_t *print_type = wasm_functype_new_1_0(wasm_valtype_new_i32());
  own wasm_func_t *print_func = wasm_func_new(store, print_type, print_callback);

  wasm_functype_delete(set_type);
  wasm_functype_delete(get_type);
  wasm_functype_delete(print_type);

  // Instantiate.
  const wasm_extern_t *imports[] = {
      wasm_func_as_extern(set_func), wasm_func_as_extern(get_func), wasm_func_as_extern(print_func)};
  own wasm_instance_t *instance =
      wasm_instance_new(store, module, imports, NULL);
  if (!instance)
  {
    printf("> Error instantiating module!\n");
    return 1;
  }

  wasm_func_delete(set_func);
  wasm_func_delete(get_func);
  wasm_func_delete(print_func);

  // Extract export.
  own wasm_extern_vec_t exports;
  wasm_instance_exports(instance, &exports);
  if (exports.size == 0)
  {
    printf("> Error accessing exports!\n");
    return 1;
  }
  const wasm_func_t *run_func = wasm_extern_as_func(exports.data[0]);
  if (run_func == NULL)
  {
    printf("> Error accessing export!\n");
    return 1;
  }

  wasm_module_delete(module);
  wasm_instance_delete(instance);

  // Call.
  wasm_val_t args[2];
  args[0].kind = WASM_I32;
  args[0].of.i32 = 3;
  args[1].kind = WASM_I32;
  args[1].of.i32 = 4;
  wasm_val_t results[1];
  if (wasm_func_call(run_func, args, results))
  {
    printf("> Error calling function!\n");
    return 1;
  }

  wasm_extern_vec_delete(&exports);

  wasm_store_delete(store);
  wasm_engine_delete(engine);

  return 0;
}
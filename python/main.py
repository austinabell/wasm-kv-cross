from wasmtime import Store, Module, Instance, Func, FuncType, ValType

vals = dict([])


def set_f(k, v):
  print("set: {}, {}".format(k, v))
  vals[k] = v


def get_f(k):
  print("get: {}".format(k))
  if k in vals:
    return vals[k]
  else:
    return 0


def print_f(v):
  print("print: {}".format(v))


store = Store()
module = Module.from_file(store, '../get_set.wat')


set_type = FuncType([ValType.i32(), ValType.i32()], [])
set_func = Func(store, set_type, set_f)

get_type = FuncType([ValType.i32()], [ValType.i32()])
get_func = Func(store, get_type, get_f)

print_type = FuncType([ValType.i32()], [])
print_func = Func(store, print_type, print_f)


instance = Instance(module, [set_func, get_func, print_func])
run = instance.exports['run']
run()

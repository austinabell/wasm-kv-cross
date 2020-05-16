(module
  (import "" "set" (func $set (param i32)))
  (import "" "get" (func $get (param i32) (result i32)))
  (func (export "run")
    i32.const 0
    call $set
    i32.const 1
    call $set
    i32.const 2
    call $get
    call $set
  )
)
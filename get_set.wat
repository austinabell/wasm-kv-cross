(module
  (import "" "set" (func $set (param i32 i32)))
  (import "" "get" (func $get (param i32) (result i32)))
  (import "" "print" (func $print (param i32)))
  (func (export "run")
    i32.const 0
    call $get
    call $print

    i32.const 0
    i32.const 2
    call $set
    i32.const 0
    call $get
    call $print

    i32.const 1
    i32.const 3
    call $set
    i32.const 1
    call $get
    call $print
  )
)
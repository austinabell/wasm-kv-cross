package main

import (
	"fmt"

	wasmtime "github.com/bytecodealliance/wasmtime-go"
)

func main() {
	store := wasmtime.NewStore(wasmtime.NewEngine())
	module, err := wasmtime.NewModuleFromFile(store, "../get_set.wat")
	check(err)

	vals := map[int32]int32{}

	set := wasmtime.WrapFunc(store, func(k, v int32) {
		vals[k] = v
		fmt.Printf("set: %d, %d\n", k, v)
	})

	get := wasmtime.WrapFunc(store, func(k int32) int32 {
		fmt.Printf("get: %d\n", k)
		return vals[k]
	})

	print := wasmtime.WrapFunc(store, func(k int32) {
		fmt.Printf("print: %d\n", k)
	})

	instance, err := wasmtime.NewInstance(module, []*wasmtime.Extern{set.AsExtern(), get.AsExtern(), print.AsExtern()})
	check(err)

	run := instance.GetExport("run").Func()
	_, err = run.Call()
	check(err)
}

func check(err error) {
	if err != nil {
		panic(err)
	}
}

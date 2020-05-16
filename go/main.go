package main

import (
	"fmt"

	wasmtime "github.com/bytecodealliance/wasmtime-go"
)

func main() {
	store := wasmtime.NewStore(wasmtime.NewEngine())
	module, err := wasmtime.NewModuleFromFile(store, "../hello.wat")
	check(err)

	set := wasmtime.WrapFunc(store, func(i int32) {
		fmt.Println("set: ", i)
	})

	get := wasmtime.WrapFunc(store, func(i int32) int32 {
		return i * 2
	})

	instance, err := wasmtime.NewInstance(module, []*wasmtime.Extern{set.AsExtern(), get.AsExtern()})
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

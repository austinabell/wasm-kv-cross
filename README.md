# wasm-kv-cross

Just a POC to test Wasm execution with callbacks for store gets and puts (mocking db interactions) which will be handled by the respective native language.

This will probably not be expanded on until wasm interface types are stabilized to be used with more general functionality rather than limited integer gets and puts.

### Rust usage
```
cd rust
cargo run
```

### Go usage
```
cd go
go run ./main.go
```

### Python usage
```
cd python
pip3 install wasmtime
python3 main.py
```

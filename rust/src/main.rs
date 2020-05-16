use std::error::Error;
use wasmtime::*;

fn main() -> Result<(), Box<dyn Error>> {
    let store = Store::default();
    let module = Module::from_file(&store, "../hello.wat")?;

    let set = Func::wrap(&store, |param: i32| {
        println!("set: {}", param);
    });

    let get = Func::wrap(&store, |param: i32| param * 2);

    let instance = Instance::new(&module, &[set.into(), get.into()])?;

    let run = instance
        .get_func("run")
        .expect("`run` was not an exported function");

    let run = run.get0::<()>()?;

    Ok(run()?)
}
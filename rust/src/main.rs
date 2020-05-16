use std::cell::RefCell;
use std::collections::HashMap;
use std::error::Error;
use std::rc::Rc;
use wasmtime::*;

fn main() -> Result<(), Box<dyn Error>> {
    let store = Store::default();
    let module = Module::from_file(&store, "../get_set.wat")?;

    let values: Rc<RefCell<HashMap<i32, i32>>> = Default::default();

    let set_values = values.clone();
    let set = Func::wrap(&store, move |k: i32, v: i32| {
        set_values.borrow_mut().insert(k, v);
        println!("set: {}, {}", k, v);
    });

    let get = Func::wrap(&store, move |k: i32| {
        println!("get: {}", k);
        values.borrow().get(&k).cloned().unwrap_or_default()
    });

    let print = Func::wrap(&store, |param: i32| {
        println!("print: {}", param);
    });

    let instance = Instance::new(&module, &[set.into(), get.into(), print.into()])?;

    let run = instance
        .get_func("run")
        .expect("`run` was not an exported function");

    let run = run.get0::<()>()?;

    Ok(run()?)
}

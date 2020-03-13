open Grammar;
type func_exp = {
  name: string,
  var: string,
  exp: node,
};

type func_static = {
  name: string,
  eval: float => float,
};

type func =
  | UserFunc(func_exp)
  | StaticFunc(func_static);

let static_functions =
  [|
    // cos
    {name: "cos", eval: cos},
    {name: "cosh", eval: cosh},
    // sin
    {name: "sin", eval: sin},
    {name: "sinh", eval: sinh},
    // tan
    {name: "tan", eval: tan},
    {name: "tanh", eval: tanh},
    {name: "log", eval: log},
    {name: "neg", eval: (~-.)},
    {
      name: "!",
      eval: {
        let rec f = n =>
          if (n < 2.0) {
            1.0;
          } else {
            n *. f(n -. 1.0);
          };
        f;
      },
    },
  |]
  |> Array.map(x => StaticFunc(x));

type operator = {
  name: string,
  eval: (float, float) => float,
  identity: float,
};

let operators = [|
  {name: "+", identity: 0.0, eval: (+.)},
  {name: "-", identity: 0.0, eval: (-.)},
  {name: "*", identity: 1.0, eval: ( *. )},
  {name: "/", identity: 1.0, eval: (/.)},
  {name: "^", identity: 1.0, eval: ( ** )},
  {name: "_", identity: 1.0, eval: (a, b) => b ** (1.0 /. a)},
  {name: "mod", identity: max_float, eval: mod_float},
|];

let eval_op = (op, f) =>
  switch (Belt.Array.getBy(operators, ({name}) => name == op)) {
  | Some(def) => f(def)
  | None =>
    raise(
      Js.Exn.raiseReferenceError(
        {j|Operator ($op) is not defined in the current scope|j},
      ),
    )
  };

type var = {
  name: string,
  val_: float,
};

let is_op = c => Array.exists(({name}: operator) => name == c, operators);
let is_suffix = c => ["!"] |> List.exists(o => o == c);
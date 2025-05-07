#let sol = content => {
  box([*Solution: * #content], stroke: 1pt, inset: 10pt, width: 100%)
}

// Solution Example:
// #let t1 = (
//   [Yousr Ans],
//   [Yousr Ans],
//   [Yousr Ans],
//   [Yousr Ans],
// )
//
// #let t2-3 = sol[
//   Your Ans
// ]
//
// $ 1 + 1 = 2 $ for math equation

#let t1 = (
  [F],
  [F],
  [F],
  [F],
)

#let t2-1 = (
  [32 btyes],
  [256 bytes],
  [2-way set associative],
  [8],
  [4],
  [5, 2, 5],
)

#let t2-2 = (
  [Miss],
  [Miss],
  [Miss],
  [Replacement],
  [Replacement],
  [Miss],
  [Replacement],
  [Hit],
  [Replacement],
  [Replacement],
  [Hit],
  [Hit],
  [Replacement],
  [Hit],
)

#let t2-3 = sol[
  Change the cache associativity to fully associative.
]
#let t3-3 = v(5cm)
#let t4-1 = v(5cm)
#let t4-2 = v(5cm)

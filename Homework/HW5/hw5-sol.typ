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

#let t3-1 = sol[
  $8% times 35% times 10% = 0.28%$
]

#let t3-2 = sol[
  $
  "AMAT" = 2 "cycle" + 8% times (12 "cycle" + 35% times (44 "cycle" + 10% times (80 "ns")))
  $

  Since the CPU runs at 4GHz, a cycle takes 0.25ns.

  $
  "AMAT" = 0.5 "ns" + 0.08 times (3 "ns" + 0.35 times (11 "ns" + 0.10 times (80 "ns"))) = 1.272 "ns"
  $
]

#let t3-3 = sol[
  $
  "AMAT" = 0.8 "ns" + 0.06 times (3 "ns" + 0.35 times (11 "ns" + 0.10 times (80 "ns"))) = 1.379 "ns"
  $
]

from math import gcd
from random import randint, seed

def gen_case(l: int, r: int, state: set) -> tuple:
    a = randint(l, r)
    b = randint(l, r)
    g = gcd(a, b)
    while a == g or b == g or g <= 500 or (a, b, g) in state:
        a = randint(l, r)
        b = randint(l, r)
        g = gcd(a, b)
    state.add((a, b, g))
    return a, b, g

def gen_cases(n: int, l: int, r: int) -> list:
    state = set()
    return [gen_case(l, r, state) for _ in range(n)]

if __name__ == '__main__':
    seed()
    cases = gen_cases(10, 1000, 20000)
    for a, b, g in cases:
        print(a, b, g)

def solve1():
    digits = [0b0110, 0b1001, 0b1111, 0b1001, 0b1001]
    for i in range(5):
        print(f"The {i + 1}-th dencimal digit is {digits[i]}")
        
def solve2():
    def print_char(max_bit):
        for digit in digits:
            for i in reversed(range(max_bit)):
                if (2 ** i) & digit:
                    print("■", end="")
                else:
                    print("□", end="")
            print()
    digits = [7, 8, 16, 8, 7]
    print_char(5)
    
    print()
    
    digits = [0xF, 0x8, 0xF, 0x8, 0x8]
    print_char(4)

def solve3():
    # 1 0 0 1
    # 1 0 0 1
    # 1 1 1 1
    # 1 0 0 1
    # 1 0 0 1
    
    # 0x9 0x9 0xF 0x9 0x9
    # 0x99F99
    
    # 1 0 0 1
    # 1 1 0 1
    # 1 0 1 1
    # 1 0 0 1
    
    # 0x9 0xD 0xB 0x9
    # 0x9DB9
    
    print()
    
    print(f"The exadecimal representation for letter 'H' is 0x99F99")
    print(f"The exadecimal representation for letter 'N' is 0x9DB9")
    
if __name__ == '__main__':
    print("Answer to problem 1")
    solve1()
    print()
    
    print("Answer to problem 2")
    solve2()
    print()
    
    print("Answer to problem 3")
    solve3()
    print()
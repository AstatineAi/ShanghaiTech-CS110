import sys

class TruthTable:
    def __init__(self):
        self.table = []
        self.variables = []
        self.outputs = []
        self.num_vars = 0
        self.num_outs = 0
        self.out_size = 0

    def build(self):
        self.num_vars = int(input("Enter the number of variables: "))
        self.variables = [(input(f"Enter variable {i+1}: "), int(
            input(f"Enter variable {i+1} size (bit): "))) for i in range(self.num_vars)]
        self.num_outs = int(input("Enter the number of outputs: "))
        self.outputs = [(input(f"Enter output {i+1}: "), int(
            input(f"Enter output {i+1} size (bit): "))) for i in range(self.num_outs)]
        self.var_size = sum(size for _, size in self.variables)
        self.out_size = sum(size for _, size in self.outputs)
        self.table = [[-1] * self.out_size for _ in range(2 ** self.var_size)]
        
    def add_insn(self):
        insn_table: list[int] = []
        out = 0
        for v in self.variables:
            v_name, v_size = v
            v_value = input(f"Enter value for {v_name}: ")
            if v_value == "-":
                insn_table += [-1] * v_size
            else:
                v_value = int(v_value, 2)
                insn_table += [int(x) for x in bin(v_value)[2:].zfill(v_size)]
        for o in self.outputs:
            o_name, o_size = o
            o_value = input(f"Enter value for {o_name}: ")
            o_value = int(o_value)
            assert o_value < (1 << o_size)
            out = (out << o_size) | o_value
        index_list: list[int] = []
        index: list[int] = [0] * len(insn_table)
        def dfs(depth: int):
            if depth == len(insn_table):
                idx = int("".join(str(x) for x in index), 2)
                index_list.append(idx)
                return
            if insn_table[depth] == -1:
                for i in range(2):
                    index[depth] = i
                    dfs(depth + 1)
            else:
                index[depth] = insn_table[depth]
                dfs(depth + 1)
        dfs(0)
        for idx in index_list:
            for i in range(self.out_size):
                bit = self.out_size - 1 - i
                assert self.table[idx][i] == -1, f"Error: {self.table[idx][i]} already taken"
                self.table[idx][i] = (out >> bit) & 1
        print("Instruction added successfully")
        
    def export_table(self):
        file_name = input("Enter the file name to export the table: ")
        if not file_name.endswith(".txt"):
            file_name += ".txt"
        with open(file_name, "w") as f:
            for v_name, v_size in self.variables:
                if v_size == 1:
                    f.write(f"{v_name} ")
                else:
                    f.write(f"{v_name}[{v_size-1}..0] ")
            f.write("| ")
            for o_name, o_size in self.outputs:
                if o_size == 1:
                    f.write(f"{o_name} ")
                else:
                    f.write(f"{o_name}[{o_size-1}..0] ")
            f.write("\n")
            # print(f"\n\n\n{self.var_size}\n\n", file=sys.stderr)
            for i in range(2 ** self.var_size):
                tot_bit = 0
                i_bin: str = bin(i)[2:].zfill(self.var_size)
                for _, v_size in self.variables:
                    f.write(f"{i_bin[tot_bit:tot_bit+v_size]} ")
                    tot_bit += v_size
                f.write("| ")
                tot_bit = 0
                for _, o_size in self.outputs:
                    out_bits = self.table[i][tot_bit:tot_bit+o_size]
                    out_str = "".join("-" if bit == -1 else str(bit) for bit in out_bits)
                    f.write(f"{out_str} ")
                    tot_bit += o_size
                f.write("\n")


def main():
    table = TruthTable()
    table.build()
    print("1. Add instruction")
    print("2. Print table")
    print("3. Exit")
    while True:
        choice = input("Enter your choice: ")
        if choice == "1":
            table.add_insn()
        elif choice == "2":
            table.export_table()
        elif choice == "3":
            break
        else:
            print("Invalid choice")


if __name__ == "__main__":
    main()

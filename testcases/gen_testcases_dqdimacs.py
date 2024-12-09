import os
import subprocess

EXEC_PATH = "../build/ramsey"
OUTPUT_PATH = "./dqdimacs/"

if not os.path.exists(OUTPUT_PATH):
    os.makedirs(OUTPUT_PATH)
if not os.path.exists(EXEC_PATH):
    print(f"Executable not found at {EXEC_PATH}")
    exit(1)

r = {(3, 3): 6, (3, 4): 9, (3, 5): 14, (3, 6): 18, (4, 4): 18, (4, 5): 25}
r_unknown = {(4, 6): (35, 40), (5, 5): (43, 48)}

for (s, t), n in r.items():
    for i in range(n - 3, n + 3):
        subprocess.run(f"{EXEC_PATH} -n {i} -s {s} -t {t} --tseitin --output {OUTPUT_PATH}", shell=True)
        if i < n:
            os.rename(f"{OUTPUT_PATH}R_{s}_{t}_{i}.dqdimacs", f"{OUTPUT_PATH}R_{s}_{t}_{i}_sat.dqdimacs")
        else:
            os.rename(f"{OUTPUT_PATH}R_{s}_{t}_{i}.dqdimacs", f"{OUTPUT_PATH}R_{s}_{t}_{i}_unsat.dqdimacs")
            
for (s, t), (n1, n2) in r_unknown.items():
    for i in range(n1, n2 + 1):
        subprocess.run(f"{EXEC_PATH} -n {i} -s {s} -t {t} --tseitin --output {OUTPUT_PATH}", shell=True)
        os.rename(f"{OUTPUT_PATH}R_{s}_{t}_{i}.dqdimacs", f"{OUTPUT_PATH}R_{s}_{t}_{i}_unknown.dqdimacs")
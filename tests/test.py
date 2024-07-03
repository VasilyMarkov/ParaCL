from subprocess import Popen, PIPE, STDOUT
import subprocess
from generators import *
from pathlib import Path
from os import listdir
from os.path import isfile, join

def runApp(app, input_data):
    pipe = Popen([app], stdout=PIPE, stdin=PIPE)
    out = pipe.communicate(input=bytes(input_data, "UTF-8"))
    return out[0].decode()

def runTest(app, generator):
    expression = ''.join(generator(3))
    result = 0
    try:
        print(expression)
        result = int(runApp(app, expression))
    except Exception as e:
        ...
    return result

def end_to_end(app): 
    print("Running tests...")
    for i in range(5): 
        result = runTest(app, gen_correct_expression)
        print(f'Result: {result}')
    print("Tests finished.")


def get_file_list(path):
    file_list = [f for f in listdir(path) if isfile(join(path, f))]
    return file_list

files = get_file_list(str(Path.cwd()) + "/tests/data/")
files.sort()
in_files = ["../tests/data/"+filename for filename in files if filename.endswith('.in')]
out_files = [str(Path.cwd()) + "/tests/data/" +filename for filename in files if filename.endswith('.out')]
app = "./paracl"

paracl_out = [subprocess.run([app, file], capture_output=True, text=True).stdout for file in in_files]
paracl_out = [i.replace('\n', '') for i in paracl_out]

files_out = []
for file in out_files:
    with open(file, 'r') as f:
        files_out.append(f.read())

files_out = [i.replace('\n', '') for i in files_out]


RED = '\033[31m'
GREEN = '\033[32m'
BLUE = '\033[34m'
RESET = '\033[0m'

print(f'{GREEN}[------------]{RESET}')
passed = True
for paracl, answer, in_file, out_file in zip(paracl_out, files_out, in_files, out_files):
    print(f'{GREEN}[ RUN        ]{RESET}' + f'{in_file}')
    if paracl == answer:
        print(f'{GREEN}[         OK ]{RESET}' + f' {paracl} == {answer}')
    else:
        passed = False
        print(f'{RED}[   FAILED   ]{RESET}')

if passed:
    print(f'{GREEN}[   PASSED   ]{RESET}')
else:
    print(f'{RED}[   FAILED   ]{RESET}')
print(f'{GREEN}[------------]{RESET}')
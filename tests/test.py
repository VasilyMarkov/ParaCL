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

app = "./paracl"
file = "../tests/data/"+files[1]
print(runApp(app, file))
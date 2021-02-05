import os
INPUT_FILE_DIR = './tests/lab1/testcase'
OUTPUT_FILE_DIR = './tests/lab1/token'

all_files = os.listdir(INPUT_FILE_DIR)
target_files = []
for f in all_files:
    if f.endswith(".cminus"):
        target_files.append(f)
print("Find %d files" % len(target_files))

if not os.path.exists(OUTPUT_FILE_DIR):
    os.makedirs(OUTPUT_FILE_DIR)

for f in target_files:
    input_file_path = os.path.join(INPUT_FILE_DIR, f)
    output_file = f.split('.cminus')[0] + '.tokens'
    output_file_path = os.path.join(OUTPUT_FILE_DIR, output_file)
    os.system('./build/lexer %s %s' % (input_file_path, output_file_path))


    
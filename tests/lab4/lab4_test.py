#!/usr/bin/env python3
import subprocess
# { name: need_input }
testcases = {
    "01": True,
    "02": False,
    "03": False,
    "04": False,
    "05": False,
    "06": False,
    "07": False,
    "08": False,
    "09": False,
    "10": False,
    "11": False,
    "12": True,
}

def eval():
    EXE_PATH = "../../build/cminusfc"
    TEST_BASE_PATH = "./testcases/"
    print('===========TEST START===========')
    for case in testcases:
        print('Case %s:' % case, end='')
        TEST_PATH = TEST_BASE_PATH + case
        INPUT_PATH = TEST_BASE_PATH + case + '.in'
        OUTPUT_PATH = TEST_BASE_PATH + case + '.out'
        need_input = testcases[case]

        COMMAND = [TEST_PATH]

        result = subprocess.run([EXE_PATH, TEST_PATH + ".cminus"], stderr=subprocess.PIPE)
        if result.returncode == 0:
            input_option = None
            if need_input:
                with open(INPUT_PATH, "rb") as fin:
                    input_option = fin.read()

            try:
                result = subprocess.run(COMMAND, input=input_option, stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=1)
                with open(OUTPUT_PATH, "rb") as fout:
                    if result.stdout == fout.read():
                        print('\t\033[32mSuccess\033[0m')
                    else:
                        print('\t\033[31mFail\033[0m')
            except Exception as _:
                print('\t\033[31mFail\033[0m')
            finally:
                subprocess.call(["rm", "-rf", TEST_PATH, TEST_PATH + ".o"])

        else:
            print('\t\033[31mFail\033[0m')

    print('============TEST END============')


if __name__ == "__main__":
    eval()

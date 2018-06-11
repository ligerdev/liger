import sys
file_name = sys.argv[1]
with open(file_name) as f:
    for line in f:
        if len(line) == 0:
            continue

        start = line.find('<error ')
        if start > -1:
            raise Exception('Cppcheck failed')


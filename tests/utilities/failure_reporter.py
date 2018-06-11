import os
import sys
from tabulate import tabulate

path = sys.argv[1]
print('Check results in folder', path)
failure = False
res = []
for filename in os.listdir(path):
    if filename.endswith('.xml') and filename.startswith('test_'):
        fullname = os.path.join(path, filename)
        #print(os.path.abspath(fullname))
        with open(fullname) as f:
            f.readline()
            content = f.readline()
            if(len(content) == 0):
                continue

            start = content.find('name="')+6
            end = content.find('">')
            name = content[start:end]

            start = content.find('tests="')+7
            end = content.find('" name=')
            print(content)
            nb_tests = int(content[start:end])

            start = content.find('failures=')
            end = content.find('" tests=')
            nb_failure = int(content[start+10:end])

            res.append([name, nb_tests, nb_failure])

            if int(nb_failure) > 0:
                failure = True
print('##################### Unit Test #####################\n')
print(tabulate(res, headers=['Test','Success', 'Failure']))
print('\n##################### Unit Test #####################\n')

if failure:
    raise Exception('Failed')

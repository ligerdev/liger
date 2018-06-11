#!/usr/bin/python

from jinja2 import Environment, FileSystemLoader
import sys, os, re

##################### User Defined Variables #####################
problem_path = '../../problem'
path_to_tigon = 'E:/Projects/Liger/src/libs/tigon'
ProblemName = 'TestProb'
ProblemDescription = """Add problem description here"""
nVar   = 5;
nParam = 5;
nObj   = 5;
nCstr  = 5;


#################### Do NOT Edit below this line ##################
###################################################################
print('* problem path:')
print('\t' + problem_path)
# Check if direcoty exsits
if not os.path.exists(problem_path):
    os.makedirs(problem_path)
else:
    print('Folder exists')
    print('Overwite folder ' + problem_path)


file_list = [ProblemName.lower() + '.pro',
             ProblemName.lower() + '.h'  ,
             ProblemName.lower() + '.cpp']

template_list = [ 't_tigonproblemsharedlibrary.pro',
                  't_tigonproblemsharedlibraryheader.h',
                  't_tigonproblemsharedlibrarysource.cpp']

params = {'path_to_tigon': path_to_tigon,
          'ProblemName'  : ProblemName,
          'ProblemDescription': ProblemDescription,
          'nVar'         : nVar,
          'nParam'       : nParam,
          'nObj'         : nObj,
          'nCstr'        : nCstr}

env = Environment(loader=FileSystemLoader('Templates/Tigon'),
                  trim_blocks=True, lstrip_blocks =True)

print('* Generating files:')

for i in range(0, len(file_list)):
    file_name = problem_path + '/' + file_list[i]
    print('\t' + str(i) + '. ' + template_list[i] + '\n\t    ==> ' + file_name)
    op = env.get_template( template_list[i] )
    r_op = op.render( params )
    f_op = open( file_name , 'w' )
    f_op.write( r_op )
    f_op.close()

               

#!/usr/bin/python
import fileinput, glob, string, sys, os
from os.path import join
import datetime

import shutil, errno

liger_version = "0.5.5"

# dont_include = ['.git', 'bin', 'share', 'DevUtils', '.obj', '*.pyc','*.','corel','*.cdr','*.user']

if(len(sys.argv)==2):
  liger_source_dir = sys.argv[1]
  thisDirectory    = os.getcwd()
else:
  print "Must specify Liger source directory."
  exit()

liger_release_dir = thisDirectory + "/liger-" + liger_version + "-src"

if(os.path.exists(liger_release_dir)):
  shutil.rmtree(liger_release_dir)

shutil.copytree(liger_source_dir,liger_release_dir, 
  ignore=shutil.ignore_patterns('.git', 'DevUtils', '.gitignore','Liger_ins',
                                '.obj', '*.pyc','*.','corel','*.cdr',
                                '*.user.*','*.user'))

# ignore_flist = ['.git', 'bin', 'share', 'DevUtils', '.obj', '*.pyc','*.','corel','*.cdr','*.user']
# ignore_dlist = ['.git', 'bin', 'share', 'DevUtils', '.obj', '*.pyc','*.','corel','*.cdr','*.user']
# for root, dirs, files in os.walk(liger_source_dir):
#   if(dirs in ignore_dlist):
#     continue
#   for f in files:
#     if(f in ignore_flist):
#       continue

#     inbuff  = os.path.join(root,f)
#     outbuff = os.path.join(liger_release_dir,f)
#     print inbuff
#     print outbuff

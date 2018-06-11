#!/usr/bin/python

from jinja2 import Environment, FileSystemLoader
import sys

#'DTLZ1' is currently the "master" node which contains manual changes.
problems_list = ['DTLZ1','DTLZ2','DTLZ3','DTLZ4','DTLZ5','DTLZ6','DTLZ7',
                 'ZDT1','ZDT2','ZDT3','ZDT4','ZDT5','ZDT6',
                 #'WFG1','WFG2','WFG3','WFG4','WFG5','WFG6','WFG7','WFG8','WFG9',
                 'WFG1','WFG2','WFG3','WFG4','WFG5','WFG6','WFG8','WFG9',
                 'LZ09_F1','LZ09_F2','LZ09_F3','LZ09_F4','LZ09_F5','LZ09_F6','LZ09_F7','LZ09_F8','LZ09_F9']
problems_min_obj = [2,2,2,2,2,2,2,
                    2,2,2,2,2,2,
                    2,2,2,2,2,2,2,2,
                    2,2,2,2,2,3,2,2,2]
problems_max_obj = [9999,9999,9999,9999,9999,9999,9999,
                    2,   2,   2,   2,   2,   2,
                    9999,9999,9999,9999,9999,9999,9999,9999,
                    2,2,2,2,2,3,2,2,2]
problems_min_nvar= [12,12,12,12,12,12,12,
                    30,30,30,10,11,10,
                    24,24,24,24,24,24,24,24,
                    30,30,30,30,30,30,30,30,30]
problems_max_nvar= [9999,9999,9999,9999,9999,9999,9999,
                    9999,9999,9999,9999,9999,9999,
                    9999,9999,9999,9999,9999,9999,9999,9999,
                    9999,9999,9999,9999,9999,9999,9999,9999,9999]

# Temp
# problems_list = ['Simple1']
# problems_min_obj = [2]
# problems_max_obj = [2]
# problems_min_nvar = [3]
# problems_max_nvar = [3]

# ClassType:: 1 : Algorithms, 2 : Operators, 3 : Externals, 4 : Visualization, 5 : Problems, 6 : root dir
ClassType = 5

operator_type = 'selection'
plugin_name   = 'qmetal'
MinNobj       = 2
MinNvar       = 10
Namespace     = 'QMetal'
#StringName    = 'WFG9'
ThingsList = problems_list

for StringName in ThingsList:
  idx = ThingsList.index(StringName)
  if ClassType==1:
    ClassName = 'QAlgo'+StringName+'Node'
  elif ClassType==2:
    ClassName = 'QOp'+StringName+'Node'
  elif ClassType==3:
    ClassName = 'QEx'+StringName+'Node'
  elif ClassType==4:
    ClassName = 'QViz'+StringName+'Node'
  elif ClassType==5:
    ClassName = 'QProb'+StringName+'Node'
  elif ClassType==6:
    ClassName = 'Q'+StringName+'Node'
        
  
  env = Environment(loader=FileSystemLoader('Templates'))
  
  if ClassType==1:
    plugin_path = '../src/plugins/'+plugin_name+'/algorithms/'
    op_head  = env.get_template('t_algorithmnodehead.h')
    op_src   = env.get_template('t_algorithmnodesource.cpp')
    op_fhead = env.get_template('t_algorithmnodefactoryhead.h')
    op_fsrc  = env.get_template('t_algorithmnodefactorysource.cpp')
  elif ClassType==2:
    plugin_path = '../src/plugins/'+plugin_name+'/operators/'+operator_type+'/'
    op_head  = env.get_template('t_operatornodehead.h')
    op_src   = env.get_template('t_operatornodesource.cpp')
    op_fhead = env.get_template('t_operatornodefactoryhead.h')
    op_fsrc  = env.get_template('t_operatornodefactorysource.cpp')
  # elif ClassType==3:
  #   plugin_path = '../src/plugins/'+plugin_name+'/externals/'
  #   op_head  = env.get_template('t_externalnodehead.h')
  #   op_src   = env.get_template('t_externalnodesource.cpp')
  #   op_fhead = env.get_template('t_externalnodefactoryhead.h')
  #   op_fsrc  = env.get_template('t_externalnodefactorysource.cpp')
  elif ClassType==4:
    plugin_path = '../src/plugins/'+plugin_name+'/'
    op_head  = env.get_template('t_viznodehead.h')
    op_src   = env.get_template('t_viznodesource.cpp')
    op_fhead = env.get_template('t_viznodefactoryhead.h')
    op_fsrc  = env.get_template('t_viznodefactorysource.cpp')
  elif ClassType==5:
    plugin_path = '../src/plugins/'+plugin_name+'/problems/'
    dialog_path = '../src/plugins/'+plugin_name+'/dialogs/'
    op_head    = env.get_template('t_probnodehead.h')
    op_src     = env.get_template('t_probnodesource.cpp')
    op_fhead   = env.get_template('t_probnodefactoryhead.h')
    op_fsrc    = env.get_template('t_probnodefactorysource.cpp')
    op_frmhead = env.get_template('t_probnodeformhead.h')
    op_frmsrc  = env.get_template('t_probnodeformsource.cpp')
    op_frmui   = env.get_template('t_probnodeform.ui')
  # elif ClassType==6:
  #   plugin_path = '../liger_gui/src/plugins/'+plugin_name+'/'
  #   op_head  = env.get_template('t_gennodehead.h')
  #   op_src   = env.get_template('t_gennodesource.cpp')
  #   op_fhead = env.get_template('t_gennodefactoryhead.h')
  #   op_fsrc  = env.get_template('t_gennodefactorysource.cpp')
  
  
  params = {'Namespace' : Namespace, 
        'plugin_name' : plugin_name,
            'ClassName': ClassName,
            'StringName': StringName,
            'operator_type': operator_type,
            'MinNobj' : problems_min_obj[idx],
            'MaxNobj' : problems_max_obj[idx],
            'MinNvar' : problems_min_nvar[idx],
            'MaxNvar' : problems_max_nvar[idx]}
  
  r_op_head   = op_head.render(params) 
  r_op_src    = op_src.render(params)  
  r_op_fhead  = op_fhead.render(params)
  r_op_fsrc   = op_fsrc.render(params) 
  r_op_fmhead = op_frmhead.render(params)
  r_op_fmsrc  = op_frmsrc.render(params)
  r_op_fmui   = op_frmui.render(params)
  
  fname_op_head  = plugin_path + ClassName + '.h'
  fname_op_src   = plugin_path + ClassName + '.cpp'
  fname_op_fhead = plugin_path + ClassName + 'Factory.h'
  fname_op_fsrc  = plugin_path + ClassName + 'Factory.cpp'
  fname_op_fmhead= dialog_path + ClassName + 'Form.h'
  fname_op_fmsrc = dialog_path + ClassName + 'Form.cpp'
  fname_op_fmui  = dialog_path + ClassName + 'form.ui'

  f_op_head  = open(fname_op_head.lower(), 'w')
  f_op_src   = open(fname_op_src.lower(), 'w')
  f_op_fhead = open(fname_op_fhead.lower(), 'w')
  f_op_fsrc  = open(fname_op_fsrc.lower() , 'w')
  f_op_fmhead= open(fname_op_fmhead.lower(), 'w')
  f_op_fmsrc = open(fname_op_fmsrc.lower(), 'w')
  f_op_fmui  = open(fname_op_fmui.lower(), 'w')

  f_op_head.write(r_op_head)
  f_op_src.write(r_op_src)
  f_op_fhead.write(r_op_fhead)
  f_op_fsrc.write(r_op_fsrc)
  f_op_fmhead.write(r_op_fmhead)
  f_op_fmsrc.write(r_op_fmsrc)
  f_op_fmui.write(r_op_fmui)
  # print rend
  #print template.render(Namespace='QMetal',ClassName='QAlgoNSGAIINode')
#!/usr/bin/python

from jinja2 import Environment, FileSystemLoader
import sys

var_types = {'int': 'int', 'qreal32': 'qreal32', 'double': 'double'}

# Setup parameters' names types etc. ( LQS - Latin Hypercube Sampling )
nind_parameter = {'name': 'nind', 'cname': 'Nind', 'type': 'int', 'dval': '100'}
nvar_parameter = {'name': 'nvar', 'cname': 'Nvar', 'type': 'int', 'dval': '20'}

parameters     = [nind_parameter,nvar_parameter]

operator_type = 'initialization'
plugin_name   = 'matlab'
Namespace     = 'Matlab'
StringName    = 'ObjFun'
space = ''

ClassName = 'QMat'+StringName+'Node'

env = Environment(loader=FileSystemLoader('Templates'))

plugin_path = '../liger_gui/src/plugins/'+plugin_name+'/'
dialog_path = '../liger_gui/src/plugins/'+plugin_name+'/dialogs/'
op_head = env.get_template('t_operatornodehead.h')
op_src = env.get_template('t_operatornodesource.cpp')
op_fhead = env.get_template('t_operatornodefactoryhead.h')
op_fsrc = env.get_template('t_operatornodefactorysource.cpp')
op_frmhead = env.get_template('t_operatornodeformhead.h')
op_frmsrc = env.get_template('t_operatornodeformsource.cpp')
op_frmui = env.get_template('t_operatornodeform.ui')


params = {'Namespace': Namespace,
          'plugin_name': plugin_name,
          'ClassName': ClassName,
          'StringName': StringName,
          'operator_type': operator_type,
          'parameters': parameters,
          'space': space,
          'var_types': var_types}

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
f_op_fsrc  = open(fname_op_fsrc.lower(), 'w')
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
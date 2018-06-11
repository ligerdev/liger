#!/usr/bin/python

from jinja2 import Environment, FileSystemLoader
import sys, os

operators = [
{'operator_type': 'initialisations', 'StringName': 'RandomInit',      'GUIText':'RInit'},
{'operator_type': 'initialisations', 'StringName': 'LHSInit',         'GUIText':'LHS'},
{'operator_type': 'evaluators',      'StringName': 'Evaluator',      'GUIText':'Eval'},
{'operator_type': 'evaluators',      'StringName': 'Validation',      'GUIText':'Valid'},
{'operator_type': 'initialisations', 'StringName': 'SparegoInit',     'GUIText':'SInit'},
{'operator_type': 'initialisations', 'StringName': 'UserDefinedInit', 'GUIText':'UInit'},
{'operator_type': 'evaluators',      'StringName': 'BatchEvaluator', 'GUIText':'BEval'}
]

def creatOperator(operator_type, plugin_name, Namespace, StringName, GUIText):
	space = ''

	ClassName = 'QOp'+StringName+'Node'

	env = Environment(loader=FileSystemLoader('Templates'))

	plugin_path = '../src/plugins/'+plugin_name+ \
	              '/operators/'+operator_type+'/'
	image_path  = '../src/plugins/'+plugin_name+'/images/'
	dialog_path = '../src/plugins/'+plugin_name+'/dialogs/'
	op_head  = env.get_template('t_operatornodehead.h')
	op_src   = env.get_template('t_operatornodesource.cpp')
	op_fhead = env.get_template('t_operatornodefactoryhead.h')
	op_fsrc  = env.get_template('t_operatornodefactorysource.cpp')
	op_svg   = env.get_template('t_operatornodeimage.svg')

	print('* Operator path:')
	print('\t' + plugin_path)

	if(os.path.exists(plugin_path)):
	    print('Found the folder')
	else:
	    print('Create folder', plugin_path)
	    os.makedirs(plugin_path)

	params = {'Namespace': Namespace,
	          'plugin_name': plugin_name,
	          'ClassName': ClassName,
	          'StringName': StringName,
	          'GUIText': GUIText,
	          'operator_type': operator_type,
	          'space': space}

	r_op_head   = op_head.render(params)
	r_op_src    = op_src.render(params)
	r_op_fhead  = op_fhead.render(params)
	r_op_fsrc   = op_fsrc.render(params)
	r_op_svg    = op_svg.render(params)

	fname_op_head  = plugin_path + ClassName + '.h'
	fname_op_src   = plugin_path + ClassName + '.cpp'
	fname_op_fhead = plugin_path + ClassName + 'Factory.h'
	fname_op_fsrc  = plugin_path + ClassName + 'Factory.cpp'
	fname_op_svg   = image_path  + ClassName + '.svg'

	f_op_head  = open(fname_op_head.lower(), 'w')
	f_op_src   = open(fname_op_src.lower(), 'w')
	f_op_fhead = open(fname_op_fhead.lower(), 'w')
	f_op_fsrc  = open(fname_op_fsrc.lower(), 'w')
	f_op_svg   = open(fname_op_svg.lower(), 'w')

	f_op_head.write(r_op_head)
	f_op_src.write(r_op_src)
	f_op_fhead.write(r_op_fhead)
	f_op_fsrc.write(r_op_fsrc)
	f_op_svg.write(r_op_svg)

	f_op_head.close()
	f_op_src.close()
	f_op_fhead.close()
	f_op_fsrc.close()
	f_op_svg.close()

for data in operators:
	operator_type = data['operator_type']
	StringName    = data['StringName']
	GUIText       = data['GUIText']

	plugin_name   = 'qtigon'
	Namespace     = 'QTigon'
	creatOperator(operator_type, plugin_name, Namespace, StringName, GUIText)

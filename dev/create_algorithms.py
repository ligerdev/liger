#!/usr/bin/python

from jinja2 import Environment, FileSystemLoader
import sys, os

algorithms = [
        #{'operator_type':'nsgaii',    'GUIText': 'NSGA-II'},
        #{'operator_type':'nsgaiipsa', 'GUIText': 'NSGAII-PSA'}, # need to manually fix svg
        #{'operator_type':'moead',     'GUIText': 'MOEA/D'},
        #{'operator_type':'sparego',   'GUIText': 'sParEGO'}
        #{'operator_type':'parego',    'GUIText': 'ParEGO'},
        #{'operator_type':'smsemoa',   'GUIText': 'SMSEMOA'},
        # {'operator_type':'pnsgaii',   'GUIText': 'pNSGA-II'}
        {'operator_type':'moga',   'GUIText': 'MOGA'}
]

#============================================
def createAlgorithm(operator_type, StringName, plugin_name, Namespace, GUIText):
	ClassName = 'QAlgo'+StringName+'Node'
	plugin_path = '../src/plugins/'+plugin_name+ \
	              '/algorithms/'+operator_type+'/'
	image_path  = '../src/plugins/'+plugin_name+'/images/'

	print('Creating ' + StringName)
	print('* Algorithm path:')
	print('\t' + plugin_path)

	if(os.path.exists(plugin_path)):
	    print('Found the folder')
	else:
	    print('Create folder', plugin_path)
	    os.makedirs(plugin_path)

	env = Environment(loader=FileSystemLoader('Templates'),
	                  lstrip_blocks =True)

	op_head  = env.get_template('t_algorithmnodehead.h')
	op_src   = env.get_template('t_algorithmnodesource.cpp')
	op_fhead = env.get_template('t_algorithmnodefactoryhead.h')
	op_fsrc  = env.get_template('t_algorithmnodefactorysource.cpp')
	op_svg  = env.get_template('t_algorithmnodeimage.svg')
		
	params = {'Namespace'    : Namespace,
                  'plugin_name'  : plugin_name,
                  'ClassName'    : ClassName,
                  'StringName'   : StringName,
                  'operator_type': operator_type,
                  'GUIText'      : GUIText}
		
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

	f_op_head  = open(fname_op_head.lower() , 'w')
	f_op_src   = open(fname_op_src.lower()  , 'w')
	f_op_fhead = open(fname_op_fhead.lower(), 'w')
	f_op_fsrc  = open(fname_op_fsrc.lower() , 'w')
	f_op_svg   = open(fname_op_svg.lower()  , 'w')

	f_op_head.write(r_op_head)
	f_op_src.write(r_op_src)
	f_op_fhead.write(r_op_fhead)
	f_op_fsrc.write(r_op_fsrc)
	f_op_svg.write(r_op_svg)

	f_op_head.close()
	f_op_src.close()
	f_op_fhead.close()
	f_op_fsrc.close()
	f_op_svg.close();

for alg in algorithms: 
	operator_type = alg['operator_type']
	StringName    = operator_type.upper()
	plugin_name   = 'qtigon'
	Namespace     = 'QTigon'
	GUIText       = alg['GUIText']
	createAlgorithm(operator_type, StringName, plugin_name, Namespace, GUIText)

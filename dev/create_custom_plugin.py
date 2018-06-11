#!/usr/bin/python

from jinja2 import Environment, FileSystemLoader
import sys, os, re

##################### User Defined Variables #####################
liger_root_path = '..'
plugin_name = 'NameOfPlugin'
plugin_description = """Add plugin description here...
                     """

lib_dependencies = ['utils','metal']

plugin_dependencies = ['coreplugin',
                     'designer']


#################### Do NOT Edit below this line ##################
###################################################################
plugin_path = liger_root_path + '/src/plugins/' + plugin_name.lower()
plugins_pro_path = liger_root_path + '/src/plugins/' + 'plugins.pro'

print('* Plugin path:')
print('\t' + plugin_path)
# Check if direcoty exsits
if not os.path.exists(plugin_path):
    os.makedirs(plugin_path)
else:
    print('Folder exists')
    print('Overwite folder ' + plugin_path )


file_list = [plugin_name.lower() + '.pro',
             plugin_name.lower() + '_dependencies.pri',
             plugin_name         + '.pluginspec.in',
             plugin_name.lower() + '.h'  ,
             plugin_name.lower() + '.cpp',
             plugin_name.lower() + '_global.h',
             plugin_name.lower() + 'constants.h']

template_list = [ 't_customplugin.pro',
                  't_customplugindependencies.pri',
                  't_customplugin.pluginspec.in',
                  't_custompluginhead.h',
                  't_custompluginsource.cpp',
                  't_custompluginglobalhead.h',
                  't_custompluginconstantshead.h']

params = {'plugin_name'           : plugin_name,
          'lib_dependencies'      : lib_dependencies,
          'plugin_dependencies'   : plugin_dependencies,
          'plugin_description'    : plugin_description}

env = Environment(loader=FileSystemLoader('Templates'),
                  trim_blocks=True, lstrip_blocks =True)

print('* Generating files:')
for i in range(0, len(file_list)):
    file_name = plugin_path + '/' + file_list[i]
    print('\t' + str(i) + '. ' + template_list[i] + '\n\t    ==> ' + file_name)
    op = env.get_template( template_list[i] )
    r_op = op.render( params )
    f_op = open( file_name , 'w' )
    f_op.write( r_op )
    f_op.close()

print('* Register ' + plugin_name + ' with Liger')
f_plugins_pro_op = open( plugins_pro_path, 'r' )

idx_sudir_start = 0
space_index = 0
index = 0

subdir_start = False
plugin_exists = False

# use regex to find the line number of the last loaded plugin
lines = f_plugins_pro_op.readlines();
for line in lines:
    my_regex = r'SUBDIRS\s*\+\s*=\s*' + plugin_name.lower()
    match_plugin = re.search( my_regex  , line )
    if match_plugin:
        plugin_exists = True
        print('\t' + plugin_name + ' already loaded; stop')
        break

if not plugin_exists:
    for line in lines:
        match_subdir = re.search( r'SUBDIRS\s*=\s*coreplugin' , line )
        #print('index: ' + str(index) + ':   ' + line)
        if match_subdir:
            idx_sudir_start = index
            subdir_start = True
            
        if subdir_start:
            match_find_space = re.search( r'SUBDIRS\s*\+*\s*=' , line )
            if not match_find_space:
                break
        index += 1


f_plugins_pro_op.close()

# append new plugin if not exists
if not plugin_exists:
    lines[index] = 'SUBDIRS += ' + plugin_name.lower()
    lines[index].strip()
    lines[index] = lines[index] + '\n\n'

    # write to plugins.pro file
    f_plugins_pro_wrt = open( plugins_pro_path, 'w' )
    for line in lines:
        f_plugins_pro_wrt.write(line)
    
    f_plugins_pro_wrt.close()

print('* Done. ')

    




#!/usr/bin/env python
# moosestart.py --- 
# 
# Filename: moosestart.py
# Description: 
# Author: Subhasis Ray
# Maintainer: 
# Created: Wed Nov 16 20:08:34 2011 (+0530)
# Version: 
# Last-Updated: Thu Nov 17 10:31:19 2011 (+0530)
#           By: Subhasis Ray
#     Update #: 104
# URL: 
# Keywords: 
# Compatibility: 
# 
# 

# Commentary: 
# 
# This script will do the initial checks for DEMOS and configurations
# in user's home directory and then start moosegui.
# 
# 

# Change log:
# 
# 
# 

# Code:

import platform
import os
import sys
from distutils.dir_util import copy_tree

from moosegui import main

moose_version = '1.4'

if __name__ == '__main__':
    moose_dir = os.path.join(sys.path[0], '..')    
    # The following code is for copying DEMOS and TESTS to user's home
    # directory is not already present.
    user_home = os.path.expanduser('~')
    user_moose_conf_dir = os.path.join(user_home, '.moose')
    user_moose_conf_file = os.path.join(user_moose_conf_dir, 'moose%s' % (moose_version))
    user_moose_dir = os.path.join(user_home, 'moose%s' % (moose_version))    
    symlinkpath = os.path.join(user_home, 'Desktop', 'moose%s' % (moose_version))
    if not os.path.lexists(user_moose_conf_dir):
        print 'Creating moose configuration directory:', user_moose_conf_dir
        os.mkdir(user_moose_conf_dir, 0755)
        fd = open(user_moose_conf_file, 'w')
        fd.write('# moose%s' % moose_version)
        fd.close()
    if not os.path.lexists(user_moose_dir):
        print 'Creating local moose directory:', user_moose_dir
        os.mkdir(user_moose_dir, 0755)
        copy_tree(os.path.join(moose_dir, 'DEMOS'), os.path.join(user_moose_dir, 'DEMOS'))
        copy_tree(os.path.join(moose_dir, 'TESTS'), os.path.join(user_moose_dir, 'TESTS'))
        print 'Copied DEMOS and TESTS to local moose directory'

    if platform.system() == 'Linux' or platform.system() == 'Darwin':
        ld_library_path = os.path.join(moose_dir, 'lib')
        try:
            ld_library_path = '%s:%s' % (ld_library_path, os.environ['LD_LIBRARY_PATH'])
        except KeyError:
            pass
        os.environ['LD_LIBRARY_PATH'] = ld_library_path
        python_module_path = '/usr/lib/python%d.%d/dist-packages' % (sys.version_info[0], sys.version_info[1])
        if python_module_path not in sys.path:
            sys.path.append(python_module_path)
        if not os.path.lexists(symlinkpath):
            os.symlink(user_moose_dir, symlinkpath)
    # finally run the gui.
    main(sys.argv)
    
    

# 
# moosestart.py ends here

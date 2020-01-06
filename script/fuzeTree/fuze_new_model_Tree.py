#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""

$Id: fuze_new_model_Tree.py 940 2015-05-29 19:13:14Z nelson $

$Rev: 940 $

@author: nelson
"""

from os import getenv, system
from CSV2NewTree import CSV2NewTree

#from worksheet_2_csvfile import all_worksheets_2_csvfile

sheetname = "fuze Tree"
#username = 'zaphdexperiment@gmail.com'
#password = 'NoKinks03'

#csvfile = all_worksheets_2_csvfile(sheetname, username, password,
#                                   striplines=1)

# the new version of gspread/oauth2 messed up the Google sheet read for
#  Python2.7
#csvfile = all_worksheets_2_csvfile(sheetname, striplines=1)

# use Python3 to write the csv file
system('python3 all_worksheets_2_csvfile_Python3.py "%s"' % sheetname)

csvfile = '%s.csv' % sheetname

treename = 'FUZE'
test_treename = 'TF'

if getenv('USER') in ['nelson', 'fuze']:
    CSV2NewTree(test_treename, csvfile, verbose=True)

print("\n\n\n")
print("******************************")
print("Test tree '%s' compiled OK." % test_treename)
print("******************************")
print("")
print("Options:")
print("1. Spawn traverser/jTraverser to look at tree %s" % test_treename)
print("2. Write Tree csv file to SVN only")
print("3. Write Tree csv file to SVN and create new %s model Tree" % treename)
print("4. Exit")
resp = raw_input("Enter choice [1-4]: ")

if resp == '1':
    system('traverser -shot -1 -tree %s &' % test_treename)
elif resp in ['2', '3']:
    comment = raw_input("Enter comment here: ")
    # need to setup SVN
    #system('svn ci -m "%s"' % comment)
    if resp == '3':
        print("Creating Tree %s ... " % treename,)
        CSV2NewTree(treename, csvfile, verbose=False)
        print("done.")

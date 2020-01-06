# -*- coding: utf-8 -*-
"""
Created on Tue Nov  5 13:24:56 2013

@author: nelson

$Id: digitizers_new_model_Tree.py 941 2015-05-29 19:19:31Z nelson $

$Rev: 941 $

"""


from os import getenv, system
from CSV2NewTree import CSV2NewTree

#from worksheet_2_csvfile import all_worksheets_2_csvfile

sheetname = "fdigitizers Tree"

#csvfile = all_worksheets_2_csvfile(sheetname, striplines=1)

# use Python3 to write the csv file
system('python3 all_worksheets_2_csvfile_Python3.py "%s"' % sheetname)

csvfile = '%s.csv' % sheetname

treename = 'FDIGITIZERS'
test_treename = 'TFD'

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
    # need to set up SVN
    #system('svn ci -m "%s"' % comment)
    if resp == '3':
        print("Creating Tree %s ... " % treename,)
        CSV2NewTree(treename, csvfile, verbose=False)
        print("done.")

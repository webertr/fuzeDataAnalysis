#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Created on Tue Oct 22 15:20:48 2013

@author: nelson

$Id: CSV2NewTree.py 1007 2015-07-22 18:00:07Z zap $

$Rev: 1007 $

"""

import csv
import MDSplus as M


def TorF(string):
    '''Converts spreadsheet TRUE or FALSE strings into Python Boolean variable
    '''
    if string.upper().startswith('T'):
        return True
    elif string.upper().startswith('F'):
        return False


def crange(c1, c2):
    """Iterator for characters from `c1` to `c2`, inclusive."""
    for c in xrange(ord(c1), ord(c2) + 1):
        yield chr(c)


class CSV2NewTree(object):

    # wildcard characters used in spreadsheet for char and num ranges
    CRChar = '@'
    NRChar = '#'
    NR2Char = '%'
    CommentChar = '!'

    # list of possible usage values for adding nodes
    Usages = ['TEXT', 'SUBTREE', 'DISPATCH', 'COMPOUND_DATA', 'DEVICE',
              'AXIS', 'NONE', 'TASK', 'SIGNAL', 'NUMERIC', 'WINDOW',
              'ACTION', 'ANY', 'STRUCTURE']

    def __init__(self, treename, csvfile, verbose=False):
        self.stop = False
        self.treename = treename
        self.tree = M.Tree(tree=treename, shot=-1, mode='New')
        self.csvfile = csvfile
        self.verbose = verbose
        self.readCSVFile()
        self.addAllNodes()
        if self.stop:
            print("")
            print("")
            print("***** !!!STOP!!! flag detected!!! *****")
            print("")
            print("***** Possible incomplete read!!! *****")
            print("")

    def readCSVFile(self):
        reader = csv.reader(open(self.csvfile, 'r'))
        self.rows = []
        self.stop = False
        for row in reader:
            if '!!!STOP!!!' in row:
                self.stop = True
                break
            if row[0] and (not row[0].startswith(CSV2NewTree.CommentChar)):
                self.rows.append(row)
        # extract header line
        self.header = self.rows[0]
        # strip off header line from rows
        self.rows = self.rows[1:]

    def addAllNodes(self):
        '''Unpack "row" into Node info
        Attributes with the suffix "Base" may have wildcards in them
        that have to be filled in before calling method addOrModifyNode()

        N.B. This of course has to be updated if columns are modified
        '''

        # open tree for Data.compile method in addOrModifyNode method
        M.Data.execute('treeopen("%s", -1)' % self.treename)

        for row in self.rows:
#            self.NodeBase, self.CharRange, self.NumRange, self.NumForm, \
#                self.NumRange2, self.NumForm2, \
#                self.ParentBase, self.Usage, self.TagsBase, \
#                self.Units, self.On, \
#                self.AddCCR, self.AddNumerics, \
#                self.AddSignals, self.AddText, self.ValueBase, \
#                self.NoWriteModel, self.NoWriteShot, self.WriteOnce, \
#                self.CompressOnPut, self.DoNotCompress, self.Help = row
            self.NodeBase, self.CharRange, self.NumRange, self.NumForm, \
                self.NumRange2, self.NumForm2, \
                self.ParentBase, self.Usage, self.TagsBase, \
                self.Units, self.On, \
                self.AddCCR, self.AddNumerics, \
                self.AddSignals, self.AddText, self.ValueBase, \
                self.NoWriteModel, self.NoWriteShot, self.WriteOnce, \
                self.CompressOnPut, self.DoNotCompress = row

            if not self.NumForm:
                self.NumForm = '%02d'
            if not self.NumForm2:
                self.NumForm2 = '%02d'

            # no ranges, just add the node using 'Base' values
            if (not self.CharRange) and (not self.NumRange):
                self.NodeName, self.Parent, self.Tags, self.Value = \
                    self.NodeBase, self.ParentBase, \
                    self.TagsBase, self.ValueBase

                self.char = None
                self.num = None
                self.num2 = None
                self.addOrModifyNode()

            # add a range over numbers
            if (not self.CharRange) and (self.NumRange):
                self.parseNumRange()
                for ii in self.nrangelist:
                    rNodeName = self.NodeBase.replace(CSV2NewTree.NRChar,
                                                      self.NumForm % ii)
                    rParent = self.ParentBase.replace(CSV2NewTree.NRChar,
                                                      self.NumForm % ii)
                    if self.TagsBase:
                        # comma delimited for multiple tags
                        rTags = self.TagsBase.replace(CSV2NewTree.NRChar,
                                                      self.NumForm % ii)
                    else:
                        rTags = ''
                    if self.ValueBase:
                        rValue = self.ValueBase.replace(CSV2NewTree.NRChar,
                                                        self.NumForm % ii)
                    else:
                        rValue = ''
                    if self.NumRange2:
                        self.parseNumRange2()
                        #print(self.nrange2list)
                        for jj in self.nrange2list:
                            #print(jj)
                            self.NodeName = \
                                rNodeName.replace(CSV2NewTree.NR2Char,
                                                  self.NumForm2 % jj)
                            self.Parent = \
                                rParent.replace(CSV2NewTree.NR2Char,
                                                self.NumForm2 % jj)
                            self.Tags = rTags.replace(CSV2NewTree.NR2Char,
                                                      self.NumForm2 % jj)
                            self.Value = rValue.replace(CSV2NewTree.NR2Char,
                                                        self.NumForm2 % jj)
                            self.char = ''
                            self.num = ii
                            self.num2 = jj
                            self.addOrModifyNode()
                    else:
                        self.NodeName = rNodeName
                        self.Parent = rParent
                        self.Tags = rTags
                        self.Value = rValue

                        self.char = ''
                        self.num = ii
                        self.num2 = None
                        self.addOrModifyNode()

            # add a range over characters
            if (self.CharRange) and (not self.NumRange):
                self.parseCharRange()
                for c in self.crangelist:
                    self.NodeName = self.NodeBase.replace(CSV2NewTree.CRChar,
                                                          '%s' % c)
                    self.Parent = self.ParentBase.replace(CSV2NewTree.CRChar,
                                                          '%s' % c)
                    if self.TagsBase:
                        self.Tags = self.TagsBase.replace(CSV2NewTree.CRChar,
                                                          '%s' % c)
                    else:
                        self.Tags = ''
                    if self.ValueBase:
                        self.Value = self.ValueBase.replace(CSV2NewTree.CRChar,
                                                            '%s' % c)
                    else:
                        self.Value = ''

                    self.char = ''
                    self.num = None
                    self.num2 = None
                    self.addOrModifyNode()

            # add a range over characters and over numbers
            if (self.CharRange) and (self.NumRange):
                # first iterate over char range
                self.parseCharRange()
                for c in self.crangelist:
                    cNodeName = self.NodeBase.replace(CSV2NewTree.CRChar,
                                                      '%s' % c)
                    cParent = self.ParentBase.replace(CSV2NewTree.CRChar,
                                                      '%s' % c)
                    if self.TagsBase:
                        cTags = self.TagsBase.replace(CSV2NewTree.CRChar,
                                                      '%s' % c)
                    else:
                        cTags = ''
                    if self.ValueBase:
                        cValue = self.ValueBase.replace(CSV2NewTree.CRChar,
                                                        '%s' % c)
                    else:
                        cValue = ''
                    # next iterate over the num range
                    self.parseNumRange()
                    #print(self.nrangelist)
                    for ii in self.nrangelist:
                        #print('ii = %s' % ii)
                        ncNodeName = cNodeName.replace(CSV2NewTree.NRChar,
                                                       self.NumForm % ii)
                        ncParent = cParent.replace(CSV2NewTree.NRChar,
                                                   self.NumForm % ii)
                        ncTags = cTags.replace(CSV2NewTree.NRChar,
                                               self.NumForm % ii)
                        ncValue = cValue.replace(CSV2NewTree.NRChar,
                                                 self.NumForm % ii)
                        if self.NumRange2:
                            self.parseNumRange2()
                            #print(self.nrange2list)
                            for jj in self.nrange2list:
                                #print('jj = %s' % jj)
                                self.NodeName = \
                                    ncNodeName.replace(CSV2NewTree.NR2Char,
                                                       self.NumForm2 % jj)
                                self.Parent = \
                                    ncParent.replace(CSV2NewTree.NR2Char,
                                                     self.NumForm2 % jj)
                                self.Tags = ncTags.replace(CSV2NewTree.NR2Char,
                                                           self.NumForm2 % jj)
                                self.Value = \
                                    ncValue.replace(CSV2NewTree.NR2Char,
                                                    self.NumForm2 % jj)
                                self.char = c
                                self.num = ii
                                self.num2 = jj
                                self.addOrModifyNode()
                        else:
                            self.NodeName = ncNodeName
                            self.Parent = ncParent
                            self.Tags = ncTags
                            self.Value = ncValue

                            self.char = c
                            self.num = ii
                            self.num2 = None
                            self.addOrModifyNode()

        self.tree.write()

    def addOrModifyNode(self):
        '''if self.Usage is not null:
            Add Node (or Device), set tags, characteristics, add CCR and
            Numeric Nodes, value, and help
        if self.Usage is null:
            Modify Node, set tags, characteristics, add CCR and
            Numeric Nodes, value, and help
        '''
        if self.verbose:
            print("NodeName = %s:\t Parent = %s" % (self.NodeName,
                                                    self.Parent))
            if self.Value:
                print("Value = %s" % self.Value)

        # flag error if NodeName winds up being more than 12 characters
        if ((len(self.NodeName) > 12) and self.Usage and
            (':' not in self.NodeName) and ('.' not in self.NodeName)):
            print('!%s!' % self.NodeName)
            print(len(self.NodeName))
            print("Node %s is longer than 12 characters!" % self.NodeName)
            raise ValueError

        # check if the following attributes ...
        atts = [self.NodeName, self.Parent, self.Value, self.Tags]

        # ... have a wildcard or comment in them
        vals = [CSV2NewTree.CRChar, CSV2NewTree.NRChar,
                CSV2NewTree.NR2Char, CSV2NewTree.CommentChar]

        # doing the check for val in att
        for val in vals:
            for att in atts:
                if val in att:
                    # allow embedding *Chars with leading backslash
                    if att[att.find(val) - 1] != '\\':
                        print(self.Value)
                        print("Character '%s' found in %s" % (val, att))
                        raise ValueError

        # if Usage defined, add a Node or Device
        if self.Usage:
            # set default position in tree to Parent
            self.tree.setDefault(self.tree.getNode(r'\%s::%s' %
                                                   (self.treename,
                                                    self.Parent)))
            # determine if Node or Device
            if self.Usage.upper() in CSV2NewTree.Usages:
                # determine prefix (pf) for Node
                if self.Usage.upper() == 'STRUCTURE':
                    pf = '.'
                else:
                    pf = ':'
                # add Node with usage
                self.node = self.tree.addNode(r'%s%s' % (pf, self.NodeName),
                                              usage=self.Usage)
            else:
                # add Device (need lower case for C device drivers)
                # can't use setDefault() for Devices (bug?), use full pathname
                #self.node = self.tree.addDevice(r'\%s::%s:%s' %
                #                                (self.treename,
                #                                 self.Parent,
                #                                 self.NodeName),
                #                                self.Usage.lower())
                # new bug in MDSplus where one can't add a Device with full
                #  path name; seems one *can* use relative path names now
                self.node = self.tree.addDevice(r'%s' % (self.NodeName),
                                                self.Usage.lower())
        else:
            # Usage is null: Node already exists, access it
            self.node = self.tree.getNode(r'\%s::%s:%s' % (self.treename,
                                                           self.Parent,
                                                           self.NodeName))
        self.addTags()
        self.setCharacteristics()
        self.AddCCRNodes()
        self.AddNumericNodes()
        self.AddSignalNodes()
        self.AddTextNodes()
        if self.Value:
            # remove any single quote at the beginning of the num range
            if self.Value.startswith("'"):
                self.Value = self.Value[1:]
            # work-around until I figure out how to do this the "new" way
#            M.Data.execute('treeopen("%s", -1)' % self.treename)
            # compile TDI expression
#            data = M.Data.compile(self.Value)
            # new way to compile TDI expression
            data = self.tree.tdiCompile(self.Value)
            # add Units to data if defined
            if self.Units:
                data.setUnits(M.String(self.Units))
#            if self.Help:
#                print("ADDING HELP NODE %s" % self.NodeName)
#                data.setHelp(M.String(self.Help))
#            self.node.putData(data, strict=False)
            self.node.putData(data)

    def setCharacteristics(self):
        '''Set values for basic node characteristics
        '''
        if self.On:
            self.node.setOn(TorF(self.On))
        if self.CompressOnPut:
            self.node.setCompressOnPut(TorF(self.CompressOnPut))
        if self.DoNotCompress:
            self.node.setDoNotCompress(TorF(self.DoNotCompress))
        if self.NoWriteModel:
            self.node.setNoWriteModel(TorF(self.NoWriteModel))
        if self.NoWriteShot:
            self.node.setNoWriteShot(TorF(self.NoWriteShot))
        if self.WriteOnce:
            self.node.setWriteOnce(TorF(self.WriteOnce))

    def addTags(self):
        '''Add comma-delimited list of tag names for this node
        Tag names must use CharRange '@' and NumRange '#' wildcards
        if either range is non-zero
        '''
        if self.Tags:
            # comma delimited for multiple tags
            Tags = self.Tags.split(',')
            for tag in Tags:
                # add tag after stripping blanks and making uppercase
                self.node.addTag(tag.strip().upper())

    def AddCCRNodes(self):
        '''Add "standard" subnodes of:
        :CAL_FACT   Numeric
        :COMMENT    Text
        :RAW        Points to digitizer or integrator channel
        '''
        if self.AddCCR.upper().startswith('T'):
            self.tree.addNode(r'%s:CAL_FACT' % self.NodeName, usage='NUMERIC')
            self.tree.addNode(r'%s:COMMENT' % self.NodeName, usage='TEXT')
            self.tree.addNode(r'%s:RAW' % self.NodeName, usage='SIGNAL')

    def AddNumericNodes(self):
        '''Add Numeric subnodes from comma delimited list "AddNumerics"
        '''
        if self.AddNumerics:
            nodelist = self.AddNumerics.split(',')
            for c in nodelist:
                if CSV2NewTree.CRChar in c:
                    c = c.replace(CSV2NewTree.CRChar, self.char)
                if CSV2NewTree.NRChar in c:
                    c = c.replace(CSV2NewTree.NRChar, self.NumForm % self.num)
                if CSV2NewTree.NR2Char in c:
                    c = c.replace(CSV2NewTree.NR2Char,
                                  self.NumForm2 % self.num2)
                self.tree.addNode(r'%s:%s' % (self.NodeName, c.strip()),
                                  usage='NUMERIC')

    def AddTextNodes(self):
        '''Add Text subnodes from comma delimited list "AddNumerics"
        '''
        if self.AddText:
            nodelist = self.AddText.split(',')
            #print(nodelist)
            for c in nodelist:
                if CSV2NewTree.CRChar in c:
                    c = c.replace(CSV2NewTree.CRChar, self.char)
                if CSV2NewTree.NRChar in c:
                    c = c.replace(CSV2NewTree.NRChar, self.NumForm % self.num)
                if CSV2NewTree.NR2Char in c:
                    c = c.replace(CSV2NewTree.NR2Char,
                                  self.NumForm2 % self.num)
                #print(r'%s:%s' % (self.NodeName, c.strip()))
                self.tree.addNode(r'%s:%s' % (self.NodeName, c.strip()),
                                  usage='TEXT')

    def AddSignalNodes(self):
        '''Add Signal subnodes from comma delimited list "AddSignals"
        '''
        if self.AddSignals:
            nodelist = self.AddSignals.split(',')
            for c in nodelist:
                if CSV2NewTree.CRChar in c:
                    c = c.replace(CSV2NewTree.CRChar, self.char)
                if CSV2NewTree.NRChar in c:
                    c = c.replace(CSV2NewTree.NRChar, self.NumForm % self.num)
                if CSV2NewTree.NR2Char in c:
                    c = c.replace(CSV2NewTree.NR2Char,
                                  self.NumForm2 % self.num)
                self.tree.addNode(r'%s:%s' % (self.NodeName, c.strip()),
                                  usage='SIGNAL')

    def parseNumRange(self):
        nums = self.NumRange.split(',')
        self.nrangelist = []
        for num in nums:
            # remove any single quote at the beginning of the num range
            if num.startswith("'"):
                num = num[1:]
            vals = num.split(':')
            if len(vals) == 1:
                self.nrangelist.append(int(vals[0]))
            elif len(vals) == 2:
                self.nrangelist.extend(range(int(vals[0]), int(vals[1]) + 1))
            elif len(vals) == 3:
                self.nrangelist.extend(range(int(vals[0]), int(vals[1]) + 1,
                                             int(vals[2])))

    def parseNumRange2(self):
        nums = self.NumRange2.split(',')
        self.nrange2list = []
        for num in nums:
            # remove any single quote at the beginning of the num range
            if num.startswith("'"):
                num = num[1:]
            vals = num.split(':')
            if len(vals) == 1:
                self.nrange2list.append(int(vals[0]))
            elif len(vals) == 2:
                self.nrange2list.extend(range(int(vals[0]), int(vals[1]) + 1))
            elif len(vals) == 3:
                self.nrange2list.extend(range(int(vals[0]), int(vals[1]) + 1,
                                              int(vals[2])))

    def parseCharRange(self):
        chars = self.CharRange.split(',')
        self.crangelist = []
        for char in chars:
            if ':' in char:
                vals = char.split(':')
                self.crangelist.extend(crange(vals[0].strip(),
                                              vals[1].strip()))
            else:
                self.crangelist.append(char.strip())

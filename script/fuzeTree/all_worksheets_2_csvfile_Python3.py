# -*- coding: utf-8 -*-
"""
Created on Wed May 28 16:26:24 2014

@author: nelson
"""

import sys
import csv
import gspread


def get_credentials(user=None):
    """get Oauth2 credentials for HIT-SI3 or ZaP-HD users
    """
    #import json
    #from oauth2client.client import SignedJwtAssertionCredentials
    # new oauth2client interface --- 9 Jun 2016 BAN
    from oauth2client.service_account import ServiceAccountCredentials
    from os import getenv

    # restrict who can run this script by user name
    if not user:
        user = getenv('USER').lower()

    #if user in ['hitsi', 'nelson']:
    if user in ['hitsi']:
        # only users nelson or hitsi can open HIT-SI3 Sheets
        jfile = 'hitsitrees-a9b8e0d66b44.json'
    elif user in ['zap', 'fuze', 'nelson']:
        # only user zap can open ZaP-HD Sheets
        jfile = 'zaphdtrees-79f446b5286c.json'
        #jfile = 'zaphdtrees-c86eaa264ed9.json'
    else:
        print("Invalid user '%s'; exiting" % user)
        sys.exit(1)

    # split env var PYTHONPATH into individual paths
    #locations = getenv('PYTHONPATH').split(':')

    # look for json file in PYTHONPATH
    #for location in locations:
    #    try:
    #        json_key = json.load(open('%s/%s' % (location, jfile)))
    #        break
    #    except IOError:
    #        continue

    # didn't find json file; exit
    #if 'json_key' not in dir():
    #    print("Can't find file %s, exiting" % jfile)
    #    sys.exit(1)

    # Oauth2 needs a "scope"
    #scope = ['https://spreadsheets.google.com/feeds']
    #scope = ['https://spreadsheets.google.com/feeds',
    #         'https://www.googleapis.com/auth/drive.readonly',
    #         'https://www.googleapis.com/auth/spreadsheets.readonly']
    scope = ['https://spreadsheets.google.com/feeds',
             'https://www.googleapis.com/auth/drive',
             'https://www.googleapis.com/auth/drive.file',
             'https://www.googleapis.com/auth/drive.readonly',
             'https://www.googleapis.com/auth/spreadsheets',
             'https://www.googleapis.com/auth/spreadsheets.readonly']


    # get the credentials
    #return SignedJwtAssertionCredentials(json_key['client_email'],
    #                                     json_key['private_key'],
    #                                     scope)
    return ServiceAccountCredentials.from_json_keyfile_name(jfile, scope)


def all_worksheets_2_csvfile(sheetname, striplines=0, user=None):
    """Download all worksheets from a Google spreadsheet,
    and write all rows to a CSV file of the same name as the
    the spreadsheet name.

    The worksheets are concatenated together in order of their
    tabs on the Google spreadsheet.

    The first striplines of other than the first worksheet are
    removed (thus if there are the value of striplines lines as
    a header in each worksheet, the concatenated rows have only
    one header).

    Note a Google "spreadsheet" may contain multiple "worksheets",
    not to be confused with the Excel terminology "workbook", which
    is the same as "spreadsheet".

    To do:

    Option for output file name
    """

    # login into Google account
    #gc = gspread.login(username, password)
    credentials = get_credentials(user)
    gc = gspread.authorize(credentials)

    # open all worksheets from spreadsheet
    worksheets = gc.open(sheetname).worksheets()

    rows = []

    # add first worksheet to rows (will include any header)
    rows.append(worksheets[0].get_all_values())

    # add all other worksheets to rows, removing first "striplines"
    for worksheet in worksheets[1:]:
        rows.append(worksheet.get_all_values()[striplines:])

    #csvfile = 'hitsi3 Tree - Sheet1.csv'
    csvfile = '%s.csv' % sheetname

    # create new csvfile
    fid = open(csvfile, 'w')

    # define writer with \n line termination
    csvwriter = csv.writer(fid, lineterminator='\n')

    # write all rows for each worksheet
    for row in rows:
        csvwriter.writerows(row)

    fid.close()

    # return csvfile name string
    return csvfile

if __name__ == "__main__":
    from sys import argv
    if len(argv) == 2:
        all_worksheets_2_csvfile(argv[1], striplines=1)
    else:
        print('usage: python3 all_worksheets_csvfile_Python3 "filename"')

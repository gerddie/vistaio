#!/usr/bin/python 
#
# Copyright (C) 2011 Gert Wollny <gw.fossdev@gmail.com>
#  
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

import sys
import re
from os import rename 
import string 
from fnmatch import fnmatch
from os.path import walk



class CPatternCollector:
   def __init__(self, pattern):
      self.pattern = pattern
      self.files=[]

      
def collect_files(arg, dirname, names):
   for f in names:
      for p in arg.pattern:
         if fnmatch(f, p):
            arg.files.append(dirname + "/" + f)
   


def find_files(root, pattern): 
   arg = CPatternCollector(pattern)
   walk(root, collect_files, arg)
   return arg.files 


root= sys.argv[1]
files = find_files(root, ["*.C", "*.H", "*.c", "*.h", "*.cc", "*.hh", "*.cpp", "*.hpp"])


# read all files and extract comment blocks 
for f in files: 

    # ignore some emacs security copies 
    h = re.search("/\.#[-a-z0-9]*\.[ch]*", f)
    if h:
        continue 


    
    infile = open(f, "r");
    lines = infile.readlines()
    infile.close(),

    changed = False
    out_lines = []
    for l in lines:
        ol = re.sub("\\bV(_*[A-Z]*[a-z][a-zA-Z0-9_]*)\\b", "VistaIO\\1",l)
        out_lines.append(ol)
        changed = changed or (ol != l)

    if changed:
        print f, " updated"
        outfile = open(f, "w")
        outfile.writelines(out_lines)
        outfile.close();
    else:
        print f, " unchanged"
    


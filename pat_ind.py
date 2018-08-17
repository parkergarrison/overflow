#!/usr/bin/env python

import sys
from binascii import unhexlify

def nextperm(s):
    news = ""
    
    donext = True # need to go to the next 
    for i in range(len(s)-1, -1, -1):
        if not donext:
            news = s[i] + news
            continue

        if s[i] == 'z':
            news = 'a' + news
        
        else:
            news = chr(ord(s[i])+1) + news
            donext = False
            continue
    
    return news


def a2n(s):
    ''' returns the value of a string in base 26 where the digits are a=0, ..., z=25 '''
    total = 0

    for i in range(len(s)-1, -1, -1):
        exp = len(s)-1 - i
        c = s[i].lower()
        total += (ord(c) - ord('a')) * 26 ** exp

    return total


p = sys.argv[1]
if len(p) == 8:
    p = unhexlify(sys.argv[1])

pattern_options = {p[::-1]: "Little Endian", p: "Big Endian"} #TODO change pattern to detect which

for p in pattern_options:
    # print p

    split_in = 0 # location of the capital letter
    for i in range(len(p)):
        if p[i].isupper():
            split_in = i
            break

    #modified p
    mp = p[split_in:] + nextperm(p[:split_in]) # get the group of 4 starting at the capital letter, regardless of location of capital
    ans = 4*a2n(mp) - split_in # the given string starts split_in positions to the left of the location of mp

    print pattern_options[p]+": "+str(ans)


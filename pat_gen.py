#!/usr/bin/env python

from __future__ import print_function
import time
import sys

BASES = ((chr(x) for x in range(ord('a'), ord('z')+1)) for i in range(4))


def formatted(perm):
    return perm[0].upper() + perm[1:]

def nextperm(s):
    ''' takes lowercase string of form aaaa and produces next permutation, eg. aazz -> abaa '''

    news = ""
    donext=1 # whether we need to advance the characters
    
    for i in range(len(s)-1, -1, -1):
        if not donext:
            news = s[i] + news
            continue

        if s[i] == 'z':
            news = 'a' + news
        
        else:
            news = chr(ord(s[i])+1) + news
            donext = 0
            continue
    
    return news

def n2a(n, bases): #block=4):
    ''' from a number to a=0, ..., z=25, padded to block '''

    ansL_rev = []
    cur_n = n
    bases = list(bases)
    for bdigits in bases[::-1]:
        bdigits = list(bdigits)
        ansL_rev.append(bdigits[cur_n % len(bdigits)])
        cur_n /= len(bdigits)

    ans = "".join(ansL_rev[::-1])
    
    return ans



if len(sys.argv) == 2:
    end = int(sys.argv[1])
    start = 0
elif len(sys.argv) == 3:
    end = int(sys.argv[2])
    start = int(sys.argv[1])
else:
    print("Usage: "+sys.argv[0]+" [start] end", file=sys.stderr)
    exit(2)
    


cg = n2a(int(start/4), BASES)# "a"*4 # current permutation
la=4*int(start/4)+4

ansA = [formatted(cg[start%4:])] # array for speed


while la < end: # generate next 4 characters until required length
    
    cg = nextperm(cg)    

    cg_formatted = formatted(cg)
    to_add = cg_formatted[:min(end-la,4)]
    ansA.append(to_add)

    
    la+=4

    #print(la, end, cg, cg_formatted)
    
ans = "".join(ansA)
print(ans)

from __future__ import division
import sys
import math

def prob(k, m, n):
    '''
    k is the number of hash functions
    n is the keys to be inserted 
    m is the table size
    '''
    return ( 1. - math.exp(k * n / m )) ** k

if __name__ == '__main__':
    print prob(200, 10000000, 500000000)

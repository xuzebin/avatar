#!/usr/bin/env python

"""
A script to rename a set of stereo image pairs so that their names are in a consecutive order (eg. 0L.jpg, 0R.jpg, 1L.jpg, 1R.jpg, etc).
When some chessboard pairs contain outliers, we remove those image pairs, and run this script to reorder.
"""

import os
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Rename files in a directory. The files must be in the format: %dL.jpg, %dR.jpg, where %d is a number.')
    parser.add_argument('-start_index', metavar='start_index', type=int,
                        help='starting index')
    parser.add_argument('end_index', metavar='end_index', type=int,
                        help='ending index')
    parser.add_argument('directory', help='the directory containing the files to be renamed')
    args = parser.parse_args()

    count = 0
    for x in range(args.end_index + 1):
        left_filename = os.path.join(args.directory, "{0}L.jpg".format(x))
        right_filename = os.path.join(args.directory, "{0}R.jpg".format(x))

        if os.path.isfile(left_filename) and os.path.isfile(right_filename):
            renamed_left = os.path.join(args.directory, "{0}L.jpg".format(count))
            renamed_right = os.path.join(args.directory, "{0}R.jpg".format(count))
            os.rename(left_filename, renamed_left)
            os.rename(right_filename, renamed_right)
            count += 1
            print 'renamed: %s -> %s' % (left_filename, renamed_left)
            print 'renamed: %s -> %s' % (right_filename, renamed_right)
            

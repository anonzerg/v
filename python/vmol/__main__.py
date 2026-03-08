#!/usr/bin/env python

import sys
from vmol import vmol


def main():
    vmol.run(args=sys.argv, with_arg0=True)


if __name__ == "__main__":
    main()

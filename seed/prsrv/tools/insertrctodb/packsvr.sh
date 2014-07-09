#!/bin/bash

dirname=`pwd`
curdir=`basename $dirname`
tar cvfz $curdir.tar.gz --exclude-from=foo ../$curdir

#!/bin/sh

echo `date +%Y%m%d` `find .. -name '*.[Ssch]' -exec cat {} \; | wc -l` >> loccount.txt


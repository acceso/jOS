#!/bin/sh

BASE=$(realpath $(dirname $0)/..)

echo gdb -s ${BASE}/jOS -x ${BASE}/scripts/rungdb.cmd



#!/bin/bash

cp file/src/magic.h lib/
cp file/magic/magic.mgc lib/
pushd lib
gzip -k -f magic.mgc
xxd -i magic.mgc.gz >magic_mgc_gz.c
popd

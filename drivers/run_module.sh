#!/bin/bash

if [[ $EUID -ne 0 ]]; then
    echo "Please run as root!"
    echo "e.g. sudo $0 $@"
    exit -1
fi

FILE=/dev/simpleBuffer

make simple_buffer.c
ret=$?

if [[ $ret -ne 0 ]]; then
    echo "Error compiling module!"
    exit $ret
fi

insmod ./simple_buffer.ko
ret=$?
if [[ $ret -ne 0 ]]; then
    echo "Error installing module"
    exit $ret
fi

majorNumber=`dmesg | grep Major | tail -1 | awk -F ": " '{print $2}'`
if [[ ! -f "$FILE" ]] && [[ ! -z "$majorNumber" ]]; then
   mknod $FILE c $majorNumber 0 # 0 for minor number
fi

chmod 666 /dev/simpleBuffer

echo "Done! Try writing to file $FILE"
echo "e.g. echo \"Hello, world!\" > $FILE"
echo "and then: cat $FILE"
    

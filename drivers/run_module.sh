#!/bin/bash

function printHelp() {
    echo ""
    echo "USAGE: $0 -n <name of file> -t <type of device>"

    echo "Options:"
    echo "  -n|--name   - Name of device (under /dev)"
    echo "  -t|--type   - Type of device file (b - block, c - character)"
    echo "  -h|--help   - Print help Message/usage"
    echo ""
}

if [[ $EUID -ne 0 ]]; then
    echo "Please run as root!"
    echo "e.g. sudo $0 $@"
    exit -1
fi

while [[ "$1" != "" ]]; do
    case "$1" in
        "-h"|"--help")
            printHelp
            exit 0
            ;;
        "-n"|"--name")
            shift
            FILE="$1"
            shift
            ;;
        "-t"|"-type")
            shift
            DEVICE_TYPE="$1"
            shift
            ;;
        *)
            echo "Unrecognized option $1"
            printHelp
            exit -1

    esac
done

if [[ -z "$FILE" ]] || [[ -z "$DEVICE_TYPE" ]]; then
    echo "Missing filename and/or device type!"
    printHelp
    exit -1
fi
echo "$DEVICE_TYPE"

#if [ "$DEVICE_TYPE" != "c" ] || [ "$DEVICE_TYPE" != "b" ]; then
    #echo "Invalid device type!"
    #printHelp
    #exit -1
#fi

#FILE=/dev/simpleBuffer

#make simple_buffer.c
make
ret=$?

if [[ $ret -ne 0 ]]; then
    echo "Error compiling module!"
    exit $ret
fi
echo "Compile succeeded!"

#insmod ./simple_buffer.ko
insmod `basename ${FILE}.ko`
echo "Installing module..."

sleep 5

ret=$?
if [[ $ret -ne 0 ]]; then
    echo "Error installing module"
    exit $ret
fi

majorNumber=`dmesg | grep Major | tail -1 | awk -F ": " '{print $2}'`
# if file doesn't exist and we got the majorNumber from dmesg
if [[ ! -f "$FILE" ]] && [[ ! -z "$majorNumber" ]]; then
   mknod $FILE $DEVICE_TYPE $majorNumber 0 # 0 for minor number
fi

chmod 666 $FILE

echo "Done! Try writing to file $FILE"
echo "e.g. echo \"Hello, world!\" > $FILE"
echo "and then: cat $FILE"
    

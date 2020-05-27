THIS_DIR=$(dirname $0)

${THIS_DIR}/../../../bin/premake5 gmake2

make config=debug
./build/bin/lab1.out my_label

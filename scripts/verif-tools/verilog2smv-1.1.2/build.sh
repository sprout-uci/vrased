#!/bin/sh

#echo "=> Downloading Yosys"
mkdir lib
cd lib
#wget https://github.com/cliffordwolf/yosys/archive/master.zip
unzip yosys-master.zip
rm -rf yosys
mv yosys-master yosys

echo "=> Configurating Yosys"
sed -i -e 's/ENABLE_TCL[[:space:]]*:=[[:space:]]*1/ENABLE_TCL := 0/g' yosys/Makefile
sed -i -e 's/ENABLE_ABC[[:space:]]*:=[[:space:]]*1/ENABLE_ABC := 0/g' yosys/Makefile
sed -i -e 's/ENABLE_PLUGINS[[:space:]]*:=[[:space:]]*0/ENABLE_PLUGINS := 1/g' yosys/Makefile
sed -i -e 's/ENABLE_READLINE[[:space:]]*:=[[:space:]]*1/ENABLE_READLINE := 0/g' yosys/Makefile
sed -i -e 's/ENABLE_COVER[[:space:]]*:=[[:space:]]*1/ENABLE_COVER := 0/g' yosys/Makefile

echo "=> Building Yosys"
cd yosys
make

echo "=> Building nuXmv plugin"
cd ../..
if [ "$(uname)" == "Darwin" ]; then
  ./lib/yosys/yosys-config --build nuxmv.so src/yosys_plugin/nuxmv.cc -I./lib/yosys -undefined dynamic_lookup
else
  ./lib/yosys/yosys-config --build nuxmv.so src/yosys_plugin/nuxmv.cc -I./lib/yosys
fi

echo "=> Creating conversion script"
cp script/verilog2smv.sh .

SCRIPTPATH=`pwd`
sed -i -e 's,YOSYS=..,YOSYS='"$SCRIPTPATH"',g' verilog2smv.sh
sed -i -e 's,NUXMV_PLUGIN=..,NUXMV_PLUGIN='"$SCRIPTPATH"',g' verilog2smv.sh

echo "=> Use verilog2smv.sh for conversion"

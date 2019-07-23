echo ".............Compiling of compiler............."
cd ./compiler
make
cd ..
echo ".............Conver to bytecode................"
./compiler/ekwa ./compiler/test.ekwa
rm -f ./runtime/instructions
cp ./instructions ./runtime/instructions
cd ./runtime
make
echo ".............Runing............................"
./program


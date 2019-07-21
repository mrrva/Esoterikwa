cd ./compiler
make
cd ..
./compiler/ekwa ./compiler/test.ekwa
rm -f ./runtime/instructions
cp ./instructions ./runtime/instructions
cd ./runtime
make
./program


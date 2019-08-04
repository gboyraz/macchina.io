echo $PWD
cd ..

echo $PWD
. env.bash

cd avioos
make DEFAULT_TARGET=shared_debug
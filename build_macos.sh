./submodules/bx/tools/bin/darwin/genie --gcc=osx gmake
make -C .build/projects/gmake-osx config=debug64 -j 8
make -C .build/projects/gmake-osx config=release64 -j 8

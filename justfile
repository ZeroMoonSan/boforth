dev_flags := "-std=c99 -pedantic -Wall -Wextra -Werror -Wshadow -Wconversion -U_FORTIFY_SOURCE -g -Og -Wunused-function -Wunused-variable -Wunused-parameter -Wno-pointer-sign"
release_flags := "-std=c99 -pedantic -Wall -Wextra -Wshadow -Wconversion -U_FORTIFY_SOURCE -O3 -DNDEBUG -flto -fomit-frame-pointer -funroll-loops -Wno-pointer-sign"
portable_flags := "-std=c99 -pedantic -Wall -Wextra -Wshadow -Wconversion -U_FORTIFY_SOURCE -O2 -DNDEBUG -Wno-pointer-sign"
out := ".build/dev/boforth"
release_out := ".build/release/boforth"
portable_out := ".build/release/boforth-portable"

build:
	@mkdir -p .build/dev
	clang {{dev_flags}} *.c -o {{out}}

release:
	@mkdir -p .build/release
	clang {{release_flags}} *.c -o {{release_out}}

release-portable:
	@mkdir -p .build/release
	clang {{portable_flags}} -march=x86-64 -mtune=generic *.c -o {{portable_out}}

release-native:
	@mkdir -p .build/release
	clang {{release_flags}} -march=native -mtune=native *.c -o {{release_out}}

release-fast:
	@mkdir -p .build/release
	clang -std=c99 -O3 -DNDEBUG -flto -fomit-frame-pointer -funroll-loops \
	      -march=native -mtune=native -Wno-pointer-sign \
	      *.c -o .build/release/boforth-fast

run *args: build
	echo "r" | gdb --args {{out}} {{args}}

clean:
	rm -rf .build

glc:
	cloc --include-lang=C,"C/C++ Header" .

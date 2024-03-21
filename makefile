name=fibonacci

${name}: ${name}.c # requires ${name}.c, will recompile on change
	gcc -ggdb -Wall -Wextra -O0 -o ${name} ${name}.c

clean:
	rm -rf ${name}

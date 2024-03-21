name=fibonacci

${name}: ${name}.c
	gcc -ggdb -Wall -Wextra -O0 -o ${name} ${name}.c

clean:
	rm -rf ${name}

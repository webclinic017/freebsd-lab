


all: doc
	date

# Build your own doc!
doc:
	asciidoc -o index.html README.adoc
	xdg-open index.html

# Build hello world userland app in c
c:
	cc -o userland/hello userland/hello.c -lm

clean:
	rm userland/hello




all:
	date

# Build your own doc!
doc:
	asciidoc -o index.html README.adoc
	xdg-open index.html

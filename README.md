[JamesM's kernel development tutorials: Roll your own toy UNIX-clone OS](http://www.jamesmolloy.co.uk/tutorial_html/index.html)

**How to set Bochs break point**

    readelf kernel
	Entry point address: 0x100020

	objdump -S boot.o
	objdump -S main.o

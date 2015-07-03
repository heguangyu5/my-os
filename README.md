[JamesM's kernel development tutorials: Roll your own toy UNIX-clone OS](http://www.jamesmolloy.co.uk/tutorial_html/index.html)

**How to set Bochs break point**

    readelf -s kernel  | egrep 'main|monitor_write'

    objdump -S

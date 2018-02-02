#!/bin/bash
echo "Content-type: text/html"
echo
echo "<html>"
echo "<body>"

echo "-----Desinstalando-----<br>"
sudo rmmod modulo_kernel_HelloWorld.ko
echo "-----IMPRIMO DMESG-----<br>"
dmesg | tail -4

echo "</body>"
echo "</html>"

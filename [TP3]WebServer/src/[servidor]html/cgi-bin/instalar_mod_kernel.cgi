#!/bin/bash
echo "Content-type: text/html"
echo "<html>"
echo "<body>"
echo
cd /var/www/html/files/uploads
echo "-----Instalando-----<br>"
sudo insmod ./modulo_usuario.ko
echo "-----IMPRIMO DMESG-----<br>"
echo | dmesg | tail -4
echo "</body>"
echo "</html>"

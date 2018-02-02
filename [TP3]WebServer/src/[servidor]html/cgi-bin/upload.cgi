#!/usr/bin/perl -w

#Tutorial: perlenespanol.com/tutoriales/cgi/upload_de_archivos.html
use strict;
use CGI::Carp qw(fatalsToBrowser);
use CGI;

#recibimos los datos enviados por el formulario
my %Input;
my $query = new CGI;
my @pairs = $query ->param;
foreach my $pair(@pairs){
	$Input{$pair} = $query ->param($pair);
}

print "Content-type: text/html\n\n";

print "<html>\n";
print "<center><head><meta charset=utf-8><title>Upload</title></head></center>\n";
print "<body>\n";

#Directorio donde queremos estacionar los archivos
my $dir = "/var/www/html/files/uploads"; #La carpeta debe tener permisos de escritura#

#Nombre del archivo en el servidor
my $nombre = "modulo_usuario.ko";

#Array con extensiones de archivos que podemos recibir
my @extensiones = ('ko');

recepcion_de_archivo(); #Iniciar la recepcion del archivo

print "</body>\n";
print "</html\n>";
exit(1);


sub recepcion_de_archivo{
	#chequeamos si la extencion es correcta
	my $nombre_en_servidor = $Input{'archivo'};
	$nombre_en_servidor =~ s/ /_/gi;
	$nombre_en_servidor =~ s!^.*(\\|\/)!!;
	my $extension_correcta = 0;
	foreach (@extensiones){
		if($nombre_en_servidor =~ /\.$_$/i){
			$extension_correcta = 1;
			last;
		}
	}

	#Recepcion y creacion del nuevo archivo#
	if($extension_correcta){
		#Abrimos el nuevo archivo
		open (OUTFILE, ">$dir/$nombre") || die "<center><p><h3>No se puedo crear el archivo</h3></p></center>";
		binmode(OUTFILE); #Para no tener problemas en Windows
				
		#Transferimos byte por byte el archivo	
		while (read($Input{'archivo'}, my $buffer , 128)){
			print OUTFILE $buffer;				
		}
		#Cerramos el archivo creado
		close (OUTFILE);
		print "<font size=4>El archivo fue recibido correctamente</font>\n";
	}

	else{
		print "Extension incorrecta. Solo se reciben archivos extención 'ko'";
		exit(0);
	}
}


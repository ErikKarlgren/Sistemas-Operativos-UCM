# Enunciado: mostrar contenido ficheros de un directorio solo si su tamaño
# es distinto a 5000B

DIR=$1
# echo $DIR

# Comprobamos si la cadena recibida es nula (no hay argumento) o no
if [ -z $DIR ];
then
		echo "Error: no argument was given"
		exit
fi

# Comprobamos que el argumento se corresponda con un directorio que
# exista
if [ -d $DIR ];
then

	cd $DIR

	# '*' equivale a los ficheros del directorio actual
    # En otro caso, por si quieres iterar 10 veces, puedes
    # escribir 'for i in `seq 0 10`.

	# 'for i in `ls`' equivale a 'for i in *'
	for FILE in *
	do
		# 'cut' corta cada línea en columnas, usando el espacio somo
		# separador, y se queda con la columna 5 (-f5), la que muestra
		# el tamaño en bytes del fichero. Las tildes `` son esenciales
		# para que el comando funcione.

		FILE_SIZE=`ls -l $FILE | cut -d" " -f5`

		# Ahora vemos si el fichero es 'normal' y si ocupa 5000 bytes

		# Es importante dejar un espacio entre los corchetes y los
		# comandos.
		if [ -f $FILE ] && [ $FILE_SIZE -ne 5000 ];
		then
				# Mostramos con 'less' para mayor comodidad de lectura
				# que con 'cat'.
				less $FILE
		fi
	done

else
		echo "Error: $DIR isn't a directory or doesn't exist"
fi

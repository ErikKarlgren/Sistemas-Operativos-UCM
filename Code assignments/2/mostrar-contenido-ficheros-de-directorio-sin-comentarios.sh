DIR=$1
if [ -z $DIR ];
then
		echo "Error: no argument was given"
		exit
fi

if [ -d $DIR ];
then
	cd $DIR

	for FILE in *
	do
		FILE_SIZE=`ls -l $FILE | cut -d" " -f5`
		if [ -f $FILE ] && [ $FILE_SIZE -ne 5000 ];
		then
				less $FILE
		fi
	done

else
		echo "Error: $DIR isn't a directory or doesn't exist"
fi

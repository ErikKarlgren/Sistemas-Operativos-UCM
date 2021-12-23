file=test5000bytes.junk

# i just wanna make sure a new file is created and don't edit an existing
# one. I'm sure there are better ways to do this, though.
touch $file
rm $file
touch $file

# echo "Haha terminal goes b" >> $file

for i in `seq 1 2500` # 2500 r's, 2500 newline characters
do
		echo r >> $file
done

echo "Junk finished"

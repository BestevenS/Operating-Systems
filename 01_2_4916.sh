read -p "Definition: " def
if [ $def = "file" ];
then
	ls -l | grep *.txt | awk '{print $9"\n"$6"\n"$7"\n"$8}' | sort
elif [ $def = "dir" ];
then
	ls -l | grep "dir" | wc -l
else
	echo "Error: The Definition is wrong..."
fi

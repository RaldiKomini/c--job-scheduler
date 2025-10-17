#for every file
for i do
#read every line of that file
while read cmd
	do 
    #execute it like a command
	$cmd
	done < $i
done

exit 0

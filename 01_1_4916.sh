read -p "The loop number: " N
y=0
for((i=0; i<$N; i++));
do
	let z=$i+1
	read -p "The number $z: " x	
	let y+=$x
done

avg=$y/$N

echo "Average: "
echo "scale=3; $avg" | bc

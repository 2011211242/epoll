killall -9 client
count=0
while [ $count -lt 1 ]
do
    ./bin/client
    count=$(($count+1))
    echo "count:"$count
done

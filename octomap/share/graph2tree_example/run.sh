mkdir build
cd build
cmake ..
make


size=(18 23 16)

for k in 10 12 14 16 18 20 22 24 26 28 30 # resolution as powers of 2 (0 1 2 3)
do
    for i in 1 2 3 # indices of size array (1 2 3)
    do
        # res=$(echo "scale=2; 2^$k / 10" | bc)
        res=$(echo "scale=2; $k / 100" | bc)
        # echo $res
        case $k in
            10|12|14) offset=0 ;;
            16|18|20|22|24) offset=1 ;;
            26|28|30) offset=2 ;;
            *) offset=0 ;; # Handle unexpected values
        esac
        cache_size=$((size[$((i-1))] - offset - 1))
        # ./test -i $i -res $res -s $cache_size -b 2 >> profiling/octocache/runtime/vec_best/$i/$res+$s.txt
        ./test -i $i -res $res -s $cache_size -b 4 >> profiling/octocache/octocacheRT/resultHashmap.txt
        # ./test -i $i -res $res -s $cache_size -b 8 >> profiling/octocache/runtime/vec_best/$i/$res+$s.txt
    done
done


for k in 1 2 3 4 5 6 7 8 9 10 # resolution as powers of 2 (0 1 2 3)
do
    for i in 1 2 3 # indices of size array (1 2 3)
    do
        # res=$(echo "scale=2; 2^$k / 10" | bc)
        res=$(echo "scale=2; $k / 10" | bc)
        # echo $res
        case $k in
            1) offset=0 ;;
            2) offset=1 ;;
            3|4|5) offset=2 ;;
            6|7|8|9|10) offset=3 ;;
            *) offset=0 ;; # Handle unexpected values
        esac
        cache_size=$((size[$((i-1))] - offset - 1))
        # ./test -i $i -res $res -s $cache_size -b 2 >> profiling/octocache/runtime/vec_best/$i/$res+$s.txt
        ./test -i $i -res $res -s $cache_size -b 4 >> profiling/octocache/octocacheRT/resultHashmap_highresolution.txt
        # ./test -i $i -res $res -s $cache_size -b 8 >> profiling/octocache/runtime/vec_best/$i/$res+$s.txt
    done
done


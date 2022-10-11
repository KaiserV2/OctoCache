# for DSNum in {1,2,3}
# do
#     for i in {1,2,4}  
#     do  
#         for j in {1,2,4}
#         do
#             s="/home/peiqing/vtune_projects/octomap/MT"
#             t=$s$DSNum"-"$i"-"$j
#             vtune -collect hotspots -call-stack-mode all -r $t ./testMorton -i $DSNum -s $i -k $j
#             # echo $t
#         done
#     done
# done

# for i in {1,2,3}
# do
#     # echo "$i original:" >> log.txt
#     for j in {1..3}
#     do
#         ./original -i $i
#     done
#     echo "\n" >> log.txt
# done

# for i in {1,2,3}
# do
#     for s in {1,2,4,8,16} # hash map size
#     do
#         for k in {1,2,4} # eviction speed
#         do
#             for j in {1..3}
#             do
#                 ./mortonCache -i $i -s $s -k $k
#             done
#             echo "\n" >> log.txt
#         done
#     done    
# done


# for i in {1,2,3}
# do
#     # echo "$i original:" >> log.txt
#     for j in {1}
#     do
#         ./original -i $i
#     done
#     echo "\n" >> log.txt
# done

# for i in {1,2,3}
# do
#     for s in {1,2,4,8,16,32,64} # hash map size
#     do
#         for k in {1,2,3,4,5,6} # eviction speed
#         do
#             for j in {1}
#             do
#                 ./mortonCache -i $i -s $s -k $k
#             done
#             echo "\n" >> log.txt
#         done
#     done    
# done

for i in {1..20}
do
    echo "Using bound "$i
    ./test -i 1 -b $i
done
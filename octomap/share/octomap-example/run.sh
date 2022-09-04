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

for i in {1..50}
do
    ./Scalar
done
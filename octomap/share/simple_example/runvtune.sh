
for j in {1,2,3,4,5,6,7,8,9}
do
    echo $j
    ./test -i 3 -s 16 -k $j
done

# vtune -collect hotspots -call-stack-mode all -r /home/peiqing/vtune_projects/octomap/CacheNonrace1-2-2 ./Scalar -i 1 -s 1 -k 2
# vtune -collect hotspots -call-stack-mode all -r /home/peiqing/vtune_projects/octomap/CacheNonrace2-2-2 ./Scalar -i 2 -s 1 -k 2
# vtune -collect hotspots -call-stack-mode all -r /home/peiqing/vtune_projects/octomap/CacheNonrace3-2-2 ./Scalar -i 3 -s 1 -k 2
# vtune -collect hotspots -call-stack-mode all -r /home/peiqing/vtune_projects/octomap/Cache1-2-2 ./Scalar -i 1 -s 2 -k 2
# vtune -collect hotspots -call-stack-mode all -r /home/peiqing/vtune_projects/octomap/Cache2-2-2 ./Scalar -i 2 -s 2 -k 2
# vtune -collect hotspots -call-stack-mode all -r /home/peiqing/vtune_projects/octomap/Cache3-2-2 ./Scalar -i 3 -s 2 -k 2
# vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/MT2 ./testMorton -i 2
# vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/MT3 ./testMorton -i 3



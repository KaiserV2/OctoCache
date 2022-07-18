# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/1O_e6 ./graph2tree -i 1 -s 1
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/2O_e6 ./graph2tree -i 2 -s 1
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/3O_e6 ./graph2tree -i 3 -s 1
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/1O_e4 ./graph2tree -i 1 -s 0
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/2O_e4 ./graph2tree -i 2 -s 0
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/3O_e4 ./graph2tree -i 3 -s 0
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/1O_e8 ./graph2tree -i 1 -s 2
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/2O_e8 ./graph2tree -i 2 -s 2
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/3O_e8 ./graph2tree -i 3 -s 2

# ./graph2tree -i 3 -s 1
# ./graph2tree -i 2 -s 1
# ./graph2tree -i 1 -s 1
# ./graph2tree -i 1 -s 0
# ./graph2tree -i 3 -s 0
# ./graph2tree -i 1 -s 2
# ./graph2tree -i 2 -s 2
# ./graph2tree -i 3 -s 2

<<<<<<< HEAD
# ./graph2tree -i 3 -s 1
# ./graph2tree -i 2 -s 1

# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/1OriginalO2 ./graph2tree -i 1
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/2OriginalO2 ./graph2tree -i 2
# vtune -collect hotspots -r /home/peiqing/vtune_projects/Octomap/3OriginalO2 ./graph2tree -i 3

make 
./tmp -i 1
./tmp -i 2
./tmp -i 3
make
./tmp -i 1
./tmp -i 2
./tmp -i 3
=======
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/2NodeUp6 ./simple -i 2 -s 6
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/2NodeUp8 ./simple -i 2 -s 8
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/3NodeUp4 ./simple -i 3 -s 4
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/3NodeUp6 ./simple -i 3 -s 6
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/3NodeUp8 ./simple -i 3 -s 8
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/1NodeUp4 ./simple -i 1 -s 4
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/1NodeUp6 ./simple -i 1 -s 6
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/1NodeUp8 ./simple -i 1 -s 8
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/1NodeUpOriginal ./simple -i 1
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/2NodeUpOriginal ./simple -i 2
vtune -collect hotspots -r /home/peiqing/vtune_projects/OctomapAnalysis/3NodeUpOriginal ./simple -i 3
>>>>>>> 264d64c90dd78e2c6d98e2a75e1f6f03b635a6e8

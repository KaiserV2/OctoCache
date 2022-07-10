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
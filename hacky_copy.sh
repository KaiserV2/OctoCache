sudo ln -sf /home/octomap/octomap/lib/liboctomath.so.1.9 /usr/local/lib/liboctomath.so
sudo ln -sf /home/octomap/octomap/lib/liboctomap.so.1.9 /usr/local/lib/liboctomap.so
                                                                                
sudo ln -sf /home/octomap/octomap/lib/liboctomath.so.1.9 /opt/ros/kinetic/lib/liboctomath.so
sudo ln -sf /home/octomap/octomap/lib/liboctomap.so.1.9 /opt/ros/kinetic/lib/liboctomap.so

sudo ln -sf /home/octomap/octomap/lib/liboctomath.so.1.9 /usr/local/lib/liboctomath.so.1.9
sudo ln -sf /home/octomap/octomap/lib/liboctomap.so.1.9 /usr/local/lib/liboctomap.so.1.9
                                                                                
SELF_PREFIX=/home/octomap/octomap/include/octomap                    
ROS_PREFIX=/opt/ros/kinetic/include/octomap                                     
                                                                                
sudo cp $SELF_PREFIX/*.h $ROS_PREFIX/                                           
sudo cp $SELF_PREFIX/*.hxx $ROS_PREFIX/
mkdir -p $ROS_PREFIX/hash
mkdir -p $ROS_PREFIX/multi-core
sudo cp $SELF_PREFIX/data-structure/*.h $ROS_PREFIX/data-structure/
sudo cp $SELF_PREFIX/hash/*.h $ROS_PREFIX/hash/
sudo cp $SELF_PREFIX/multi-core/*.h $ROS_PREFIX/multi-core/

# sudo cp $SELF_PREFIX/* $ROS_PREFIX/
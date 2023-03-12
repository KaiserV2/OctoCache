sudo ln -sf /home/nvidia/octomap/lib/liboctomath.so.1.9 /usr/local/lib/liboctomath.so
sudo ln -sf /home/nvidia/octomap/lib/liboctomap.so.1.9 /usr/local/lib/liboctomap.so
                                                                                
sudo ln -sf /home/nvidia/octomap/lib/liboctomath.so.1.9 /opt/ros/kinetic/lib/liboctomath.so
sudo ln -sf /home/nvidia/octomap/lib/liboctomap.so.1.9 /opt/ros/kinetic/lib/liboctomap.so

sudo ln -sf /home/nvidia/octomap/lib/liboctomath.so.1.9 /usr/local/lib/liboctomath.so.1.9
sudo ln -sf /home/nvidia/octomap/lib/liboctomap.so.1.9 /usr/local/lib/liboctomap.so.1.9
                                                                                
SELF_PREFIX=/home/nvidia/octomap/octomap/include/octomap                    
ROS_PREFIX=/opt/ros/kinetic/include/octomap                                     
                                                                                
sudo cp $SELF_PREFIX/*.h $ROS_PREFIX/                                           
sudo cp $SELF_PREFIX/*.hxx $ROS_PREFIX/
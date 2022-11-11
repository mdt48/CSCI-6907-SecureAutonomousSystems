# **Report 1 - Marshall Thompson**

## **Introduction**
The plots in this report contain the the values read from a genuine (A0) car. The real message (type 2 messages) values for speed, position, heading and acceleration sent by the A0 car are plotted against the received values (type 3 messages) from an attacking car. 

## **Part 1**
<img src="part6_data\pos.png" width=500>
<img src="part6_data\spd.png" width=500>
<img src="part6_data\acl.png" width=500>
<img src="part6_data\hed.png" width=500>


## **Part 2**
The genuine car moves at a more consistent and slower acceleration. This is reflected well in the position graph as the genuine_x position rising rapidly. On the acceleration graph there are several periods where genuine car's acceleration is near, or around 0, corresponding to the more smooth linear portions on the position graph. The attacking car does not have this. Instead the acceleration is constantly bouncing between high and low values, leading to its more drastic changes in the speed, heading and position graphs. To tell an attacking car from a real car, I would look for this erratic behavior in the speed, heading, acceleration and position graphs. 

## **Part 3**
<!-- ![Part 6 Genuine Car](part_8_data\pos.png "Part 6 Genuine Car")
![Part 6 Attacker Car](part_8_data\spd.png "Part 6 Genuine Car")
![Part 6 Attacker Car](part_8_data\acl.png "Part 6 Genuine Car")
![Part 6 Attacker Car](part_8_data\hed.png "Part 6 Genuine Car") -->
<img src="part_8_data\spd.png" width=500>
<img src="part_8_data\pos.png" width=500>
<img src="part_8_data\acl.png" width=500>
<img src="part_8_data\hed.png" width=500>

## **Part 4**

The same observations made in part 3 are true here as well. The attacker by definition is random, and therefore has many random movements, accelerations, positions and headings while all movements by the genuine car are more consistent and calm. Again, o tell an attacking car from a real car, I would look for this erratic behavior in the speed, heading, acceleration and position graphs. The change in the attack parameters did have a significant impact on the graphs. Looking at the speed graph it is obvious. It can be seen that the speed oscillates much more quickly than in the Part 3 (40 speed) experiment, leading to even more varied, and  drastic movements.
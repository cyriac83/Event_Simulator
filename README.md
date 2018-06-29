# Event_Simulator
An event simulator (in C) that implements Wieghted Fair Queueing (WFQ), Self Clocked Fair Queueing (SCFQ) and Random Early Detection (RED) employed in network routers for packet scheduling. This implementation could be used to compare the performance of these scheduling algorithms.

How to run :

Extract the tar file named CS09S006_CS08M015_lab7.tar.gz
Go to the concerned directory and type the following

# make clean
# make all		
# ./scfq -in <input file name> -out <output file name> -wfq -red -wt

Input and output file names are compulsory. Other run time arguments, 'wfq', 'red' and 'wt', are optional. 

Meaning of optional arguments:
wfq – program will use WFQ scheduling algorithm instead of SCFQ(by default).
red -  RED option gets enabled
wt – Connections are associated with weights (given in input file)

Input file format :

An input file will contain values in the following format:

N=4 T=10 C=100000 B=100
10 1000 1500
20 500 1200
20 750 1500
100 1000 1800
 
N – Number of sources
T – Simulation time
C -  Output link’s packet processing capacity
 (in packet length units per unit time)
 
B – Queue capacity in packet length units. 

From second row onwards, per connection metrics are mentioned in the order : number of packets per unit time,  minimum packet length and maximum packet length.

For simplicity, we assume  each packet length unit as 1 byte  and each time unit as 1 second. All results are written in to the output file mentioning these units. All comparisons and explanations  use the above units.

If weight option is enabled, then input file should contain values in the following format:

N=4 T=10 C=100000 B=100
10 1000 1500 4
20 500 1200 10 
20 750 1500 2
100 1000 1800 5

Weights are also mentioned along with other per connection metrics as the last value in each row(except first row).



Sample Input files: input1, input2,.......input10
Sample Output files:output1, output2,.....output10

Overview.pdf contains simulator design details



#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/*
Global variables : 
(All global variable names start with capital letter and local variable names with small letter)
*/

int Min_packetlen[1000]; 	/* store the minimum packet length of the packet for each source */ 
int Max_packetlen[1000]; 	/* store the maxinum packet length of the packet for each source */ 
float Curr_finish;		/* Currently executing packet's finish number */
float Finish_num[1000];	/* store finish number of last packet generated by the source */
float Server_proc_rate;		/* stores the packet processing rate of output link(server) */
float Source_packet_delay[1000]; /* stores total packet delay  for each source  */ 
float Packet_delay;		/* stores total packet delay*/
int Num_pck_trans[1000];	/* stores total number of packet transmitted for each source */
int Pck_trans;			/* stores total number of packet transmitted */
int Num_pck_gen[1000];	/* stores total number of packet generated for each source */
int Pck_gen;			/* stores total number of packet generated */
int Queue_size;		/* stores values of maximum queue size */
int Source_pck_drop[1000];	/* stores total number of packets dropped for each source */
int Pck_drop;			/* stores total number of packets dropped */
int Curr_queue_size;		/* stores current queue size value */
float Sim_time;		/* stores total simulation time */
float Interarrival_time[1000]; /* stores inter arrival time for each source */
float Weight[1000];		/* stores the weight for each connection */
int Weight_flag;
float Queue_util;		/* stores sum of all queue utilization taken over the entire simulation 
				time*/
int Queue_sample;		/* stores number of samples considered for calculating  queue 
				utilization*/
int Num_sources;

float Num_pckbytes_trans[1000]; /* stores total number of bytes transmitted for each source */
float Pckbytes_trans; /* stores total number of bytes transmitted */
float Num_pckbytes_gen[1000];/* stores total number of bytes generated for each source */
float Pckbytes_gen;/* stores total number of bytes generated */
float Worst_case_delay[1000]; /* stores worst case delay for each connection */
#define Queue_util_sampletime	0.1	/* find queue utilization after every 5 units of time */

struct Event_Structure /* structure to store each event */
{ 
float event_time; 
float event_handler; 		/* 0 - packet generation event, 1 - packet processing event */ 
int source_id; 			/* which source to generae packet, used only in case of packet 
				generation event */ 
struct Event_Structure *next; 		/* pointer to next event in future  */ 
struct Event_Structure *sibling_event; 	/* pointer to next event for the same time */ 
}; 




struct Packet_Structure 	/* structure that represents each packet – structure of each element in 
				queue will be this*/
{ 
float arrival_time; 
float finish_num; 
int length; 
int source_id; 
struct Packet_Structure *next; 
};

struct Event_Structure *front_event ;	/* pointer to first event scheduled in the event list */
struct Packet_Structure *front_queue, *rear_queue;	/* pointer to front and rear of the queue */

#define EVENTSIZE 50000
int test;

int packetgen(int source_id, float current_time, float inter_arrival_time);
int server(float current_time);
int wfqpacketgen(int source_id, float current_time, float inter_arrival_time);
int wfqserver(float current_time);

/******************* RED VARIABLES ********************************/

float Maxthreshold;
float Minthreshold;
float Maxprob;
float Count_packet;
float Red_weight;
float Avglength;
int Redflag;
#define REDWEIGHT 0.001
#define MAXPROB 1

/******************************************************************/



/******************  WFQ variables *************************************/

int Activeconn;
int Activesource[1000];
float Lasttime;
float Lastroundno;
float Correction;
int Wfq_flag;

/*************************************************************************/



/************** Event scheduler  module - MAIN FUNCTION ***************/

int main(int argc, char *argv[])
{


int i = 1, source_id, j, proc_flag, ct, event_count, random, event_processed[EVENTSIZE], randomlimit, k;
float t = 0; /* initialise the current time */
char str[1000], strtmp[50], c;
struct Event_Structure *temp_event, *temp_front, *temp_rear, *temp_event_list[EVENTSIZE];  /* pointer to event structure */
struct Event_Structure *curr_event, *prev_event;  /* pointer to event structure */
float queue_util, avg_pktdelay, pck_drop_prob, service_rate, fraction_bw, sumoffraction;
FILE *infile, *outfile;
int seed = 10000;		/* choose a seed value */

srand(seed);		/*initialize random number generator*/



/* read the input file and make the necessary initialisations */ 


j = 1;
while(j < argc) /* getting file information from run time arguments passed...put in loop */
{

	if( !strcmp(argv[j], "-in") )
	{
		if( (infile = fopen(argv[j+1], "r")) == NULL)
		{
			printf("Error in opening input file\n");
			exit(0);
		}
		j = j+2;
		
	}
	else if( !strcmp(argv[j], "-out") )
	{	
		if( (outfile = fopen(argv[j+1], "w+")) == NULL)
		{
			printf("Error in opening input file\n");
			exit(0);
		}
		j = j+2;
	
	}
	else if( !strcmp(argv[j], "-wt") )
	{	
		Weight_flag = 1;
		printf("Weight flag set\n");
		j++;
	
	}
	else if( !strcmp(argv[j], "-wfq") )
	{	
		Wfq_flag = 1;
		printf("WFQ flag set\n");
		j++;
	
	}
	else if( !strcmp(argv[j], "-red") )
	{	
		Redflag = 1;
		printf("RED flag set\n");
		j++;
	
	}
	else
		j++;
	
}


fscanf(infile, "%s", str);

j = 0;
c = str[j];
while(c != '\0')
{
	if( c == '=' )
	{
		c = str[++j];
		break;
	}
	c = str[++j];
}

k = 0;
while(c != '\0')
{
	strtmp[k++] = c;
	c = str[++j];
}
strtmp[k] = '\0';
Num_sources = atoi(strtmp);


fscanf(infile, "%s", str);
j = 0;
c = str[j];

while(c != '\0')
{
	if( c == '=' )
	{
		c = str[++j];
		break;
	}
	c = str[++j];
}
k = 0;
while(c != '\0')
{
	strtmp[k++] = c;
	c = str[++j];
}
strtmp[k] = '\0';

Sim_time = atoi(strtmp);


fscanf(infile, "%s", str);
j = 0;
c = str[j];
while(c != '\0')
{
	if( c == '=' )
	{
		c = str[++j];
		break;
	}
	c = str[++j];
}

k = 0;
while(c != '\0')
{
	strtmp[k++] = c;
	
	c = str[++j];
}
strtmp[k] = '\0';

Server_proc_rate =  atoi(strtmp);



fscanf(infile, "%s", str);
j = 0;
c = str[j];
while(c != '\0')
{
	if( c == '=' )
	{
		c = str[++j];
		break;
	}
	c = str[++j];
}
k = 0;
while(c != '\0')
{
	strtmp[k++] = c;
	c = str[++j];
}
strtmp[k] = '\0';
Queue_size =  atoi(strtmp);


for(j = 0; j < Num_sources; j++)
{
	fscanf(infile, "%s", str);
	Interarrival_time[j] = (float)1/(float)atoi(str);
	fscanf(infile, "%s", str);
	Min_packetlen[j] = atoi(str);
	
	fscanf(infile, "%s", str);
	Max_packetlen[j]= atoi(str);
	
	if(Weight_flag == 1)
	{
		fscanf(infile, "%s", str);
		Weight[j]= atoi(str);
	}
	
}


fclose(infile); 

/******** read data from input file ******************/




/***** default values ********************/
/*
Num_sources = 8;
Sim_time = 5;
Queue_size = 100000;
Server_proc_rate = 100000;

Interarrival_time[0] = 0.01;
Interarrival_time[1] = 0.05;
Interarrival_time[2] = 0.05;
Interarrival_time[3] = 0.05;

Interarrival_time[4] = 0.05;
Interarrival_time[5] = 0.05;
Interarrival_time[6] = 0.01;
Interarrival_time[7] = 0.05;

Min_packetlen[0] = 1000;
Max_packetlen[0] = 1200;

Min_packetlen[1] = 1000;
Max_packetlen[1] = 1200;

Min_packetlen[2] = 1000;
Max_packetlen[2] = 1200;


Min_packetlen[3] = 1000;
Max_packetlen[3] = 1200;


Min_packetlen[4] = 1000;
Max_packetlen[4] = 1200;

Min_packetlen[5] = 1000;
Max_packetlen[5] = 1200;

Min_packetlen[6] = 1000;
Max_packetlen[6] = 1200;


Min_packetlen[7] = 1000;
Max_packetlen[7] = 1200;

Weight[0] = 1;
Weight[1] = 1;
Weight[2] = 1;
Weight[3] = 25;
Weight[4] = 1;
Weight[5] = 1;
Weight[6] = 1;
Weight[7] = 25;

*/



/************  RED VALUE INITIALISATION ********************/

Maxthreshold = Queue_size;
Minthreshold = Queue_size/2;
Red_weight = REDWEIGHT;
Maxprob = MAXPROB;

/*************************************************************/



/******** WFQ VALUE INITIALISATION *****************************/

Correction = 1/Server_proc_rate;
Activeconn = Num_sources;
for(j = 0; j < Num_sources; j++)
{
	Activesource[j] = 1;
}

/***************************************************************/



/* initialisation of event structure */
for(j = 0; j < Num_sources; j++)
{
	
	temp_event = malloc(sizeof(struct Event_Structure));
	temp_event->event_time = 0;
	temp_event->event_handler = 0;
	temp_event->source_id = j;
	temp_event->next = NULL;
	temp_event->sibling_event = NULL;

	if( front_event == NULL)
	{
		front_event = temp_event;
		curr_event = front_event;
	}
	else
	{
		curr_event->sibling_event = temp_event;
		curr_event = curr_event->sibling_event;
	}
}
		
	
temp_event = malloc(sizeof(struct Event_Structure));
temp_event->event_time = 0;
temp_event->event_handler = 1;
temp_event->next = NULL;
temp_event->sibling_event = NULL;

curr_event->sibling_event = temp_event;



ct = 0;
while( t <= Sim_time ) 
{ 
		
	/* Calculate queue size every sampling interval time during the course of simulation */
	if(  t <= Sim_time &&  t >=  (i* Queue_util_sampletime) )
	{
		Queue_util += Curr_queue_size;
		Queue_sample += 1;
		i ++ ;
	}
	
	if( front_event != NULL ) 	/*  check if event_list != empty */
	{ 
		temp_front = front_event;	/* point temp_event to next event */
		front_event = front_event->next;	/* move front_event forward in the event list */
		t = temp_front->event_time ; 
		
		proc_flag = 0;
		event_count = 1;
		
		temp_event = temp_front;
		prev_event = temp_event;
		temp_event_list[0] = temp_event;
		temp_event = temp_event->sibling_event;
		while(temp_event != NULL)
		{
			temp_event_list[event_count] = temp_event;
			prev_event = temp_event;
			temp_event = temp_event->sibling_event;
			event_count ++;
		}
		temp_rear = prev_event;
		randomlimit = event_count;
		
		for(j = 0; j < EVENTSIZE; j++)
			event_processed[j] = 0;	

		
		while( event_count > 0 ) 		/*  Continue until no more event scheduled for 								time t - call the respective handlers in sequential 								order*/
		{ 


			/*ct ++;		
			 if( ct == 25)
			exit(0); */


			random = rand() % randomlimit;
			temp_event = temp_event_list[random];
			
			if(event_processed[random] == 0)
			{
				event_processed[random] = 1;

				curr_event = temp_event;
				
				temp_event = temp_front;
				while(temp_event != NULL)
				{
					if(temp_event == curr_event)
						break;
					prev_event = temp_event;
					temp_event = temp_event->sibling_event;
				}

				temp_event = curr_event;
	
				
				if(temp_event->event_handler == 0) /* packet generation event */
				{ 
					source_id =  temp_event->source_id;
					if( Wfq_flag == 1 )
						wfqpacketgen(source_id, t, Interarrival_time[source_id]); 
					else
						packetgen(source_id, t, Interarrival_time[source_id]); 
					
				} 
				else if(temp_event->event_handler == 1 && event_count == 1) 
				{ 
					if( Wfq_flag == 1 )
						wfqserver(t); 
					else
						server(t); 
					
				} 
				else
				{
				
					proc_flag = 1; /* execute the packet generation event last */
				
				}

				
				if(temp_front == temp_event && temp_rear == temp_event)
				{	
					
					temp_front  = temp_event->sibling_event;
					temp_rear = temp_event->sibling_event;
				}
				else if(temp_front == temp_event && temp_rear != temp_event)
				{
					
					temp_front  = temp_event->sibling_event;
				}
				else if(temp_front != temp_event && temp_rear == temp_event)
				{
					
					temp_rear = prev_event;
					temp_rear->sibling_event = NULL;
				}
				else
				{
					
					prev_event->sibling_event = temp_event->sibling_event;
				}
	
				
				free(temp_event);	/* free the memory for the currently processed event */
				event_count -- ;	
				
				
		 	} /* end of inner if loop */ 
		}/* end of inner while loop */
		if( proc_flag == 1)	/* schedule packet processing event at the last */
		{
			if( Wfq_flag == 1 )
				wfqserver(t); 
			else
				server(t); 
		}

	} /* end of outer if loop */
	else /* simulation over */
	{
		printf("\nSimulation Over\n");
		printf("********************************************\n\n");
		break;
	}

} /* end of outer while loop */ 

/*printf("val time = %f	%f\n",t, Curr_queue_size);*/

/* Do the calculation */


/********************* write to o/p file **********************************/

fprintf(outfile, "%s","		Overall Switch Metrics\n\n");

fprintf(outfile, "%s","Queue util		Avg. Pkt Delay(ms)		Pkt Drop Prob\n");

printf("Overall switch metrics \n\n");

/*** queue utilisation **************/
queue_util = Queue_util/Queue_sample;
printf("queue utilization = %.3f\n",queue_util);

/**** Average Pkt delay ****************/
avg_pktdelay = (Packet_delay/Pck_trans) * 1000;
printf("Avg. pakcet delay(ms) = %.3f\n", avg_pktdelay);

/*********** Pck drop probability ****************/
pck_drop_prob = (float)Pck_drop/(float)Pck_gen;
printf("packet drop probability = %.3f\n",pck_drop_prob);

fprintf(outfile, "%.3f		%.3f				%.3f\n\n",queue_util, avg_pktdelay, pck_drop_prob);


printf("\n\nPer Connection metrics \n");

fprintf(outfile, "%s","		Per Connection Metrics\n\n");
fprintf(outfile, "%s","ConnID	BG		BT		BG/BT		Fraction of Link BW	Avg. Pkt Delay(ms)	Pkt Drop Prob\n\n");

sumoffraction = 0;
for(j = 0; j < Num_sources; j++)
{
	printf("\n\nFor connection :  %d\n",j);
	fprintf(outfile, "%d	",j);
	printf("total number of packet length units generated(BG) = %.3f\n",Num_pckbytes_gen[j]);
	fprintf(outfile, "%.3f	",Num_pckbytes_gen[j]);
	printf("total number of packet length units transmitted(BT) = %.3f\n",Num_pckbytes_trans[j]);
	fprintf(outfile, "%.3f	",Num_pckbytes_trans[j]);

	printf("Ratio, BG/BT = %.3f\n",(float)Num_pckbytes_gen[j]/(float)Num_pckbytes_trans[j]);
	fprintf(outfile, "%.3f		",(float)Num_pckbytes_gen[j]/(float)Num_pckbytes_trans[j]);

	service_rate = Num_pckbytes_trans[j]/Sim_time;
	fraction_bw = service_rate/Server_proc_rate;
	printf("fraction of bw = %.3f\n", fraction_bw);
	fprintf(outfile, "%.3f			",fraction_bw);

	sumoffraction += fraction_bw;

	avg_pktdelay = Source_packet_delay[j]/Num_pck_trans[j];
	printf("Avg. pakcet delay(ms) = %.3f\n", avg_pktdelay*1000);
	fprintf(outfile, "%.3f			",avg_pktdelay*1000);

	pck_drop_prob = (float)Source_pck_drop[j]/(float)Num_pck_gen[j];
	printf("packet drop probability = %.3f\n",pck_drop_prob);
	fprintf(outfile, "%.3f\n",pck_drop_prob);

	printf("\nWorst case delay(ms) = %.3f \n",Worst_case_delay[j]*1000);
	fprintf(outfile, "\nWorst case delay(ms)  =  %.3f\n\n",Worst_case_delay[j]*1000);
	
}

printf("sum of all bw fraction = %.3f\n",sumoffraction);

printf("\n********************************************\n");

fclose(outfile);

}  /* end of main function */




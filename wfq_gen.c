




#include "lab7_header.h"

#define FINISHNO 999999999999
/*************** Packet generator module ***************/
int wfqpacketgen(int source_id, float current_time, float inter_arrival_time) 
{ 
int i, drop, dropchance, j;
float next_event_time, tempprob, prob, p, multiply;
float round, slopert, timediff, finishno, deltatime, time;
int packetlen, diff; 
struct Packet_Structure *temp_pkt; 	 /*  pointer to packet structure */

struct Event_Structure *prev_event, *temp_event;  /* pointer to event structure */
struct Event_Structure *curr_event;  /* pointer to event structure */




/* The length of a packet is uniformly distributed between maximum and minimum packet length */ 
diff = Max_packetlen[source_id] - Min_packetlen[source_id];
packetlen = Min_packetlen[source_id] + rand()% (diff + 1);


temp_pkt = malloc( sizeof(struct Packet_Structure) ); 

temp_pkt->arrival_time = current_time; 
temp_pkt->source_id = source_id; 
temp_pkt->length = packetlen; 
temp_pkt->next = NULL;



/* compute finish number as specified in WFQ algorithm */

/************************* WFQ Implementation ****************************************/

time = Lasttime;
while(1)
{
	timediff = current_time - time;
	slopert = 1/Activeconn;
	round = Lastroundno + (timediff * slopert)/ Correction;
	
	finishno = FINISHNO;
	for(j = 0; j < Num_sources; j++) /******* find the smallest finish number ***/
	{
		if( finishno > Finish_num[j] && Activesource[j] == 1)
		{
			finishno = Finish_num[j];
		}
	}
	
	if( round > finishno)
	{
		deltatime = (finishno - Lastroundno) *  Activeconn * Correction;
		time = time + deltatime;
		Lastroundno = finishno;
		for(j = 0; j < Num_sources; j++) 
		{
			if( round == Finish_num[j] && Activesource[j] == 1)
			{
				Activeconn --;
				Activesource[j] = 0;
			}
		}
		continue;
		
	}
	else if(round == finishno)
	{
		for(j = 0; j < Num_sources; j++) 
		{
			if( round == Finish_num[j] && Activesource[j] == 1)
			{
				Activeconn --;
				Activesource[j] = 0;
			}
		}
		break;
	}
	else
		break;
}
		

		
/*********************************************************************************/

Lasttime = current_time;
Lastroundno = round;




if(Weight_flag == 0)
{
	if( Finish_num[source_id] > round )
		temp_pkt->finish_num = Finish_num[source_id] + packetlen;
	else
		temp_pkt->finish_num = round + packetlen; 
}
else
{
	if( Finish_num[source_id] > round )
		temp_pkt->finish_num = Finish_num[source_id] + packetlen/Weight[source_id];
	else
		temp_pkt->finish_num = round + packetlen/Weight[source_id]; 
}
	

drop = 0;
if(Redflag == 1)
{
	/*****************  RED IMPLEMENTATION ********************************************************/

	Avglength = (1 - Red_weight) * Avglength + Red_weight * Curr_queue_size;

	if(Avglength  <= Minthreshold)
		drop = 0;
	else if( Avglength  > Minthreshold && Avglength  < Maxthreshold)
	{
	
		tempprob = Maxprob * (Avglength - Minthreshold)/(Maxthreshold - Minthreshold);
		multiply = Count_packet * tempprob;
		if( multiply >= 1)
		{
		
			prob = tempprob;
		}
		else
		{
			prob = (tempprob)/(1 - multiply );
		
		}
		dropchance = 1000000 * prob;
			
		if( dropchance != 0)
			p = ( rand() % dropchance )* 0.000001;  /* randomly generate a value between 0 and 9 */
		else
			p = 1;
	
		if(p >= prob) /* packet not generated */
			drop = 0;
		else
		{
			drop = 1;
			Count_packet = 0;
		}
	}
	else
	{
		drop = 1;
		Count_packet = 0;
	}

}
/*************************************************************************************************/


if( (Curr_queue_size + packetlen) >  Queue_size || drop == 1) /* If queue is full */
{
	Source_pck_drop[source_id] += 1;
	Pck_drop += 1;
	
	free(temp_pkt);
}
else /* if queue is not full */
{

	/* place the packet in output queue(at the rear end) */
	
	Finish_num[source_id] = temp_pkt->finish_num;
	if(front_queue == NULL)
	{
		front_queue = temp_pkt;
		rear_queue = temp_pkt;
	}
	else
	{
		
		
		rear_queue->next = temp_pkt;
		rear_queue = temp_pkt;
		
	}

	if( Activesource[source_id] == 0 )
	{
		Activesource[source_id] = 1;
		Activeconn ++;
	}
	/* increment queue size   */
	Curr_queue_size += packetlen;
	if(Redflag == 1)
		Count_packet ++;
}
	


Num_pck_gen[source_id] += 1;
Pck_gen += 1;
Num_pckbytes_gen[source_id] += packetlen;
Pckbytes_gen += packetlen; 


/* compute the time for next packet generation event */
next_event_time = current_time + Interarrival_time[source_id];

if(  next_event_time <= Sim_time) /* schedule 'packet generation' event only within simulation 
					time*/
{

	temp_event = malloc(sizeof(struct Event_Structure));
	temp_event->event_time = next_event_time;
	temp_event->event_handler = 0;
	temp_event->source_id = source_id;
	temp_event->next = NULL;
	temp_event->sibling_event = NULL;
	
	if( front_event == NULL) /* event list empty */
	{
		
		front_event = temp_event;
	}
	else
	{ 
		curr_event = front_event;
		prev_event = curr_event;
		i = 0;
		while( curr_event != NULL && curr_event->event_time < next_event_time)
		{
			
			prev_event = curr_event;
			curr_event = curr_event->next;
			
		}
		if(curr_event == NULL)
		{
			
			prev_event->next = temp_event;
		}
		else if(curr_event->event_time == next_event_time)
		{
			
			temp_event->next = curr_event->next;

			if(front_event == curr_event)
				front_event = temp_event;
			else
				prev_event->next = temp_event;

			temp_event->sibling_event = curr_event;
			curr_event->next = NULL;
		}
		else
		{
			
			temp_event->next = curr_event;
			if(front_event == curr_event)
				front_event = temp_event;
			else
				prev_event->next = temp_event;
		}
	}
			
			
	
		
	/* schedule the next 'packet generation' event for this particular source in the event list(see 	figure 1).If there are more than one event scheduled for that particular time, place the new 	'packet generation' event at the front. Each element in the event list is of type 	Event_Structure */

}
else
{
	
	/* do nothing – no more packet generation event */
}





return 0;



} 






#include "lab7_header.h"

#define FINISHNO 999999999999
/*************** Packet processor module ***************/

int wfqserver(float current_time) 
{ 

float process_time;	/* stores the processing time for the current packet */
struct Packet_Structure *temp_pkt, *curr_pkt, *prev_pkt, *select_pkt[50000]; 	/* pointer to packet structure */
struct Event_Structure *curr_event;  /* pointer to event structure */
struct Event_Structure *prev_event, *temp_event;  /* pointer to event structure */
int source_id, same_count, j;
float end_time, finishno;
float next_event_time, delay;
float round, slopert, timediff, deltatime, time;




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






if( Curr_queue_size > 0 ) /* check if atleast one packet is there in the queue */ 
{ 
	
	/* Queue is implemented as link list */
	/* Traverse the queue to find the packet with minimum finish number */
	/* find the number of packets having finish number same as minimum finish number and 
	their respective positions in queue */
	/* If there are more than one packet with finish number equal to minimum finish number, 	then randomly choose one from it. Point the previous packet of the selected packet to the 	next packet  in the queue. Point variable 'temp' to the selected packet */

	curr_pkt = front_queue;
	
	finishno = curr_pkt->finish_num;
	curr_pkt = curr_pkt->next;
	
	/* to get lowest finish number */
	while(curr_pkt != NULL)
	{
		
		if( finishno > curr_pkt->finish_num )
		{
			finishno = curr_pkt->finish_num;
		}
		curr_pkt = curr_pkt->next;
		
	}


	
	curr_pkt = front_queue;
	same_count = 0;
	
	/* check if there are more than one packet with same finish number */
	while(curr_pkt != NULL)
	{
		
		if( finishno == curr_pkt->finish_num )
		{
			select_pkt[same_count] = curr_pkt;
			same_count ++;
		}
		curr_pkt = curr_pkt->next;
	}
	
	/* randomly choose one packet among packets of same finish number */
	if(same_count > 0)
	{
		if(same_count > 1)		
			test ++;
		same_count = rand() % same_count;
		temp_pkt = select_pkt[same_count];
	}

	
	curr_pkt = front_queue;
	prev_pkt = curr_pkt;
	
	while(curr_pkt != NULL)
	{

	
		if( temp_pkt == curr_pkt )
			break;
		
		prev_pkt = curr_pkt;		
		curr_pkt = curr_pkt->next;
		
	}
	
	
	Curr_finish = temp_pkt->finish_num;
	
	source_id = temp_pkt->source_id;
	
	/* compute the processing time for the packet */ 
	process_time  = temp_pkt->length / Server_proc_rate;
	end_time = process_time + current_time;
	delay = end_time - temp_pkt->arrival_time;
	
	if( Worst_case_delay[source_id] < delay )
		Worst_case_delay[source_id] = delay;

	/* compute the delay for the processed packet */ 
	Source_packet_delay[source_id] +=  delay;
	
	Packet_delay +=  delay;

	/* modify variables that store number of packets transmitted */
	Num_pck_trans[source_id] += 1;
	Pck_trans += 1;
	Num_pckbytes_trans[source_id] += temp_pkt->length;
	Pckbytes_trans += temp_pkt->length;
	
	
	/* schedule the next 'packet processing' event in event list based on the processing time of 	the current packet */ 
	next_event_time = end_time;
	if(  next_event_time <= Sim_time) /* schedule 'packet generation' event only within simulation 
					time*/
	{
	temp_event = malloc(sizeof(struct Event_Structure));
	temp_event->event_time = next_event_time;
	temp_event->event_handler = 1;
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
	}
	/* modify packet list */
	

	if(front_queue == temp_pkt && rear_queue == temp_pkt)
	{		
		
		front_queue = temp_pkt->next;
		rear_queue = temp_pkt->next;
	}
	else if(front_queue == temp_pkt && rear_queue != temp_pkt)
	{
	
		front_queue = temp_pkt->next;
	}
	else if(front_queue != temp_pkt && rear_queue == temp_pkt)
	{
		
		rear_queue = prev_pkt;
		rear_queue->next = NULL;
	}
	else
		prev_pkt->next = temp_pkt->next;
	
	
	Curr_queue_size = Curr_queue_size - temp_pkt->length;
	free(temp_pkt);	/* free the processed packet's memory */
		/* reduce current queue size  */
	
	

} 

else /* if queue length is zero i.e no packet in queue */
{ 

	/* schedule the next 'packet processing' event along with the next event in future. Next event 	will always be a packet generation event. */ 
	/* If there are no outstanding events scheduled for future, then do nothing. This means that 	simulation is over */

	/* At any time, there will be only one outstanding packet processing event(scheduled for 	future), but there can be more than one outstanding packet generation event(scheduled for 	future) */ 

	temp_event = malloc(sizeof(struct Event_Structure));
	temp_event->event_handler = 1;
	temp_event->next = NULL;
	temp_event->sibling_event = NULL;
	
	if( front_event == NULL) /* event list empty */
	{
		free(temp_event);
		
	}
	else
	{ 
		curr_event = front_event;
		next_event_time = curr_event->event_time;
		temp_event->event_time = next_event_time;
		
	
		temp_event->next = curr_event->next;

		front_event = temp_event;
		
		temp_event->sibling_event = curr_event;
		curr_event->next = NULL;
		
		
	}

} 


return 0; 



} 


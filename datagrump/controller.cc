#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), the_window_size(20), ack_counter(0), total_rtt(0), rtt_standing(1000),
   rtt_min(1000), srtt(0), rtime_rtt_standing(0), rtime_rtt_min(0), v(1), lst_window(0), rpt_window(0),
   slow_start(1)
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size()
{
  /* Default: fixed window size of 100 outstanding datagrams */
  //unsigned int the_window_size = 50;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
   << " window size is " << the_window_size << endl;
   cerr << " last window size was " << lst_window << endl;
  }

  return the_window_size;
}

/* A datagram was sent */
void Controller::datagram_was_sent( const uint64_t sequence_number,
            /* of the sent datagram */
            const uint64_t send_timestamp,
                                    /* in milliseconds */
            const bool after_timeout
            /* datagram was sent because of a timeout */ )
{
  /* Default: take no action */
  // if (after_timeout ) {
  //   the_window_size = the * 9/10;
  // }


  if ( debug_ ) {
    cerr << "At time " << send_timestamp
   << " sent datagram " << sequence_number << " (timeout = " << after_timeout << ")\n";
  }
}

/* An ack was received */
void Controller::ack_received( const uint64_t sequence_number_acked,
             /* what sequence number was acknowledged */
             const uint64_t send_timestamp_acked,
             /* when the acknowledged datagram was sent (sender's clock) */
             const uint64_t recv_timestamp_acked,
             /* when the acknowledged datagram was received (receiver's clock)*/
             const uint64_t timestamp_ack_received )
                               /* when the ack was received (by sender) */
{
  /** ----Implementação do COPA ---- **/


  /* Default: take no action */
  // const uint64_t threshold = 90;
  const uint64_t rtt_value = timestamp_ack_received - send_timestamp_acked;

  if (timestamp_ack_received - rtime_rtt_min >= 10000) {
    rtt_min = rtt_value;
    rtime_rtt_min = timestamp_ack_received;
  }
  else if(rtt_value < rtt_min)
      rtt_min = rtt_value;
  
  // cerr << "Current RTT: " << rtt_value << " Smalest RTT: " << rtt_min << endl;

  // Calcula rtt standing em uma janela com a metade do valor do rtt smooting
  if (timestamp_ack_received - rtime_rtt_standing >= srtt/2) {
    rtt_standing = rtt_value;
    rtime_rtt_standing = timestamp_ack_received;
  }
  else 
    if(rtt_value < rtt_standing)
      rtt_standing = rtt_value;
  // cerr << "Time window (for stt standing): " << twnd << endl;

  
  if (srtt == 9999) 
    srtt = rtt_value;
  else {
    srtt = (1 - 0.125) * srtt + 0.125 * (double) rtt_value;
    // cerr << "srtt: " << srtt << endl;
  }



  // Algorithm Starts here
  double queing_delay = rtt_standing - rtt_min;
    
  double lambdaT = 1.0 / (0.5*queing_delay);
  double lambda = double (the_window_size)/rtt_standing;

  cerr << "lambdaT: " << lambdaT << endl;
  cerr << "lambda: " << lambda << endl;

    
  if(lambda <= lambdaT) {

      
    // }
    the_window_size = the_window_size + v/(0.5 * the_window_size) + 0.5;
        // the_window_size = the_window_size + 2;
  }
  else {
    the_window_size = the_window_size - v/(0.4 * the_window_size) + 0.4;
    slow_start = 0;
    // the_window_size = the_window_size - 2;
  }

  ack_counter += 1;
  if (ack_counter >= the_window_size) {
    if (slow_start == 1) {
        if (lambda <= lambdaT) {
          the_window_size *= 2;
        } else {
          slow_start = 0;
        }
    }
  }



  if((int)lst_window == (int)the_window_size) 
  {
    rpt_window++;
    
    if(rpt_window >= 3) {
      //cerr << "INCREMENT: " << lambda << endl;
      v = v + v;
      rpt_window = 0;
    }
  }
  else v = 1;


  lst_window = the_window_size;
  


  if ( debug_ ) {
    cerr << "At time " << timestamp_ack_received
   << " received ack for datagram " << sequence_number_acked
   << " (send @ time " << send_timestamp_acked
   << ", received @ time " << recv_timestamp_acked << " by receiver's clock)"
   << endl;
  }
}

/* How long to wait (in milliseconds) if there are no acks
   before sending one more datagram */
unsigned int Controller::timeout_ms()
{
  return 60; /* timeout of one second */
}

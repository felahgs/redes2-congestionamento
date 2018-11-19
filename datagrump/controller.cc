#include <iostream>

#include "controller.hh"
#include "timestamp.hh"

using namespace std;

/* Default constructor */
Controller::Controller( const bool debug )
  : debug_( debug ), the_window_size(50), package_counter(0), total_rtt(0)
{}

/* Get current window size, in datagrams */
unsigned int Controller::window_size()
{
  /* Default: fixed window size of 100 outstanding datagrams */
  //unsigned int the_window_size = 50;

  if ( debug_ ) {
    cerr << "At time " << timestamp_ms()
	 << " window size is " << the_window_size << endl;
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
  /* Default: take no action */

  const uint64_t rtt_value = timestamp_ack_received - send_timestamp_acked;
  
  //cerr << "Current RTT" << rtt_value << endl;


  /** ----Implementação do aimd ----
  / além de chegar um limite para fazer a redução da janela, é necessário considerar
  / todos os pacotes que estavam conjestionados após a identificação do primeiro
  / sem essa validação, o algoritmo irá considerar o RTT de todos esses pacotes
  / reduzindo o tamanho da janela mais vezes do que o necessário
  **/
  //   const uint64_t threshold = 90;
  // if(rtt_value >= threshold) {
  //   cerr << "LIMIT BREAK" << endl;
  //   the_window_size = (the_window_size * 1/2);
  // } else {
  //   the_window_size += 1;
  // }

  const uint64_t threshold = 40;
  if(rtt_value >= threshold) {
    // cerr << "LIMIT BREAK" << endl;
    // if (the_window_size <= 20)
      // the_window_size = 20;
    // else
      the_window_size = 1 + floor(the_window_size / 2);
  } else {
    the_window_size += 1;
  }

    //if(rtt_value >= threshold) {
    //cerr << "LIMIT BREAK" << endl;
 // }

  // if (package_counter >= the_window_size) {
  //   const uint64_t threshold = 90;
  //   if (rtt_value >= threshold) {
  //     the_window_size = the_window_size * 2 / 3;
  //   } else {
  //     the_window_size += 1;
  //   }
  //   package_counter = 0;
  // } else {
  //   package_counter += 1;
  // }

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
  return 1000; /* timeout of one second */
}

#ifndef CONTROLLER_HH
#define CONTROLLER_HH

#include <cstdint>
#include <math.h> 
#include <deque>


/* Rtt class for for recents rtt */
class RttReg {
  public:
     uint64_t value;
     uint64_t rtime;
};

/* Congestion controller interface */

class Controller
{
private:
  bool debug_; /* Enables debugging output */
  unsigned int the_window_size;
  unsigned int ack_counter;
  uint64_t total_rtt; 
  uint64_t rtt_standing; // valor do rtt stantnding dentro de um ingervalor t1
  uint64_t rtt_min; // Valor do rtt mínimo dentro de um intervalo de tempo t
  double srtt; // RTT smoothing
  // std::deque<RttReg> rttQueue;
  uint64_t rtime_rtt_standing; // Tempo de recebimento do ultimo rtt standing
  uint64_t rtime_rtt_min; // Tempo de recebinmento do ultimo rtt minimo
  double v; // variável velocity para o COPA
  unsigned int lst_window; // Janela da ultima mensagem recebida (Ack anterior)
  int rpt_window; // Variável de controle para janelas com tam repetido
  int slow_start;

  /* Add member variables here */

public:
  /* Public interface for the congestion controller */
  /* You can change these if you prefer, but will need to change
     the call site as well (in sender.cc) */

  /* Default constructor */
  Controller( const bool debug );

  /* Get current window size, in datagrams */
  unsigned int window_size();

  /* A datagram was sent */
  void datagram_was_sent( const uint64_t sequence_number,
        const uint64_t send_timestamp,
        const bool after_timeout );

  /* An ack was received */
  void ack_received( const uint64_t sequence_number_acked,
         const uint64_t send_timestamp_acked,
         const uint64_t recv_timestamp_acked,
         const uint64_t timestamp_ack_received );

  /* How long to wait (in milliseconds) if there are no acks
     before sending one more datagram */
  unsigned int timeout_ms();
};

#endif

#ifndef TYPES_H
#define TYPES_H

#include <systemc>
#include <vector>
#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#define QUANTUM
#define CLK_PERIOD 10

//using namespace sc_dt;
using namespace tlm;

typedef tlm_base_protocol_types::tlm_payload_type pl_t;






#endif // TYPES_H
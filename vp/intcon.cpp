#ifndef INT_CON_C
#define INT_CON_C
#include "intcon.hpp"

InterCon::InterCon(sc_module_name name):sc_module(name)
{
        s_ic_t.register_b_transport(this, &InterCon::b_transport);
        cout<< "InterCon created" << endl;
}

void InterCon::b_transport(pl_t& pl, sc_time& offset)
{
        sc_dt::uint64 address = pl.get_address();
        string msg;
        offset += sc_time(2, SC_NS);
        
        switch(address & 0xFF000000)
        {
                case 0x80000000:
                        cout << "inc: pristupa ip" << endl;
                        s_ic_i0->b_transport(pl, offset);
                        break; 

                case 0x81000000:
                        cout << "inc: pristupa dma" << endl;
                        s_ic_i1->b_transport(pl, offset);
                        break;

                default: 
                        msg = "ERROR WRONG ADDRESS";
                        break;
        }
        
}

#endif
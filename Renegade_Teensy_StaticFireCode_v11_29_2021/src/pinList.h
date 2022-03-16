#ifndef PINLIST_H
#define PINLIST_H

namespace pin
{
    
    const int led = 13;
    
    // Node Addressing pins
    const int NodeAddress0{0};
    const int NodeAddress1{1};
    const int NodeAddress2{28};
    const int NodeAddress3{};   // Need to figure out this pin assignment to get the extra NodeID space

    // abort interrupt pins - Not fully implemented, need to test and check
    const int abort{};
    const int reset{};
/*     const int abort{29};
    const int reset{30}; */

    const int Node2Node1{};  //These are for a non CAN bus digital I/O pin.
    const int Node2Node2{};


/*     // Igniter Pins - Node 2 - Engine Node
    const int Igniter0FireOut{};
    const int Igniter1FireOut{}; */
    

    // I DO NEED THIS FOR PINMODE SETUP - For now until I implement a class based pin setup for the enables?
    // safety enable pins - Node 2 - Engine Node
/*     const int HiPressHiVentSafe{29};
    const int MainValvesSafe{30};

    // safety enable pins - Node 3 - Prop Node
    const int FuelVentSafe{24};
    const int LoxDomeRegVentSafe{25};
    const int FuelDomeRegVentSafe{26};
    const int LoxVentSafe{27}; */
    


};

#endif
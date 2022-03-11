#ifndef PINLIST_H
#define PINLIST_H

namespace pin
{
    
    const int led = 13;
    
    // Node Addressing pins
    const int NodeAddress0{0};
    const int NodeAddress1{1};
    const int NodeAddress2{28};
    
    // abort interrupt pins
    const int abort{29};
    const int reset{30};
    

    // safety enable pins - Node 2 - Engine Node
    const int HiPressHiVentSafe{24};
    const int MainValvesSafe{25};

    // Igniter Pins - Node 2 - Engine Node
    const int Igniter0FireOut{};
    const int Igniter1FireOut{};


    // safety enable pins - Node 3 - Prop Node
    const int FuelVentSafe{24};
    const int LoxDomeRegVentSafe{25};
    const int FuelDomeRegVentSafe{26};
    const int LoxVentSafe{27};
    

};

#endif
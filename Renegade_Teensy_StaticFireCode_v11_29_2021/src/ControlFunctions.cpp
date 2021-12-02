#include "ControlFunctions.h"
#include "pinList.h"

void startupStateCheck(const State& currentState, Command& currentCommand)
{
    switch (currentState)
    {
    case State::passive:
        currentCommand = command_passive;
        break;
    case State::test:
        currentCommand = command_test;
        break;
    case State::HiPressArm:
        currentCommand = command_HiPressArm;
        break;
    case State::HiPressPressurized:
        currentCommand = command_HiPressPressurized;
        break;
    case State::TankPressArm:
        currentCommand = command_TankPressArm;
        break;
    case State::TankPressPressurized:
        currentCommand = commend_TankPressPressurized;
        break;
    case State::fireArmed:
        currentCommand = command_fireArm;
        break;
    case State::fire: // if we powercycle mid fire, we just vent
        currentCommand = command_vent;
        break;
    case State::abort:
        currentCommand = command_abort;
        break;
    case State::vent:
        currentCommand = command_vent;
        break;
    default:
        break;
    }
}

void haltFlagCheck(bool & haltFlag, const std::array<Valve*, NUM_VALVES>& valveArray, const std::array<Pyro*, NUM_PYROS>& pyroArray, const std::array<ValveEnable*, NUM_VALVEENABLE>& valveEnableArray)
{
    if(haltFlag)
    {
        valveArray.at(8)->setState(ValveState::CloseCommanded);
        valveArray.at(9)->setState(ValveState::CloseCommanded);
        pyroArray.at(0)->setState(PyroState::OffCommanded);
        pyroArray.at(1)->setState(PyroState::OffCommanded);
        //valveEnableArray.at(0)->setState(ValveEnableState::Off);
    }
    

}


void commandExecute(State& currentState, Command& currentCommand, const std::array<Valve*, NUM_VALVES>& valveArray, const std::array<Pyro*, NUM_PYROS>& pyroArray, const std::array<ValveEnable*, NUM_VALVEENABLE>& valveEnableArray, bool & haltFlag)
{
    switch (currentCommand)
    {
        case command_debug:
            currentState = State::debug;
            break;
        case command_passive:
            valveArray.at(0)->setState(ValveState::CloseCommanded);
            valveArray.at(1)->setState(ValveState::CloseCommanded);
            valveArray.at(2)->setState(ValveState::CloseCommanded);
            valveArray.at(3)->setState(ValveState::CloseCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::CloseCommanded);
            valveArray.at(6)->setState(ValveState::CloseCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::CloseCommanded);
            valveArray.at(9)->setState(ValveState::CloseCommanded);
            pyroArray.at(0)->setState(PyroState::OffCommanded);
            pyroArray.at(1)->setState(PyroState::OffCommanded);
/*             valveEnableArray.at(0)->setState(ValveEnableState::Off);
            valveEnableArray.at(1)->setState(ValveEnableState::Off);
            valveEnableArray.at(2)->setState(ValveEnableState::Off);
            valveEnableArray.at(3)->setState(ValveEnableState::Off);
            valveEnableArray.at(4)->setState(ValveEnableState::Off);
            valveEnableArray.at(5)->setState(ValveEnableState::Off); */
            currentState = State::passive;
            haltFlag = false;
            break;
        case command_test:
            currentState = State::test;
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::On);
            valveEnableArray.at(2)->setState(ValveEnableState::On);
            valveEnableArray.at(3)->setState(ValveEnableState::On);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::On);  */                       
            break;
        case command_abort:
            haltFlag = true;
            currentState = State::abort;
            break;
        case command_vent:
            valveArray.at(0)->setState(ValveState::CloseCommanded);
            valveArray.at(1)->setState(ValveState::OpenCommanded);
            valveArray.at(2)->setState(ValveState::OpenCommanded);
            valveArray.at(3)->setState(ValveState::OpenCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::OpenCommanded);
            valveArray.at(6)->setState(ValveState::OpenCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::CloseCommanded);
            valveArray.at(9)->setState(ValveState::CloseCommanded);
            pyroArray.at(0)->setState(PyroState::OffCommanded);
            pyroArray.at(1)->setState(PyroState::OffCommanded);            
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::On);
            valveEnableArray.at(2)->setState(ValveEnableState::On);
            valveEnableArray.at(3)->setState(ValveEnableState::On);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::Off);  */           
            currentState = State::vent;
            break;
// Fire Sequence commands will only be executed from the proper state
        case command_HiPressArm:
            if(currentState == State::passive)
            {
            valveArray.at(0)->setState(ValveState::CloseCommanded);
            valveArray.at(1)->setState(ValveState::CloseCommanded);
            valveArray.at(2)->setState(ValveState::CloseCommanded);
            valveArray.at(3)->setState(ValveState::CloseCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::CloseCommanded);
            valveArray.at(6)->setState(ValveState::CloseCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::CloseCommanded);
            valveArray.at(9)->setState(ValveState::CloseCommanded);
            pyroArray.at(0)->setState(PyroState::OffCommanded);
            pyroArray.at(1)->setState(PyroState::OffCommanded);            
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::Off);
            valveEnableArray.at(2)->setState(ValveEnableState::Off);
            valveEnableArray.at(3)->setState(ValveEnableState::Off);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::Off); */
            currentState = State::HiPressArm;
            }
            break;
        case command_HiPressPressurized:
            if(currentState == State::HiPressArm || currentState == State::TankPressArm) //added second conditional to allow entry backwards in a "disarm" state change
            {
            valveArray.at(0)->setState(ValveState::OpenCommanded);
            valveArray.at(1)->setState(ValveState::CloseCommanded);
            valveArray.at(2)->setState(ValveState::CloseCommanded);
            valveArray.at(3)->setState(ValveState::CloseCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::CloseCommanded);
            valveArray.at(6)->setState(ValveState::CloseCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::CloseCommanded);
            valveArray.at(9)->setState(ValveState::CloseCommanded);
            pyroArray.at(0)->setState(PyroState::OffCommanded);
            pyroArray.at(1)->setState(PyroState::OffCommanded);            
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::Off);
            valveEnableArray.at(2)->setState(ValveEnableState::Off);
            valveEnableArray.at(3)->setState(ValveEnableState::Off);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::Off); */
            currentState = State::HiPressPressurized;
            }
            break;
        case command_TankPressArm:
            if(currentState == State::HiPressPressurized)
            {
            valveArray.at(0)->setState(ValveState::OpenCommanded);
            valveArray.at(1)->setState(ValveState::CloseCommanded);
            valveArray.at(2)->setState(ValveState::CloseCommanded);
            valveArray.at(3)->setState(ValveState::CloseCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::CloseCommanded);
            valveArray.at(6)->setState(ValveState::CloseCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::CloseCommanded);
            valveArray.at(9)->setState(ValveState::CloseCommanded);
            pyroArray.at(0)->setState(PyroState::OffCommanded);
            pyroArray.at(1)->setState(PyroState::OffCommanded);            
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::Off);
            valveEnableArray.at(2)->setState(ValveEnableState::On);
            valveEnableArray.at(3)->setState(ValveEnableState::On);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::Off); */
            currentState = State::TankPressArm;
            }
            break;
        case commend_TankPressPressurized:
            if(currentState == State::TankPressArm)
            {
            valveArray.at(0)->setState(ValveState::OpenCommanded);
            valveArray.at(1)->setState(ValveState::CloseCommanded);
            valveArray.at(2)->setState(ValveState::CloseCommanded);
            valveArray.at(3)->setState(ValveState::OpenCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::CloseCommanded);
            valveArray.at(6)->setState(ValveState::OpenCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::CloseCommanded);
            valveArray.at(9)->setState(ValveState::CloseCommanded);
            pyroArray.at(0)->setState(PyroState::OffCommanded);
            pyroArray.at(1)->setState(PyroState::OffCommanded);            
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::Off);
            valveEnableArray.at(2)->setState(ValveEnableState::On);
            valveEnableArray.at(3)->setState(ValveEnableState::On);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::Off); */
            currentState = State::TankPressPressurized;
            }
            break;
        case command_fireArm:
            if(currentState == State::TankPressPressurized)
            {
            valveArray.at(0)->setState(ValveState::OpenCommanded);
            valveArray.at(1)->setState(ValveState::CloseCommanded);
            valveArray.at(2)->setState(ValveState::CloseCommanded);
            valveArray.at(3)->setState(ValveState::OpenCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::CloseCommanded);
            valveArray.at(6)->setState(ValveState::OpenCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::CloseCommanded);
            valveArray.at(9)->setState(ValveState::CloseCommanded);
            pyroArray.at(0)->setState(PyroState::OffCommanded);
            pyroArray.at(1)->setState(PyroState::OffCommanded);         
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::Off);
            valveEnableArray.at(2)->setState(ValveEnableState::On);
            valveEnableArray.at(3)->setState(ValveEnableState::On);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::On); */            
            currentState = State::fireArmed;
            }
            break;
        case command_fire:
            if(currentState == State::fireArmed)
            {
            valveArray.at(0)->setState(ValveState::OpenCommanded);
            valveArray.at(1)->setState(ValveState::CloseCommanded);
            valveArray.at(2)->setState(ValveState::CloseCommanded);
            valveArray.at(3)->setState(ValveState::OpenCommanded);
            valveArray.at(4)->setState(ValveState::CloseCommanded);
            valveArray.at(5)->setState(ValveState::CloseCommanded);
            valveArray.at(6)->setState(ValveState::OpenCommanded);
            valveArray.at(7)->setState(ValveState::CloseCommanded);
            valveArray.at(8)->setState(ValveState::FireCommanded);
            valveArray.at(9)->setState(ValveState::FireCommanded);
            pyroArray.at(0)->setState(PyroState::FireCommanded);
            pyroArray.at(1)->setState(PyroState::FireCommanded);
/*             valveEnableArray.at(0)->setState(ValveEnableState::On);
            valveEnableArray.at(1)->setState(ValveEnableState::Off);
            valveEnableArray.at(2)->setState(ValveEnableState::On);
            valveEnableArray.at(3)->setState(ValveEnableState::On);
            valveEnableArray.at(4)->setState(ValveEnableState::On);
            valveEnableArray.at(5)->setState(ValveEnableState::On); */
            currentState = State::fire;
            }
            break;

        
        
        
        
        case command_closeHiPress:
            if(currentState == State::test)
            {
                valveArray.at(0)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openHiPress:
             if(currentState == State::test)
            {
                valveArray.at(0)->setState(ValveState::OpenCommanded);
            }
            break;
        case command_closeHiPressVent:
            if(currentState == State::test)
            {
                valveArray.at(1)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openHiPressVent:
             if(currentState == State::test)
            {
                valveArray.at(1)->setState(ValveState::OpenCommanded);
            }
            break;
        case command_closeLoxVent:
            if(currentState == State::test)
            {
                valveArray.at(2)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openLoxVent:
             if(currentState == State::test)
            {
                valveArray.at(2)->setState(ValveState::OpenCommanded);
            }
            break;
        case command_closeLoxDomeReg:
            if(currentState == State::test)
            {
                valveArray.at(3)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openLoxDomeReg:
             if(currentState == State::test)
            {
                valveArray.at(3)->setState(ValveState::OpenCommanded);
            }
            break; 
        case command_closeLoxDomeRegVent:
            if(currentState == State::test)
            {
                valveArray.at(4)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openLoxDomeRegVent:
             if(currentState == State::test)
            {
                valveArray.at(4)->setState(ValveState::OpenCommanded);
            }
            break; 
        case command_closeFuelVent:
            if(currentState == State::test)
            {
                valveArray.at(5)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openFuelVent:
             if(currentState == State::test)
            {
                valveArray.at(5)->setState(ValveState::OpenCommanded);
            }
            break;
        case command_closeFuelDomeReg:
            if(currentState == State::test)
            {
                valveArray.at(6)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openFuelDomeReg:
             if(currentState == State::test)
            {
                valveArray.at(6)->setState(ValveState::OpenCommanded);
            }
            break; 
        case command_closeFuelDomeRegVent:
            if(currentState == State::test)
            {
                valveArray.at(7)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openFuelDomeRegVent:
             if(currentState == State::test)
            {
                valveArray.at(7)->setState(ValveState::OpenCommanded);
            }
            break; 
        case command_closeLoxMV:
            if(currentState == State::test)
            {
                valveArray.at(8)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openLoxMV:
             if(currentState == State::test)
            {
                valveArray.at(8)->setState(ValveState::OpenCommanded);
            }
            break;
        case command_closeFuelMV:
            if(currentState == State::test)
            {
                valveArray.at(9)->setState(ValveState::CloseCommanded);
            }
            break;
        case command_openFuelMV:
             if(currentState == State::test)
            {
                valveArray.at(9)->setState(ValveState::OpenCommanded);
            }
            break;
        case command_disableHiPressHiVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(0)->setState(ValveEnableState::Off);
                //digitalWrite(pin::HiPressHiVentSafe, 0);
            }
            break;
        case command_enableHiPressHiVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(0)->setState(ValveEnableState::On);
                //digitalWrite(pin::HiPressHiVentSafe, 1);
            }
            break;
        case command_disableFuelVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(1)->setState(ValveEnableState::Off);
                //digitalWrite(pin::FuelVentSafe, 0);
            }
            break;
        case command_enableFuelVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(1)->setState(ValveEnableState::On);
                //digitalWrite(pin::FuelVentSafe, 1);
            }
            break;
        case command_disableLoxDomeRegLoxDomeVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(2)->setState(ValveEnableState::Off);
                //digitalWrite(pin::LoxDomeRegVentSafe, 0);
            }
            break;
        case command_enableLoxDomeRegLoxDomeVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(2)->setState(ValveEnableState::On);
                //digitalWrite(pin::LoxDomeRegVentSafe, 1);
            }
            break;
        case command_disableFuelDomeRegFuelDomeVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(3)->setState(ValveEnableState::Off);
                //digitalWrite(pin::FuelDomeRegVentSafe, 0);
            }
            break;
        case command_enableFuelDomeRegFuelDomeVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(3)->setState(ValveEnableState::On);
                //digitalWrite(pin::FuelDomeRegVentSafe, 1);
            }
            break;
        case command_disableLoxVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(4)->setState(ValveEnableState::Off);
                //digitalWrite(pin::LoxVentSafe, 0);
            }
            break;
        case command_enableLoxVentSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(4)->setState(ValveEnableState::On);
                //digitalWrite(pin::LoxVentSafe, 1);
            }
            break;
        case command_disableMainValvesSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(5)->setState(ValveEnableState::Off);
                //digitalWrite(pin::MainValvesSafe, 0);
            }
            break;
        case command_enableMainValvesSafety:
            if(currentState == State::test)
            {
                valveEnableArray.at(5)->setState(ValveEnableState::On);
                //digitalWrite(pin::MainValvesSafe, 1);
            }
            break;
        default:
            break;
    }
}
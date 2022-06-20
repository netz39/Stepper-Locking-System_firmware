#include "StateMachine.hpp"

void StateMachine::taskMain()
{
    while (true)
    {
        switch (currentState)
        {
        case State::Unknown:
            // get door state
            //      if true -> get actual hall encoder value and set state to "Opened"
            //      if false -> get lock state
            //          if true -> door is closed correctly - get actual hall encoder value and
            //              set state to "Closed"
            //          if false -> set state to "Calibrating"

            break;

        case State::Opened:
            // wait for close command
            //      after that, check for door state
            //          if true -> close door and set state to "Closing"
            //          if false -> set state to "WantToClose"

            break;

        case State::Closed:
            // wait for open command
            //      after that, open door and set state to "Opening"
            break;

        case State::Opening:
            // show progess bar
            break;

        case State::Closing:
            // show progess bar
            break;

        case State::WantToClose:
            // wait for door state is triggered
            //      after that, close door and set state to "Closing"
            break;

        case State::Calibrating:
            // move stepper slowly until lock state is triggered
            // get actual hall encoder value and set state to "Closed"
            break;

        case State::Warning:
            break;

        default:
        case State::FatalError:
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------------

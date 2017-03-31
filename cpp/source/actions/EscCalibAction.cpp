#include "EscCalibAction.hpp"

#include "AppAction.hpp"

#include "UserUavEvent.hpp"

EscCalibAction::EscCalibAction(Listener* const _listener):
    ICommAction(_listener)
{
    state = IDLE;
    breaking = false;
}

void EscCalibAction::start(void)
{
    monitor->trace("ESC calibration procedure requested");
    state = INITIAL_COMMAND;
    sendSignal(SignalData::CALIBRATE_ESC, SignalData::START);
}

bool EscCalibAction::isActionDone(void) const
{
    return IDLE == state;
}

ICommAction::Type EscCalibAction::getType(void) const
{
    return ESC_CALIB;
}

std::string EscCalibAction::getStateName(void) const
{
    switch (state)
    {
    case IDLE: return "IDLE";
    case INITIAL_COMMAND: return "INITIAL_COMMAND";
    case ESC_DISCONNECT: return "ESC_DISCONNECT";
    case ESC_DISCONNECT_ACK: return "ESC_DISCONNECT_ACK";
    case ESC_CONNECT: return "ESC_CONNECT";
    case ESC_CONNECT_ACK: return "ESC_CONNECT_ACK";
    case CALIBRATING: return "CALIBRATING";
    default:
        __RL_EXCEPTION__("EscCalib::getStateName::Unexpected state");
    }
}

void EscCalibAction::proceed(const Parameter userCommand)
{
    switch (state)
    {
    case ESC_CONNECT:
        state = ESC_CONNECT_ACK;
        sendSignal(Command::CALIBRATE_ESC, userCommand);
        break;

    case ESC_DISCONNECT:
        state = ESC_DISCONNECT_ACK;
        sendSignal(Command::CALIBRATE_ESC, userCommand);
        break;

    default:
        except("Uav event received at unexpected state");
    }
}

void EscCalibAction::handleReception(const IMessage& message)
{
    switch (state)
    {
    case IDLE:
    case INITIAL_COMMAND:
        handleIdleReception(message);
        break;

    default:
        except("Message received in unexpected state", message);
    }
}

void EscCalibAction::handleSignalReception(const Parameter parameter)
{
    switch (state)
    {
    case INITIAL_COMMAND:
        switch (parameter)
        {
        case SignalData::ACK:
            monitor->trace("ESC calibration procedure started");
            listener->enableConnectionTimeoutTask(false);
            state = ESC_DISCONNECT;
            monitor->notifyUavEvent(new UavEvent(UavEvent::CALIBRATE_ESC_STARTED));
            break;

        case SignalData::NOT_ALLOWED:
            monitor->trace("ESC calibration not allowed");
            state = IDLE;
            listener->startAction(new AppAction(listener));
            monitor->notifyUavEvent(new UavEvent(UavEvent::CALIBRATE_ESC_NOT_ALLOWED));
            break;

        default:
            except("Unexpected signal parameter received");
        }
        break;

    case ESC_DISCONNECT_ACK:
    case ESC_CONNECT_ACK:
        switch (parameter)
        {
        case SignalData::ACK:
            switch (state)
            {
            case ESC_DISCONNECT_ACK:
                state = ESC_CONNECT;
                break;

            case ESC_CONNECT_ACK:
                state = CALIBRATING;
                startSignalTimeout(SignalData::CALIBRATE_ESC, 3000);
                break;

            default:
                except("Received unexpected ACK for calibration proceed");
            }
            monitor->notifyUavEvent(new UavEvent(UavEvent::CALIBRATE_ESC_DONE));
            break;

        case SignalData::BREAK_ACK:
            if (breaking)
            {
                monitor->notifyUavEvent(new UavEventMessage(UavEventMessage::INFO,
                                                            "ESC calibration broken."));
                state = IDLE;
                listener->startAction(new AppAction(listener));
            }
            else
            {
                 except("Received unexpected ACK for calibration break");
            }
            break;

        default:
            except("Unexpected signal parameter received");
        }
        break;

    case CALIBRATING:
        switch (parameter)
        {
        case SignalData::DONE:
            state = IDLE;
            listener->startAction(new AppAction(listener));
            monitor->notifyUavEvent(new UavEvent(UavEvent::CALIBRATE_ESC_ENDED));
            break;

        default:
            except("Unexpected signal parameter received");
        }
        break;

    default:
        except("Signal parameter in unexpected state");
    }
}


void EscCalibAction::handleUserEvent(const UserUavEvent& event)
{
    switch (event.getType())
    {
    case UserUavEvent::ESC_CALIB_DONE:
        proceed(Parameter::DONE);
        break;

    case UserUavEvent::ESC_CALIB_ABORT:
        breaking = true;
        proceed(Parameter::BREAK_FAIL);
        break;

    default:
        except("Unexpected user event received");
    }
}

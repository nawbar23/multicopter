// =========== roboLib ============
// ===  *** BARTOSZ NAWROT ***  ===
// ================================
#ifndef FLIGHTACTION_HPP
#define FLIGHTACTION_HPP

#include "ICommAction.hpp"

#include <atomic>

class FlightAction : public ICommAction
{
public:
    FlightAction(Listener* const _listener, const double _controlFreq);
    ~FlightAction(void);

    void start(void) override;

    bool isActionDone(void) const override;

    Type getType(void) const override;

    std::string getStateName(void) const override;

private:
    enum State
    {
        IDLE,
        FLING,
        BREAKING,
    };

    const double controlFreq;

    std::atomic<State> state;

    IAppTimer* controlTimer;

    void handleReception(const IMessage& message) override;
    void handleUserEvent(const UserUavEvent& event) override;

    void handleRunningReception(const IMessage& message);

    void controlTaskHandler(void);

    void flightEnded(const bool byBoard);
};

#endif // FLIGHTACTION_HPP

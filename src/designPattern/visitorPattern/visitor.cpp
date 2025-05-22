#include <iostream>
#include <vector>
#include <memory>

class ActionAllowed;
class ActionNotAllowed;
class BrokerRejected;

class Visitor
{
    public:
        virtual void visit(ActionAllowed* result) = 0;
        virtual void visit(ActionNotAllowed* result) = 0;
        virtual void visit(BrokerRejected* result) = 0;

        virtual ~Visitor() = default;
};

class StateTransitionResult
{
    public:
        virtual void accept(Visitor* visitor) = 0;
        virtual ~StateTransitionResult() = default;
};

class ActionAllowed : public StateTransitionResult
{
    public:
        void accept(Visitor* visitor) override
        {
            visitor->visit(this);
        }
};

class ActionNotAllowed : public StateTransitionResult
{
    public:
        void accept(Visitor* visitor) override
        {
            visitor->visit(this);
        }
};

class BrokerRejected : public StateTransitionResult
{
    public:
        void accept(Visitor* visitor) override
        {
            visitor->visit(this);
        }
};

class RouteVisitor : public Visitor
{
    public:
        void visit(ActionAllowed* state_trans_rslt) override
        {
            std::cout << "RouteVisitor: Apply business logic for ActionAllowed" << std::endl;
        }

        void visit(ActionNotAllowed* state_trans_rslt) override
        {
            std::cout << "RouteVisitor: Apply business logic for ActionNotAllowed" << std::endl;
        }

        void visit(BrokerRejected* state_trans_rslt) override
        {
            std::cout << "RouteVisitor: Apply business logic for BrokerRejected" << std::endl;
        }
};

class OrderVisitor : public Visitor {
public:
    void visit(ActionAllowed* state_trans_rslt) override {
        std::cout << "OrderVisitor: Apply business logic for ActionAllowed" << std::endl;
    }
    void visit(ActionNotAllowed* state_trans_rslt) override {
        std::cout << "OrderVisitor: Apply business logic for ActionNotAllowed" << std::endl;
    }
    void visit(BrokerRejected* state_trans_rslt) override {
        std::cout << "OrderVisitor: Apply business logic for BrokerRejected" << std::endl;
    }
};


int main()
{
    std::vector<std::unique_ptr<StateTransitionResult>> state_trans_results;
    state_trans_results.push_back(std::make_unique<ActionAllowed>());
    state_trans_results.push_back(std::make_unique<ActionNotAllowed>());
    state_trans_results.push_back(std::make_unique<BrokerRejected>());

    RouteVisitor routeVisitor;
    OrderVisitor orderVisitor;

    state_trans_results[0]->accept(&routeVisitor);
    state_trans_results[1]->accept(&orderVisitor);


    // for (const auto& result : state_trans_results) {
    //     result->accept(&routeVisitor);
    // }

    // for (const auto& result : state_trans_results) {
    //     result->accept(&orderVisitor);
    // }

    // given the results, we can use the visitor to apply different business logic

    return 0;
}

// g++ -std=c++20 -Wall visitor.cpp -o visitor.tsk
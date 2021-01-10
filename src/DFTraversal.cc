#include "DFTraversal.h"

Define_Module(DFTraversal);

DFTraversal::DFTraversal()
  : status(Status::UNVISITED)
  , initiator(false)
  , emisor(-1)
  , token(nullptr)
  , timer(nullptr)
  , unvisitedNeighbors(0) {

}

DFTraversal::~DFTraversal() {
  cancelAndDelete(timer);
}

void DFTraversal::initialize() {
  if (par("initiator").intValue() == getIndex()) {
    omnetpp::simtime_t startTime = par("startTime");
    timer = new omnetpp::cMessage("timer", MsgKind::TIMER);
    scheduleAt(startTime, timer);
    status = Status::INITIATOR;
  }
  WATCH(unvisitedNeighbors);
  WATCH(status);
}

void DFTraversal::handleMessage(omnetpp::cMessage* recvMsg) {
  if (status == Status::INITIATOR) {
    if (recvMsg->getKind() == MsgKind::TIMER) { //A1
      int neighborhoodSize = gateSize("port$o");
      unvisitedNeighbors = (1 << neighborhoodSize) - 1;
      initiator = true;
      token = new omnetpp::cMessage("token");
      visit(token);
    }
    else
      error("Invalid event for INITIATOR\n");
  }
  else if (status == Status::UNVISITED) {
    if (recvMsg->getKind() == MsgKind::TOKEN) {
      int neighborhoodSize = gateSize("port$o");
      unvisitedNeighbors = (1 << neighborhoodSize) - 1;
      emisor = recvMsg->getArrivalGate()->getIndex();
      EV_INFO << "Receiving token from port " << emisor << '\n';
      unvisitedNeighbors -= (1 << emisor);
      visit(recvMsg);
    }
    else
      error("Invalid event for UNVISITED\n");
  }
  else if (status == Status::VISITED) {
    if (recvMsg->getKind() == MsgKind::TOKEN) {
      int senderPort = recvMsg->getArrivalGate()->getIndex();
      EV_INFO << "Receiving token from port " << senderPort << '\n';
      unvisitedNeighbors -= (1 << senderPort);
      recvMsg->setKind(MsgKind::BACKEDGE);
      send(recvMsg, "port$o", senderPort);
      EV_INFO << "Node " << getIndex() << " send the token through port "
              << senderPort << '\n';
    }
    else if (recvMsg->getKind() != MsgKind::TIMER) {//BACKEDGE or RETURN
      EV_INFO << "Receiving token from port "
              << recvMsg->getArrivalGate()->getIndex() << '\n';
      visit(recvMsg);
    }
    else 
      error("Invalid event for VISITED\n");
  }
  else 
    error("Invalid status");
}

void DFTraversal::visit(omnetpp::cMessage* msg) {
  uint64_t mask = 1;
  const int neighborhoodSize = gateSize("port$o");
  if (unvisitedNeighbors > 0) {
    int nextNeighbor = 0;
    for (int i = 0; i < neighborhoodSize; i++) {
      EV_INFO << "Node " << getIndex() << ": "
              << mask << " & " << unvisitedNeighbors << " = "
              << (mask&unvisitedNeighbors) << '\n';
      if (unvisitedNeighbors & mask) {
        nextNeighbor = i;
        unvisitedNeighbors -= mask;
        mask = mask << 1;
        break;
      }
      else
        mask = mask << 1;
    }
    msg->setKind(MsgKind::TOKEN);
    send(msg, "port$o", nextNeighbor);
    EV_INFO << "Node " << getIndex() << " send the token through port "
            << nextNeighbor << '\n';
    status = Status::VISITED;
  }
  else {
    if (!initiator) {
      msg->setKind(MsgKind::RETURN);
      send(msg, "port$o", emisor);
      EV_INFO << "Node " << getIndex() << " send the token through port "
              << emisor << '\n';
    }
    status = Status::DONE;
  }
}
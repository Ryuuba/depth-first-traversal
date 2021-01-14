#include "DFTraversal.h"

Define_Module(DFTraversal);

DFTraversal::DFTraversal()
  : status(Status::UNVISITED)
  , initiator(false)
  , emisor(-1)
  , token(nullptr)
  , timer(nullptr)
  , unvisitedNeighbors(nullptr) {

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
  WATCH(status);
}

void DFTraversal::handleMessage(omnetpp::cMessage* recvMsg) {
  if (status == Status::INITIATOR) {
    if (recvMsg->getKind() == MsgKind::TIMER) { //A1
      int neighborhoodSize = gateSize("port$o");
      unvisitedNeighbors = std::make_unique<BitArray>(neighborhoodSize);
      WATCH(*unvisitedNeighbors);
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
      unvisitedNeighbors = std::make_unique<BitArray>(neighborhoodSize);
      WATCH(*unvisitedNeighbors);
      emisor = recvMsg->getArrivalGate()->getIndex();
      unvisitedNeighbors->reset(emisor); // U - {emisor}
      EV_INFO << "Receiving token from port " << emisor << '\n';
      visit(recvMsg);
    }
    else
      error("Invalid event for UNVISITED\n");
  }
  else if (status == Status::VISITED) {
    if (recvMsg->getKind() == MsgKind::TOKEN) {
      int senderPort = recvMsg->getArrivalGate()->getIndex();
      unvisitedNeighbors->reset(senderPort);
      recvMsg->setKind(MsgKind::BACKEDGE);
      recvMsg->setName("backedge");
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
  if (unvisitedNeighbors->decimal() > 0) {
    // Extract a neighbor from unvisitedNeighbors
    int nextNeighbor = unvisitedNeighbors->find_msone();
    unvisitedNeighbors->reset(nextNeighbor);
    EV_INFO << "Node " << getIndex() << " send the token through port "
            << nextNeighbor << '\n';
    msg->setKind(MsgKind::TOKEN);
    msg->setName("token");
    send(msg, "port$o", nextNeighbor);
    status = Status::VISITED;
  }
  else {
    if (initiator)
      delete msg;
    else {
      msg->setKind(MsgKind::RETURN);
      msg->setName("return");
      send(msg, "port$o", emisor);
      EV_INFO << "Node " << getIndex() << " send the token through port "
              << emisor << '\n';
    }
    status = Status::DONE;
  }
}

void DFTraversal::refreshDisplay() const {
  std::string info("Status: ");
  switch (status)
  {
  case Status::INITIATOR:
    info += "initiator";
    break;
  case Status::UNVISITED:
    info += "unvisited";
    break;
  case Status::VISITED:
    info += "visited";
    break;
  default:
    info += "done";
  }
  getDisplayString().setTagArg("t", 0, info.c_str());
}
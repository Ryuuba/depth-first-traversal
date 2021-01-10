//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef DFTRAVERSAL_H
#define DFTRAVERSAL_H

#include <omnetpp.h>
#include <cstdint>
#include <string>

class DFTraversal : public omnetpp::cSimpleModule {
  public:
    enum Status {
      INITIATOR = 0,
      UNVISITED,
      VISITED,
      DONE
    };
    enum MsgKind {
      TOKEN = 10,
      BACKEDGE,
      RETURN,
      TIMER
    };
  protected:
    /** @brief The node status */
    Status status;
    /** @brief Indicates whether this node starts as initiator or not */
    bool initiator;
    /** @brief The port nuumber at which the TOKEN was received at the first 
     * time
     */
    int emisor;
    /** @brief The only-one token traversing the network */
    omnetpp::cMessage* token;
    /** @brief The timer that starts the communications process */
    omnetpp::cMessage* timer;
    /** @brief The set of unvisited neighbors (64 neighbors at max). Think
     * this integer as a boolean array. 
     */
    uint64_t unvisitedNeighbors;
    // /** @brief The mask to access the index the last contacted neighbor */
    // uint64_t mask;
    // /** @brief The size of the neighborhood */
    // int neighborhoodSize;
  protected:
    /** @brief Prints the node state in de simulation canvas */
    virtual void refreshDisplay() const override;
    /** @brief A member function that senders call to contact their neighbors */
    virtual void visit(omnetpp::cMessage*);
  public:
    DFTraversal();
    ~DFTraversal();
    virtual void initialize();
    virtual void handleMessage(omnetpp::cMessage*);
};

#endif

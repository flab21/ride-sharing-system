// Assignment 5: Class-Based Ride Sharing System (C++)
// Frenie Labrador
// MSCS-632-A01
//
// Shows encapsulation, inheritance, and polymorphism.

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

// ---------------------------------------------------------------
// Ride: the base class
// ---------------------------------------------------------------
class Ride {
private:
    // private so nothing outside the class can change these directly
    int rideID;
    string pickupLocation;
    string dropoffLocation;
    double distance;

protected:
    // protected so the subclasses can store their own fare
    double fareAmount;

public:
    Ride(int id, string pickup, string dropoff, double dist) {
        rideID = id;
        pickupLocation = pickup;
        dropoffLocation = dropoff;
        // no negative miles allowed
        if (dist < 0) {
            dist = 0;
        }
        distance = dist;
        fareAmount = 0;
    }

    // virtual destructor, otherwise deleting a Ride* that really points
    // to a PremiumRide would not clean up properly
    virtual ~Ride() {}

    // getters
    int getRideID() const { return rideID; }
    string getPickupLocation() const { return pickupLocation; }
    string getDropoffLocation() const { return dropoffLocation; }
    double getDistance() const { return distance; }
    double getFareAmount() const { return fareAmount; }

    // virtual is what makes polymorphism work here
    virtual double fare() {
        fareAmount = distance * 1.00;   // basic rate, $1.00 per mile
        return fareAmount;
    }

    virtual string rideType() const {
        return "Basic Ride";
    }

    virtual void rideDetails() const {
        cout << "Ride #" << rideID << " (" << rideType() << ")" << endl;
        cout << "  Pickup:   " << pickupLocation << endl;
        cout << "  Dropoff:  " << dropoffLocation << endl;
        cout << "  Distance: " << fixed << setprecision(1) << distance << " miles" << endl;
        cout << "  Fare:     $" << fixed << setprecision(2) << fareAmount << endl;
    }
};

// ---------------------------------------------------------------
// StandardRide: inherits from Ride
// ---------------------------------------------------------------
class StandardRide : public Ride {
public:
    StandardRide(int id, string pickup, string dropoff, double dist)
        : Ride(id, pickup, dropoff, dist) {}

    // $2.50 base fee plus $1.50 per mile
    double fare() override {
        fareAmount = 2.50 + (getDistance() * 1.50);
        return fareAmount;
    }

    string rideType() const override {
        return "Standard Ride";
    }
};

// ---------------------------------------------------------------
// PremiumRide: inherits from Ride
// ---------------------------------------------------------------
class PremiumRide : public Ride {
public:
    PremiumRide(int id, string pickup, string dropoff, double dist)
        : Ride(id, pickup, dropoff, dist) {}

    // $5.00 base fee plus $3.00 per mile
    double fare() override {
        fareAmount = 5.00 + (getDistance() * 3.00);
        return fareAmount;
    }

    string rideType() const override {
        return "Premium Ride";
    }

    // reuse the base version, then add one extra line
    void rideDetails() const override {
        Ride::rideDetails();
        cout << "  Perk:     Leather seats and bottled water" << endl;
    }
};

// ---------------------------------------------------------------
// SharedRide: my extra subclass, the fare gets split between riders
// ---------------------------------------------------------------
class SharedRide : public Ride {
private:
    int passengers;

public:
    SharedRide(int id, string pickup, string dropoff, double dist, int numPassengers)
        : Ride(id, pickup, dropoff, dist) {
        if (numPassengers < 1) {
            numPassengers = 1;
        }
        passengers = numPassengers;
    }

    // $2.00 base fee plus $1.00 per mile, divided by the passengers
    double fare() override {
        fareAmount = (2.00 + getDistance() * 1.00) / passengers;
        return fareAmount;
    }

    string rideType() const override {
        return "Shared Ride";
    }

    void rideDetails() const override {
        Ride::rideDetails();
        cout << "  Split:    " << passengers << " passengers" << endl;
    }
};

// ---------------------------------------------------------------
// Driver: assignedRides is private, only reachable through methods
// ---------------------------------------------------------------
class Driver {
private:
    int driverID;
    string name;
    double rating;
    vector<Ride*> assignedRides;

public:
    Driver(int id, string driverName, double driverRating) {
        driverID = id;
        name = driverName;
        rating = driverRating;
    }

    void addRide(Ride* ride) {
        assignedRides.push_back(ride);
    }

    int getRideCount() const {
        return assignedRides.size();
    }

    // calls fare() on every ride, each one uses its own version
    double getTotalEarnings() {
        double total = 0;
        for (Ride* ride : assignedRides) {
            total += ride->fare();
        }
        return total;
    }

    void getDriverInfo() {
        cout << "Driver #" << driverID << ": " << name << endl;
        cout << "  Rating:         " << fixed << setprecision(1) << rating << endl;
        cout << "  Rides assigned: " << getRideCount() << endl;
        cout << "  Total earnings: $" << fixed << setprecision(2) << getTotalEarnings() << endl;
    }
};

// ---------------------------------------------------------------
// Rider: requestedRides is private too
// ---------------------------------------------------------------
class Rider {
private:
    int riderID;
    string name;
    vector<Ride*> requestedRides;

public:
    Rider(int id, string riderName) {
        riderID = id;
        name = riderName;
    }

    void requestRide(Ride* ride) {
        requestedRides.push_back(ride);
    }

    void viewRides() const {
        cout << "Ride history for " << name << " (Rider #" << riderID << "):" << endl;
        if (requestedRides.empty()) {
            cout << "  No rides yet." << endl;
            return;
        }
        for (const Ride* ride : requestedRides) {
            ride->rideDetails();
            cout << endl;
        }
    }
};

// ---------------------------------------------------------------
// main: the polymorphism demo
// ---------------------------------------------------------------
int main() {
    cout << "=== Ride Sharing System (C++) ===" << endl << endl;

    // one vector holding three different kinds of rides
    vector<Ride*> rides;
    rides.push_back(new StandardRide(101, "Campus Library", "Central Mall", 6.5));
    rides.push_back(new PremiumRide(102, "City Airport", "Downtown Hotel", 8.0));
    rides.push_back(new SharedRide(103, "North Gate", "Riverside Park", 4.2, 3));

    // polymorphism: same call, different fare() each time
    cout << "--- Fares (polymorphic fare() call) ---" << endl;
    for (Ride* ride : rides) {
        double amount = ride->fare();
        cout << ride->rideType() << " #" << ride->getRideID()
             << ": $" << fixed << setprecision(2) << amount << endl;
    }
    cout << endl;

    // polymorphism again with rideDetails()
    cout << "--- Ride details (polymorphic rideDetails() call) ---" << endl;
    for (Ride* ride : rides) {
        ride->rideDetails();
        cout << endl;
    }

    // set up a driver and a rider
    Driver driver(1, "Marcus Bell", 4.8);
    Rider rider(1, "Dana Ortiz");

    for (Ride* ride : rides) {
        driver.addRide(ride);
        rider.requestRide(ride);
    }

    cout << "--- Driver info ---" << endl;
    driver.getDriverInfo();
    cout << endl;

    cout << "--- Rider history ---" << endl;
    rider.viewRides();

    // encapsulation check: this line would NOT compile if I un-commented it
    // driver.assignedRides.clear();   // error: 'assignedRides' is private

    // clean up the memory
    for (Ride* ride : rides) {
        delete ride;
    }
    rides.clear();

    return 0;
}

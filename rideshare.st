"Assignment 5: Class-Based Ride Sharing System (Smalltalk)"
"Frenie Labrador"
"MSCS-632-A01"
"Written for GNU Smalltalk. Run it with:  gst rideshare.st"

"---------------------------------------------------------------"
"Ride: the base class"
"---------------------------------------------------------------"
Object subclass: Ride [
    | rideID pickupLocation dropoffLocation distance fareAmount |

    "class-side method to make a new ride"
    Ride class >> id: anId from: aPickup to: aDropoff distance: aDistance [
        ^self new setId: anId from: aPickup to: aDropoff distance: aDistance
    ]

    setId: anId from: aPickup to: aDropoff distance: aDistance [
        rideID := anId.
        pickupLocation := aPickup.
        dropoffLocation := aDropoff.
        "no negative miles allowed"
        aDistance < 0
            ifTrue: [distance := 0]
            ifFalse: [distance := aDistance].
        fareAmount := 0
    ]

    "getters"
    rideID [ ^rideID ]
    pickupLocation [ ^pickupLocation ]
    dropoffLocation [ ^dropoffLocation ]
    distance [ ^distance ]
    fareAmount [ ^fareAmount ]

    "basic rate, $1.00 per mile. The subclasses override this."
    fare [
        fareAmount := distance * 1.0.
        ^fareAmount
    ]

    rideType [ ^'Basic Ride' ]

    "turns 12.25 into the text '12.25' so the output looks like money"
    money: anAmount [
        | cents |
        cents := (anAmount * 100) rounded.
        ^(cents // 100) printString, '.', ((cents \\ 100) printPaddedWith: $0 to: 2)
    ]

    rideDetails [
        ('Ride #', rideID printString, ' (', self rideType, ')') displayNl.
        ('  Pickup:   ', pickupLocation) displayNl.
        ('  Dropoff:  ', dropoffLocation) displayNl.
        ('  Distance: ', distance printString, ' miles') displayNl.
        ('  Fare:     $', (self money: fareAmount)) displayNl
    ]
]

"---------------------------------------------------------------"
"StandardRide: inherits from Ride"
"---------------------------------------------------------------"
Ride subclass: StandardRide [
    "$2.50 base fee plus $1.50 per mile"
    fare [
        fareAmount := 2.5 + (self distance * 1.5).
        ^fareAmount
    ]

    rideType [ ^'Standard Ride' ]
]

"---------------------------------------------------------------"
"PremiumRide: inherits from Ride"
"---------------------------------------------------------------"
Ride subclass: PremiumRide [
    "$5.00 base fee plus $3.00 per mile"
    fare [
        fareAmount := 5.0 + (self distance * 3.0).
        ^fareAmount
    ]

    rideType [ ^'Premium Ride' ]

    "use the Ride version first, then add one more line"
    rideDetails [
        super rideDetails.
        '  Perk:     Leather seats and bottled water' displayNl
    ]
]

"---------------------------------------------------------------"
"SharedRide: my extra subclass, fare gets split between riders"
"---------------------------------------------------------------"
Ride subclass: SharedRide [
    | passengers |

    SharedRide class >> id: anId from: aPickup to: aDropoff distance: aDistance passengers: aCount [
        ^(self id: anId from: aPickup to: aDropoff distance: aDistance) setPassengers: aCount
    ]

    setPassengers: aCount [
        aCount < 1
            ifTrue: [passengers := 1]
            ifFalse: [passengers := aCount]
    ]

    "$2.00 base fee plus $1.00 per mile, divided by the passengers"
    fare [
        fareAmount := (2.0 + (self distance * 1.0)) / passengers.
        ^fareAmount
    ]

    rideType [ ^'Shared Ride' ]

    rideDetails [
        super rideDetails.
        ('  Split:    ', passengers printString, ' passengers') displayNl
    ]
]

"---------------------------------------------------------------"
"Driver: assignedRides can only be reached through methods"
"---------------------------------------------------------------"
Object subclass: Driver [
    | driverID name rating assignedRides |

    Driver class >> id: anId name: aName rating: aRating [
        ^self new setId: anId name: aName rating: aRating
    ]

    setId: anId name: aName rating: aRating [
        driverID := anId.
        name := aName.
        rating := aRating.
        assignedRides := OrderedCollection new
    ]

    addRide: aRide [
        assignedRides add: aRide
    ]

    rideCount [ ^assignedRides size ]

    "sends fare to every ride, each one uses its own version"
    totalEarnings [
        ^assignedRides inject: 0 into: [:sum :each | sum + each fare]
    ]

    getDriverInfo [
        | cents |
        ('Driver #', driverID printString, ': ', name) displayNl.
        ('  Rating:         ', rating printString) displayNl.
        ('  Rides assigned: ', self rideCount printString) displayNl.
        cents := (self totalEarnings * 100) rounded.
        ('  Total earnings: $', (cents // 100) printString, '.',
            ((cents \\ 100) printPaddedWith: $0 to: 2)) displayNl
    ]
]

"---------------------------------------------------------------"
"Rider: requestedRides is private too"
"---------------------------------------------------------------"
Object subclass: Rider [
    | riderID name requestedRides |

    Rider class >> id: anId name: aName [
        ^self new setId: anId name: aName
    ]

    setId: anId name: aName [
        riderID := anId.
        name := aName.
        requestedRides := OrderedCollection new
    ]

    requestRide: aRide [
        requestedRides add: aRide
    ]

    viewRides [
        ('Ride history for ', name, ' (Rider #', riderID printString, '):') displayNl.
        requestedRides isEmpty
            ifTrue: ['  No rides yet.' displayNl]
            ifFalse: [
                requestedRides do: [:each |
                    each rideDetails.
                    '' displayNl]]
    ]
]

"---------------------------------------------------------------"
"Main script: the polymorphism demo"
"---------------------------------------------------------------"
| rides driver rider |

'=== Ride Sharing System (Smalltalk) ===' displayNl.
'' displayNl.

"one collection holding three different kinds of rides"
rides := OrderedCollection new.
rides add: (StandardRide id: 101 from: 'Campus Library' to: 'Central Mall' distance: 6.5).
rides add: (PremiumRide id: 102 from: 'City Airport' to: 'Downtown Hotel' distance: 8.0).
rides add: (SharedRide id: 103 from: 'North Gate' to: 'Riverside Park' distance: 4.2 passengers: 3).

"polymorphism: same message, different fare method each time"
'--- Fares (polymorphic fare message) ---' displayNl.
rides do: [:each |
    | amount |
    amount := each fare.
    (each rideType, ' #', each rideID printString, ': $', (each money: amount)) displayNl].
'' displayNl.

"polymorphism again with rideDetails"
'--- Ride details (polymorphic rideDetails message) ---' displayNl.
rides do: [:each |
    each rideDetails.
    '' displayNl].

"set up a driver and a rider"
driver := Driver id: 1 name: 'Marcus Bell' rating: 4.8.
rider := Rider id: 1 name: 'Dana Ortiz'.

rides do: [:each |
    driver addRide: each.
    rider requestRide: each].

'--- Driver info ---' displayNl.
driver getDriverInfo.
'' displayNl.

'--- Rider history ---' displayNl.
rider viewRides.

#include <bandit/bandit.h>
#include <tripsselector.h>
#include <trip.h>

using namespace bandit;
using namespace snowhouse;

go_bandit([](){
    describe("test tripsselector", [](){

        TripsSelectionParams params;
        QVector<Trip> trips1(0);

        /* tn : create some trips for the tests*/
        
        Trip t1; // this trip is false (start 13:50)
        t1.startDateTime = QDateTime(QDate(2016, 5, 5), QTime(13, 50));
        t1.endDateTime =  QDateTime(QDate(2016, 5, 5), QTime(14, 30));
        t1.startStationId = 1;
        t1.endStationId = 1;
        trips1.append(t1);

        Trip t2; // this one must be accept (start 14:00)
        t2.startDateTime = QDateTime(QDate(2016, 5, 5), QTime(14, 00));
        t2.endDateTime =  QDateTime(QDate(2016, 5, 5), QTime(14, 01));
        t2.startStationId = 1;
        t2.endStationId = 1;
        trips1.append(t2);

        it("Hour min", [&](){ //test if the minimun works fine (here 14)
            params.fromHour = 14;
            params.toHour = 15;
            params.stationsIds.append(1);
            const TripsSelector tripselector= TripsSelector(params);
            QVector<Trip> outTrips = tripselector.selectFrom(trips1);
            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t2.id));
        });

        QVector<Trip> trips2(0);

        Trip t3; // this one have to be accept (max : 16h00)
        t3.startDateTime = QDateTime(QDate(2016, 4, 21), QTime(15, 40));
        t3.endDateTime = QDateTime(QDate(2016, 4, 21), QTime(16, 00));
        t3.startStationId = 1;
        t3.endStationId = 1;
        trips2.append(t3);

        Trip t4; //this one is false (16h50)
        t4.startDateTime = QDateTime(QDate(2016, 4, 21), QTime(15, 40));
        t4.endDateTime = QDateTime(QDate(2016, 4, 21), QTime(16, 50));
        t4.startStationId = 1;
        t4.endStationId = 1;
        trips2.append(t4);

        it("Hour max", [&](){ //test if the maximum works fine (here 16)
            params.fromHour = 15;
            params.toHour = 16;
            params.stationsIds.append(1);
            const TripsSelector tripselector= TripsSelector(params);
            QVector<Trip> outTrips = tripselector.selectFrom(trips2);
            AssertThat(outTrips.size(), Equals(1));
            if(outTrips.size() == 1) AssertThat(outTrips[0].id, Equals(t3.id));
        });

        QVector<Trip> trips3(0);

        Trip t5;
        t5.startDateTime = QDateTime(QDate(2016, 4, 21), QTime(12, 00));
        t5.endDateTime = QDateTime(QDate(2016, 4, 21), QTime(13, 00));
        t5.startStationId = 0; 
        t5.endStationId = 1; // this station is correct so the trip must be accept
        trips3.append(t5);

        Trip t6;
        t6.startDateTime = QDateTime(QDate(2016, 4, 21), QTime(12, 00));
        t6.endDateTime = QDateTime(QDate(2016, 4, 21), QTime(13, 00));
        t6.startStationId = 1; // this one too
        t6.endStationId = 0;
        trips3.append(t6);

        Trip t7;
        t7.startDateTime = QDateTime(QDate(2016, 4, 21), QTime(12, 00));
        t7.endDateTime = QDateTime(QDate(2016, 4, 21), QTime(13, 00));
        t7.startStationId = 2; // the two stations are false so the trip is false
        t7.endStationId = 3;
        trips3.append(t7);

        it("Station verification", [&](){ // test if the verification for the station is correct
            params.fromHour = 0;
            params.toHour = 24;
            params.stationsIds.append(1);
            const TripsSelector tripselector= TripsSelector(params);
            QVector<Trip> outTrips = tripselector.selectFrom(trips3);
            AssertThat(outTrips.size(), Equals(2));
            if(outTrips.size() == 2){
                AssertThat(outTrips[0].id, Equals(t5.id));
                AssertThat(outTrips[1].id, Equals(t6.id));
            }
        });
    });
});
